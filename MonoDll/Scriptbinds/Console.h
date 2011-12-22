#ifndef __LOGGING_BINDING_H__
#define __LOGGING_BINDING_H__

#include "Headers\IMonoScriptBind.h"

class CScriptBind_Console : public IMonoScriptBind
{
public:
	CScriptBind_Console();
	virtual ~CScriptBind_Console() {}

protected:
	// IMonoScriptBind
	virtual const char* GetClassName() { return "CryConsole"; }
	// ~IMonoScriptBind

	// Logging
	//static void LogAlways(MonoString *msg);
	//static void Log(MonoString *msg);
	//static void Warning(MonoString *msg);

	// Console commands
	static void OnMonoCmd(IConsoleCmdArgs *cmdArgs);
	static void RegisterCommand(MonoString *cmd, MonoString  *desc, EVarFlags flags);

	// CVars
	static void RegisterCVarFloat(MonoString *name, float &val, float defaultVal, EVarFlags flags, MonoString *description);
	static void RegisterCVarInt(MonoString *name, int &val, int defaultVal, EVarFlags flags, MonoString *description);
	//static void RegisterCVarString(MonoString *name, MonoString *&val, MonoString *defaultVal, EVarFlags flags, MonoString *description);

	static bool HasCVar(MonoString *name);

	static float GetCVarFloat(MonoString *name);
	static int GetCVarInt(MonoString *name);
	static MonoString *GetCVarString(MonoString *name);

	static void SetCVarFloat(MonoString *name, float val);
	static void SetCVarInt(MonoString *name, int val);
	static void SetCVarString(MonoString *name, MonoString *val);
};

#endif //__LOGGING_BINDING_H__