#include "StdAfx.h"
#include "ConsoleBinding.h"
#include "MonoClassUtils.h"

struct IConsole;

CConsoleBinding::CConsoleBinding()
{
	mono_add_internal_call("Cemono.API.Console::_GetCVar",_GetCVar);
	mono_add_internal_call("Cemono.API.Console::_GetCVarHelpText", _GetCVarHelpText);
	mono_add_internal_call("Cemono.API.Console::_GetCVarValueInt", _GetCVarValueInt);
	mono_add_internal_call("Cemono.API.Console::_GetCVarValueFloat", _GetCVarValueFloat);
	mono_add_internal_call("Cemono.API.Console::_GetCVarValueString", _GetCVarValueString);
	mono_add_internal_call("Cemono.API.Console::_SetCVarValueInt", _SetCVarValueInt);
	mono_add_internal_call("Cemono.API.Console::_SetCVarValueFloat", _SetCVarValueFloat);
	mono_add_internal_call("Cemono.API.Console::_SetCVarValueString", _SetCVarValueString);
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

ICVar* CConsoleBinding::GetCvarByName(MonoString* cvarName)
{
	return gEnv->pConsole->GetCVar(mono_string_to_utf8(cvarName));
}