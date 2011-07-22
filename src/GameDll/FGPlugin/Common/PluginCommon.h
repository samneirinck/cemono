/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// PluginCommon.h
//
// Purpose: Common functionality shared by plugins and system
//
// History:
//	- 6/01/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#ifndef _PLUGINCOMMON_H_
#define _PLUGINCOMMON_H_

#include <CryVersion.h>
#include "Game.h"

class CG2AutoRegFlowNodeBase;

// Game dll resources for plugin declarations
enum EPluginResources
{
	ePR_Invalid = -1,

	// ALWAYS MUST BE LAST!
	ePR_Count,
};

// Registration table
struct SPluginRegister
{
	// Version information
	SFileVersion version;

	// Flownode registration list
	CG2AutoRegFlowNodeBase *nodesFirst;
	CG2AutoRegFlowNodeBase *nodesLast;

	SPluginRegister() : nodesFirst(NULL), nodesLast(NULL) {}
};

// DLL FG plugin definition
struct SFGPlugin
{
	string name;
	MonoImage *pImage;

	SFGPlugin() : pImage(NULL) {}
};
typedef std::vector<SFGPlugin> FGPluginList;

// Extended resources
struct SExtendedResource
{
	EPluginResources eId;
	void *pData;

	SExtendedResource(EPluginResources _eId, void *_pData) : eId(_eId), pData(_pData)
	{
		CRY_ASSERT(pData);
		CRY_ASSERT_MESSAGE(eId > ePR_Invalid && eId < ePR_Count, "Invalid Resource Id");
	}

	bool operator ==(const SExtendedResource& o)
	{
		return (eId == o.eId);
	}
};
typedef std::vector<SExtendedResource> ExtendedResourceList;

// Typedef for plug-in functions
typedef bool (*RegisterFunc)(ISystem *pSystem, SPluginRegister &outRegister);
typedef bool (*AddResourceFunc)(EPluginResources eResourceType, void *pResource);
typedef const char* (*GetDataFunc)();

// Plug-in function names
#define PLUGIN_NAME_RegisterFunc	("RegisterWithPluginSystem")
#define PLUGIN_NAME_AddResourceFunc	("AddExtendedResource")
#define PLUGIN_NAME_GetName			("GetName")
#define PLUGIN_NAME_GetAuthor		("GetAuthor")
#define PLUGIN_NAME_GetVersion		("GetVersionStr")
#define PLUGIN_NAME_GetNodeList		("GetNodeList")
#define PLUGIN_NAME_Release			("Release")

#endif //_PLUGINCOMMON_H_
