#include "StdAfx.h"
#include "ConsoleBinding.h"
#include "MonoClassUtils.h"

struct IConsole;

CConsoleBinding::CConsoleBinding()
{
	mono_add_internal_call("Cemono.API.Console.GetCVar",_GetCVar);
}


CConsoleBinding::~CConsoleBinding()
{
}


CCemonoCVar* CConsoleBinding::_GetCVar(MonoString* cvarName)
{
	ICVar* pCvar = gEnv->pConsole->GetCVar(mono_string_to_utf8(cvarName));

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
