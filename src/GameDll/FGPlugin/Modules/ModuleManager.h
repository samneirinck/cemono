/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// ModuleManager.h
//
// Purpose: Manages module loading and application
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#ifndef _MODULEMANAGER_H_
#define _MODULEMANAGER_H_

#include "ModuleCommon.h"

struct SActivationInfo;
class CFlowBaseNode;
class CModule;

class CModuleManager
{
	// Mod path
	string m_szModPath;

	// Module Id map
	typedef std::map<string, TModuleId> TModuleIdMap;
	TModuleIdMap m_ModuleIds;

	// Loaded modules
	typedef std::map<TModuleId, CModule*> TModuleMap;
	TModuleMap m_Modules;
	TModuleId m_moduleIdMaker;

public:
	////////////////////////////////////////////////////
	CModuleManager();
private:
	CModuleManager(CModuleManager const&) {}
	CModuleManager& operator =(CModuleManager const&) {return *this;}

public:
	////////////////////////////////////////////////////
	virtual ~CModuleManager();

	////////////////////////////////////////////////////
	// Initialize manager
	////////////////////////////////////////////////////
	bool Init(char const* modPath);

	////////////////////////////////////////////////////
	// Shutdown
	////////////////////////////////////////////////////
	void Shutdown();

	////////////////////////////////////////////////////
	// GetModuleParamsHolder
	//
	// Purpose: Helper to extract and construct a module
	//	params holder from Flownode activation data
	////////////////////////////////////////////////////
	bool GetModuleParamsHolder(TModuleParams &params, CFlowBaseNode *pNode, IFlowNode::SActivationInfo *pActInfo, int startPos) const;

	////////////////////////////////////////////////////
	// ClearModules
	//
	// Purpose: Unload all loaded modules
	////////////////////////////////////////////////////
	void ClearModules();

	////////////////////////////////////////////////////
	// LoadModule
	//
	// Purpose: Loads a module and begins its execution
	////////////////////////////////////////////////////
	bool LoadModule(MODULE_FLOWNODE_CALLERNODE* pCaller, char const* module, EntityId const& entityId, TModuleParams const& params);

	////////////////////////////////////////////////////
	// OnModuleFinished
	//
	// Purpose: Called when a module is done executing
	////////////////////////////////////////////////////
	void OnModuleFinished(TModuleId const& moduleId, IFlowGraph *pGraph, bool bSuccess, TModuleParams const& params);
};

#endif //_MODULEMANAGER_H_
