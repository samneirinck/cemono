/*************************************************************************
	Crytek Source File.
	Copyright (C), Crytek Studios, 2001-2004.
	-------------------------------------------------------------------------
	$Id: GameRealtimeRemoveUpdate.cpp,v 1.1 23/09/2009 by Johnmichael Quinlan 
	Description:  This is the source file for the game module Realtime remote 
								update. The purpose of this module is to allow data update to happen 
								remotely so that you can, for example, edit the terrain and see the changes
								in the console.
-------------------------------------------------------------------------
History:
- 23:09:2009 :  Created by Johnmichael Quinlan
 ************************************************************************/

#include "StdAfx.h"
#include "GameRealtimeRemoteUpdate.h"
#include "LivePreview/RealtimeRemoteUpdate.h"
#include "IGameFramework.h"
#include "IViewSystem.h"
#include <IEntitySystem.h>
#include "Player.h"
#include "Game.h"
#include "IGameRulesSystem.h"
#include "ILevelSystem.h"
#include "GameCVars.h"


//0 no fly, 1 fly mode, 2 fly mode + noclip
enum eCameraModes
{
	eCameraModes_no_fly=0,
	eCameraModes_fly_mode=1,
	eCameraModes_fly_mode_noclip=2,
};

//////////////////////////////////////////////////////////////////////////
CGameRealtimeRemoteUpdateListener &	CGameRealtimeRemoteUpdateListener::GetGameRealtimeRemoteUpdateListener()
{
	static CGameRealtimeRemoteUpdateListener oRealtimeUpdateListener;
	return oRealtimeUpdateListener;
}

CGameRealtimeRemoteUpdateListener::CGameRealtimeRemoteUpdateListener()
{
	m_nPreviousFlyMode = eCameraModes_no_fly;
	m_bCameraSync = false;
	m_Position.zero();
	m_ViewDirection.zero();
	m_CameraUpdateInfo=NULL;
	m_headPositionDelta.zero();
}

CGameRealtimeRemoteUpdateListener::~CGameRealtimeRemoteUpdateListener()
{
}

bool CGameRealtimeRemoteUpdateListener::Enable(bool boEnable)
{
	if ( boEnable )
	{	
		g_pGame->GetIGameFramework()->GetIRealTimeRemoteUpdate()->Enable(true);
		g_pGame->GetIGameFramework()->GetIRealTimeRemoteUpdate()->AddGameHandler(this);
	}
	else
	{
		g_pGame->GetIGameFramework()->GetIRealTimeRemoteUpdate()->Enable(false);
		g_pGame->GetIGameFramework()->GetIRealTimeRemoteUpdate()->RemoveGameHandler(this);
	}

	return boEnable;
}

bool CGameRealtimeRemoteUpdateListener::Update()
{
	if (m_CameraUpdateInfo)
	{
		UpdateCamera(m_CameraUpdateInfo);
		m_CameraUpdateInfo=NULL;
	}

	return true;
}

bool CGameRealtimeRemoteUpdateListener::UpdateGameData(XmlNodeRef oXmlNode, unsigned char * auchBinaryData)
{
	string oSyncType = oXmlNode->getAttr("Type");

	if ( oSyncType.compare("ChangeLevel")==0 )
	{
		CloseLevel();
	}
	else if ( oSyncType.compare("Camera")==0 )
	{
		m_CameraUpdateInfo=oXmlNode;
		return false;
	}
	return true;
}

void CGameRealtimeRemoteUpdateListener::UpdateCamera(XmlNodeRef oXmlNode)
{
	int	nStartSync(0);

	if (!oXmlNode->getAttr("Position",m_Position))
		return;

	if (!oXmlNode->getAttr("Direction",m_ViewDirection))
		return;

	if (oXmlNode->getAttr("StartSync",nStartSync))
	{
		if (nStartSync)
		{				
			CameraSync();
		}
		else
		{
			EndCameraSync();
		}
	}
}

static int s_tpView = 0;
static bool s_bTpsMode = 0;
static int s_godMode = 0;

void CGameRealtimeRemoteUpdateListener::CameraSync()
{
	IGame *pGame = gEnv->pGame;
	if(!pGame)
		return;

	IGameFramework *pGameFramework=pGame->GetIGameFramework();
	if(!pGameFramework)
		return;

	IViewSystem *pViewSystem=pGameFramework->GetIViewSystem();	
	if(!pViewSystem)
		return;

	IView *pView=pViewSystem->GetActiveView();
	if(!pView)
		return;

	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pView->GetLinkedId());
	if(!pEntity)
		return;

	CPlayer *pPlayer=static_cast<CPlayer *>(pGameFramework->GetClientActor());
	if ( !pPlayer )
		return;

	IEntity * pPlayerEntity = pPlayer->GetEntity();
	if (!pPlayerEntity)
		return;

	IPhysicalEntity * piPlayerPhysics = pPlayerEntity->GetPhysics();
	if ( !piPlayerPhysics )
		return;

	pe_player_dimensions dim;
	piPlayerPhysics->GetParams( &dim );

	if (!m_bCameraSync)
	{
		s_tpView = g_pGameCVars->g_tpview_enable;
		s_bTpsMode = pPlayer->IsThirdPerson();
		s_godMode = g_pGameCVars->g_godMode;
		g_pGameCVars->g_tpview_enable = 0;
		pPlayer->SetThirdPerson(false);
	}

	//TODO: only GDCE2011, in the future make this game magic constant be gone in LiveCreate 2.0
	// game player view code has a complex position code path, the sync position should be
	// honoured by game code when live creaate camera sync is enabled
	m_Position.z -= 1.62f;
	pEntity->SetPos(m_Position);
	pPlayerEntity->Hide(false);
	pViewSystem->SetOverrideCameraRotation(true,Quat::CreateRotationVDir(m_ViewDirection));

	if ( m_bCameraSync == false && m_nPreviousFlyMode != eCameraModes_fly_mode_noclip ) 
	{
		m_nPreviousFlyMode=pPlayer->GetFlyMode();
		pPlayer->SetFlyMode(eCameraModes_fly_mode_noclip);
	}

	pPlayerEntity->Hide(true);
	SEntityUpdateContext ctx;
	pPlayer->Update( ctx, 0 );
	m_bCameraSync = true;
}

void CGameRealtimeRemoteUpdateListener::EndCameraSync()
{
	IGame *pGame = gEnv->pGame;
	if(!pGame)
		return;

	IGameFramework *pGameFramework=pGame->GetIGameFramework();								
	if(!pGameFramework)
		return;

	IViewSystem *pViewSystem=pGameFramework->GetIViewSystem();	
	if(!pViewSystem)
		return;

	IView *pView=pViewSystem->GetActiveView();
	if(!pView)
		return;

	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pView->GetLinkedId());
	if(!pEntity)
		return;

	CPlayer *pPlayer=static_cast<CPlayer *>(pGameFramework->GetClientActor());
	if (!pPlayer)
		return;

	IEntity * pPlayerEntity = pPlayer->GetEntity();
	if ( !pPlayerEntity )
		return;
	
	if (m_bCameraSync)
	{
		g_pGameCVars->g_tpview_enable = s_tpView;
		pPlayer->SetThirdPerson(s_bTpsMode);
		pPlayer->SetFlyMode(m_nPreviousFlyMode);
		pPlayer->StandUp();
		pPlayer->Revive(false);
		pPlayer->SetHealth(100);
		g_pGameCVars->g_godMode = s_godMode;
	}

	SEntityUpdateContext ctx;
	pPlayer->Update( ctx, 0 );
	pPlayerEntity->Hide(false);
	pViewSystem->SetOverrideCameraRotation(false,Quat::CreateRotationVDir(m_ViewDirection));
	pEntity->SetRotation(Quat::CreateRotationVDir(m_ViewDirection));

	m_bCameraSync = false;
	// reset our head-position delta on end camera sync
	m_headPositionDelta.zero();
}

void CGameRealtimeRemoteUpdateListener::CloseLevel()
{
	if ( m_bCameraSync )
		EndCameraSync();

	char packPath[256];
	g_pGame->GetIGameFramework()->EndGameContext();
	
	if ( !g_pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel() )
		return;

	const char * fullPath = g_pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel()->GetLevelInfo()->GetPath();

	_snprintf(packPath, sizeof(packPath), "%s\\*.pak", fullPath);
	gEnv->pCryPak->ClosePacks(packPath,0);
}

