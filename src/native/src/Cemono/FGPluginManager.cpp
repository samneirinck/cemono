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
	args [0] = CCemono::ToMonoString(mono_domain_get(), /*CMonoPathUtils::GetFGNodePath()"");

	CMonoClassUtils::CallMethod("RegisterNodes", nativeClass, m_pFlowSystemInstance, args);*/
}

void CFGPluginManager::RegisterNode(MonoString *category, MonoString *nodeName)
{
	/*string sNodeName = CCemono::ToString(nodeName);
	string sCategory = CCemono::ToString(category) + (string)":" + sNodeName; 

	CMonoFlowNode *pNode = new CMonoFlowNode();
	if(IFlowSystem *pFlow = g_pGame->GetIGameFramework()->GetIFlowSystem())
		pFlow->RegisterType(sCategory, pNode);

		*/
}

uint16 CFGPluginManager::GetNodeId(MonoString *name)
{
	//return g_pGame->GetIGameFramework()->GetIFlowSystem()->GetTypeId(CCemono::ToString(name));
	return 0;
}