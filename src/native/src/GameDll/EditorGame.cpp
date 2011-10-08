#include "StdAfx.h"

#include <IActorSystem.h>
#include <ILevelSystem.h>
#include <IGameRulesSystem.h>

#include "EditorGame.h"
#include "GameStartup.h"
#include "Game.h"

extern "C" 
{
	GAME_API IGameStartup *CreateGameStartup();
};

const uint16 CEditorGame::EDITOR_SERVER_PORT = 0xed17;

CEditorGame::CEditorGame() : m_pGame(0), m_pGameStartup(0), m_bEnabled(false), m_bPlayer(false), m_bGameMode(false)
{
}

CEditorGame::~CEditorGame()
{
}

bool CEditorGame::Init( ISystem *pSystem,IGameToEditorInterface *pEditorInterface )
{
	SSystemInitParams startupParams;
	startupParams.bEditor = true;
	startupParams.pSystem = pSystem;
	startupParams.bExecuteCommandLine = false;

	m_pGameStartup = CreateGameStartup();
	m_pGame = m_pGameStartup->Init(startupParams);
	if (!m_pGame)
	{
		return false;
	}

	IGameFramework* pGameFramework = m_pGame->GetIGameFramework();
	if (pGameFramework != NULL)
	{
		pGameFramework->InitEditor(pEditorInterface);
	}

	gEnv->bServer = true;
	gEnv->bMultiplayer = false;
	gEnv->SetIsClient(true);

	SetGameMode(false);
	ConfigureNetContext(true);


	return true;
}

int CEditorGame::Update(bool haveFocus, unsigned int updateFlags)
{
	return m_pGameStartup->Update(haveFocus, updateFlags);
}

void CEditorGame::Shutdown()
{
	EnablePlayer(false);
	SetGameMode(false);
	m_pGameStartup->Shutdown();
}

bool CEditorGame::SetGameMode(bool bGameMode)
{
	m_bGameMode = bGameMode;
	bool on = m_bPlayer;
	bool ok = ConfigureNetContext(on);
	if (ok)
	{
		if (gEnv->IsEditor())
		{
			m_pGame->EditorResetGame(bGameMode);
		}
		IGameFramework* pGameFramework = m_pGame->GetIGameFramework();
		pGameFramework->OnEditorSetGameMode(bGameMode);
	} else {
		GameWarning("Failed configuring net context");
	}

	return ok;
}

IEntity* CEditorGame::GetPlayer()
{
	IGameFramework* pGameFramework = m_pGame->GetIGameFramework();	

	if (!m_pGame)
	{
		return NULL;
	}
	IActor* pActor = pGameFramework->GetClientActor();
	return pActor ? pActor->GetEntity() : NULL;
}

void CEditorGame::SetPlayerPosAng(Vec3 pos,Vec3 viewDir)
{
	IActor * pClActor = m_pGame->GetIGameFramework()->GetClientActor();

	if (pClActor)
	{
		// pos coming from editor is a camera position, we must convert it into the player position by subtructing eye height.
		IEntity *myPlayer = pClActor->GetEntity();
		if (myPlayer)
		{
			pe_player_dimensions dim;
			dim.heightEye = 0;
			if (myPlayer->GetPhysics())
			{
				myPlayer->GetPhysics()->GetParams( &dim );
				pos.z = pos.z - dim.heightEye;
			}
		}

		pClActor->GetEntity()->SetPosRotScale( pos,Quat::CreateRotationVDir(viewDir),Vec3(1,1,1),ENTITY_XFORM_EDITOR );
	}
}

void CEditorGame::HidePlayer(bool bHide)
{
	IEntity * pEntity = GetPlayer();
	if (pEntity)
		pEntity->Hide( bHide );
}

void CEditorGame::OnBeforeLevelLoad()
{
	EnablePlayer(false);
	ConfigureNetContext(true);
	const char *pGameRulesName = GetGameRulesName();
	auto createdRules = m_pGame->GetIGameFramework()->GetIGameRulesSystem()->CreateGameRules(pGameRulesName);
	m_pGame->GetIGameFramework()->GetILevelSystem()->OnLoadingStart(0);
}

void CEditorGame::OnAfterLevelLoad(const char *levelName, const char *levelFolder)
{
	m_pGame->GetIGameFramework()->SetEditorLevel(levelName, levelFolder);
	ILevel* pLevel = m_pGame->GetIGameFramework()->GetILevelSystem()->SetEditorLoadedLevel(levelName);
	m_pGame->GetIGameFramework()->GetILevelSystem()->OnLoadingComplete(pLevel);

	EnablePlayer(true);
}

void CEditorGame::OnCloseLevel()
{
	// Do nothing
}

bool CEditorGame::BuildEntitySerializationList(XmlNodeRef output)
{
	return true;
}

bool CEditorGame::GetAdditionalMinimapData(XmlNodeRef output)
{
	return true;
}

IFlowSystem* CEditorGame::GetIFlowSystem()
{
	return m_pGame->GetIGameFramework()->GetIFlowSystem();
}

IGameTokenSystem* CEditorGame::GetIGameTokenSystem()
{
	return m_pGame->GetIGameFramework()->GetIGameTokenSystem();
}

IEquipmentSystemInterface* CEditorGame::GetIEquipmentSystemInterface()
{
	// TODO
	return NULL;
}

bool CEditorGame::SupportsMultiplayerGameRules()
{
	return false;
}
void CEditorGame::ToggleMultiplayerGameRules()
{

}

bool CEditorGame::ConfigureNetContext(bool on)
{
	bool ok = false;
	IGameFramework* pGameFramework = m_pGame->GetIGameFramework();

	if (on == m_bEnabled)
	{
		ok = true;
	} else if (on)
	{
		CryLogAlways("EDITOR: Set game mode: on");
		
		SGameContextParams ctx;
		
		SGameStartParams gameParams;
		gameParams.flags = eGSF_Server | eGSF_NoSpawnPlayer | eGSF_Client | eGSF_NoLevelLoading | eGSF_BlockingClientConnect | eGSF_NoGameRules | eGSF_NoQueries | eGSF_LocalOnly;
		gameParams.connectionString = "";
		gameParams.hostname = "localhost";
		gameParams.port = EDITOR_SERVER_PORT;
		gameParams.pContextParams = &ctx;
		gameParams.maxPlayers = 1;

		if (pGameFramework->StartGameContext(&gameParams))
		{
			ok = true;
		}
	} else {
		CryLogAlways("EDITOR: Set game mode: off");

		pGameFramework->EndGameContext();
		gEnv->pNetwork->SyncWithGame(eNGS_Shutdown);
		ok = true;
	}
	m_bEnabled = on && ok;
	return ok;
}

void CEditorGame::EnablePlayer(bool player)
{
	bool spawnPlayer = false;
	if (m_bPlayer != player)
	{
		spawnPlayer = m_bPlayer = player;
	}
	if (!SetGameMode(m_bGameMode))
	{
		GameWarning("Failed setting  game mode");
	} else if (m_bEnabled && spawnPlayer) {
		if (!m_pGame->GetIGameFramework()->BlockingSpawnPlayer())
		{
			GameWarning("Failed spawning player");
		}
	}
}

const char* CEditorGame::GetGameRulesName()
{
	return "CemonoRules";
}