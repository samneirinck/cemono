#pragma once

#include <mono/metadata/object.h>

#include "IMonoAPIBinding.h"

class CMonoLogging : public IMonoAPIBinding
{
public:
	CMonoLogging();
	~CMonoLogging();

private:
	static void _LogAlways(MonoString *msg);
	static void _Log(MonoString *msg);
	static void _CryWarning(MonoString *msg);
};

