#include "StdAfx.h"
#include "CallbackHandler.h"

#include "MonoScriptSystem.h"
#include "MonoCommon.h"

CCallbackHandler::CCallbackHandler()
{
}


CCallbackHandler::~CCallbackHandler()
{
}

extern "C"
{
	_declspec(dllexport) void __cdecl _RegisterCallback(const char *func, const char *className, MonoCallback callback)
	{
		CCallbackHandler *pCallbackHandler = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetCallbackHandler();
		pCallbackHandler->RegisterCallback(func, className, callback);

		//pCallbackHandler->InvokeCallback(func, className);Z
	}
}

void CCallbackHandler::RegisterCallback(const char *funcName, const char *className, MonoCallback callback)
{
	m_callBacks.push_back(SCallbackIdent(funcName, className, callback));
}

void CCallbackHandler::InvokeCallback(const char *func, const char *className)
{
	CryLogAlways("Attempting to invoke callback %s in class %s", func, className);

	TCallbacks::iterator it = m_callBacks.begin();
	TCallbacks::iterator end = m_callBacks.end();
	for( ; it != end; ++it)
	{
		CryLogAlways("Stored callback: %s %s", (*it).funcName, (*it).className);

		if(!strcmp((*it).funcName, func) && !strcmp((*it).className, className))
			(*(*it).callback)();
	}
}