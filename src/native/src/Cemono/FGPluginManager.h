#pragma once

#include <map>
#include <mono/jit/jit.h>

#include "MonoFlowBaseNode.h"
#include "BaseCemonoClassBinding.h"

class CFGPluginManager : public BaseCemonoClassBinding
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
	virtual const char* GetClassName() override { return "FlowManager"; }
	virtual const char* GetNamespaceExtension() override { return "FlowSystem"; }

private:
	MonoObject *m_pFlowSystemInstance;
};