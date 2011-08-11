#pragma once

#include "MonoAPIBinding.h"

class CLoggingBinding : public MonoAPIBinding
{
public:
	CLoggingBinding();
	virtual ~CLoggingBinding();

protected:
	virtual const char* GetClassName() { return "Logging"; }
	virtual const char* GetNamespaceExtension() { return "API"; }

	static void _LogAlways(MonoString *msg);
	static void _Log(MonoString *msg);
	static void _CryWarning(MonoString *msg);

};

