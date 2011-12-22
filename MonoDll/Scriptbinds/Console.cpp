#include "StdAfx.h"
#include "Console.h"

#include "MonoClass.h"
#include "MonoString.h"
#include "MonoArray.h"

#include "MonoScriptSystem.h"

CScriptBind_Console::CScriptBind_Console()
{
	// Logging
	//REGISTER_METHOD(LogAlways);
	//REGISTER_METHOD(Log);
	//REGISTER_METHOD(Warning);

	// Console commands
	REGISTER_METHOD(RegisterCommand);

	// CVars
	REGISTER_METHOD(RegisterCVarFloat);
	REGISTER_METHOD(RegisterCVarInt);
	//REGISTER_METHOD(RegisterCVarString);

	REGISTER_METHOD(HasCVar);

	REGISTER_METHOD(GetCVarFloat);
	REGISTER_METHOD(GetCVarInt);
	REGISTER_METHOD(GetCVarString);

	REGISTER_METHOD(SetCVarFloat);
	REGISTER_METHOD(SetCVarInt);
	REGISTER_METHOD(SetCVarString);
}

extern "C"
{
	_declspec(dllexport) void __cdecl _LogAlways(const char *msg)
	{
		CryLogAlways(msg);
	}

	_declspec(dllexport) void __cdecl _Log(const char *msg)
	{
		CryLog(msg);
	}

	_declspec(dllexport) void __cdecl _Warning(const char *msg)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, msg); 
	}
}

void CScriptBind_Console::OnMonoCmd(IConsoleCmdArgs *cmdArgs)
{
	string cmdLine = cmdArgs->GetCommandLine();

	int start = 0;
	string cmdName = cmdLine.Tokenize(" ", start);

	CMonoArray args(1);
	args.InsertString(cmdName);

	static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetCustomClass("CryConsole")->CallMethod("OnMonoCmd", &args, true);
}

void CScriptBind_Console::RegisterCommand(MonoString *cmd, MonoString  *desc, EVarFlags flags)
{
	gEnv->pConsole->AddCommand(ToCryString(cmd), OnMonoCmd, flags, ToCryString(desc));
}

void CScriptBind_Console::RegisterCVarFloat(MonoString *name, float &val, float defaultVal, EVarFlags flags, MonoString *description)
{
	gEnv->pConsole->Register(ToCryString(name), &val, defaultVal, flags, ToCryString(description));
}

void CScriptBind_Console::RegisterCVarInt(MonoString *name, int &val, int defaultVal, EVarFlags flags, MonoString *description)
{
	gEnv->pConsole->Register(ToCryString(name), &val, defaultVal, flags, ToCryString(description));
}
/*
void CScriptBind_Console::RegisterCVarString(MonoString *name, MonoString *&val, MonoString *defaultVal, EVarFlags flags, MonoString *description)
{
	gEnv->pConsole->Register(ToCryString(name), &ToCryString(val), ToCryString(defaultVal), flags, ToCryString(description));
}*/

bool CScriptBind_Console::HasCVar(MonoString *name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return true;

	return false;
}

float CScriptBind_Console::GetCVarFloat(MonoString *name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return pCVar->GetFVal();

	return 0.0f;
}

int CScriptBind_Console::GetCVarInt(MonoString *name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return pCVar->GetIVal();

	return 0;
}

MonoString *CScriptBind_Console::GetCVarString(MonoString *name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return ToMonoString(pCVar->GetString());

	return ToMonoString("");
}

void CScriptBind_Console::SetCVarFloat(MonoString *name, float val)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		pCVar->Set(val);
}

void CScriptBind_Console::SetCVarInt(MonoString *name, int val)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		pCVar->Set(val);
}

void CScriptBind_Console::SetCVarString(MonoString *name, MonoString *val)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		pCVar->Set(ToCryString(val));
}