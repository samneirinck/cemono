#include "stdafx.h"

#include "MonoScriptSystem.h"

#ifdef PLUGIN_SDK
#include <IPluginManager.h>
#include "CPluginCryMono.h"

PluginManager::IPluginManager* gPluginManager = NULL; //!< pointer to plugin manager

extern "C"
{
    MONO_API PluginManager::IPluginBase *GetPluginInterface(const char *sInterfaceVersion)
    {
        // This function should not create a new interface class each call.
        static CryMonoPlugin::CPluginCryMono modulePlugin;
        CryMonoPlugin::gPlugin = &modulePlugin;
        return static_cast<PluginManager::IPluginBase *>(CryMonoPlugin::gPlugin);
    }
}
#else
extern "C"
{
	MONO_API IMonoScriptSystem *InitCryMono(ISystem *pSystem)
	{
		ModuleInitISystem(pSystem, "CryMono");

		return new CScriptSystem();
	}
}
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
    switch ( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}