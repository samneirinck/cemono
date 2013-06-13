#include "StdAfx.h"
#include "GameRules.h"

#include <IGameRulesSystem.h>
#include <IActorSystem.h>

CScriptbind_GameRules::CScriptbind_GameRules()
{
	REGISTER_METHOD(RegisterGameMode);
	REGISTER_METHOD(AddGameModeAlias);
	REGISTER_METHOD(AddGameModeLevelLocation);
	REGISTER_METHOD(SetDefaultGameMode);
}

//-----------------------------------------------------------------------------
void CScriptbind_GameRules::RegisterGameMode(mono::string gamemode)
{
	// gEnv->pGameFramework is set too late, so we'll have to set it earlier in CGameStartup::InitFramework. (gEnv->pGameFramework = m_pFramework after the ModuleInitISystem call)
	if(IGameRulesSystem *pGameRulesSystem = gEnv->pGameFramework ? gEnv->pGameFramework->GetIGameRulesSystem() : nullptr)
	{
		const char *gameModeStr = ToCryString(gamemode);

		if(!pGameRulesSystem->HaveGameRules(gameModeStr))
			pGameRulesSystem->RegisterGameRules(gameModeStr, "GameRules");
	}
}

//-----------------------------------------------------------------------------
void CScriptbind_GameRules::AddGameModeAlias(mono::string gamemode, mono::string alias)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesAlias(ToCryString(gamemode), ToCryString(alias));
}

//-----------------------------------------------------------------------------
void CScriptbind_GameRules::AddGameModeLevelLocation(mono::string gamemode, mono::string location)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesLevelLocation(ToCryString(gamemode), ToCryString(location));
}

//-----------------------------------------------------------------------------
void CScriptbind_GameRules::SetDefaultGameMode(mono::string gamemode)
{
	gEnv->pConsole->GetCVar("sv_gamerulesdefault")->Set(ToCryString(gamemode));
}

//-----------------------------------------------------------------------------
EntityId CScriptbind_GameRules::GetPlayer()
{
	return gEnv->pGame->GetIGameFramework()->GetClientActorId();
}