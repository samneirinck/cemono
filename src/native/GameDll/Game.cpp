#include "StdAfx.h"
#include "Game.h"
#include <IFlowSystem.h>
#include "G2FlowBaseNode.h"

CGame *g_pGame = 0;
CG2AutoRegFlowNodeBase *CG2AutoRegFlowNodeBase::m_pFirst=0;
CG2AutoRegFlowNodeBase *CG2AutoRegFlowNodeBase::m_pLast=0;


CGame::CGame()
	: m_pFramework(0),
	m_pMono(0)
{
	g_pGame = this;

	GetISystem()->SetIGame( this );
}

CGame::~CGame()
{
	m_pFramework->EndGameContext();
	m_pFramework->UnregisterListener(this);
	g_pGame = 0;

	SAFE_DELETE(m_pMono);
}

bool CGame::Init(IGameFramework *pFramework)
{
	m_pFramework = pFramework;
	m_pFramework->RegisterListener(this, "Game", FRAMEWORKLISTENERPRIORITY_GAME);

	m_pMono = new CMono();

	return true;
}

void CGame::GetMemoryStatistics(ICrySizer * s)
{

}

bool CGame::CompleteInit() 
{

	if (IFlowSystem *pFlow = m_pFramework->GetIFlowSystem())
	{
		CG2AutoRegFlowNodeBase *pFactory = CG2AutoRegFlowNodeBase::m_pFirst;

		while (pFactory)
		{
			pFlow->RegisterType( pFactory->m_sClassName,pFactory );
			pFactory = pFactory->m_pNext;
		}
	}
	
	m_pMono->Init();

	return true;
}

void CGame::Shutdown()
{

}

string CGame::InitMapReloading()
{
	return "";
}

int CGame::Update(bool haveFocus, unsigned int updateFlags)
{
	bool updated = m_pFramework->PreUpdate(haveFocus, updateFlags);

	m_pFramework->PostUpdate( haveFocus, updateFlags );


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
}

void CGame::OnClearPlayerIds()
{
}

bool CGame::IsReloading()
{
	return false;
}

const char* CGame::CreateSaveGameName()
{
	return "";
}

const char* CGame::GetMappedLevelName(const char* levelName) const
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
