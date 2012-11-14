#include "StdAfx.h"
#include "MonoCVars.h"

#include "MonoScriptSystem.h"

void CmdReload(IConsoleCmdArgs *pCmdArgs)
{
	gEnv->pMonoScriptSystem->Reload();
}

void SCVars::InitCVars(IConsole *pConsole)
{
	REGISTER_CVAR(mono_exceptionsTriggerMessageBoxes, 1, VF_NULL, "If true, exceptions will trigger a message box to appear");
	REGISTER_CVAR(mono_exceptionsTriggerFatalErrors, 0, VF_NULL, "If true, exceptions will trigger a fatal error");

	REGISTER_CVAR(mono_boxUnsignedIntegersAsEntityIds, 1, VF_NULL, "Determines if all unsigned integers will be boxed as entity id's");
	
	REGISTER_CVAR(mono_realtimeScripting, 1, VF_NULL, "Enables / Disables Realtime Scripting functionality. (Editor-only)");
	
#ifndef RELEASE
	REGISTER_CVAR(mono_realtimeScriptingDebug, 1, VF_NULL, "Toggles on realtime scripting debug, useful for finding serialization bugs");
#endif

	REGISTER_CVAR(mono_realtimeScriptingDetectChanges, 1, VF_NULL, "Toggles whether realtime scripting should automatically reload scripts when any *.cs file is modified in the scripts directory.");
	REGISTER_COMMAND("mono_reload", CmdReload, VF_NULL, "[Realtime Scripting] Reloads / recompiles all scripts");

	REGISTER_CVAR(mono_softBreakpoints, 1, VF_REQUIRE_APP_RESTART, "[Performance Warning] Enables / Disables soft breakpoints, preventing managed null reference exceptions causing crashes in unmanaged code.");
}

//------------------------------------------------------------------------
void SCVars::ReleaseCVars()
{
	IConsole *pConsole = gEnv->pConsole;
}