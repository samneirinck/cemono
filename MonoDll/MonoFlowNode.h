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
	virtual IFlowNodePtr CreateNode(IFlowNode::SActivationInfo*, TFlowNodeTypeId typeId) override { return NULL; }
	virtual bool CreatedNode(TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) override;
	virtual void CancelCreatedNode(TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) override {}

	virtual void AddRef() override { CFlowBaseNodeInternal::AddRef(); }
	virtual void Release() override { delete this; }

	virtual IFlowGraphHook::EActivation PerformActivation(IFlowGraphPtr pFlowgraph, TFlowNodeId srcNode, TFlowPortId srcPort, TFlowNodeId toNode, TFlowPortId toPort, const TFlowInputData* value) override { return eFGH_Pass; }
	// ~IFlowGraphHook

	SActivationInfo *GetActivationInfo() { return m_pActInfo; }
	TFlowNodeId GetId() const { return m_pActInfo->myID; }

	IMonoClass *GetScript() const { return m_pScriptClass; }
	void SetScript(IMonoClass *pScript) { m_pScriptClass = pScript; }

	inline void SetRegularlyUpdated(bool update) { m_pActInfo->pGraph->SetRegularlyUpdated(m_pActInfo->myID, update); }

private:

	SActivationInfo *m_pActInfo;
	IFlowGraph *m_pHookedGraph;

	IMonoClass *m_pScriptClass;
	std::shared_ptr<SNodeType> m_pNodeType;

	ENodeCloneType m_cloneType;
};

template <class T>
ILINE SInputPortConfig MonoInputPortConfig(mono::string name, mono::object defaultVal, mono::string humanName, mono::string desc, mono::string UIConfig)
{
	IMonoObject *pObject = *defaultVal;
	SInputPortConfig config = InputPortConfig<T>(ToCryString(name), pObject->Unbox<T>(), ToCryString(humanName), _HELP(ToCryString(desc)), ToCryString(UIConfig));

	SAFE_RELEASE(pObject);
	return config;
}

struct SMonoInputPortConfig
{
	SInputPortConfig Convert()
	{
		switch(type)
		{
		case eFDT_Void:
			{
				return InputPortConfig_Void(ToCryString(name), _HELP(ToCryString(description)), ToCryString(humanName), ToCryString(uiConfig));
			}
			break;
		case eFDT_Int:
			{
				return MonoInputPortConfig<int>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_Float:
			{
				return MonoInputPortConfig<float>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_EntityId:
			{
				return MonoInputPortConfig<EntityId>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_Vec3:
			{
				return MonoInputPortConfig<Vec3>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_String:
			{
				return InputPortConfig<string>(ToCryString(name), ToCryString((mono::string)defaultValue), _HELP(ToCryString(description)), ToCryString(humanName), ToCryString(uiConfig));
			}
			break;
		case eFDT_Bool:
			{
				return MonoInputPortConfig<bool>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		}

		return *(SInputPortConfig *)0;
	}
	mono::string name;
	mono::string humanName;
	mono::string description;

	EFlowDataTypes type;

	mono::string uiConfig;

	mono::object defaultValue;
};

struct SMonoOutputPortConfig
{
	SOutputPortConfig Convert()
	{
		const char *sName = ToCryString(name);
		const char *sHumanName = ToCryString(humanName);
		const char *sDesc = ToCryString(description);

		switch(type)
		{
		case eFDT_Void:
			{
				return OutputPortConfig_Void(sName, _HELP(sDesc), sHumanName);
			}
		case eFDT_Int:
			{
				return OutputPortConfig<int>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_Float:
			{
				return OutputPortConfig<float>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_EntityId:
			{
				return OutputPortConfig<EntityId>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_Vec3:
			{
				return OutputPortConfig<Vec3>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_String:
			{
				return OutputPortConfig<string>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_Bool:
			{
				return OutputPortConfig<bool>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		}

		return *(SOutputPortConfig *)0;
	}

	mono::string name;
	mono::string humanName;
	mono::string description;

	EFlowDataTypes type;
};


struct SMonoNodeConfig
{
	EFlowNodeFlags flags;
	EFlowNodeFlags category;

	mono::string description;
};

struct SMonoNodePortConfig
{
	mono::array inputs;
	mono::array outputs;
};

#endif // __MONO_FLOWBASENODE_H__