/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Throw Fire Mode Implementation

-------------------------------------------------------------------------
History:
- 261:10:2005   15:45 : Created by Márcio Martins

*************************************************************************/
#ifndef __THROW_H__
#define __THROW_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Single.h"

class CThrowSharedData;

class CThrow : public CSingle
{
	struct StartThrowAction;
	struct ThrowAction;
		
public:
	struct SThrowActions
	{
		SThrowActions() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValueEx("throw", throwit,		"throw");
			ResetValueEx("drop", dropit, "drop");
			ResetValue(hold,		"hold");
			ResetValue(pull,		"pull");
			ResetValue(next,		"next");
		}

		ItemString throwit;
		ItemString dropit;
		ItemString hold;
		ItemString pull;
		ItemString next;

		void GetMemoryUsage(ICrySizer * s) const
		{
			s->Add(throwit);
			s->Add(dropit);
			s->Add(hold);
			s->Add(pull);
			s->Add(next);
		}
	};

	struct SThrowParams
	{
		SThrowParams() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(delay, 				0.15f);
			ResetValue(hide_ammo,			true);
			ResetValue(auto_select_last,true);

			ResetValue(hold_duration, 1.0f);
			ResetValue(hold_min_scale,1.0f);
			ResetValue(hold_max_scale,1.0f);
			ResetValue(strenght_scale, 1.5f);
		}

		void GetMemoryUsage(ICrySizer * s) const {};

		float hold_duration;
		float hold_min_scale;
		float hold_max_scale;

		float strenght_scale;

		float	delay;
		bool	hide_ammo;
		bool	auto_select_last;

	};
public:
	CThrow();
	virtual ~CThrow();

	virtual void Update(float frameTime, uint32 frameId);
	virtual void GetMemoryUsage(ICrySizer * s) const;

	virtual void ResetParams(const struct IItemParamsNode *params);
	virtual void PatchParams(const struct IItemParamsNode *patch);

	virtual void InitSharedParams();
	virtual void CacheSharedParamsPtr();

	virtual void Activate(bool activate);

	virtual bool CanFire(bool considerAmmo) const;
	virtual bool CanReload() const;

	virtual const char* GetType() const
	{
		return "Thrown";
	}

	virtual bool IsReadyToFire() const;
	virtual void StartFire();
	virtual void StopFire();

	virtual void NetStartFire();
	virtual void NetStopFire();

	virtual void SetThrowable(EntityId entityId, bool forceThrow, ISchedulerAction *action);
	virtual EntityId GetThrowable() const;

	virtual void SetProjectileLaunchParams(const SProjectileLaunchParams& launchParams);
	virtual Vec3 GetFireTarget() const;

	void SetSpeedScale(float speedScale) { m_speed_scale = speedScale; }
	void ThrowingGrenade(bool throwing) { m_usingGrenade = throwing; }

protected:
	virtual void CheckNearMisses(const Vec3 &probableHit, const Vec3 &pos, const Vec3 &dir, float range, float radius);

	virtual void CheckAmmo();
	virtual void DoThrow();
	virtual void DoDrop();

private:

	void   ThrowGrenade();
	void   ThrowObject(IEntity* pEntity, IPhysicalEntity* pPE);
	void   ThrowLivingEntity(IEntity* pEntity, IPhysicalEntity* pPE);

	bool   CheckForIntersections(IPhysicalEntity* heldEntity, Vec3 &dir);

	bool  m_usingGrenade;
	bool	m_thrown;
	bool	m_pulling;
	bool	m_throwing;
	bool	m_netfiring;
	float	m_throw_time;
	bool  m_forceNextThrow;
	
	float	m_hold_timer;
	Vec3	m_vFireTarget;

	EntityId					m_throwableId;
	ISchedulerAction	*m_throwableAction;

private:
	CThrowSharedData*	m_pShared;
};


class CThrowSharedData: public CSingleSharedData
{
public:
	CThrowSharedData(){};
	virtual ~CThrowSharedData(){};

	virtual const char* GetDataType() const { return "ThrowData"; }
	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		throwactions.GetMemoryUsage(s);
		throwparams.GetMemoryUsage(s);
		CSingleSharedData::GetMemoryUsage(s);
	}

	CThrow::SThrowActions		throwactions;
	CThrow::SThrowParams		throwparams;
};
#endif 