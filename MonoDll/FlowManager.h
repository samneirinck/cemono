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

#include "MonoCommon.h"

#include <IMonoScriptBind.h>
#include <IFlowSystem.h>

struct IMonoArray;
struct IMonoClass;

class CFlowNode;

struct SNodeType
{
	typedef std::vector<CFlowNode *> TFlowNodes;

	SNodeType(const char *name) : typeName(name), scriptName(name), pInputs(NULL), pOutputs(NULL) {}
	SNodeType(const char *name, const char *scriptname) : typeName(name), scriptName(scriptname), pInputs(NULL), pOutputs(NULL) {}

	void ReloadPorts(IMonoClass *pScript);
	
	/// <summary>
	/// Gets the complete node type name, i.e. entity:Bouncy
	/// </summary>
	const char *GetTypeName() const { return typeName.c_str(); }
	/// <summary>
	/// Gets the script name, same as GetTypeName unless this is an entity node.
	/// </summary>
	const char *GetScriptName() { return scriptName.c_str(); }

	SOutputPortConfig *GetOutputPorts(IMonoClass *pScript) { if(!pOutputs) ReloadPorts(pScript); return pOutputs; }
	SInputPortConfig *GetInputPorts(IMonoClass *pScript) { if(!pInputs) ReloadPorts(pScript); return pInputs; }

	TFlowNodes nodes;

	void RemoveNode(CFlowNode *pNode) { nodes.erase(std::remove(nodes.begin(), nodes.end(), pNode), nodes.end()); }

private:

	string typeName;
	string scriptName;

	SOutputPortConfig *pOutputs;
	SInputPortConfig *pInputs;
};

class CFlowManager : 
	public IMonoScriptBind,
	public IFlowNodeFactory
{
public:
	CFlowManager();
	~CFlowManager() {}

	typedef std::vector<SNodeType *> TFlowTypes;

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

	static SNodeType *InstantiateNode(CFlowNode *pNode, const char *typeName);
	static void UnregisterNode(CFlowNode *pNode);

	static CFlowNode *GetNodeById(int scriptId);

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "FlowNode"; }
	// ~IMonoScriptBind


	static void RegisterNode(mono::string typeName);

	static bool IsPortActive(int, int);

	template <class T>
	static void ActivateOutputOnNode(int scriptId, int index, const T &value);
	
	static int GetPortValueInt(int, int);
	static float GetPortValueFloat(int, int);
	static EntityId GetPortValueEntityId(int, int);
	static mono::string GetPortValueString(int, int);
	static bool GetPortValueBool(int, int);
	static mono::object GetPortValueVec3(int, int);

	static void ActivateOutput(int, int);
	static void ActivateOutputInt(int, int, int);
	static void ActivateOutputFloat(int, int, float);
	static void ActivateOutputEntityId(int, int, EntityId);
	static void ActivateOutputString(int, int, mono::string);
	static void ActivateOutputBool(int, int, bool);
	static void ActivateOutputVec3(int, int, Vec3);

	static TFlowTypes m_nodeTypes;

	int m_refs;
};

#endif //__FLOW_MANAGER__