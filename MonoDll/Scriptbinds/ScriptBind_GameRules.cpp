#include "StdAfx.h"
#include "ScriptBind_GameRules.h"

#include <IGameRulesSystem.h>

REGISTER_SCRIPTBIND(CScriptBind_GameRules);

CScriptBind_GameRules::CScriptBind_GameRules()
{
	REGISTER_METHOD(RegisterGameMode);
	REGISTER_METHOD(AddGameModeAlias);
	REGISTER_METHOD(AddGameModeLevelLocation);
	REGISTER_METHOD(SetDefaultGameMode);
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::RegisterGameMode(mono::string gamemode)
{
	// gEnv->pGameFramework is set too late, so we'll have to set it earlier in CGameStartup::InitFramework. (gEnv->pGameFramework = m_pFramework after the ModuleInitISystem call)
	if(IGameRulesSystem *pGameRulesSystem = gEnv->pGameFramework ? gEnv->pGameFramework->GetIGameRulesSystem() : NULL)
	{
		if(!pGameRulesSystem->HaveGameRules(*gamemode))
			pGameRulesSystem->RegisterGameRules(*gamemode, "GameRules");
	}
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::AddGameModeAlias(mono::string gamemode, mono::string alias)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesAlias(*gamemode, *alias);
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::AddGameModeLevelLocation(mono::string gamemode, mono::string location)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesLevelLocation(*gamemode, *location);
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::SetDefaultGameMode(mono::string gamemode)
{
	gEnv->pConsole->GetCVar("sv_gamerulesdefault")->Set(*gamemode);
}