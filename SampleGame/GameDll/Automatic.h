/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: C++ Item Implementation

-------------------------------------------------------------------------
History:
- 11:9:2004   15:00 : Created by Márcio Martins

*************************************************************************/
#ifndef __AUTOMATIC_H__
#define __AUTOMATIC_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Single.h"

class CAutomaticSharedData;

class CAutomatic : public CSingle
{
public:
	typedef struct SAutomaticActions
	{
		SAutomaticActions() { Reset(); };
		void Reset(const IItemParamsNode *params=0, bool defaultInit=true)
		{
			CItemParamReader reader(params);
			ResetValue(automatic_fire,"automatic_fire");
		}

		void GetMemoryUsage(ICrySizer * s) const
		{
			s->Add(automatic_fire);
		}

		ItemString automatic_fire;

	} SAutomaticActions;

public:
	CAutomatic();
	virtual ~CAutomatic();

	// CSingle

	virtual void ResetParams(const struct IItemParamsNode *params);
	virtual void PatchParams(const struct IItemParamsNode *patch);

	virtual void GetMemoryUsage(ICrySizer * s) const;

	virtual void Update(float frameTime, uint32 frameId);
	virtual void StartFire();
	virtual void StopFire();
	virtual const char *GetType() const;

	virtual void InitSharedParams();
	virtual void CacheSharedParamsPtr();
	// ~CSingle

protected:

	uint32							m_soundId;

private:

	CAutomaticSharedData*	m_pShared;
};

class CAutomaticSharedData: public CSingleSharedData
{
public:
	CAutomaticSharedData(){};
	virtual ~CAutomaticSharedData(){};

	virtual const char* GetDataType() const { return "AutomaticData"; }
	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
		automaticactions.GetMemoryUsage(s);
		CSingleSharedData::GetMemoryUsage(s);
	}

	CAutomatic::SAutomaticActions	automaticactions;
};

#endif //__AUTOMATIC_H__