/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id:$
$DateTime$
Description:  AI grenade weapon
-------------------------------------------------------------------------
History:
- 17:01:2008: Created by Benito G.R.
							Split from OffHand to a separate, smaller and simpler class

*************************************************************************/

#ifndef __AIGRENADE_H__
#define __AIGRENADE_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Weapon.h"

class CAIGrenade : public CWeapon
{
	struct FinishGrenadeAction;

public:

	virtual ~CAIGrenade() {};

	//IItem, IGameObject
	virtual void Reset();

	//~IItem

	//IWeapon
	virtual void StartFire();
	virtual void StartFire(const SProjectileLaunchParams& launchParams);
	virtual int  GetAmmoCount(IEntityClass* pAmmoType) const { return 1; } //Always has ammo

};


#endif // __AIGRENADE_H__