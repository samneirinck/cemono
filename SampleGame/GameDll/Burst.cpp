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
#include "Burst.h"
#include "Actor.h"

#include "WeaponSharedParams.h"


//------------------------------------------------------------------------
CBurst::CBurst()
{
}

//------------------------------------------------------------------------
CBurst::~CBurst()
{
}

//------------------------------------------------------------------------
void CBurst::Update(float frameTime, uint32 frameId)
{
	CSingle::Update(frameTime, frameId);

	if (m_firing)
	{
		m_bursting = true;

		if (m_next_shot <= 0.0f)
		{
			// workaround: save current burst rate, and fake it so that the CanFire check in CSingle::Shoot passes...
			float saved_next_burst=m_next_burst;
			m_next_burst=0.0f;

			if(m_pShared->burstparams.noSound)
				m_firing = Shoot(true,true,true);
			else
				m_firing = Shoot(true);
			m_burst_shot = m_burst_shot+1;

			if (!m_firing || (m_burst_shot >= m_pShared->burstparams.nshots))
			{
				m_bursting = false;
				m_firing = false;
				m_burst_shot = 1;
			}

			m_next_burst=saved_next_burst;
		}
	}

	m_next_burst -= frameTime;
	if (m_next_burst <= 0.0f)
		m_next_burst = 0.0f;
}

//------------------------------------------------------------------------
void CBurst::ResetParams(const struct IItemParamsNode *params)
{
	if(!m_fireParams->Valid())
	{
		CSingle::ResetParams(params);

		const IItemParamsNode *burst = params?params->GetChild("burst"):0;
		m_pShared->burstparams.Reset(burst);
	}
}

//------------------------------------------------------------------------
void CBurst::PatchParams(const struct IItemParamsNode *patch)
{
	if(!m_fireParams->Valid())
	{
		CSingle::PatchParams(patch);

		const IItemParamsNode *burst = patch->GetChild("burst");
		m_pShared->burstparams.Reset(burst, false);
	}
}

//------------------------------------------------------------------
void CBurst::InitSharedParams()
{
	CWeaponSharedParams * pWSP = m_pWeapon->GetWeaponSharedParams();
	assert(pWSP);

	m_fireParams	= pWSP->GetFireSharedParams("BurstData", m_fmIdx);
}

//-----------------------------------------------------------------------
void CBurst::CacheSharedParamsPtr()
{
	CSingle::CacheSharedParamsPtr();

	m_pShared			= static_cast<CBurstSharedData*>(m_fireParams.get());
}

//------------------------------------------------------------------------
void CBurst::Activate(bool activate)
{
	CSingle::Activate(activate);

	m_next_burst = 0.0f;
	m_next_burst_dt = 60.0f/(float)m_pShared->burstparams.rate;
	m_bursting = false;
	m_burst_shot = 1;
}

//------------------------------------------------------------------------
bool CBurst::CanFire(bool considerAmmo) const
{
	return CSingle::CanFire(considerAmmo) && m_next_burst<=0.0f;
}

//------------------------------------------------------------------------
void CBurst::StartFire()
{
	if (!m_bursting)
	{
		if (m_next_burst <= 0.0f)
		{
			CSingle::StartFire();

			if(m_fired) //Only set if first shot was successful
				m_next_burst = m_next_burst_dt;
		}
	}
}

//------------------------------------------------------------------------
void CBurst::StopFire()
{
	if(m_firing)
		SmokeEffect();
}

//------------------------------------------------------------------------
const char *CBurst::GetType() const
{
	return "Burst";
}