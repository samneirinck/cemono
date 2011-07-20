#pragma once
#include "IMonoAPIBinding.h"
#include "CemonoCVar.h"
#include "IConsole.h"

class CConsoleBinding : public IMonoAPIBinding
{
public:
	CConsoleBinding();
	virtual ~CConsoleBinding();

protected:
	static CCemonoCVar* _GetCVar(MonoString* cvarName);
	static MonoString* _GetCVarHelpText(MonoString* cvarName);

private:
	static ICVar* GetCvarByName(MonoString* cvarName);
};

