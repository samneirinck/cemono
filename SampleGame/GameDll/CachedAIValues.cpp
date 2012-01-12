/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Collection of cached AI values to be synced to network clients.

-------------------------------------------------------------------------
History:
- 31:3:2009 : Created by Alex McCarthy

*************************************************************************/

#include "StdAfx.h"
#include "CachedAIValues.h"

static const int ASPECT_AI_VALUES			= eEA_GameServerStatic;

CCachedAIValues::CCachedAIValues()
	: m_pGameObject(NULL)
	, m_iAlertnessState(-1)
	, m_readabilitySoundID(INVALID_SOUNDID)
	, m_bReadabilitySoundFinished(false)
	, ICachedAIValues()
{
}

void CCachedAIValues::Init(IGameObject* pGameObject)
{
	m_pGameObject = pGameObject;
}

bool CCachedAIValues::operator==(const ICachedAIValues& _rhs) const
{
	const CCachedAIValues& rhs = static_cast<const CCachedAIValues&>(_rhs);

	return ((m_iAlertnessState == rhs.m_iAlertnessState) &&
		(m_readabilitySoundID == rhs.m_readabilitySoundID));
}

bool CCachedAIValues::NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags )
{
	if (aspect == ASPECT_AI_VALUES)
	{
		ser.Value("alertState", m_iAlertnessState, 'ui3');
		ser.Value("readabilitySound", m_readabilitySoundParams);
	}

	return true;
}

int CCachedAIValues::GetAlertnessState() const
{
	return m_iAlertnessState;
}

void CCachedAIValues::SetAlertnessState(int iAlertnessState)
{
	if(iAlertnessState == m_iAlertnessState)
		return;

	TriggerNetUpdate();
	m_iAlertnessState = iAlertnessState;
}

tSoundID CCachedAIValues::GetReadabilitySoundID() const
{
	return m_readabilitySoundID;
}

void CCachedAIValues::SetReadabilitySoundID(tSoundID readabilitySoundID)
{
	m_readabilitySoundID = readabilitySoundID;
}

bool CCachedAIValues::HasReadabilitySoundFinished() const
{
	return m_bReadabilitySoundFinished;
}
void CCachedAIValues::SetReadabilitySoundFinished(bool bReadabilitySoundFinished)
{
	m_bReadabilitySoundFinished = bReadabilitySoundFinished;
}

const SReadabilitySoundParams* CCachedAIValues::PopUnplayedReadabilitySoundParams()
{
	return m_readabilitySoundParams.GetLatestValue();
}
void CCachedAIValues::SetReadabilitySoundParams(const SReadabilitySoundParams& params)
{
	TriggerNetUpdate();
	m_readabilitySoundParams.SetAndDirty(params);
}

void CCachedAIValues::TriggerNetUpdate()
{
	CRY_ASSERT(m_pGameObject);
	if(m_pGameObject)
	{
		m_pGameObject->ChangedNetworkState(ASPECT_AI_VALUES);
	}
}
