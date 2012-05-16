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
		IMonoClass *pScript = gEnv->pMonoScriptSystem->InstantiateScript(nodeType->GetScriptName());
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
			IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->InstantiateScript(nodeType->GetScriptName());

			IMonoClass *pNodeInfo = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("NodeInfo");
			CallMonoScript<void>(pScriptClass, "InternalInitialize", gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(pNodeInfo, &SMonoNodeInfo(pNode)));

			pNode->SetScript(pScriptClass);

			return nodeType;
		}
	}

	return NULL;
}

void CFlowManager::ActivateOutput(CFlowNode *pNode, int index) { ActivateOutputOnNode(pNode, index, 0); }
void CFlowManager::ActivateOutputInt(CFlowNode *pNode, int index, int value) { ActivateOutputOnNode(pNode, index, value); }
void CFlowManager::ActivateOutputFloat(CFlowNode *pNode, int index, float value) { ActivateOutputOnNode(pNode, index, value); }
void CFlowManager::ActivateOutputEntityId(CFlowNode *pNode, int index, EntityId value) { ActivateOutputOnNode(pNode, index, value); }
void CFlowManager::ActivateOutputString(CFlowNode *pNode, int index, mono::string value) { ActivateOutputOnNode(pNode, index, (string)ToCryString(value)); }
void CFlowManager::ActivateOutputBool(CFlowNode *pNode, int index, bool value) { ActivateOutputOnNode(pNode, index, value); }
void CFlowManager::ActivateOutputVec3(CFlowNode *pNode, int index, Vec3 value) { ActivateOutputOnNode(pNode, index, value); }

template <class T>
void CFlowManager::ActivateOutputOnNode(CFlowNode *pFlowNode, int index, const T &value)
{
	pFlowNode->ActivateOutput(pFlowNode->GetActivationInfo(), index, value);
}

bool CFlowManager::IsPortActive(CFlowNode *pFlowNode, int port)
{
	return pFlowNode->IsPortActive(pFlowNode->GetActivationInfo(), port);
}

int CFlowManager::GetPortValueInt(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortInt(pFlowNode->GetActivationInfo(), index);
}

float CFlowManager::GetPortValueFloat(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortFloat(pFlowNode->GetActivationInfo(), index);
}

EntityId CFlowManager::GetPortValueEntityId(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortEntityId(pFlowNode->GetActivationInfo(), index);
}

mono::string CFlowManager::GetPortValueString(CFlowNode *pFlowNode, int index)
{
	return (mono::string)ToMonoString(pFlowNode->GetPortString(pFlowNode->GetActivationInfo(), index));
}

bool CFlowManager::GetPortValueBool(CFlowNode *pFlowNode, int index)
{
	return pFlowNode->GetPortBool(pFlowNode->GetActivationInfo(), index);
}

mono::object CFlowManager::GetPortValueVec3(CFlowNode *pFlowNode, int index)
{
	return *gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(eCMT_Vec3, pFlowNode->GetPortVec3(pFlowNode->GetActivationInfo(), index));
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