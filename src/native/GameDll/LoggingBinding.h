#pragma once

#include "IMonoAPIBinding.h"
#include "Mono.h"

class CLoggingBinding : public IMonoAPIBinding
{
public:
	CLoggingBinding();
	virtual ~CLoggingBinding();

protected:
	static void _LogAlways(MonoString *msg);
	static void _Log(MonoString *msg);
	static void _CryWarning(MonoString *msg);

};

