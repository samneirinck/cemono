/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Beam Fire Mode Implementation

-------------------------------------------------------------------------
History:
- 19:12:2005   12:16 : Created by Márcio Martins

*************************************************************************/
#ifndef __BEAM_H__
#define __BEAM_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Single.h"

class CBeamSharedData;

class CBeam :
	public CSingle
{
public:
	struct SBeamEffectParams
	{
		SBeamEffectParams() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
		
			if (defaultInit)
			{
				effect[0].clear(); effect[1].clear();
				helper[0].clear(); helper[1].clear();
				scale[0]=scale[1]=1.0f;
			}

			if (params)
			{
				const IItemParamsNode *fp=params->GetChild("firstperson");
				if (fp)
				{
					effect[0] = fp->GetAttribute("effect");
					helper[0] = fp->GetAttribute("helper");
					fp->GetAttribute("scale", scale[0]);
				}

				const IItemParamsNode *tp=params->GetChild("thirdperson");
				if (tp)
				{
					effect[1] = tp->GetAttribute("effect");
					helper[1] = tp->GetAttribute("helper");
					tp->GetAttribute("scale", scale[1]);
				}

				PreLoadAssets();
			}
		};

		void PreLoadAssets()
		{
			for (int i = 0; i < 2; i++)
				gEnv->pParticleManager->FindEffect(effect[i]);
		}

		void GetMemoryUsage(ICrySizer * s) const
		{
			for (int i=0; i<2; i++)
			{
				s->Add(effect[i]);
				s->Add(helper[i]);
			}
		}

		float	scale[2];
		ItemString effect[2];
		ItemString helper[2];

	};

	struct SBeamParams
	{
		SBeamParams() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);

			ResetValue(hit_effect, "");
			ResetValue(hit_effect_scale, 1.0f);
			ResetValue(hit_decal, "");
			ResetValue(hit_decal_size, 0.45f);
      ResetValue(hit_decal_size_min, 0.f);
			ResetValue(hit_decal_lifetime, 60.0f);
			ResetValue(range,			75.0f);
			ResetValue(tick,			0.25f);
			ResetValue(ammo_tick,	0.15f);
			ResetValue(ammo_drain,2);

			PreLoadAssets();
		};

		void PreLoadAssets()
		{
			gEnv->pParticleManager->FindEffect(hit_effect);
		}
		void GetMemoryUsage(ICrySizer * s) const
		{
			s->Add(hit_effect);
			s->Add(hit_decal);
		}

		
		float		hit_effect_scale;
		float		hit_decal_size;
    float		hit_decal_size_min;
		float		hit_decal_lifetime;

		float		range;
		float		tick;
		float		ammo_tick;
		int			ammo_drain;

		ItemString	hit_decal;
		ItemString	hit_effect;

	};
	struct SBeamActions
	{
		SBeamActions() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);

			ResetValue(blast,			"blast");
			ResetValue(hit,				"hit");
		};

		ItemString	blast;
		ItemString	hit;

		void GetMemoryUsage(ICrySizer * s) const
		{
			s->Add(blast);
			s->Add(hit);
		}
	};
public:
	CBeam();
	virtual ~CBeam();

	// IFireMode
	virtual void Update(float frameTime, uint32 frameId);
	virtual void GetMemoryUsage(ICrySizer * s) const;

	virtual void ResetParams(const struct IItemParamsNode *params);
	virtual void PatchParams(const struct IItemParamsNode *patch);

	virtual void Activate(bool activate);

	//virtual bool OutOfAmmo() const;
	//virtual bool CanReload() const;

	virtual bool CanFire(bool considerAmmo = true) const;
	virtual void StartFire();
	virtual void StopFire();

	virtual void NetStartFire();
	virtual void NetStopFire();
	virtual const char *GetType() const { return "Beam"; };

	//~IFireMode

	virtual void InitSharedParams();
	virtual void CacheSharedParamsPtr();

	virtual void Serialize(TSerialize ser) {};

	virtual void DecalLine(const Vec3 &org0, const Vec3 &org1, const Vec3 &hit0, const Vec3 &hit1, float step);
	virtual void Decal(const ray_hit &rayhit, const Vec3 &dir);
  virtual void Hit(ray_hit &hit, const Vec3 &dir);
	virtual void Tick(ray_hit &hit, const Vec3 &dir);
	virtual void TickDamage(ray_hit &hit, const Vec3 &dir);
protected:

	uint32							m_effectId;
	uint32							m_hitbeameffectId;
	tSoundID					m_fireLoopId;
	tSoundID					m_hitSoundId;
	bool							m_lastHitValid;
	bool							m_remote;
	float							m_tickTimer;
	float							m_ammoTimer;
	float							m_spinUpTimer;

	Vec3							m_lastHit;
	Vec3							m_lastOrg;

	bool              m_viewFP;

private:
	CBeamSharedData*	m_pShared;

};


class CBeamSharedData: public CSingleSharedData
{
public:
	CBeamSharedData(){};
	virtual ~CBeamSharedData(){};

	virtual const char* GetDataType() const { return "BeamData"; }
	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		beamparams.GetMemoryUsage(s);
		beamactions.GetMemoryUsage(s);
		effectparams.GetMemoryUsage(s);
		hitbeameffectparams.GetMemoryUsage(s);
		CSingleSharedData::GetMemoryUsage(s);
	}

	CBeam::SBeamParams				beamparams;
	CBeam::SBeamActions				beamactions;
	CBeam::SBeamEffectParams	effectparams;
	CBeam::SBeamEffectParams	hitbeameffectparams;

};

#endif //__BEAM_H__