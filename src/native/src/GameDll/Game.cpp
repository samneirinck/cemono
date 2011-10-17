#include "StdAfx.h"
#include "Game.h"
#include "Actor.h"
#include "GameRules.h"

#include <IFlowSystem.h>
#include <IGameFramework.h>
#include <IPlayerProfiles.h>
#include <IViewSystem.h>

CGame* g_pGame = 0;

CGame::CGame()
	: m_pFramework(0)
{
	g_pGame = this;

	GetISystem()->SetIGame( this );
}

CGame::~CGame()
{
	m_pFramework->EndGameContext();
	m_pFramework->UnregisterListener(this);
	g_pGame = 0;
}

bool CGame::Init(IGameFramework *pFramework)
{
	m_pFramework = pFramework;
	m_pFramework->RegisterListener(this, "Game", FRAMEWORKLISTENERPRIORITY_GAME);

	REGISTER_FACTORY(pFramework, "Actor", CActor, false);
	REGISTER_FACTORY(pFramework, "GameRules", CGameRules, false);

	pFramework->GetIGameRulesSystem()->RegisterGameRules("CemonoRules", "GameRules");
	
	LoadActionMaps("libs/config/defaultProfile.xml");


	return true;
}

void CGame::GetMemoryStatistics(ICrySizer * s) const
{

}

bool CGame::CompleteInit() 
{

	//if (IFlowSystem *pFlow = m_pFramework->GetIFlowSystem())
	//{
	//	CG2AutoRegFlowNodeBase *pFactory = CG2AutoRegFlowNodeBase::m_pFirst;

	//	while (pFactory)
	//	{
	//		pFlow->RegisterType( pFactory->m_sClassName,pFactory );
	//		pFactory = pFactory->m_pNext;
	//	}
	//}
	
	//m_pMono->Init();

	return true;
}

void CGame::Shutdown()
{

}

int CGame::Update(bool haveFocus, unsigned int updateFlags)
{
	bool updated = m_pFramework->PreUpdate(haveFocus, updateFlags);

	if (g_pGame && g_pGame->GetIGameFramework() && g_pGame->GetIGameFramework()->GetIViewSystem() && g_pGame->GetIGameFramework()->GetIViewSystem()->GetActiveView())
	{
		auto viewParams = *g_pGame->GetIGameFramework()->GetIViewSystem()->GetActiveView()->GetCurrentParams();
		//viewParams.position = Vec3(0,0,0);
		////viewParams.rotation = Quat::CreateRotationXYZ(Ang3(-90,0,0));
		viewParams.fov = 1.3f;
		g_pGame->GetIGameFramework()->GetIViewSystem()->GetActiveView()->SetCurrentParams(viewParams);
	
		//m_pFramework->PostUpdate( haveFocus, updateFlags );
		CCamera cam = gEnv->pSystem->GetViewCamera();
		cam.SetAngles( Ang3(DEG2RAD(-90),0,0) );
		cam.SetPosition( Vec3(2048/2,2048/2,200) );
		gEnv->pSystem->SetViewCamera( cam );
	}

	m_pFramework->PostUpdate(true, updateFlags);

	
	return updated;
}

void CGame::ConfigureGameChannel(bool isServer, IProtocolBuilder *pBuilder)
{

}

void CGame::EditorResetGame(bool bStart)
{
}

void CGame::PlayerIdSet(EntityId playerId)
{

}

IGameFramework* CGame::GetIGameFramework()
{
	return m_pFramework;
}

const char* CGame::GetLongName()
{
	return "";
}

const char* CGame::GetName()
{
	return "";
}

void CGame::LoadActionMaps(const char* filename)
{
	IActionMapManager *pActionMapMan = m_pFramework->GetIActionMapManager();
	pActionMapMan->AddInputDeviceMapping(eAID_KeyboardMouse, "keyboard");
	pActionMapMan->AddInputDeviceMapping(eAID_XboxPad, "xboxpad");
	pActionMapMan->AddInputDeviceMapping(eAID_PS3Pad, "ps3pad");

	// make sure that they are also added to the GameActions.actions file!
	XmlNodeRef rootNode = m_pFramework->GetISystem()->LoadXmlFromFile(filename);
	if(rootNode)
	{
		pActionMapMan->Clear();
		pActionMapMan->LoadFromXML(rootNode);

		// enable defaults
		pActionMapMan->EnableActionMap("default",true);

		// enable debug
		pActionMapMan->EnableActionMap("debug",gEnv->pSystem->IsDevMode());

		// enable player action map
		pActionMapMan->EnableActionMap("player",true);
	}
	else
	{
		CryLogAlways("[game] error: Could not open configuration file %s", filename);
		CryLogAlways("[game] error: this will probably cause an infinite loop later while loading a map");
	}

}

void CGame::OnClearPlayerIds()
{
}

IGame::TSaveGameName CGame::CreateSaveGameName()
{
	return NULL;
}

const char* CGame::GetMappedLevelName(const char* levelName) const
{
	return "";
}

IGameStateRecorder* CGame::CreateGameStateRecorder(IGameplayListener* pL)
{
	return NULL;
}

const bool CGame::DoInitialSavegame() const
{
	return true;
}

uint32 CGame::AddGameWarning(const char* stringId, const char* paramMessage, IGameWarningsListener* pListener)
{
	return 0;
}

void CGame::RenderGameWarnings()
{
}

void CGame::RemoveGameWarning(const char* stringId)
{
}

bool CGame::GameEndLevel(const char* stringId)
{
	return true;
}

void CGame::SetUserProfileChanged(bool yesNo)
{
}

void CGame::OnRenderScene()
{
}
const uint8* CGame::GetDRMKey()
{
	return 0;
}

const char* CGame::GetDRMFileList()
{
	return "";
}


void CGame::OnPostUpdate(float fDeltaTime)
{

}

void CGame::OnSaveGame(ISaveGame* pSaveGame)
{

}

void CGame::OnLoadGame(ILoadGame* pLoadGame)
{

}

void CGame::OnLevelEnd(const char* nextLevel)
{

}

void CGame::OnActionEvent(const SActionEvent& event)
{

}

bool CGame::OnInputEvent( const SInputEvent &event )
{
	return false;
}
bool CGame::OnInputEventUI( const SInputEvent &event )
{
	return false;
}

