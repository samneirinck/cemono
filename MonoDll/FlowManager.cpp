#include "StdAfx.h"
#include "FlowManager.h"

#include "MonoFlowNode.h"

#include "MonoCommon.h"
#include <IMonoArray.h>
#include <IMonoAssembly.h>

#include "MonoScriptSystem.h"
#include <IGameFramework.h>

CFlowManager::TFlowTypes CFlowManager::m_nodeTypes = TFlowTypes();

CFlowManager::CFlowManager()
	: m_refs(0)
{
	REGISTER_METHOD(RegisterNode);
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
}

void CFlowManager::Reset()
{
	for each(auto nodeType in m_nodeTypes)
	{
		IMonoClass *pScript = gEnv->pMonoScriptSystem->GetScriptById(gEnv->pMonoScriptSystem->InstantiateScript(nodeType->GetScriptName()));
		nodeType->ReloadPorts(pScript);
		SAFE_RELEASE(pScript);
	}
}

void CFlowManager::RegisterNode(mono::string monoTypeName)
{
	IFlowSystem *pFlowSystem = gEnv->pFlowSystem;
	if(!pFlowSystem)
	{
		CryLogAlways("[Warning] Failed to register node %s, gEnv->pFlowSystem was null!", ToCryString(monoTypeName));
		return;
	}

	CFlowManager *pFlowManager = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager();

	const char *typeName = ToCryString(monoTypeName);

	m_nodeTypes.push_back(std::shared_ptr<SNodeType>(new SNodeType(typeName)));
	pFlowSystem->RegisterType(typeName, (IFlowNodeFactoryPtr)pFlowManager);
}

void CFlowManager::UnregisterNode(CFlowNode *pNode)
{
	for each(auto nodeType in m_nodeTypes)
		nodeType->RemoveNode(pNode);
}

IFlowNodePtr CFlowManager::Create(IFlowNode::SActivationInfo *pActInfo)
{
	return new CFlowNode(pActInfo);
}

std::shared_ptr<SNodeType> CFlowManager::InstantiateNode(CFlowNode *pNode, const char *name)
{
	for each(auto nodeType in m_nodeTypes)
	{
		if(!strcmp(nodeType->GetTypeName(), name))
		{
			IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->GetScriptById(gEnv->pMonoScriptSystem->InstantiateScript(nodeType->GetScriptName()));
			pNode->SetScript(pScriptClass);
			nodeType->nodes.push_back(pNode);

			return nodeType;
		}
	}

	return NULL;
}

CFlowNode *CFlowManager::GetNodeById(int scriptId)
{
	for each(auto nodeType in m_nodeTypes)
	{
		for each(auto node in nodeType->nodes)
		{
			IMonoClass *pScriptClass = node->GetScript();
			if(!pScriptClass)
				continue;

			if(pScriptClass->GetScriptId() == scriptId)
				return node;
		}
	}

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
		pFlowNode->ActivateOutput(pFlowNode->GetActivationInfo(), index, value);
}

bool CFlowManager::IsPortActive(int scriptId, int port)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->IsPortActive(pFlowNode->GetActivationInfo(), port);

	return false;
}

int CFlowManager::GetPortValueInt(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortInt(pFlowNode->GetActivationInfo(), index);

	return -1;
}

float CFlowManager::GetPortValueFloat(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortFloat(pFlowNode->GetActivationInfo(), index);

	return -1;
}

EntityId CFlowManager::GetPortValueEntityId(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortEntityId(pFlowNode->GetActivationInfo(), index);

	return -1;
}

mono::string CFlowManager::GetPortValueString(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return (mono::string)ToMonoString(pFlowNode->GetPortString(pFlowNode->GetActivationInfo(), index));

	return NULL;
}

bool CFlowManager::GetPortValueBool(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return pFlowNode->GetPortBool(pFlowNode->GetActivationInfo(), index);

	return false;
}

mono::object CFlowManager::GetPortValueVec3(int scriptId, int index)
{
	if(CFlowNode *pFlowNode = GetNodeById(scriptId))
		return *gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(eCMT_Vec3, pFlowNode->GetPortVec3(pFlowNode->GetActivationInfo(), index));

	return NULL;
}

static const int MAX_NODE_PORT_COUNT = 20;
void SNodeType::ReloadPorts(IMonoClass *pScriptClass)
{
	if(IMonoObject *pResult = pScriptClass->CallMethod("GetPortConfig"))
	{
		auto monoConfig = pResult->Unbox<SMonoNodePortConfig>();

		SInputPortConfig nullConfig = {0};
		SOutputPortConfig nullOutputConfig = {0};

		IMonoArray *pInputPorts = *monoConfig.inputs;

		pInputs = new SInputPortConfig[MAX_NODE_PORT_COUNT];

		for(int i = 0; i < pInputPorts->GetSize(); i++)
			pInputs[i] = pInputPorts->GetItem(i)->Unbox<SMonoInputPortConfig>().Convert();

		for(int i = pInputPorts->GetSize(); i < MAX_NODE_PORT_COUNT; i++)
			pInputs[i] = nullConfig;

		SAFE_RELEASE(pInputPorts);

		// Convert MonoArray type to our custom CScriptArray for easier handling.
		IMonoArray *pOutputPorts = *monoConfig.outputs;

		pOutputs = new SOutputPortConfig[MAX_NODE_PORT_COUNT];

		for(int i = 0; i < pOutputPorts->GetSize(); i++)
			pOutputs[i] = pOutputPorts->GetItem(i)->Unbox<SMonoOutputPortConfig>().Convert();

		for(int i = pOutputPorts->GetSize(); i < MAX_NODE_PORT_COUNT; i++)
			pOutputs[i] = nullOutputConfig;

		SAFE_RELEASE(pOutputPorts);
	}
}