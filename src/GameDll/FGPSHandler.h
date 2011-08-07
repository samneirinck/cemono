#ifndef __FLOWGRAPH_PLUGIN_MANAGER_H__
#define __FLOWGRAPH_PLUGIN_MANAGER_H__

#pragma once

#include <map>

#include <IFlowSystem.h>

#include "Mono.h"
#include "MonoFlowBaseNode.h"

class CG2AutoRegFlowNodeBase;

class CFGPluginManager
{
public:
	CFGPluginManager();
	~CFGPluginManager() {}

	void Reset();

	MonoObject *GetFlowSystemInstance() const { return m_pFlowSystemInstance; }

	static void RegisterNode(MonoString *category, MonoString *nodeName); // Called from C# node dll

private:
	// Dll Plugin list
	int m_nPluginCounter;

	MonoObject *m_pFlowSystemInstance;
};

#endif //__FLOWGRAPH_PLUGIN_MANAGER_H__