/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Beam Fire Mode Implementation

-------------------------------------------------------------------------
History:
- 12:1:2006   18:34 : Created by Márcio Martins

*************************************************************************/
#ifndef __FREEZINGBEAM_H__
#define __FREEZINGBEAM_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Beam.h"

class CFreezingBeamSharedData;

class CFreezingBeam :
	public CBeam
{
public:

  typedef struct SFreezingBeamParams
  {
    SFreezingBeamParams() { Reset(); };
    void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
    {
      CItemParamReader reader(params);      
      ResetValue(freeze_speed, 0.f);
    };

    void GetMemoryUsage(ICrySizer * s) const
    {
      s->Add(freeze_speed);      
    }
    
    float		freeze_speed;    
  } SFreezingBeamParams;

	CFreezingBeam();
	virtual ~CFreezingBeam();

  virtual void ResetParams(const struct IItemParamsNode *params);
  virtual void PatchParams(const struct IItemParamsNode *patch);

	virtual void Hit(ray_hit &hit, const Vec3 &dir);
	virtual void Tick(ray_hit &hit, const Vec3 &dir);

	virtual void GetMemoryUsage(ICrySizer * s) const;

	virtual void InitSharedParams();
	virtual void CacheSharedParamsPtr();

private:
	CFreezingBeamSharedData*	m_pShared;
  
};

class CFreezingBeamSharedData: public CBeamSharedData
{
public:

	CFreezingBeamSharedData(){};
	virtual ~CFreezingBeamSharedData(){};

	virtual const char* GetDataType() const { return "FreezingBeamData"; }

	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		freezeparams.GetMemoryUsage(s);
		CBeamSharedData::GetMemoryUsage(s);
	}

	CFreezingBeam::SFreezingBeamParams freezeparams;

};

#endif //__FREEZINGBEAM_H__