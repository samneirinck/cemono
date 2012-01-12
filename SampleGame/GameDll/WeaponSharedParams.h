/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------

Description: Stores all shared weapon parameters (shared by class)...
							Allows for some memory savings...

-------------------------------------------------------------------------
History:
- 30:1:2008   10:54 : Benito G.R.

*************************************************************************/
#ifndef __WEAPONSHAREDPARAMS_H__
#define __WEAPONSHAREDPARAMS_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Weapon.h"
#include "IronSight.h"
#include "ItemDefinitions.h"

struct SWeaponAmmoParams
{
	SWeaponAmmoParams() : extraItems(0) {};

	void GetMemoryUsage(ICrySizer* s) const
	{
		s->AddObject(ammo);
		s->AddObject(bonusAmmo);
		s->AddObject(accessoryAmmo);
		s->AddObject(minDroppedAmmo);
		s->AddObject(capacityAmmo);
	}

	int					extraItems;
	TAmmoVector	ammo;
	TAmmoVector	bonusAmmo;
	TAmmoVector	accessoryAmmo;
	TAmmoVector	minDroppedAmmo;
	TAmmoVector	capacityAmmo;
};

class CWeaponSharedParams
{
	typedef std::map<int, _smart_ptr<IWeaponSharedData> > TSharedMap;

protected:
	mutable uint32	m_refs;
	bool					m_valid;

public:
	CWeaponSharedParams(): m_refs(0), m_valid(false) {};
	virtual ~CWeaponSharedParams();

	virtual void AddRef() const { ++m_refs; };
	virtual uint32 GetRefCount() const { return m_refs; };
	virtual void Release() const { 
		if (--m_refs <= 0)
			delete this;
	};

	virtual bool Valid() const { return m_valid; };
	virtual void SetValid(bool valid) { m_valid=valid; };

	virtual IWeaponSharedData *GetZoomSharedParams(const char* name, int zoomIdx);
	virtual IWeaponSharedData *CreateZoomParams(const char* name);
	virtual IWeaponSharedData *GetFireSharedParams(const char* name, int fireIdx);
	virtual IWeaponSharedData *CreateFireParams(const char* name);

	void GetMemoryUsage(ICrySizer *s) const;

	AIWeaponDescriptor				aiWeaponDescriptor;
	CWeapon::SAIWeaponOffset		aiWeaponOffsets;	
	SWeaponAmmoParams				ammoParams;
	TSharedMap						m_zoomParams;
	TSharedMap						m_fireParams;

private:
	void ResetInternal();

};


class CWeaponSharedParamsList
{
	typedef std::map<string, _smart_ptr<CWeaponSharedParams> > TSharedParamsMap;
public:
	CWeaponSharedParamsList() {};
	virtual ~CWeaponSharedParamsList() {};

	void Reset() { m_params.clear(); };
	CWeaponSharedParams *GetSharedParams(const char *className, bool create);

	void GetMemoryUsage(ICrySizer *s) const;

	TSharedParamsMap m_params;
};

#endif //__WEAPONSHAREDPARAMS_H__