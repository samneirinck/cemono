#ifndef __FLOWGRAPH_PLUGIN_MANAGER_H__
#define __FLOWGRAPH_PLUGIN_MANAGER_H__

#pragma once

#include "FGPlugin\Common\PluginCommon.h"
#include <IFlowSystem.h>

class CModuleManager;

class CFGPluginManager : public ISystemEventListener
{
public:
	CFGPluginManager();
	~CFGPluginManager();

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);
	// ~ISystemEventListener

	void Reset();
	void RetrieveNodes();

	CModuleManager *GetModuleManager() const { return m_pModuleManager; }

private:
	bool RegisterPlugin(const char *fullPath, const char *DllName);
	 
	void FreePluginLibraries();

private:
	// Dll Plugin list
	int m_nPluginCounter;

	static FGPluginList m_Plugins;
	std::vector<uint16>	m_nodeIds;

	static ExtendedResourceList m_ResourceList;
	static CG2AutoRegFlowNodeBase *m_LastNext;
	static CG2AutoRegFlowNodeBase *m_Last;

	CModuleManager *m_pModuleManager;
};

struct SMonoInputPortConfig
{
	    // name of this port
	    MonoString *name;
	    // Human readable name of this port (default: same as name)
	    MonoString *humanName;
	    // Human readable description of this port (help)
	    MonoString *description;
	    // UIConfig: enums for the variable e.g 
	    // "enum_string:a,b,c"
	    // "enum_string:something=a,somethingelse=b,whatever=c"
	    // "enum_int:something=0,somethingelse=10,whatever=20"
	    // "enum_float:something=1.0,somethingelse=2.0"
	    // "enum_global:GlobalEnumName"
	    MonoString *sUIConfig;
};

struct SMonoOutputPortConfig
{
	    // name of this port
	    MonoString *name;
	    // Human readable name of this port (default: same as name)
	    MonoString *humanName;
	    // Human readable description of this port (help)
	    MonoString *description;
	    // type of our output (or -1 for "dynamic")
	    int type;
};

enum EMonoFlowNodeCategory
{
        EMFLN_APPROVED							 = 0x0010, // CATEGORY:  This node is approved for designers
	    EMFLN_ADVANCED              = 0x0020, // CATEGORY:  This node is slightly advanced and approved
	    EMFLN_DEBUG                 = 0x0040, // CATEGORY:  This node is for debug purpose only
	    EMFLN_WIP                   = 0x0080, // CATEGORY:  This node is work-in-progress and shouldn't be used by designers
	    EMFLN_LEGACY                = 0x0100, // CATEGORY:  This node is legacy and will VERY soon vanish
	    EMFLN_OBSOLETE              = 0x0200, // CATEGORY:  This node is obsolete and is not available in the editor
};

struct SMonoFlowNodeConfig
{
	void ToRealSettings()
	{

	}

	bool targetsEntity;

	SMonoInputPortConfig pInputPorts;
	SMonoOutputPortConfig pOutputPorts;

	MonoString *sDescription;
	MonoString *sUIClassName;

	EMonoFlowNodeCategory category;

	SFlowNodeConfig realConfig;
};

struct SMonoNodeInfo
{
	SMonoNodeInfo(uint16 id, EntityId entId)
	{
		nodeId = id;
		entityId = entId;
	}

	uint16 nodeId;
	EntityId entityId;
};

#endif //__FLOWGRAPH_PLUGIN_MANAGER_H__