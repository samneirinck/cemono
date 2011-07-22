#include "StdAfx.h"
#include "MonoCVars.h"

#include "IMonoAPIBinding.h"

CMonoCVars::CMonoCVars()
{
	mono_add_internal_call("CryEngine.CVar::_GetIVal", &_GetIVal);
	mono_add_internal_call("CryEngine.CVar::_GetFVal", &_GetFVal);
	mono_add_internal_call("CryEngine.CVar::_GetString", &_GetString);
	mono_add_internal_call("CryEngine.CVar::_SetCVarFVal", &_SetCVarFVal);
	mono_add_internal_call("CryEngine.CVar::_SetCVarIVal", &_SetCVarIVal);
	mono_add_internal_call("CryEngine.CVar::_SetCVar", &_SetCVar);
}


CMonoCVars::~CMonoCVars()
{
}

int CMonoCVars::_GetIVal(MonoString *CVar)
{
	if(IConsole *pConsole = gEnv->pConsole)
	{
		if(ICVar *pCVar = pConsole->GetCVar(mono_string_to_utf8(CVar)))
			return pCVar->GetIVal();
	}

	return -1;
}

float CMonoCVars::_GetFVal(MonoString *CVar)
{
	if(IConsole *pConsole = gEnv->pConsole)
	{
		if(ICVar *pCVar = pConsole->GetCVar(mono_string_to_utf8(CVar)))
			return pCVar->GetFVal();
	}

	return -1.0f;
}

string CMonoCVars::_GetString(MonoString *CVar)
{
	if(IConsole *pConsole = gEnv->pConsole)
	{
		if(ICVar *pCVar = pConsole->GetCVar(mono_string_to_utf8(CVar)))
			return pCVar->GetString();
	}

	return "";
}

void CMonoCVars::_SetCVar(MonoString *CVar, MonoString *newVal)
{
	if(IConsole *pConsole = gEnv->pConsole)
	{
		if(ICVar *pCVar = pConsole->GetCVar(mono_string_to_utf8(CVar)))
			pCVar->Set(mono_string_to_utf8(newVal));
	}
}

void CMonoCVars::_SetCVarIVal(MonoString *CVar, int newVal)
{
	if(IConsole *pConsole = gEnv->pConsole)
	{
		if(ICVar *pCVar = pConsole->GetCVar(mono_string_to_utf8(CVar)))
			pCVar->Set(newVal);
	}
}

void CMonoCVars::_SetCVarFVal(MonoString *CVar, float newVal)
{
	if(IConsole *pConsole = gEnv->pConsole)
	{
		if(ICVar *pCVar = pConsole->GetCVar(mono_string_to_utf8(CVar)))
			pCVar->Set(newVal);
	}
}