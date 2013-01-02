#include "StdAfx.h"
#include "MonoFlowNode.h"

#include "MonoScriptSystem.h"
#include "FlowManager.h"

#include "MonoEntity.h"

#include <IGameFramework.h>

#include <MonoCommon.h>
#include <IMonoObject.h>
#include <IMonoArray.h>
#include <IMonoClass.h>

CFlowNode::CFlowNode(SActivationInfo *pActInfo)
	: m_pScript(nullptr)
	, m_pActInfo(pActInfo)
	, m_cloneType(eNCT_Instanced)
{
	// We *have* to get the id right away or inputs won't work, so lets use this fugly solution.
	pActInfo->pGraph->RegisterHook(this);
	// Keep it so we can unregister it after we've got what we came for.
	m_pHookedGraph = pActInfo->pGraph;
}

CFlowNode::~CFlowNode()
{
	SAFE_RELEASE(m_pScript);
}

bool CFlowNode::CreatedNode(TFlowNodeId id, const char *name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) 
{ 
	if(pNode==this)
	{
		IMonoObject *pScript = g_pScriptSystem->InstantiateScript(gEnv->pFlowSystem->GetTypeName(typeId), eScriptFlag_FlowNode);

		IMonoClass *pNodeInfo = g_pScriptSystem->GetCryBraryAssembly()->GetClass("NodeInfo", "CryEngine.FlowSystem.Native");
		pScript->CallMethod("InternalInitialize", pNodeInfo->BoxObject(&SMonoNodeInfo(this, id, m_pActInfo->pGraph->GetGraphId())));

		m_pScript = pScript;

		return pScript != nullptr;
	}

	return true; 
}

IFlowNodePtr CFlowNode::Clone(SActivationInfo *pActInfo)
{
	switch(m_cloneType)
	{
	case eNCT_Singleton:
		return this;
	case eNCT_Instanced:
		return new CFlowNode(pActInfo);
	default:
		break;
	}

	return nullptr;
}

IEntity *CFlowNode::GetTargetEntity()
{
	return m_pActInfo->pEntity;
}

void CFlowNode::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{	
	m_pActInfo = pActInfo;

	if(m_pHookedGraph && m_pScript != nullptr)
	{
		m_pHookedGraph->UnregisterHook(this);
		m_pHookedGraph = nullptr;
	}

	switch(event)
	{
	case eFE_Activate:
		{
			IFlowNodeData *pNodeData = pActInfo->pGraph->GetNodeData(pActInfo->myID);
			if(!pNodeData)
				return;

			for(int i = 0; i < pNodeData->GetNumInputPorts(); i++)
			{
				if(IsPortActive(i))
				{
					switch(GetPortType(pActInfo, i))
					{
					case eFDT_Void:
						m_pScript->CallMethod("OnPortActivated", i);
						break;
					case eFDT_Int:
						m_pScript->CallMethod("OnPortActivated", i, CFlowBaseNodeInternal::GetPortInt(pActInfo, i));
						break;
					case eFDT_Float:
						m_pScript->CallMethod("OnPortActivated", i, CFlowBaseNodeInternal::GetPortFloat(pActInfo, i));
						break;
					case eFDT_EntityId:
						m_pScript->CallMethod("OnPortActivated", i, CFlowBaseNodeInternal::GetPortEntityId(pActInfo, i));
						break;
					case eFDT_Vec3:
						m_pScript->CallMethod("OnPortActivated", i, CFlowBaseNodeInternal::GetPortVec3(pActInfo, i));
						break;
					case eFDT_String:
						m_pScript->CallMethod("OnPortActivated", i, CFlowBaseNodeInternal::GetPortString(pActInfo, i));
						break;
					case eFDT_Bool:
						m_pScript->CallMethod("OnPortActivated", i, CFlowBaseNodeInternal::GetPortBool(pActInfo, i));
						break;
					default:
						break;
					}
				}
			}

		}
		break;
	case eFE_Initialize:
		m_pScript->CallMethod("OnInit");
		break;
	case eFE_SetEntityId:
		{
			EntityId id = pActInfo->pGraph->GetEntityId(pActInfo->myID);

			void *params[2];
			params[0] = gEnv->pEntitySystem->GetEntity(id);
			params[1] = &id;

			m_pScript->GetClass()->Invoke(m_pScript, "InternalSetTargetEntity", params, 2);
		}
		break;
	}
}

void CFlowNode::GetConfiguration(SFlowNodeConfig &config)
{
	CRY_ASSERT(m_pScript);

	if(IMonoObject *pResult = m_pScript->CallMethod("GetNodeConfig"))
	{
		SMonoNodeConfig monoConfig = pResult->Unbox<SMonoNodeConfig>();

		config.nFlags |= monoConfig.flags;
		config.sDescription = _HELP(ToCryString(monoConfig.description));
		config.SetCategory(monoConfig.category);

		m_cloneType = monoConfig.cloneType;

		// Ports
		static const int MAX_NODE_PORT_COUNT = 20;

		SInputPortConfig nullptrConfig = {0};
		SOutputPortConfig nullptrOutputConfig = {0};

		IMonoArray *pInputPorts = *monoConfig.inputs;

		auto pInputs = new SInputPortConfig[MAX_NODE_PORT_COUNT];

		for(int i = 0; i < pInputPorts->GetSize(); i++)
			pInputs[i] = pInputPorts->GetItem(i)->Unbox<SMonoInputPortConfig>().Convert();

		for(int i = pInputPorts->GetSize(); i < MAX_NODE_PORT_COUNT; i++)
			pInputs[i] = nullptrConfig;

		config.pInputPorts = pInputs;

		SAFE_RELEASE(pInputPorts);

		// Convert MonoArray type to our custom CScriptArray for easier handling.
		IMonoArray *pOutputPorts = *monoConfig.outputs;

		auto pOutputs = new SOutputPortConfig[MAX_NODE_PORT_COUNT];

		for(int i = 0; i < pOutputPorts->GetSize(); i++)
			pOutputs[i] = pOutputPorts->GetItem(i)->Unbox<SMonoOutputPortConfig>().Convert();

		for(int i = pOutputPorts->GetSize(); i < MAX_NODE_PORT_COUNT; i++)
			pOutputs[i] = nullptrOutputConfig;

		config.pOutputPorts = pOutputs;

		SAFE_RELEASE(pOutputPorts);
	}
}