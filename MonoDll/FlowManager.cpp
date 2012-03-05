#include "StdAfx.h"
#include "FlowManager.h"

#include "MonoFlowNode.h"

#include "MonoCommon.h"
#include <IMonoArray.h>
#include <IMonoAssembly.h>

#include "MonoScriptSystem.h"
#include <IGameFramework.h>

CFlowManager::TFlowNodes CFlowManager::m_nodes = TFlowNodes();

CFlowManager::CFlowManager()
	: m_refs(0)
{
	REGISTER_EXPOSED_METHOD(RegisterNode);
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

	m_pEntityFlowManager = new CEntityFlowManager();
}

void CFlowManager::Reset()
{
	for each(auto node in m_nodes)
		node.second->ReloadPorts();
}

void CFlowManager::RegisterNode(mono::string name, mono::string category, bool isEntity)
{
	if(IFlowSystem *pFlowSystem = gEnv->pFlowSystem)
	{
		CFlowManager *pFlowManager = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager();

		pFlowSystem->RegisterType(ToCryString(category) + (string)":" + ToCryString(name), isEntity ? pFlowManager->GetEntityFlowManager() : (IFlowNodeFactoryPtr)pFlowManager);
	}
}

void CFlowManager::UnregisterFlowNode(int id)
{
	for(TFlowNodes::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		if((*it).first==id)
			m_nodes.erase(it);
	}
}

IFlowNodePtr CEntityFlowManager::Create(IFlowNode::SActivationInfo *pActInfo)
{
	return new CFlowNode(pActInfo, true);
}

IFlowNodePtr CFlowManager::Create(IFlowNode::SActivationInfo *pActInfo)
{
	return new CFlowNode(pActInfo, false);
}

SNodeData *CFlowManager::GetNodeDataById(int scriptId)
{
	for each(auto node in m_nodes)
	{
		if(node.first==scriptId)
			return node.second;
	}

	return NULL;
}

CFlowNode *CFlowManager::GetNodeById(int scriptId)
{
	if(SNodeData *pData = GetNodeDataById(scriptId))
		return pData->pNode;

	return NULL;
}

void CFlowManager::ActivateOutput(int scriptId, int index) { ActivateOutputOnNode(scriptId, index, 0); }
void CFlowManager::ActivateOutputInt(int scriptId, int index, int value) { ActivateOutputOnNode(scriptId, index, value); }
void CFlowManager::ActivateOutputFloat(int scriptId, int index, float value) { ActivateOutputOnNode(scriptId, index, value); }
void CFlowManager::ActivateOutputEntityId(int scriptId, int index, EntityId value) { ActivateOutputOnNode(scriptId, index, value); }
void CFlowManager::ActivateOutputString(int scriptId, int index, mono::string value) { ActivateOutputOnNode(scriptId, index, (string)ToCryString(value)); }
void CFlowManager::ActivateOutputBool(int scriptId, int index, bool value) { ActivateOutputOnNode(scriptId, index, value); }
void CFlowManager::ActivateOutputVec3(int scriptId, int index, Vec3 value) { ActivateOutputOnNode(scriptId, index, value); }

template <class T>
void CFlowManager::ActivateOutputOnNode(int scriptId, int index, const T &value)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		pFlowNode->ActivateOutput(index, value);
}

bool CFlowManager::IsPortActive(int scriptId, int port)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->IsPortActive(port);

	return false;
}

int CFlowManager::GetPortValueInt(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortInt(index);

	return -1;
}

float CFlowManager::GetPortValueFloat(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortFloat(index);

	return -1;
}

EntityId CFlowManager::GetPortValueEntityId(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortEntityId(index);

	return -1;
}

mono::string CFlowManager::GetPortValueString(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return (mono::string)ToMonoString(pFlowNode->GetPortString(index));

	return NULL;
}

bool CFlowManager::GetPortValueBool(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortBool(index);

	return false;
}

mono::object CFlowManager::GetPortValueVec3(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return *gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(eCMT_Vec3, pFlowNode->GetPortVec3(index));

	return NULL;
}

SInputPortConfig *SNodeData::pInputs = nullptr;
SOutputPortConfig *SNodeData::pOutputs = nullptr;

static const int maxPortCount = 20;

#define GetInput(index) ((pInputPorts->GetSize() > index) ? pInputPorts->GetItem(index)->Unbox<SMonoInputPortConfig>().Convert() : nullConfig)
#define GetOutput(index) ((pOutputPorts->GetSize() > index) ? pOutputPorts->GetItem(index)->Unbox<SMonoOutputPortConfig>().Convert() : nullOutputConfig)
void SNodeData::ReloadPorts()
{
	SMonoNodePortConfig monoConfig = CallMonoScript<SMonoNodePortConfig>(pNode->GetScriptId(), "GetPortConfig");

	SInputPortConfig nullConfig = {0};
	SOutputPortConfig nullOutputConfig = {0};

	CScriptArray *pInputPorts = new CScriptArray(monoConfig.inputs);

	static SInputPortConfig inputs[maxPortCount];

	pInputs = inputs;

	for(int i = 0; i < pInputPorts->GetSize(); i++)
		pInputs[i] = pInputPorts->GetItem(i)->Unbox<SMonoInputPortConfig>().Convert();

	for(int i = 0; i < maxPortCount; i++)
	{
		if(i >= pInputPorts->GetSize())
			pInputs[i] = nullConfig;
	}

	// Convert MonoArray type to our custom CScriptArray for easier handling.
	CScriptArray *pOutputPorts = new CScriptArray(monoConfig.outputs);

	static SOutputPortConfig outputs[maxPortCount];

	pOutputs = outputs;

	for(int i = 0; i < pOutputPorts->GetSize(); i++)
		pOutputs[i] = pOutputPorts->GetItem(i)->Unbox<SMonoOutputPortConfig>().Convert();

	for(int i = 0; i < maxPortCount; i++)
	{
		if(i >= pOutputPorts->GetSize())
			pOutputs[i] = nullOutputConfig;
	}
}