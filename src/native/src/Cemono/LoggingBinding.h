#pragma once

#include "BaseCemonoClassBinding.h"

class CLoggingBinding : public BaseCemonoClassBinding
{
public:
	CLoggingBinding();
	virtual ~CLoggingBinding();

protected:
	virtual const char* GetClassName() { return "Logging"; }

	static void _LogAlways(MonoString *msg);
	static void _Log(MonoString *msg);
	static void _CryWarning(MonoString *msg);

};

