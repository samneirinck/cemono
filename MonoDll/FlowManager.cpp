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
}

void CFlowManager::Reset()
{
	for each(auto nodeType in m_nodeTypes)
	{
		IMonoClass *pScript = gEnv->pMonoScriptSystem->GetScriptById(gEnv->pMonoScriptSystem->InstantiateScript(nodeType->GetTypeName()));
		nodeType->ReloadPorts(pScript);
		SAFE_RELEASE(pScript);
	}
}

void CFlowManager::RegisterNode(mono::string monoTypeName)
{
	if(IFlowSystem *pFlowSystem = NULL)//gEnv->pFlowSystem)
	{
		CFlowManager *pFlowManager = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager();

		const char *typeName = ToCryString(monoTypeName);

		CryLogAlways("Registering node %s", typeName);
		m_nodeTypes.push_back(new SNodeType(typeName));
		pFlowSystem->RegisterType(typeName, (IFlowNodeFactoryPtr)pFlowManager);
	}
}

void CFlowManager::UnregisterNode(CFlowNode *pNode)
{
	CryLogAlways("hai.");

	for each(auto nodeType in m_nodeTypes)
		nodeType->RemoveNode(pNode);

	CryLogAlways("sup");
}

IFlowNodePtr CFlowManager::Create(IFlowNode::SActivationInfo *pActInfo)
{
	return new CFlowNode(pActInfo);
}

IMonoClass *CFlowManager::InstantiateNode(CFlowNode *pNode, const char *name)
{
	CryLogAlways("instantiating node %s", name);

	string nodeTypeName = "";
	string typeName = name;

	for each(auto nodeType in m_nodeTypes)
	{
		nodeTypeName = nodeType->GetTypeName();
		CryLogAlways("found node %s", nodeTypeName.c_str());

		if(typeName.find(nodeTypeName.find(":") == -1 ? ":" : "" + nodeTypeName))
		{
			IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->GetScriptById(gEnv->pMonoScriptSystem->InstantiateScript(typeName));
			nodeType->nodes.push_back(pNode);

			return pScriptClass;
		}
	}

	return NULL;
}

SNodeType *CFlowManager::GetNodeTypeById(int scriptId)
{
	for each(auto nodeType in m_nodeTypes)
	{
		for each(auto node in nodeType->nodes)
		{
			if(!node)
				continue;

			IMonoClass *pScriptClass = node->GetScript();
			if(!pScriptClass)
				continue;

			if(pScriptClass->GetScriptId() == scriptId)
				return nodeType;
		}
	}

	return NULL;
}

CFlowNode *CFlowManager::GetNodeById(int scriptId)
{
	for each(auto node in GetNodeTypeById(scriptId)->nodes)
	{
		if(node->GetScript()->GetScriptId() == scriptId)
			return node;
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

		IMonoArray *pInputPorts = new CScriptArray(monoConfig.inputs);

		static SInputPortConfig inputs[MAX_NODE_PORT_COUNT];

		pInputs = inputs;

		for(int i = 0; i < pInputPorts->GetSize(); i++)
			pInputs[i] = pInputPorts->GetItem(i)->Unbox<SMonoInputPortConfig>().Convert();

		for(int i = 0; i < MAX_NODE_PORT_COUNT; i++)
		{
			if(i >= pInputPorts->GetSize())
				pInputs[i] = nullConfig;
		}

		SAFE_RELEASE(pInputPorts);

		// Convert MonoArray type to our custom CScriptArray for easier handling.
		IMonoArray *pOutputPorts = new CScriptArray(monoConfig.outputs);

		static SOutputPortConfig outputs[MAX_NODE_PORT_COUNT];

		pOutputs = outputs;

		for(int i = 0; i < pOutputPorts->GetSize(); i++)
			pOutputs[i] = pOutputPorts->GetItem(i)->Unbox<SMonoOutputPortConfig>().Convert();

		for(int i = 0; i < MAX_NODE_PORT_COUNT; i++)
		{
			if(i >= pOutputPorts->GetSize())
				pOutputs[i] = nullOutputConfig;
		}

		SAFE_RELEASE(pOutputPorts);
	}
}