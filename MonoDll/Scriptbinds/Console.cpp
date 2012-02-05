#include "StdAfx.h"
#include "Console.h"

#include <IMonoAssembly.h>
#include <MonoClass.h>
#include <MonoArray.h>
#include <MonoCommon.h>

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
	REGISTER_METHOD(RegisterCVarString);

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
	args.Insert(cmdName);

	gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("CryConsole")->CallMethod("OnMonoCmd", &args, true);
}

void CScriptBind_Console::RegisterCommand(mono::string cmd, mono::string desc, EVarFlags flags)
{
	gEnv->pConsole->AddCommand(ToCryString(cmd), OnMonoCmd, flags, ToCryString(desc));
}

void CScriptBind_Console::RegisterCVarFloat(mono::string name, float &val, float defaultVal, EVarFlags flags, mono::string description)
{
	gEnv->pConsole->Register(ToCryString(name), &val, defaultVal, flags, ToCryString(description));
}

void CScriptBind_Console::RegisterCVarInt(mono::string name, int &val, int defaultVal, EVarFlags flags, mono::string description)
{
	gEnv->pConsole->Register(ToCryString(name), &val, defaultVal, flags, ToCryString(description));
}

void CScriptBind_Console::RegisterCVarString(mono::string name, mono::string &val, mono::string defaultVal, EVarFlags flags, mono::string description)
{
	//gEnv->pConsole->Register(ToCryString(name), &val, ToCryString(defaultVal), flags, ToCryString(description));
}

bool CScriptBind_Console::HasCVar(mono::string name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return true;

	return false;
}

float CScriptBind_Console::GetCVarFloat(mono::string name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return pCVar->GetFVal();

	return 0.0f;
}

int CScriptBind_Console::GetCVarInt(mono::string name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return pCVar->GetIVal();

	return 0;
}

mono::string CScriptBind_Console::GetCVarString(mono::string name)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		return (mono::string )ToMonoString(pCVar->GetString());

	return (mono::string )ToMonoString("");
}

void CScriptBind_Console::SetCVarFloat(mono::string name, float val)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		pCVar->Set(val);
}

void CScriptBind_Console::SetCVarInt(mono::string name, int val)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		pCVar->Set(val);
}

void CScriptBind_Console::SetCVarString(mono::string name, mono::string val)
{
	if(ICVar *pCVar = gEnv->pConsole->GetCVar(ToCryString(name)))
		pCVar->Set(ToCryString(val));
}