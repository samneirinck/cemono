/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Test to invoke C# methods faster than Mono scriptbinds.
// Allows for calls to non-static C# methods from C++.
//////////////////////////////////////////////////////////////////////////
// 19/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __CALLBACK_HANDLER_H__
#define __CALLBACK_HANDLER_H__

#include <IMonoScriptBind.h>

typedef void (*MonoCallback)(void);

struct SCallbackIdent
{
public:
	SCallbackIdent(const char *func, const char *_class, MonoCallback _callback)
		: funcName(func), className(_class), callback(&_callback) {};

	//void Invoke() { return (*callback)(); }

	const char *className;
	const char *funcName;
	MonoCallback *callback;
};

class CMonoCallbackHandler
{
public:
	typedef std::deque<SCallbackIdent> TCallbacks;

	CMonoCallbackHandler();
	~CMonoCallbackHandler();

	void InvokeCallback(const char *func, const char *className);
	void RegisterCallback(const char *funcName, const char *className, MonoCallback cb);

protected:
	TCallbacks m_callBacks;
};

#endif //__CALLBACK_HANDLER_H__