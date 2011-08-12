#ifndef __FLOWGRAPH_PLUGIN_MANAGER_H__
#define __FLOWGRAPH_PLUGIN_MANAGER_H__

#pragma once

#include <map>

#include "Mono.h"
#include "MonoFlowBaseNode.h"

class CFGPluginManager : public MonoAPIBinding
{
public:
	CFGPluginManager();
	~CFGPluginManager() {}

	void Reset();

	MonoObject *GetFlowSystemInstance() const { return m_pFlowSystemInstance; }

	// Mono funcs
	static void RegisterNode(MonoString *category, MonoString *nodeName);

	static uint16 GetNodeId(MonoString *name);

protected:
	virtual const char* GetClassName() { return "FlowManager"; }
	virtual const char* GetNamespaceExtension() { return "FlowSystem"; }

private:
	MonoObject *m_pFlowSystemInstance;
};

#endif //__FLOWGRAPH_PLUGIN_MANAGER_H__