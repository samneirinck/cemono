#include "StdAfx.h"
#include "LoggingBinding.h"


CLoggingBinding::CLoggingBinding()
{
	mono_add_internal_call("Cemono.API.Logging::_LogAlways", &_LogAlways);
	mono_add_internal_call("Cemono.API.Logging::_Log", &_Log);
	mono_add_internal_call("Cemono.API.Logging::_CryWarning", &_CryWarning);
}


CLoggingBinding::~CLoggingBinding()
{
}

void CLoggingBinding::_LogAlways(MonoString *msg)
{
	CryLogAlways(mono_string_to_utf8(msg));
}

void CLoggingBinding::_Log(MonoString *msg)
{
	CryLog(mono_string_to_utf8(msg));
}

void CLoggingBinding::_CryWarning(MonoString *msg) 
{ 
	CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, mono_string_to_utf8(msg)); 
}