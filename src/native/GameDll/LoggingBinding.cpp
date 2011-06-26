#include "StdAfx.h"
#include "LoggingBinding.h"
#include "IThreadTask.h"
#include <windows.h>


void SetCurrentThreadId( DWORD dwThreadId )
{
	_asm
	{
		mov eax, fs:[0x18];
		mov ecx, dwThreadId;
		mov [eax + 0x24], ecx; //not sure yet if this works on al operating systems, but it does on Windows 7 x64
	}
}


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