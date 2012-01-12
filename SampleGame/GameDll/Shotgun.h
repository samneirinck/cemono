#ifndef _SHOTGUN_H_
#define _SHOTGUN_H_

#include "Single.h"
#pragma once

class CShotgunSharedData;

class CShotgun :
	public CSingle
{
	struct BeginReloadLoop;
	struct SliderBack;
	class PartialReloadAction;
	class ReloadEndAction;
	class ScheduleReload;

public:
	typedef struct SShotgunParams
	{
		SShotgunParams() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(pellets,			10);
			ResetValue(pelletdamage,				20);
			ResetValue(spread,			.1f);
			ResetValue(partial_reload, true);
		}

		void GetMemoryUsage(ICrySizer * s) const { s->Add(*this); }

		short pellets;
		short	pelletdamage;
		float spread;
		bool partial_reload;

	} SShotgunParams;

public:
	virtual void GetMemoryUsage(ICrySizer * s) const;
	virtual void Activate(bool activate);
	virtual void Reload(int zoomed);
	virtual void StartReload(int zoomed);
	virtual void ReloadShell(int zoomed);
	virtual void EndReload(int zoomed);
	using CSingle::EndReload;
	virtual void CancelReload();
	virtual bool CanCancelReload() { return false; };

	virtual bool CanFire(bool considerAmmo) const;

	virtual bool Shoot(bool resetAnimation, bool autoreload = true , bool noSound = false );
	virtual void NetShootEx(const Vec3 &pos, const Vec3 &dir, const Vec3 &vel, const Vec3 &hit, float extra, int ph);

	virtual void ResetParams(const struct IItemParamsNode *params);
	virtual void PatchParams(const struct IItemParamsNode *patch);

	virtual const char* GetType() const;

	void InitSharedParams();
	void CacheSharedParamsPtr();

protected:

	bool					 m_reload_pump;
	bool					 m_break_reload;
	bool					 m_reload_was_broken;

	int            m_max_shells;

private:
	CShotgunSharedData*		m_pShared;
};

class CShotgunSharedData: public CSingleSharedData
{
public:
	virtual const char* GetDataType() const { return "ShotgunData"; }
	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		shotgunparams.GetMemoryUsage(s);
		CSingleSharedData::GetMemoryUsage(s);
	}

	CShotgun::SShotgunParams	shotgunparams;
};
#endif