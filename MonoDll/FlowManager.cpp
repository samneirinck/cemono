#include "StdAfx.h"
#include "FlowManager.h"

#include "MonoFlowNode.h"

#include "MonoCommon.h"
#include <IMonoArray.h>
#include <IMonoAssembly.h>

#include "MonoScriptSystem.h"
#include <IGameFramework.h>

CFlowManager::CFlowManager()
	: m_refs(0)
{
	REGISTER_METHOD(RegisterNode);
	REGISTER_METHOD(GetNode);

	REGISTER_METHOD(IsPortActive);

	REGISTER_METHOD(ActivateOutput);
	REGISTER_METHOD(ActivateOutputInt);
	REGISTER_METHOD(ActivateOutputFloat);
	REGISTER_METHOD(ActivateOutputEntityId);
	REGISTER_METHOD(ActivateOutputString);
	REGISTER_METHOD(ActivateOutputBool);
	REGISTER_METHOD(ActivateOutputVec3);

	REGISTER_METHOD(GetPortValueInt);
	REGISTER_METHOD(GetPortValueFloat);
	REGISTER_METHOD(GetPortValueEntityId);
	REGISTER_METHOD(GetPortValueString);
	REGISTER_METHOD(GetPortValueBool);
	REGISTER_METHOD(GetPortValueVec3);

	REGISTER_METHOD(GetTargetEntity);
}

void CFlowManager::Reset()
{
	//gEnv->pFlowSystem->ReloadAllNodeTypes();
}

void CFlowManager::RegisterNode(mono::string monoTypeName)
{
	IFlowSystem *pFlowSystem = gEnv->pGameFramework->GetIFlowSystem();
	if(!pFlowSystem)
		return;

	CFlowManager *pFlowManager = g_pScriptSystem->GetFlowManager();

	pFlowSystem->RegisterType(ToCryString(monoTypeName), (IFlowNodeFactoryPtr)pFlowManager);
}

IFlowNodePtr CFlowManager::Create(IFlowNode::SActivationInfo *pActInfo)
{
	return new CFlowNode(pActInfo);
}

// Used after serialization to get the valid flownode pointer.
IFlowNode *CFlowManager::GetNode(TFlowGraphId graphId, TFlowNodeId id)
{
	if(IFlowGraph *pGraph = gEnv->pFlowSystem->GetGraphById(graphId))
	{
		if(IFlowNodeData *pNodeData = pGraph->GetNodeData(id))
			return pNodeData->GetNode();
	}

	return nullptr;
}

void CFlowManager::ActivateOutput(CFlowNode *pNode, int index) { pNode->ActivateOutput(index, 0); }
void CFlowManager::ActivateOutputInt(CFlowNode *pNode, int index, int value) { pNode->ActivateOutput(index, value); }
void CFlowManager::ActivateOutputFloat(CFlowNode *pNode, int index, float value) { pNode->ActivateOutput(index, value); }
void CFlowManager::ActivateOutputEntityId(CFlowNode *pNode, int index, EntityId value) { pNode->ActivateOutput(index, value); }
void CFlowManager::ActivateOutputString(CFlowNode *pNode, int index, mono::string value) { pNode->ActivateOutput(index, string(ToCryString(value))); }
void CFlowManager::ActivateOutputBool(CFlowNode *pNode, int index, bool value) { pNode->ActivateOutput(index, value); }
void CFlowManager::ActivateOutputVec3(CFlowNode *pNode, int index, Vec3 value) { pNode->ActivateOutput(index, value); }

bool CFlowManager::IsPortActive(CFlowNode *pFlowNode, int port)
{
	return pFlowNode->IsPortActive(port);
}

int CFlowManager::GetPortValueInt(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortInt(index);
}

float CFlowManager::GetPortValueFloat(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortFloat(index);
}

EntityId CFlowManager::GetPortValueEntityId(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortEntityId(index);
}

mono::string CFlowManager::GetPortValueString(CFlowNode *pFlowNode, int index)
{
	return (mono::string)ToMonoString(pFlowNode->GetPortString(index));
}

bool CFlowManager::GetPortValueBool(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortBool(index);
}

Vec3 CFlowManager::GetPortValueVec3(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortVec3(index);
}

IEntity *CFlowManager::GetTargetEntity(CFlowNode *pNode, EntityId &id)
{
	if(IEntity *pEntity = pNode->GetTargetEntity())
	{
		id = pEntity->GetId();

		return pEntity;
	}

	MonoWarning("CFlowManager::GetTargetEntity returning nullptr target entity!");
	return nullptr;
}

/*
static const int MAX_NODE_PORT_COUNT = 20;
void SNodeType::ReloadPorts(IMonoObject *pScript)
{
	if(IMonoObject *pResult = pScript->CallMethod("GetPortConfig"))
	{
		auto monoConfig = pResult->Unbox<SMonoNodePortConfig>();

		SInputPortConfig nullptrConfig = {0};
		SOutputPortConfig nullptrOutputConfig = {0};

		IMonoArray *pInputPorts = *monoConfig.inputs;

		pInputs = new SInputPortConfig[MAX_NODE_PORT_COUNT];

		for(int i = 0; i < pInputPorts->GetSize(); i++)
			pInputs[i] = pInputPorts->GetItem(i)->Unbox<SMonoInputPortConfig>().Convert();

		for(int i = pInputPorts->GetSize(); i < MAX_NODE_PORT_COUNT; i++)
			pInputs[i] = nullptrConfig;

		SAFE_RELEASE(pInputPorts);

		// Convert MonoArray type to our custom CScriptArray for easier handling.
		IMonoArray *pOutputPorts = *monoConfig.outputs;

		pOutputs = new SOutputPortConfig[MAX_NODE_PORT_COUNT];

		for(int i = 0; i < pOutputPorts->GetSize(); i++)
			pOutputs[i] = pOutputPorts->GetItem(i)->Unbox<SMonoOutputPortConfig>().Convert();

		for(int i = pOutputPorts->GetSize(); i < MAX_NODE_PORT_COUNT; i++)
			pOutputs[i] = nullptrOutputConfig;

		SAFE_RELEASE(pOutputPorts);
	}
}*/