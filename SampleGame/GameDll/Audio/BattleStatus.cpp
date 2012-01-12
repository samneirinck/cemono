/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 
	G04 Audio BattleStatus 
	
-------------------------------------------------------------------------
History:
- 11:08:2008: Created by Tomas Neumann based on MarcoC code in Game02 HUD

*************************************************************************/
#include "StdAfx.h"

#include "Game.h"
#include "GameCVars.h"
#include "BattleStatus.h"

//-----------------------------------------------------------------------------------------------------


CBattleStatus::CBattleStatus() 
{
	m_fBattleStatus = 0.0f;
	m_fBattleStatusDelay = 0.0f;
}

//-----------------------------------------------------------------------------------------------------

CBattleStatus::~CBattleStatus()
{
}

//-----------------------------------------------------------------------------------------------------

void CBattleStatus::TickBattleStatus(float fValue)
{		
	m_fBattleStatus += fValue; 
	m_fBattleStatusDelay = g_pGameCVars->g_combatFadeTimeDelay;
}

//-----------------------------------------------------------------------------------------------------

void CBattleStatus::Update()
{
	if (!gEnv->pGame->GetIGameFramework()->IsGamePaused())
	{
		float delta = gEnv->pTimer->GetFrameTime();
		m_fBattleStatusDelay -= delta;
		if (m_fBattleStatusDelay <= 0.0f)
		{	
			m_fBattleStatus -= delta / (g_pGameCVars->g_combatFadeTime + 1.0f);
			m_fBattleStatusDelay = 0.0f;
		}
		m_fBattleStatus = CLAMP( m_fBattleStatus, 0.0f, 1.0f );		
	}

}

//-----------------------------------------------------------------------------------------------------

float CBattleStatus::GetBattleRange()
{
	return g_pGameCVars->g_battleRange;
}

//-----------------------------------------------------------------------------------------------------

float CBattleStatus::QueryBattleStatus()
{
	return m_fBattleStatus;
}
//-----------------------------------------------------------------------------------------------------

void CBattleStatus::Serialize(TSerialize ser)
{
	ser.Value("fBattleStatus",m_fBattleStatus);
	ser.Value("fBattleStatusDelay",m_fBattleStatusDelay);
}