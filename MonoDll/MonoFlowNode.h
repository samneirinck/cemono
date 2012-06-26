/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Flow node class used to forward events to Mono scripts.
//////////////////////////////////////////////////////////////////////////
// 23/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_FLOWBASENODE_H__
#define __MONO_FLOWBASENODE_H__

#include "MonoCommon.h"
#include <IMonoObject.h>
#include <IMonoArray.h>

#include "FlowManager.h"

#include <IMonoScriptSystem.h>
#include <IFlowSystem.h>

#include "FlowBaseNode.h"

struct IMonoArray;

class CFlowNode 
	: public CFlowBaseNodeInternal
	, public IFlowGraphHook
{
public:
	enum EInputPorts
	{
		EIP_Start,
		EIP_Cancel
	};

	enum EOutputPorts
	{
		EOP_Started,
		EOP_Cancelled
	};

	CFlowNode(SActivationInfo *pActInfo);
	virtual ~CFlowNode();

	// IFlowNode
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo) override;
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo) override;

	virtual void GetMemoryUsage(ICrySizer * s) const override { s->Add(*this); }
	virtual void GetConfiguration(SFlowNodeConfig&) override;
	// ~IFlowNode

	// IFlowGraphHook
	virtual IFlowNodePtr CreateNode(IFlowNode::SActivationInfo*, TFlowNodeTypeId typeId) override { return nullptr; }
	virtual bool CreatedNode(TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) override;
	virtual void CancelCreatedNode(TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) override {}

	virtual void AddRef() override { CFlowBaseNodeInternal::AddRef(); }
	virtual void Release() override { CFlowBaseNodeInternal::Release(); }

	virtual IFlowGraphHook::EActivation PerformActivation(IFlowGraphPtr pFlowgraph, TFlowNodeId srcNode, TFlowPortId srcPort, TFlowNodeId toNode, TFlowPortId toPort, const TFlowInputData* value) override { return eFGH_Pass; }
	// ~IFlowGraphHook

	TFlowNodeId GetId() const { return m_pActInfo->myID; }
	IEntity *GetTargetEntity();

	IMonoObject *GetScript() const { return m_pScript; }
	void SetScript(IMonoObject *pScript) { m_pScript = pScript; }

	inline void SetRegularlyUpdated(bool update) { m_pActInfo->pGraph->SetRegularlyUpdated(m_pActInfo->myID, update); }

	template <class T>
	void ActivateOutput(int nPort, const T &value) { return CFlowBaseNodeInternal::ActivateOutput<T>(m_pActInfo, nPort, value); }

	bool IsPortActive(int nPort) { return CFlowBaseNodeInternal::IsPortActive(m_pActInfo, nPort); }

	bool GetPortBool(int nPort) { return CFlowBaseNodeInternal::GetPortBool(m_pActInfo, nPort); }
	int GetPortInt(int nPort) { return CFlowBaseNodeInternal::GetPortInt(m_pActInfo, nPort); }
	float GetPortFloat(int nPort) { return CFlowBaseNodeInternal::GetPortFloat(m_pActInfo, nPort); }
	Vec3 GetPortVec3(int nPort) { return CFlowBaseNodeInternal::GetPortVec3(m_pActInfo, nPort); }
	EntityId GetPortEntityId(int nPort) { return CFlowBaseNodeInternal::GetPortEntityId(m_pActInfo, nPort); }
	string GetPortString(int nPort) { return CFlowBaseNodeInternal::GetPortString(m_pActInfo, nPort); }

private:

	SActivationInfo *m_pActInfo;
	IFlowGraph *m_pHookedGraph;

	IMonoObject *m_pScript;
	std::shared_ptr<SNodeType> m_pNodeType;

	ENodeCloneType m_cloneType;
};

struct SMonoInputPortConfig
{
	mono::string name;
	mono::string humanName;
	mono::string description;

	EFlowDataTypes type;

	mono::string uiConfig;

	mono::object defaultValue;

	SInputPortConfig Convert()
	{
		const char *sName = ToCryString(name);
		const char *sDesc = description ? ToCryString(description) : nullptr;
		const char *sHumanName = humanName ? ToCryString(humanName) : nullptr;
		const char *sUIConfig = uiConfig ? ToCryString(uiConfig) : nullptr;

		IMonoObject *pObject = nullptr;
		if(defaultValue != 0)
			pObject = *defaultValue;

		switch(type)
		{
		case eFDT_Void:
			return InputPortConfig_Void(sName, _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		case eFDT_Int:
			return InputPortConfig<int>(sName, pObject ? pObject->Unbox<int>() : 0, _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		case eFDT_Float:
			return InputPortConfig<float>(sName, pObject ? pObject->Unbox<float>() : 0, _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		case eFDT_EntityId: 
			return InputPortConfig<EntityId>(sName, pObject ? pObject->Unbox<EntityId>() : 0, _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		case eFDT_Vec3:
			return InputPortConfig<Vec3>(sName, pObject ? pObject->Unbox<Vec3>() : Vec3(0,0,0), _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		case eFDT_String:
			return InputPortConfig<string>(sName, defaultValue ? ToCryString((mono::string)defaultValue) : "", _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		case eFDT_Bool:
			return InputPortConfig<bool>(sName, pObject ? pObject->Unbox<bool>() : false, _HELP(sDesc), sHumanName, _UICONFIG(sUIConfig));
		}

		return *(SInputPortConfig *)0;
	}
};

struct SMonoOutputPortConfig
{
	mono::string name;
	mono::string humanName;
	mono::string description;

	EFlowDataTypes type;

	SOutputPortConfig Convert()
	{
		const char *sName = ToCryString(name);
		const char *sHumanName = ToCryString(humanName);
		const char *sDesc = ToCryString(description);

		switch(type)
		{
		case eFDT_Void:
			return OutputPortConfig_Void(sName, _HELP(sDesc), sHumanName);
		case eFDT_Int:
			return OutputPortConfig<int>(sName, _HELP(sDesc), sHumanName);
		case eFDT_Float:
			return OutputPortConfig<float>(sName, _HELP(sDesc), sHumanName);
		case eFDT_EntityId:
			return OutputPortConfig<EntityId>(sName, _HELP(sDesc), sHumanName);
		case eFDT_Vec3:
			return OutputPortConfig<Vec3>(sName, _HELP(sDesc), sHumanName);
		case eFDT_String:
			return OutputPortConfig<string>(sName, _HELP(sDesc), sHumanName);
		case eFDT_Bool:
			return OutputPortConfig<bool>(sName, _HELP(sDesc), sHumanName);
		}

		return *(SOutputPortConfig *)0;
	}
};


struct SMonoNodeConfig
{
	EFlowNodeFlags flags;
	EFlowNodeFlags category;

	mono::string description;
};

struct SMonoNodePortConfig
{
	mono::object inputs;
	mono::object outputs;
};

#endif // __MONO_FLOWBASENODE_H__