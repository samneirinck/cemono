#include "StdAfx.h"
#include "LoggingBinding.h"

CLoggingBinding::CLoggingBinding()
{
	RegisterAPIBinding("_LogAlways", _LogAlways);
	RegisterAPIBinding("_Log", _Log);
	RegisterAPIBinding("_CryWarning", _CryWarning);
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