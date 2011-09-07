#include "StdAfx.h"
#include "LoggingBinding.h"

CLoggingBinding::CLoggingBinding()
{
	REGISTER_METHOD(_LogAlways);
	REGISTER_METHOD(_Log);
	REGISTER_METHOD(_CryWarning);
}


CLoggingBinding::~CLoggingBinding()
{
}

void CLoggingBinding::_LogAlways(MonoString *msg)
{
	CryLogAlways(CCemonoString::ToString(msg));
}

void CLoggingBinding::_Log(MonoString *msg)
{
	CryLog(CCemonoString::ToString(msg));
}

void CLoggingBinding::_CryWarning(MonoString *msg) 
{ 
	CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, CCemonoString::ToString(msg)); 
}