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
	: m_pScript(NULL)
	, m_pActInfo(pActInfo)
	, m_cloneType(eNCT_Instanced)
	, m_pNodeType(NULL)
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
		m_pNodeType = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager()->GetNodeType(gEnv->pFlowSystem->GetTypeName(typeId));

		IMonoObject *pScript = gEnv->pMonoScriptSystem->InstantiateScript(m_pNodeType->GetScriptName(), m_pNodeType->IsEntityNode() ? eScriptFlag_Entity : eScriptFlag_FlowNode);

		IMonoObject *pNodeInfo = NULL;
		if(IMonoAssembly *pCryBraryAssembly = gEnv->pMonoScriptSystem->GetCryBraryAssembly())
		{
			if(IMonoClass *pClass = pCryBraryAssembly->GetClass("NodeInfo"))
			{
				IMonoArray *pArgs = CreateMonoArray(3);
				pArgs->InsertNativePointer(this);
				pArgs->Insert(id);
				pArgs->Insert(m_pActInfo->pGraph->GetGraphId());

				pNodeInfo = pClass->CreateInstance(pArgs);
			}
		}

		CallMonoScript<void>(pScript, "InternalInitialize", pNodeInfo);

		m_pScript = pScript;

		return pScript != NULL;
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

	return NULL;
}

IEntity *CFlowNode::GetTargetEntity()
{
	return m_pActInfo->pEntity;
}

void CFlowNode::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{	
	m_pActInfo = pActInfo;

	if(m_pHookedGraph && m_pScript != NULL)
	{
		m_pHookedGraph->UnregisterHook(this);
		m_pHookedGraph = NULL;
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
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i);
						}
						break;
					case eFDT_Int:
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i, CFlowBaseNodeInternal::GetPortInt(pActInfo, i));
						}
						break;
					case eFDT_Float:
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i, CFlowBaseNodeInternal::GetPortFloat(pActInfo, i));
						}
						break;
					case eFDT_EntityId:
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i, CFlowBaseNodeInternal::GetPortEntityId(pActInfo, i));
						}
						break;
					case eFDT_Vec3:
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i, CFlowBaseNodeInternal::GetPortVec3(pActInfo, i));
						}
						break;
					case eFDT_String:
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i, CFlowBaseNodeInternal::GetPortString(pActInfo, i));
						}
						break;
					case eFDT_Bool:
						{
							CallMonoScript<void>(m_pScript, "OnPortActivated", i, CFlowBaseNodeInternal::GetPortBool(pActInfo, i));
						}
						break;
					default:
						break;
					}
				}
			}

		}
		break;
	case eFE_Initialize:
		{
			CallMonoScript<void>(m_pScript, "OnInit");
		}
		break;
	case eFE_SetEntityId:
		{
			if(m_pNodeType->IsEntityNode())
			{
				IMonoScript *pEntityScript = NULL;
				EntityId entId = pActInfo->pGraph->GetEntityId(pActInfo->myID);
				if(pActInfo && entId)
				{
					if(IGameObject *pGameObject = gEnv->pGameFramework->GetGameObject(entId))
					{
						if(CEntity *pEntity = static_cast<CEntity *>(pGameObject->QueryExtension("MonoEntity")))
						{
							SAFE_RELEASE(m_pScript);

							m_pScript = pEntity->GetScript();
						}
					}
				}
			}
		}
		break;
	}
}

void CFlowNode::GetConfiguration(SFlowNodeConfig &config)
{
	if(!m_pScript)
	{
		MonoWarning("CFlowNode::GetConfiguration: m_pScript was NULL!");
		return;
	}

	if(IMonoObject *pResult = m_pScript->CallMethod("GetNodeConfig"))
	{
		SMonoNodeConfig monoConfig = pResult->Unbox<SMonoNodeConfig>();

		config.pInputPorts = m_pNodeType->GetInputPorts(m_pScript);
		config.pOutputPorts = m_pNodeType->GetOutputPorts(m_pScript);

		config.nFlags |= monoConfig.flags;
		config.sDescription = _HELP(ToCryString(monoConfig.description));
		config.SetCategory(monoConfig.category);
	}
}