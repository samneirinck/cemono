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
class CMonoFlowNode;

struct SNodeData
{
	SNodeData(CMonoFlowNode *pFlowNode);

	CMonoFlowNode *pNode;

	static SOutputPortConfig *pOutputs;
	static SInputPortConfig *pInputs;
};

class CEntityFlowManager : public IFlowNodeFactory
{
public:
	CEntityFlowManager() : m_refs(0) {}

	// IFlowNodeFactory
	virtual void AddRef() { ++m_refs; }
	// We want to manually kill this off, since it's used so often.
	virtual void Release() { if( 0 >= --m_refs) delete this; }
	IFlowNodePtr Create( IFlowNode::SActivationInfo *pActInfo );

	virtual void GetMemoryUsage(ICrySizer * s) const
	{ 
		SIZER_SUBCOMPONENT_NAME(s, "CEntityFlowManager");
		s->Add(*this); 
	}
	virtual void Reset() {}
	// ~IFlowNodeFactory

protected:
	int m_refs;
};

class CFlowManager : public IMonoScriptBind, IFlowNodeFactory
{
public:
	CFlowManager();
	~CFlowManager() {}

	typedef std::map<int, SNodeData *> TFlowNodes;

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

	static void RegisterFlowNode(CMonoFlowNode *pNode, int scriptId) { m_nodes.insert(TFlowNodes::value_type(scriptId, new SNodeData(pNode))); }
	static void UnregisterFlowNode(int id);

	static SNodeData *GetNodeDataById(int scriptId);
	static CMonoFlowNode *GetNodeById(int scriptId);

	CEntityFlowManager *GetEntityFlowManager() const { return m_pEntityFlowManager; }

protected:
	// IMonoScriptBind
	virtual const char* GetClassName() { return "FlowSystem"; }
	// ~IMonoScriptBind

	ExposedMonoMethod(void, RegisterNode, mono::string, mono::string, bool);

	MonoMethod(bool, IsPortActive, int, int);

	template <class T>
	static void ActivateOutputOnNode(int scriptId, int index, const T &value);
	
	MonoMethod(int, GetPortValueInt, int, int);
	MonoMethod(float, GetPortValueFloat, int, int);
	MonoMethod(EntityId, GetPortValueEntityId, int, int);
	MonoMethod(mono::string, GetPortValueString, int, int);
	MonoMethod(bool, GetPortValueBool, int, int);
	MonoMethod(mono::object, GetPortValueVec3, int, int);

	MonoMethod(void, ActivateOutput, int, int);
	MonoMethod(void, ActivateOutputInt, int, int, int);
	MonoMethod(void, ActivateOutputFloat, int, int, float);
	MonoMethod(void, ActivateOutputEntityId, int, int, EntityId);
	MonoMethod(void, ActivateOutputString, int, int, mono::string);
	MonoMethod(void, ActivateOutputBool, int, int, bool);
	MonoMethod(void, ActivateOutputVec3, int, int, Vec3);

	static TFlowNodes m_nodes;

	CEntityFlowManager *m_pEntityFlowManager;

	int m_refs;
};

#endif //__FLOW_MANAGER__