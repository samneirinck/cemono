#include "StdAfx.h"
#include "CallbackHandler.h"

#include "MonoScriptSystem.h"
#include "MonoCommon.h"

CMonoCallbackHandler::CMonoCallbackHandler()
{
}


CMonoCallbackHandler::~CMonoCallbackHandler()
{
}

extern "C"
{
	_declspec(dllexport) void __cdecl _RegisterCallback(const char *func, const char *className, MonoCallback callback)
	{
		CMonoCallbackHandler *pCallbackHandler = static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetCallbackHandler();
		pCallbackHandler->RegisterCallback(func, className, callback);

		//pCallbackHandler->InvokeCallback(func, className);Z
	}
}

void CMonoCallbackHandler::RegisterCallback(const char *funcName, const char *className, MonoCallback callback)
{
	m_callBacks.push_back(SCallbackIdent(funcName, className, callback));
}

void CMonoCallbackHandler::InvokeCallback(const char *func, const char *className)
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