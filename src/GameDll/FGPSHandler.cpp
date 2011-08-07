#include "StdAfx.h"
#include "FGPSHandler.h"

#include "Game.h"
#include "PathUtils.h"

CFGPluginManager::CFGPluginManager()
	: m_nPluginCounter(0)
{
	MonoClass *nativeClass = mono_class_from_name(g_pMono->GetNativeLibraryImage(), "CryEngine.FlowSystem", "FlowManager");
	m_pFlowSystemInstance = g_pMono->CreateClassInstance(nativeClass);
	mono_runtime_object_init(m_pFlowSystemInstance);

	// Retrieve C# nodes
	mono_add_internal_call("CryEngine.FlowSystem.FlowManager::_RegisterNode", &RegisterNode);
	
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