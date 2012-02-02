/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// CryConsole scriptbind; used for logging and CVars / CCommands
//////////////////////////////////////////////////////////////////////////
// 20/11/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __LOGGING_BINDING_H__
#define __LOGGING_BINDING_H__

#include <IMonoAutoRegScriptBind.h>

#include <MonoCommon.h>

class CScriptBind_Console : public IMonoAutoRegScriptBind
{
public:
	CScriptBind_Console();
	virtual ~CScriptBind_Console() {}

protected:
	// IMonoScriptBind
	virtual void Release() { delete this; }

	virtual const char* GetClassName() { return "Console"; }
	// ~IMonoScriptBind

	// Logging
	//static void LogAlways(mono::string msg);
	//static void Log(mono::string msg);
	//static void Warning(mono::string msg);

	// Console commands
	MonoMethod(void, OnMonoCmd, IConsoleCmdArgs *);
	MonoMethod(void, RegisterCommand, mono::string, mono::string, EVarFlags);

	// CVars
	MonoMethod(void, RegisterCVarFloat, mono::string, float&, float, EVarFlags, mono::string);
	MonoMethod(void, RegisterCVarInt, mono::string, int&, int, EVarFlags, mono::string);
	MonoMethod(void, RegisterCVarString, mono::string, mono::string &, mono::string , EVarFlags, mono::string);

	MonoMethod(bool, HasCVar, mono::string);

	MonoMethod(float, GetCVarFloat, mono::string);
	MonoMethod(int, GetCVarInt, mono::string);
	MonoMethod(mono::string, GetCVarString, mono::string);

	MonoMethod(void, SetCVarFloat, mono::string, float);
	MonoMethod(void, SetCVarInt, mono::string, int);
	MonoMethod(void, SetCVarString, mono::string, mono::string);
};

#endif //__LOGGING_BINDING_H__