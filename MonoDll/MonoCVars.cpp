#include "StdAfx.h"
#include "MonoCVars.h"

void SCVars::InitCVars(IConsole *pConsole)
{
	REGISTER_CVAR(mono_revertScriptsOnError, 1, VF_NULL, "Determines if the last functional compiled scripts should be reloaded upon script compilation failure");

	REGISTER_CVAR(mono_exceptionsTriggerMessageBoxes, 1, VF_NULL, "If true, exceptions will trigger a message box to appear");
	REGISTER_CVAR(mono_exceptionsTriggerFatalErrors, 0, VF_NULL, "If true, exceptions will trigger a fatal error");

	REGISTER_CVAR(mono_useExperimentalMethodFinding, 1, VF_NULL, "Uses the new method of finding script methods");
}

//------------------------------------------------------------------------
void SCVars::ReleaseCVars()
{
	IConsole *pConsole = gEnv->pConsole;
}