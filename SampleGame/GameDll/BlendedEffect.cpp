/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
History:
- 23:1:2008   Created by Benito G.R. - Refactor'd from John N. ScreenEffects.h/.cpp

*************************************************************************/

#include "StdAfx.h"
#include "BlendedEffect.h"

#include "Player.h"

//-------------FOV EFFECT-------------------------------

CFOVEffect::CFOVEffect(EntityId ownerID, float goalFOV)
{
	m_currentFOV = m_startFOV = m_goalFOV = goalFOV;
	m_ownerID = ownerID;
}

//---------------------------------
void CFOVEffect::Init()
{
	IActor *client = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_ownerID);
	if (client && client->IsPlayer())
	{
		CPlayer *player = (CPlayer *)client;
		SActorParams *params = player->GetActorParams();
		m_startFOV = params->viewFoVScale;
		m_currentFOV = m_startFOV;
	}
}

//---------------------------------
void CFOVEffect::Update(float point)
{
	m_currentFOV = (point * (m_goalFOV - m_startFOV)) + m_startFOV;
	IActor *client = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_ownerID);
	if (client && client->IsPlayer())
	{
		CPlayer *player = (CPlayer *)client;
		SActorParams *params = player->GetActorParams();
		params->viewFoVScale = m_currentFOV;
	}
}

//-------------------POST PROCESS FX--------------------

CPostProcessEffect::CPostProcessEffect(EntityId ownerID, string paramName, float goalVal)
{
	m_paramName = paramName;
	m_goalVal = goalVal;
	m_ownerID = ownerID;
}

//---------------------------------
void CPostProcessEffect::Init()
{
	IActor *client = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_ownerID);
	if (client && client->IsClient())
	{
		gEnv->p3DEngine->GetPostEffectParam(m_paramName, m_currentVal);
		m_startVal = m_currentVal;
	}
}

//---------------------------------
void CPostProcessEffect::Update(float point)
{
	IActor *client = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_ownerID);
	if (client && client->IsClient())
	{
		m_currentVal = (point * (m_goalVal - m_startVal)) + m_startVal;
		gEnv->p3DEngine->SetPostEffectParam(m_paramName, m_currentVal);
	}
}