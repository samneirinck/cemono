#ifndef __SCRIPTBIND_GAMERULES_H__
#define __SCRIPTBIND_GAMERULES_H__

#include "Headers\IMonoScriptBind.h"

class CScriptBind_GameRules : public IMonoScriptBind
{
public:
	CScriptBind_GameRules();
	~CScriptBind_GameRules() {}

protected:
	// IMonoScriptBind
	virtual const char* GetClassName() { return "GameRules"; }
	// ~IMonoScriptBind

	static void RegisterGameMode(MonoString *gamemode);
	static void AddGameModeAlias(MonoString *gamemode, MonoString *alias);
	static void AddGameModeLevelLocation(MonoString *gamemode, MonoString *location);
	static void SetDefaultGameMode(MonoString *gamemode);

	static EntityId SpawnPlayer(int channelId, MonoString *name, MonoString *className, Vec3 pos, Vec3 angles);
};

#endif //__SCRIPTBIND_GAMERULES_H__