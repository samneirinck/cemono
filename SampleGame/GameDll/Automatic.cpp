/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 11:9:2005   15:00 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "Automatic.h"
#include "Actor.h"

#include "WeaponSharedParams.h"


//------------------------------------------------------------------------
CAutomatic::CAutomatic()
{
	m_soundId = INVALID_SOUNDID;
}

//------------------------------------------------------------------------
CAutomatic::~CAutomatic()
{
}

//--------------------------------------------------
void CAutomatic::StartFire()
{
	CSingle::StartFire();

	if(m_soundId==INVALID_SOUNDID && !m_pShared->automaticactions.automatic_fire.empty())
		m_soundId = m_pWeapon->PlayAction(m_pShared->automaticactions.automatic_fire);
}
//------------------------------------------------------------------------
void CAutomatic::Update(float frameTime, uint32 frameId)
{
	CSingle::Update(frameTime, frameId);

	if (m_firing && CanFire(false))
		m_firing = Shoot(true);
}

//------------------------------------------------------------------------
void CAutomatic::StopFire()
{
	if (m_zoomtimeout > 0.0f)
	{
		CActor *pActor = m_pWeapon->GetOwnerActor();
		CScreenEffects *pSE = pActor?pActor->GetScreenEffects():NULL;
		if (pSE)
		{
			pSE->ResetBlendGroup(CScreenEffects::eSFX_GID_ZoomIn);

			// this is so we will zoom out always at the right speed
			//float speed = (1.0f/.1f) * (1.0f - pActor->GetScreenEffects()->GetCurrentFOV())/(1.0f - .75f);
			//speed = fabs(speed);
			float speed = 1.0f/.1f;
			//if (pActor->GetScreenEffects()->HasJobs(pActor->m_autoZoomOutID))
			//	speed = pActor->GetScreenEffects()->GetAdjustedSpeed(pActor->m_autoZoomOutID);

			pSE->ResetBlendGroup(CScreenEffects::eSFX_GID_ZoomOut);

			IBlendedEffect *fov	= CBlendedEffect<CFOVEffect>::Create(CFOVEffect(pActor->GetEntityId(),1.0f));
			IBlendType *blend		= CBlendType<CLinearBlend>::Create(CLinearBlend(1.0f));
			pSE->StartBlend(fov, blend, speed, CScreenEffects::eSFX_GID_ZoomOut);
		}
		m_zoomtimeout = 0.0f;
	}

	if(m_firing)
		SmokeEffect();

	m_firing = false;

	if(m_soundId)
	{
		m_pWeapon->StopSound(m_soundId);
		m_soundId = INVALID_SOUNDID;
	}
}

//------------------------------------------------------------------------
const char *CAutomatic::GetType() const
{
	return "Automatic";
}

//---------------------------------------------------
void CAutomatic::GetMemoryUsage(ICrySizer * s) const
{
	s->Add(*this);
	CSingle::GetMemoryUsage(s);
	if(m_useCustomParams)
		m_pShared->automaticactions.GetMemoryUsage(s);
}

//------------------------------------------------------------------------
void CAutomatic::ResetParams(const struct IItemParamsNode *params)
{
	if(!m_fireParams->Valid())
	{
		CSingle::ResetParams(params);

		const IItemParamsNode *actions = params?params->GetChild("actions"):0;

		m_pShared->automaticactions.Reset(actions);
	}
}

//------------------------------------------------------------------------
void CAutomatic::PatchParams(const struct IItemParamsNode *patch)
{
	if(!m_fireParams->Valid())
	{
		CSingle::PatchParams(patch);

		const IItemParamsNode *actions = patch->GetChild("actions");

		m_pShared->automaticactions.Reset(actions, false);
	}
}

//------------------------------------------------------------------
void CAutomatic::InitSharedParams()
{
	CWeaponSharedParams * pWSP = m_pWeapon->GetWeaponSharedParams();
	assert(pWSP);

	m_fireParams	= pWSP->GetFireSharedParams("AutomaticData", m_fmIdx);
}

//-----------------------------------------------------------------------
void CAutomatic::CacheSharedParamsPtr()
{
	CSingle::CacheSharedParamsPtr();

	m_pShared			= static_cast<CAutomaticSharedData*>(m_fireParams.get());
}