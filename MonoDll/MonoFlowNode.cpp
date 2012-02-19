#include "StdAfx.h"
#include "MonoFlowNode.h"

#include "MonoCommon.h"
#include "MonoArray.h"

#include <IMonoObject.h>

#include "MonoScriptSystem.h"
#include "FlowManager.h"

#include <IGameFramework.h>

CMonoFlowNode::CMonoFlowNode(SActivationInfo *pActInfo, bool isEntityClass)
	: m_scriptId(-1)
	, m_refs(0)
	, m_pActInfo(pActInfo)
	, m_bInitialized(false)
	, m_bEntityNode(isEntityClass)
{
	// We *have* to get the id right away or inputs won't work, so lets use this fugly solution.
	pActInfo->pGraph->RegisterHook(this);
	// Keep it so we can unregister it after we've got what we came for.
	m_pHookedGraph = pActInfo->pGraph;
}

CMonoFlowNode::~CMonoFlowNode()
{
	 gEnv->pMonoScriptSystem->RemoveScriptInstance(m_scriptId);

	 static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager()->UnregisterFlowNode(m_scriptId);
}

bool CMonoFlowNode::CreatedNode(TFlowNodeId id, const char *name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) 
{ 
	if(pNode==this)
		InstantiateScript(gEnv->pFlowSystem->GetTypeName(typeId));
	
	return true; 
}

bool CMonoFlowNode::InstantiateScript(const char *nodeName)
{
	string fullTypeName = nodeName;

	string typeName = "";
	int curPos = 0;

	string next = fullTypeName.Tokenize(":", curPos);
	while(!next.empty())
	{
	typeName = next;
		next = fullTypeName.Tokenize(":", curPos);
	}

	m_scriptId = gEnv->pMonoScriptSystem->InstantiateScript(EMonoScriptType_FlowNode, typeName);
	if(m_scriptId!=-1)
		static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager()->RegisterFlowNode(this, m_scriptId);

	return m_scriptId!=-1;
}

void CMonoFlowNode::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{	
	if(m_pHookedGraph && m_scriptId!=-1)
	{
		m_pHookedGraph->UnregisterHook(this);
		m_pHookedGraph = NULL;
	}

	m_pActInfo = pActInfo;

	switch(event)
	{
	case eFE_Activate:
		{
			IFlowNodeData *pNodeData = m_pActInfo->pGraph->GetNodeData(m_pActInfo->myID);
			if(!pNodeData)
				return;

			for(int i = 0; i < pNodeData->GetNumInputPorts(); i++)
			{
				if(IsPortActive(i))
				{
					switch(GetPortType(i))
					{
					case eFDT_Void:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i);
						}
						break;
					case eFDT_Int:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i, GetPortInt(i));
						}
						break;
					case eFDT_Float:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i, GetPortFloat(i));
						}
						break;
					case eFDT_EntityId:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i, GetPortEntityId(i));
						}
						break;
					case eFDT_Vec3:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i, GetPortVec3(i));
						}
						break;
					case eFDT_String:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i, GetPortString(i));
						}
						break;
					case eFDT_Bool:
						{
							CallMonoScript<void>(m_scriptId, "OnPortActivated", i, GetPortBool(i));
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
			CallMonoScript<void>(m_scriptId, "OnInitialized");
		}
		break;
	case eFE_SetEntityId:
		{
			if(m_bEntityNode)
			{
				int entityScriptId = gEnv->pMonoScriptSystem->GetEntityManager()->GetScriptId(m_pActInfo->pEntity->GetId());
				if(entityScriptId!=m_scriptId)
				{
					gEnv->pMonoScriptSystem->RemoveScriptInstance(m_scriptId);
					m_scriptId = entityScriptId;
				}
			}
		}
		break;
	}
}

void CMonoFlowNode::GetConfiguration(SFlowNodeConfig &config)
{
	SMonoNodeConfig monoConfig = CallMonoScript<SMonoNodeConfig>(m_scriptId, "GetNodeConfig");

	SNodeData *pNodeData = static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager()->GetNodeDataById(m_scriptId);

	config.nFlags |= monoConfig.flags;
	config.pInputPorts = pNodeData->pInputs;
	config.pOutputPorts = pNodeData->pOutputs;

	config.sDescription = _HELP(ToCryString(monoConfig.description));
	config.SetCategory(monoConfig.category);
}