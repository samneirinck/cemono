#pragma once
#include "BaseCemonoClassBinding.h"
#include "IConsole.h"

class CConsoleBinding : public BaseCemonoClassBinding
{
public:
	CConsoleBinding();
	virtual ~CConsoleBinding();

protected:
	virtual const char* GetClassName() override { return "Console"; }

	static bool _GetCVar(MonoString* cvarName);
	static MonoString* _GetCVarHelpText(MonoString* cvarName);
	static int _GetCVarValueInt(MonoString* cvarName);
	static float _GetCVarValueFloat(MonoString* cvarName);
	static MonoString* _GetCVarValueString(MonoString* cvarName);
	static void _SetCVarValueInt(MonoString* cvarName, int value);
	static void _SetCVarValueFloat(MonoString* cvarName, float value);
	static void _SetCVarValueString(MonoString* cvarName, MonoString* value);
	static int _GetCVarFlags(MonoString* cvarName);
	static void _SetCVarFlags(MonoString* cvarName, int value);
	
	static void _RegisterCVarInt(MonoString* cvarName, int defaultValue, int flags, MonoString* help);
	static void _RegisterCVarFloat(MonoString* cvarName, float defaultValue, int flags, MonoString* help);
	static void _RegisterCVarString(MonoString* cvarName, MonoString* defaultValue, int flags, MonoString* help);

private:
	static ICVar* GetCvarByName(MonoString* cvarName);
};

