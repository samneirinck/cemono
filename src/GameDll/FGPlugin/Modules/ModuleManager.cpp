/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// ModuleManager.cpp
//
// Purpose: Manages module loading and application
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ModuleManager.h"
#include "Module.h"
#include "G2FlowBaseNode.h"

////////////////////////////////////////////////////
CModuleManager::CModuleManager()
{
	m_moduleIdMaker = MODULEID_INVALID;
}

////////////////////////////////////////////////////
CModuleManager::~CModuleManager()
{
	Shutdown();
}

////////////////////////////////////////////////////
bool CModuleManager::Init(char const* modPath)
{
	m_szModPath = modPath;

	ClearModules();

	return true;
}

////////////////////////////////////////////////////
void CModuleManager::Shutdown()
{
	ClearModules();
}

////////////////////////////////////////////////////
bool CModuleManager::GetModuleParamsHolder(TModuleParams &params, CFlowBaseNode *pNode, IFlowNode::SActivationInfo *pActInfo, int startPos) const
{
	bool bResult = false;

	if (pActInfo)
	{
		params.resize(MODULE_MAX_PARAMS);
		for (int i = 0, port = startPos; i < MODULE_MAX_PARAMS; ++i, ++port)
		{
			params[i] = pNode->GetPortAny(pActInfo, port);
		}
		bResult = true;
	}

	return bResult;
}

////////////////////////////////////////////////////
void CModuleManager::ClearModules()
{
	TModuleMap::iterator i = m_Modules.begin();
	TModuleMap::iterator end = m_Modules.end();
	for (; i != end; ++i)
	{
		if (i->second)
		{
			i->second->Destroy();
			SAFE_DELETE(i->second);
		}
	}
	m_Modules.clear();
	m_ModuleIds.clear();
	m_moduleIdMaker = MODULEID_INVALID;
}

////////////////////////////////////////////////////
bool CModuleManager::LoadModule(MODULE_FLOWNODE_CALLERNODE* pCaller, char const* module, EntityId const& entityId, TModuleParams const& params)
{
	bool bResult = false;

	// Get module container
	bool bFound = false;
	CModule *pModule = NULL;
	TModuleIdMap::iterator moduleIdEntry = m_ModuleIds.find(module);
	if (m_ModuleIds.end() != moduleIdEntry)
	{
		TModuleMap::iterator moduleEntry = m_Modules.find(moduleIdEntry->second);
		if (m_Modules.end() != moduleEntry)
		{
			pModule = moduleEntry->second;
			bFound = (pModule != NULL);
		}
	}
	if (!bFound)
	{
		// Create new container (first load)
		pModule = new CModule;
		if (pModule->Init(module, ++m_moduleIdMaker))
		{
			TModuleId id = pModule->GetId();
			m_ModuleIds[module] = id;
			m_Modules[id] = pModule;
		}
		else
		{
			// Drop it if it doesn't init correctly
			SAFE_DELETE(pModule);
		}
	}

	// Make instance
	if (pModule)
	{
		bResult = pModule->CreateInstance(pCaller, entityId, params);
	}

	return bResult;
}

////////////////////////////////////////////////////
void CModuleManager::OnModuleFinished(TModuleId const& moduleId, IFlowGraph *pGraph, bool bSuccess, TModuleParams const& params)
{
	TModuleMap::iterator moduleEntry = m_Modules.find(moduleId);
	if (m_Modules.end() != moduleEntry)
	{
		CModule *pModule = moduleEntry->second;
		if (pModule)
		{
			pModule->DestroyInstance(pGraph, bSuccess, params);
		}
	}
}
