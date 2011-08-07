#include "StdAfx.h"
#include "FGPSHandler.h"

#include "Game.h"
#include "PathUtils.h"

CFGPluginManager::CFGPluginManager()
{
	MonoClass *nativeClass = mono_class_from_name(g_pMono->GetNativeLibraryImage(), "CryEngine.FlowSystem", "FlowManager");
	m_pFlowSystemInstance = g_pMono->CreateClassInstance(nativeClass);
	mono_runtime_object_init(m_pFlowSystemInstance);
	
	// Register exposed mono functions
	mono_add_internal_call("CryEngine.FlowSystem.FlowManager::_RegisterNode", &RegisterNode);
	mono_add_internal_call("CryEngine.FlowSystem.FlowManager::_GetNodeId", &GetNodeId);
	
	// Retrieve C# nodes
	void *args[1];
	args[0] = g_pMono->ToMonoString(CPathUtils::GetFGNodePath());

	g_pMono->InvokeFunc("RegisterNodes", nativeClass, m_pFlowSystemInstance, args);
}

void CFGPluginManager::RegisterNode(MonoString *category, MonoString *nodeName)
{
	string sNodeName = mono_string_to_utf8(nodeName);
	string sCategory = mono_string_to_utf8(category) + (string)":" + sNodeName; 

	CMonoFlowNode *pNode = new CMonoFlowNode();
	if(IFlowSystem *pFlow = g_pGame->GetIGameFramework()->GetIFlowSystem())
		pFlow->RegisterType(sCategory, pNode);
}

uint16 CFGPluginManager::GetNodeId(MonoString *name)
{
	return g_pGame->GetIGameFramework()->GetIFlowSystem()->GetTypeId(mono_string_to_utf8(name));
}