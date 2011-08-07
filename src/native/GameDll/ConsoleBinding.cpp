#include "StdAfx.h"
#include "ConsoleBinding.h"
#include "MonoClassUtils.h"

struct IConsole;

CConsoleBinding::CConsoleBinding()
{
	RegisterAPIBinding("_GetCVar", _GetCVar);
	RegisterAPIBinding("_GetCVarHelpText", _GetCVarHelpText);
	RegisterAPIBinding("_GetCVarValueInt", _GetCVarValueInt);
	RegisterAPIBinding("_GetCVarValueFloat", _GetCVarValueFloat);
	RegisterAPIBinding("_GetCVarValueString", _GetCVarValueString);
	RegisterAPIBinding("_SetCVarValueInt", _SetCVarValueInt);
	RegisterAPIBinding("_SetCVarValueFloat", _SetCVarValueFloat);
	RegisterAPIBinding("_SetCVarValueString", _SetCVarValueString);
	RegisterAPIBinding("_GetCVarFlags", _GetCVarFlags);
	RegisterAPIBinding("_SetCVarFlags", _SetCVarFlags);
}


CConsoleBinding::~CConsoleBinding()
{
}


bool CConsoleBinding::_GetCVar(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	
	return pCvar != NULL;
}

MonoString* CConsoleBinding::_GetCVarHelpText(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		return mono_string_new(mono_domain_get(), pCvar->GetHelp());
	}
	return NULL;
}

int CConsoleBinding::_GetCVarValueInt(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		return pCvar->GetIVal();
	}
	else {
		return -1;
	}
}

float CConsoleBinding::_GetCVarValueFloat(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		return pCvar->GetFVal();
	}
	else {
		return -1.0f;
	}
}

MonoString* CConsoleBinding::_GetCVarValueString(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		return mono_string_new(mono_domain_get(), pCvar->GetString());
	}
	else {
		return NULL;
	}
}

void CConsoleBinding::_SetCVarValueInt(MonoString* cvarName, int value)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		pCvar->Set(value);
	}
}
void CConsoleBinding::_SetCVarValueFloat(MonoString* cvarName, float value)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		pCvar->Set(value);
	}
}
void CConsoleBinding::_SetCVarValueString(MonoString* cvarName, MonoString* value)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		pCvar->Set(mono_string_to_utf8(value));
	}
}

int CConsoleBinding::_GetCVarFlags(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		return pCvar->GetFlags();
	}
	return -1;
}
void CConsoleBinding::_SetCVarFlags(MonoString* cvarName, int value)
{
	ICVar* pCvar = GetCvarByName(cvarName);
	if (pCvar)
	{
		pCvar->SetFlags(value);
	}
}

ICVar* CConsoleBinding::GetCvarByName(MonoString* cvarName)
{
	return gEnv->pConsole->GetCVar(mono_string_to_utf8(cvarName));
}