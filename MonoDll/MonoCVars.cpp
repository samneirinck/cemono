#include "StdAfx.h"
#include "MonoCVars.h"

void SCVars::InitCVars(IConsole *pConsole)
{
	REGISTER_CVAR(mono_exceptionsTriggerMessageBoxes, 1, VF_NULL, "If true, exceptions will trigger a message box to appear");
	REGISTER_CVAR(mono_exceptionsTriggerFatalErrors, 0, VF_NULL, "If true, exceptions will trigger a fatal error");

	REGISTER_CVAR(mono_boxUnsignedIntegersAsEntityIds, 1, VF_NULL, "Determines if all unsigned integers will be boxed as entity id's");
	REGISTER_CVAR(mono_realtimeScripting, 1, VF_NULL, "Enables / Disables Realtime Scripting functionality. (Editor-only)");

}

//------------------------------------------------------------------------
void SCVars::ReleaseCVars()
{
	IConsole *pConsole = gEnv->pConsole;
}