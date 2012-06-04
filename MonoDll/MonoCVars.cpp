#include "StdAfx.h"
#include "MonoCVars.h"

void SCVars::InitCVars(IConsole *pConsole)
{
	REGISTER_CVAR(mono_exceptionsTriggerMessageBoxes, 0, VF_NULL, "If true, exceptions will trigger a message box to appear");
	REGISTER_CVAR(mono_exceptionsTriggerFatalErrors, 0, VF_NULL, "If true, exceptions will trigger a fatal error");
}

//------------------------------------------------------------------------
void SCVars::ReleaseCVars()
{
	IConsole *pConsole = gEnv->pConsole;
}