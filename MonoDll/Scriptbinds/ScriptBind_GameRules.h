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
#include <IMonoAutoRegScriptBind.h>

class CScriptBind_GameRules : public IMonoAutoRegScriptBind
{
public:
	CScriptBind_GameRules();
	~CScriptBind_GameRules() {}

	// IMonoScriptBind
	virtual void Release() { delete this; }

	virtual const char *GetClassName() { return "GameRulesSystem"; }
	// ~IMonoScriptBind

	MonoMethod(void, RegisterGameMode, mono::string);
	MonoMethod(void, AddGameModeAlias, mono::string, mono::string);
	MonoMethod(void, AddGameModeLevelLocation, mono::string, mono::string);
	MonoMethod(void, SetDefaultGameMode, mono::string);
};

#endif //__SCRIPTBIND_GAMERULES_SYSTEM__