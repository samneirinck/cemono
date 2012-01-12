/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Collection of cached AI values to be synced to network clients.

-------------------------------------------------------------------------
History:
- 31:3:2009 : Created by Alex McCarthy

*************************************************************************/

#ifndef __CACHED_AI_VALUES_H__
#define __CACHED_AI_VALUES_H__

#include <ICachedAIValues.h>

#include <ReadabilitySound.h>

#include <CountedValue.h>

class CCachedAIValues : public ICachedAIValues
{
public:
	CCachedAIValues();

	void Init(IGameObject* pGameObject);

	VIRTUAL bool operator==(const ICachedAIValues& _rhs) const;

	VIRTUAL bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags );

	VIRTUAL int GetAlertnessState() const;
	VIRTUAL void SetAlertnessState(int iAlertnessState);

	// Readability sounds
	VIRTUAL tSoundID GetReadabilitySoundID() const;
	VIRTUAL void SetReadabilitySoundID(tSoundID readabilitySoundID);

	VIRTUAL bool HasReadabilitySoundFinished() const;
	VIRTUAL void SetReadabilitySoundFinished(bool bReadabilitySoundFinished);

	virtual const SReadabilitySoundParams* PopUnplayedReadabilitySoundParams();
	virtual void SetReadabilitySoundParams(const SReadabilitySoundParams& params);
	//~ Readability sounds

private:
	void TriggerNetUpdate();

	IGameObject* m_pGameObject;

	int m_iAlertnessState;
	CountedValue<SReadabilitySoundParams> m_readabilitySoundParams;

	tSoundID m_readabilitySoundID; // NOT NET SERIALIZED
	bool m_bReadabilitySoundFinished; // NOT NET SERIALIZED
};

#endif //__CACHED_AI_VALUES_H__
