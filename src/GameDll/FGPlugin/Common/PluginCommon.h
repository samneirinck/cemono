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

// Registration table
struct SPluginRegister
{
	// Flownode registration list
	CG2AutoRegFlowNodeBase *nodesFirst;
	CG2AutoRegFlowNodeBase *nodesLast;

	SPluginRegister() : nodesFirst(NULL), nodesLast(NULL) {}
};

// DLL FG plugin definition
struct SFGPlugin
{
	string name;
	MonoAssembly *pAssembly;
	CG2AutoRegFlowNodeBase *nodes;

	SFGPlugin() : pAssembly(NULL) {}
};
typedef std::vector<SFGPlugin> FGPluginList;

#endif //_PLUGINCOMMON_H_
