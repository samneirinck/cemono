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
struct IMonoScript;

class CFlowNode;

// Passed down to node script when initializing
struct SMonoNodeInfo
{
	SMonoNodeInfo(CFlowNode *node, TFlowNodeId _id, TFlowGraphId _graphId)
		: pNode(node)
		, id(_id)
		, graphId(_graphId)
	{
	}

	CFlowNode *pNode;
	TFlowNodeId id;
	TFlowGraphId graphId;
};

class CFlowManager
	: public IMonoScriptBind
	, public IFlowNodeFactory
{
public:
	CFlowManager();
	~CFlowManager();

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
	virtual void Reset() override {}
	// ~IFlowNodeFactory

	// IMonoScriptSystemListener
	virtual void OnPreScriptCompilation(bool isReload) {}
	virtual void OnPostScriptCompilation(bool isReload, bool compilationSuccess) {}

	virtual void OnPreScriptReload(bool initialLoad) {}
	virtual void OnPostScriptReload(bool initialLoad) { if(initialLoad) Reset(); }
	// ~IMonoScriptSystemListener

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "NativeFlowNodeMethods"; }
	virtual const char *GetNamespace() { return "CryEngine.FlowSystem.Native"; }
	// ~IMonoScriptBind

	static void RegisterNode(mono::string typeName);

	static bool IsPortActive(CFlowNode *pNode, int);

	static int GetPortValueInt(CFlowNode *pNode, int);
	static float GetPortValueFloat(CFlowNode *pNode, int);
	static EntityId GetPortValueEntityId(CFlowNode *pNode, int);
	static mono::string GetPortValueString(CFlowNode *pNode, int);
	static bool GetPortValueBool(CFlowNode *pNode, int);
	static Vec3 GetPortValueVec3(CFlowNode *pNode, int);

	static void ActivateOutput(CFlowNode *pNode, int);
	static void ActivateOutputInt(CFlowNode *pNode, int, int);
	static void ActivateOutputFloat(CFlowNode *pNode, int, float);
	static void ActivateOutputEntityId(CFlowNode *pNode, int, EntityId);
	static void ActivateOutputString(CFlowNode *pNode, int, mono::string);
	static void ActivateOutputBool(CFlowNode *pNode, int, bool);
	static void ActivateOutputVec3(CFlowNode *pNode, int, Vec3);

	static IEntity *GetTargetEntity(CFlowNode *pNode, EntityId &id);

	static void SetRegularlyUpdated(CFlowNode *pNode, bool update);

	int m_refs;
};

#endif //__FLOW_MANAGER__