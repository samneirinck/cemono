/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// GameRules scriptbind, handles GameRulesSystem tasks which shouldn't
// be in the GameDll. (As these should always be present)
//////////////////////////////////////////////////////////////////////////
// 13/01/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_GAMERULES_SYSTEM__
#define __SCRIPTBIND_GAMERULES_SYSTEM__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

class CScriptBind_GameRules : public IMonoScriptBind
{
public:
	CScriptBind_GameRules();
	~CScriptBind_GameRules() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "GameRulesSystem"; }
	// ~IMonoScriptBind

	static void RegisterGameMode(mono::string);
	static void AddGameModeAlias(mono::string, mono::string);
	static void AddGameModeLevelLocation(mono::string, mono::string);
	static void SetDefaultGameMode(mono::string);
};

#endif //__SCRIPTBIND_GAMERULES_SYSTEM__