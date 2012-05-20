/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// The FlowManager handles registering of Mono flownodes.
//////////////////////////////////////////////////////////////////////////
// 23/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __FLOW_MANAGER__
#define __FLOW_MANAGER__

#include "EntityManager.h"
#include "MonoCommon.h"
#include <IMonoScriptBind.h>

#include <IFlowSystem.h>

struct IMonoArray;
struct IMonoClass;

class CFlowNode;

struct SNodeType
{
	SNodeType(const char *name) : typeName(name), pInputs(NULL), pOutputs(NULL)
	{
		if(bEntityNode = typeName.find("entity:") != string::npos)
			scriptName = typeName.substr(7);
		else 
			scriptName = typeName;
	}

	void ReloadPorts(IMonoClass *pScript);
	
	/// <summary>
	/// Gets the complete node type name, i.e. entity:Bouncy
	/// </summary>
	const char *GetTypeName() const { return typeName.c_str(); }

	/// <summary>
	/// Gets the node's script name, i.e. Bouncy
	/// </summary>
	const char *GetScriptName() const { return scriptName; }

	bool IsEntityNode() const { return bEntityNode; }

	SOutputPortConfig *GetOutputPorts(IMonoClass *pScript) { if(!pOutputs) ReloadPorts(pScript); return pOutputs; }
	SInputPortConfig *GetInputPorts(IMonoClass *pScript) { if(!pInputs) ReloadPorts(pScript); return pInputs; }

private:

	string typeName;
	string scriptName;
	bool bEntityNode;

	SOutputPortConfig *pOutputs;
	SInputPortConfig *pInputs;
};

// Passed down to node script when initializing
struct SMonoNodeInfo
{
	SMonoNodeInfo(CFlowNode *node)
		: pNode(node)
	{
	}

	CFlowNode *pNode;
};

class CFlowManager
	: public IMonoScriptBind
	, public IFlowNodeFactory
	, public IMonoScriptSystemListener
{
public:
	CFlowManager();
	~CFlowManager() {}

	typedef std::vector<std::shared_ptr<SNodeType>> TFlowTypes;

	// IFlowNodeFactory
	virtual void AddRef() override { ++m_refs; }
	// We want to manually kill this off, since it's used so often.
	virtual void Release() override { if( 0 >= --m_refs) delete this; }
	IFlowNodePtr Create( IFlowNode::SActivationInfo *pActInfo ) override;

	virtual void GetMemoryUsage(ICrySizer * s) const override
	{ 
		SIZER_SUBCOMPONENT_NAME(s, "CFlowManager");
		s->Add(*this);
	}
	virtual void Reset() override;
	// ~IFlowNodeFactory

	// IMonoScriptSystemListener
	virtual void OnPreScriptCompilation(bool isReload) {}
	virtual void OnPostScriptCompilation(bool isReload, bool compilationSuccess) {}

	virtual void OnPreScriptReload(bool initialLoad) {}
	virtual void OnPostScriptReload(bool initialLoad) { if(initialLoad) Reset(); }
	// ~IMonoScriptSystemListener

	static std::shared_ptr<SNodeType> GetNodeType(const char *typeName);

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "FlowNode"; }
	// ~IMonoScriptBind

	static void RegisterNode(mono::string typeName);

	static bool IsPortActive(CFlowNode *pNode, int);

	static int GetPortValueInt(CFlowNode *pNode, int);
	static float GetPortValueFloat(CFlowNode *pNode, int);
	static EntityId GetPortValueEntityId(CFlowNode *pNode, int);
	static mono::string GetPortValueString(CFlowNode *pNode, int);
	static bool GetPortValueBool(CFlowNode *pNode, int);
	static mono::object GetPortValueVec3(CFlowNode *pNode, int);

	static void ActivateOutput(CFlowNode *pNode, int);
	static void ActivateOutputInt(CFlowNode *pNode, int, int);
	static void ActivateOutputFloat(CFlowNode *pNode, int, float);
	static void ActivateOutputEntityId(CFlowNode *pNode, int, EntityId);
	static void ActivateOutputString(CFlowNode *pNode, int, mono::string);
	static void ActivateOutputBool(CFlowNode *pNode, int, bool);
	static void ActivateOutputVec3(CFlowNode *pNode, int, Vec3);

	static mono::object GetTargetEntity(CFlowNode *pNode);

	static TFlowTypes m_nodeTypes;

	int m_refs;
};

#endif //__FLOW_MANAGER__