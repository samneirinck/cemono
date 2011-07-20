#include "StdAfx.h"
#include "ConsoleBinding.h"
#include "MonoClassUtils.h"

struct IConsole;

CConsoleBinding::CConsoleBinding()
{
	mono_add_internal_call("Cemono.API.Console._GetCVar",_GetCVar);
	mono_add_internal_call("Cemono.API.Console._GetCVarHelpText", _GetCVarHelpText);
}


CConsoleBinding::~CConsoleBinding()
{
}


CCemonoCVar* CConsoleBinding::_GetCVar(MonoString* cvarName)
{
	ICVar* pCvar = GetCvarByName(cvarName);

	if (pCvar)
	{
		MonoClass* pCvarClass = CMonoClassUtils::GetClassByName("Cemono", "CVar");
		if (pCvarClass)
		{
			MonoObject* pCvarInstance = CMonoClassUtils::CreateInstanceOf(pCvarClass);
			if (pCvarInstance)
			{

			}
		}
	}

	return NULL;
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

ICVar* CConsoleBinding::GetCvarByName(MonoString* cvarName)
{
	return gEnv->pConsole->GetCVar(mono_string_to_utf8(cvarName));
}