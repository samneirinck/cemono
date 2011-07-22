#include "StdAfx.h"
#include "FGPSHandler.h"

#include "FGPlugin\Modules\ModuleManager.h"

#include <CryLibrary.h>
#include <windows.h>

#include "G2FlowBaseNode.h"

#include "PathUtils.h"

ExtendedResourceList CFGPluginManager::m_ResourceList;
CG2AutoRegFlowNodeBase* CFGPluginManager::m_LastNext;
CG2AutoRegFlowNodeBase* CFGPluginManager::m_Last;
FGPluginList CFGPluginManager::m_Plugins;

CFGPluginManager::CFGPluginManager()
	: m_pModuleManager(0)
	, m_nPluginCounter(0)
{
	// Create module manager
	m_pModuleManager = new CModuleManager();
	if (!m_pModuleManager || !m_pModuleManager->Init(CPathUtils::GetModPath(false)))
		CryError("[Error] Failed to initialize FG Module Manager.");
}

CFGPluginManager::~CFGPluginManager()
{
	if(!m_Plugins.empty())
		FreePluginLibraries();

	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (m_pModuleManager)
		m_pModuleManager->Shutdown();

	SAFE_DELETE(m_pModuleManager);
}

void CFGPluginManager::Reset()
{
	if (m_pModuleManager)
		m_pModuleManager->ClearModules();
}

////////////////////////////////////////////////////
void CFGPluginManager::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	if (event == ESYSTEM_EVENT_LEVEL_LOAD_START ||
		event == ESYSTEM_EVENT_LEVEL_RELOAD)
	{
		if (m_pModuleManager)
			m_pModuleManager->ClearModules();
	}
}

void CFGPluginManager::RetrieveNodes()
{
	string szPlugins = CPathUtils::GetFGNodePath();

	CryLogAlways("---------------------------------");
	CryLogAlways("Searching for FGPS nodes in %s", szPlugins);

	ICryPak *pCryPak = gEnv->pCryPak;
	
	_finddata_t fileData;
	char szPluginPath[MAX_PATH];

	strncpy_s(szPluginPath, MAX_PATH, szPlugins, MAX_PATH);
	strncat_s(szPluginPath, MAX_PATH, "*.dll", MAX_PATH);

	intptr_t hFile = pCryPak->FindFirst(szPluginPath, &fileData);
	if (hFile > -1)
	{
		do
		{
			if(strcmp(fileData.name, "CryEngine.dll")) // Required library for flownodes
			{
				m_nPluginCounter++;

				string dllPath = (szPlugins+fileData.name);
				CryLog("[%d] Attempting to load plugin \'%s\'...", m_nPluginCounter, fileData.name);
				if (!RegisterPlugin(dllPath.c_str(), fileData.name))
					CryLogAlways("[Warning] [%d] Failed to load plugin!", m_nPluginCounter);
			}
		} while (pCryPak->FindNext(hFile, &fileData) > -1);
		pCryPak->FindClose(hFile);
	}
	else
		CryLog("No custom nodes found.");
}

void Tokenize(const string& str, std::vector<string>& tokens, const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

////////////////////////////////////////////////////
bool CFGPluginManager::RegisterPlugin(const char *fullPath, const char *DllName)
{
	MonoAssembly *pAssembly = g_pMono->LoadAssembly(fullPath);

	// remove ".dll" from the string
	std::vector<string> tokens;
	Tokenize(DllName, tokens, "."); // "SampleNode.dll" -> "SampleNode" & "dll

	string sDllName = *tokens.begin();

	MonoImage *pImage = mono_assembly_get_image(pAssembly);

	MonoObject *result = g_pMono->InvokeFunc("GetNodeProperties", mono_class_from_name(pImage, "FGPlugin", sDllName ), true);
	SMonoFlowNodeConfig flowConfig = *(SMonoFlowNodeConfig*)mono_object_unbox (result);

	string sResult = mono_string_to_utf8(flowConfig.sDescription);

	SFGPlugin pluginEntry;
	pluginEntry.pImage = pImage;
	pluginEntry.name = sDllName;
	m_Plugins.push_back(pluginEntry);
		
	return false;
}

////////////////////////////////////////////////////
void CFGPluginManager::FreePluginLibraries()
{
	m_Plugins.clear();
}

/*
void CFGPluginManager::Plugin_ProcessEvent(IFlowNode::EFlowEvent event, IFlowNode::SActivationInfo *pActInfo)
{
	for(std::vector<uint16>::iterator intIt = m_nodeIds.begin(); intIt != m_nodeIds.end(); intIt++)
	{
		if((*intIt)==pActInfo->myID)
		{
			for (std::vector<SFGPlugin>::iterator pluginIt = m_Plugins.begin(); pluginIt != m_Plugins.end(); pluginIt++)
			{
				void *args [2];
				args[0] = &event;
				args[1] = &SMonoNodeInfo(pActInfo->myID, pActInfo->pEntity->GetId());

				g_pMono->InvokeFunc("ProcessEvent", mono_class_from_name((*pluginIt).pImage, "FGPlugin", (*pluginIt).name ), false, args);
			}
		}
	}
}*/