/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Single-shot Fire Mode Implementation

-------------------------------------------------------------------------
History:
- 11:9:2004   15:00 : Created by Márcio Martins

*************************************************************************/
#ifndef __CHARGE_H__
#define __CHARGE_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Automatic.h"

class CChargeSharedData;

class CCharge :
	public CAutomatic
{
public:
	typedef struct SChargeParams
	{
		SChargeParams() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(time,						0.5f);
			ResetValue(max_charges,			1);
			ResetValue(shoot_on_stop,		false);
			ResetValue(reset_spinup,		false);
		};

		void GetMemoryUsage(ICrySizer * s) const{}

		float		time;
		int			max_charges;
		bool		shoot_on_stop;
		bool		reset_spinup;
	} SChargeParams;

	typedef struct SChargeActions
	{
		SChargeActions() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(charge,	"charge");
			ResetValue(uncharge,"uncharge");
		};

		void GetMemoryUsage(ICrySizer * s) const
		{
			s->Add(charge);
			s->Add(uncharge);
		}

		ItemString charge;
		ItemString uncharge;

	} SChargeActions;

public:
	CCharge();
	virtual ~CCharge();

	virtual void Update(float frameTime, uint32 frameId);
	virtual void GetMemoryUsage(ICrySizer * s) const;

	virtual void ResetParams(const struct IItemParamsNode *params);
	virtual void PatchParams(const struct IItemParamsNode *patch);

	virtual void Activate(bool activate);

	virtual void StopFire();

	virtual bool Shoot(bool resetAnimation, bool autoreload, bool noSound /* =false */);

	virtual void ChargeEffect(bool attach);
	virtual void ChargedShoot();

	virtual void InitSharedParams();
	virtual void CacheSharedParamsPtr();

protected:

	int							m_charged;
	bool						m_charging;
	float						m_chargeTimer;
	bool						m_autoreload;

	uint32					m_chId;
	uint32					m_chlightId;
	float					m_chTimer;

private:
	CChargeSharedData* m_pShared;
};


class CChargeSharedData: public CAutomaticSharedData
{
public:
	CChargeSharedData(){};
	virtual ~CChargeSharedData(){};

	virtual const char* GetDataType() const { return "ChargeData"; }
	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		chargeeffect.GetMemoryUsage(s);
		chargeparams.GetMemoryUsage(s);
		chargeactions.GetMemoryUsage(s);
		CAutomaticSharedData::GetMemoryUsage(s);
	}

	CSingle::SEffectParamsEx	chargeeffect;	
	CCharge::SChargeParams		chargeparams;
	CCharge::SChargeActions		chargeactions;
};

#endif //__CHARGE_H__