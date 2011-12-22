#include "StdAfx.h"
#include "GameRulesSystem.h"

#include "MonoString.h"
#include <IGameFramework.h>
#include <IGameRulesSystem.h>

CScriptBind_GameRules::CScriptBind_GameRules()
{
	REGISTER_METHOD(RegisterGameMode);
	REGISTER_METHOD(AddGameModeAlias);
	REGISTER_METHOD(AddGameModeLevelLocation);
	REGISTER_METHOD(SetDefaultGameMode);

	REGISTER_METHOD(SpawnPlayer);
}

void CScriptBind_GameRules::RegisterGameMode(MonoString *gamemode)
{
	// gEnv->pGameFramework is set too late, so we'll have to set it earlier in CGameStartup::InitFramework. (gEnv->pGameFramework = m_pFramework after the ModuleInitISystem call)
	if(IGameRulesSystem *pGameRulesSystem = gEnv->pGameFramework ? gEnv->pGameFramework->GetIGameRulesSystem() : NULL)
	{
		if(!pGameRulesSystem->HaveGameRules(ToCryString(gamemode)))
			pGameRulesSystem->RegisterGameRules(ToCryString(gamemode), "GameRules");
	}
}

void CScriptBind_GameRules::AddGameModeAlias(MonoString *gamemode, MonoString *alias)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesAlias(ToCryString(gamemode), ToCryString(alias));
}

void CScriptBind_GameRules::AddGameModeLevelLocation(MonoString *gamemode, MonoString *location)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesLevelLocation(ToCryString(gamemode), ToCryString(location));
}

void CScriptBind_GameRules::SetDefaultGameMode(MonoString *gamemode)
{
	gEnv->pConsole->GetCVar("sv_gamerulesdefault")->Set(ToCryString(gamemode));
}

// TODO: Create Scriptbind in GameDLL and make use of CGameRules::SpawnPlayer, lets not duplicate base logic. -i59
#include <IActorSystem.h>
EntityId CScriptBind_GameRules::SpawnPlayer(int channelId, MonoString *name, MonoString *className, Vec3 pos, Vec3 angles)
{
	if (!gEnv->bServer)
		return 0;

	return gEnv->pGameFramework->GetIActorSystem()->CreateActor(channelId, ToCryString(name), ToCryString(className), pos, Quat(Ang3(angles)), Vec3(1, 1, 1))->GetEntityId();
}