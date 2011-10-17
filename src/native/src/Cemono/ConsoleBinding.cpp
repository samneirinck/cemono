#include "StdAfx.h"
#include "ConsoleBinding.h"

struct IConsole;

CConsoleBinding::CConsoleBinding()
{
	REGISTER_METHOD(_GetCVar);
	REGISTER_METHOD(_GetCVarHelpText);
	REGISTER_METHOD(_GetCVarValueInt);
	REGISTER_METHOD(_GetCVarValueFloat);
	REGISTER_METHOD(_GetCVarValueString);
	REGISTER_METHOD(_SetCVarValueInt);
	REGISTER_METHOD(_SetCVarValueFloat);
	REGISTER_METHOD(_SetCVarValueString);
	REGISTER_METHOD(_GetCVarFlags);
	REGISTER_METHOD(_SetCVarFlags);
	REGISTER_METHOD(_RegisterCVarInt);
	REGISTER_METHOD(_RegisterCVarFloat);
	REGISTER_METHOD(_RegisterCVarString);
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
		return CCemonoString::ToMonoString(pCvar->GetHelp());
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
		return CCemonoString::ToMonoString(pCvar->GetString());
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
		pCvar->Set(CCemonoString::ToString(value));
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
	return gEnv->pConsole->GetCVar(CCemonoString::ToString(cvarName));
}

void CConsoleBinding::_RegisterCVarInt(MonoString* cvarName, int defaultValue, int flags, MonoString* help)
{
	gEnv->pConsole->RegisterInt(CCemonoString::ToString(cvarName), defaultValue, flags, CCemonoString::ToString(help));
}
void CConsoleBinding::_RegisterCVarFloat(MonoString* cvarName, float defaultValue, int flags, MonoString* help)
{
	gEnv->pConsole->RegisterFloat(CCemonoString::ToString(cvarName), defaultValue, flags, CCemonoString::ToString(help));
}
void CConsoleBinding::_RegisterCVarString(MonoString* cvarName, MonoString* defaultValue, int flags, MonoString* help)
{
	gEnv->pConsole->RegisterString(CCemonoString::ToString(cvarName), CCemonoString::ToString(defaultValue), flags, CCemonoString::ToString(help));
}
