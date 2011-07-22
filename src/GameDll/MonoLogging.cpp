#include "StdAfx.h"
#include "MonoLogging.h"

#include "IMonoAPIBinding.h"

CMonoLogging::CMonoLogging()
{
	mono_add_internal_call("CryEngine.CryConsole::_LogAlways", &_LogAlways);
	mono_add_internal_call("CryEngine.CryConsole::_Log", &_Log);
	mono_add_internal_call("CryEngine.CryConsole::_CryWarning", &_CryWarning);
}


CMonoLogging::~CMonoLogging()
{
}

void CMonoLogging::_LogAlways(MonoString *msg)
{
	CryLogAlways(mono_string_to_utf8(msg));
}

void CMonoLogging::_Log(MonoString *msg)
{
	CryLog(mono_string_to_utf8(msg));
}

void CMonoLogging::_CryWarning(MonoString *msg) 
{ 
	CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, mono_string_to_utf8(msg)); 
}