/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id:$
$DateTime$
Description:  AI grenade weapon implemenation
-------------------------------------------------------------------------
History:
- 17:01:2008: Created by Benito G.R.
		
************************************************/

#include "StdAfx.h"
#include "AIGrenade.h"

#include "Actor.h"
#include "Throw.h"


void CAIGrenade::Reset()
{
	CWeapon::Reset();

	SetOwnerId(0);
}
//--------------------------------------
struct CAIGrenade::FinishGrenadeAction
{
	FinishGrenadeAction(CWeapon *_weapon): weapon(_weapon){};
	CWeapon *weapon;

	void execute(CItem *_this)
	{
		CActor *pActor = weapon->GetOwnerActor();
		if (pActor)
		{
			CItem			*pMain = weapon->GetActorItem(pActor);
			if (pMain)
				pMain->PlayAction(g_pItemStrings->idle, 0, false, CItem::eIPAF_Default|CItem::eIPAF_NoBlend);
		}
	}
};

//-------------------------------------------------------------------------
void CAIGrenade::StartFire()
{
	CRY_ASSERT_MESSAGE(false, "CAIGrenade::StartFire Called without launch params, ignoring");
}

//-------------------------------------------------------------------------
void CAIGrenade::StartFire(const SProjectileLaunchParams& launchParams)
{
	if (!m_fm)
		return;

	m_fm->SetProjectileLaunchParams(launchParams);
	m_fm->StartFire();
	m_fm->StopFire();

	// Adjust this time value to work with the delay values in the scripts!! (must be a greater delay)
	// Schedule to revert back to main weapon.
	GetScheduler()->TimerAction(2500,
		CSchedulerAction<FinishGrenadeAction>::Create(FinishGrenadeAction(this)), false);
}
