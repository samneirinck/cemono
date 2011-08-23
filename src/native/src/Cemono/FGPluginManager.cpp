#include "StdAfx.h"
#include "FGPluginManager.h"

#include <IFlowSystem.h>

CFGPluginManager::CFGPluginManager()
{
	/*MonoClass *nativeClass = mono_class_from_name(g_pMono->GetBclImage(), "CryEngine.FlowSystem", "FlowManager");

	m_pFlowSystemInstance = CMonoClassUtils::CreateInstanceOf(nativeClass);
	
	RegisterAPIBinding("_RegisterNode", RegisterNode);
	RegisterAPIBinding("_GetNodeId", GetNodeId);
	
	// Retrieve C# nodes
	void *args[1];
	args [0] = mono_string_new(mono_domain_get(), /*CMonoPathUtils::GetFGNodePath()"");

	CMonoClassUtils::CallMethod("RegisterNodes", nativeClass, m_pFlowSystemInstance, args);*/
}

void CFGPluginManager::RegisterNode(MonoString *category, MonoString *nodeName)
{
	/*string sNodeName = mono_string_to_utf8(nodeName);
	string sCategory = mono_string_to_utf8(category) + (string)":" + sNodeName; 

	CMonoFlowNode *pNode = new CMonoFlowNode();
	if(IFlowSystem *pFlow = g_pGame->GetIGameFramework()->GetIFlowSystem())
		pFlow->RegisterType(sCategory, pNode);

		*/
}

uint16 CFGPluginManager::GetNodeId(MonoString *name)
{
	//return g_pGame->GetIGameFramework()->GetIFlowSystem()->GetTypeId(mono_string_to_utf8(name));
	return 0;
}