/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Collection of cached AI values to be synced to network clients.

-------------------------------------------------------------------------
History:
- 26:3:2009 : Created by Alex McCarthy

*************************************************************************/
#include DEVIRTUALIZE_HEADER_FIX(ICachedAIValues.h)

#ifndef __I_CACHED_AI_VALUES_H__
#define __I_CACHED_AI_VALUES_H__

#include "IGameObject.h"

UNIQUE_IFACE struct ICachedAIValues
{
	virtual ~ICachedAIValues() {}

	virtual bool operator==(const ICachedAIValues& rhs) const = 0;
	ILINE bool operator!=(const ICachedAIValues& rhs) const
	{
		return !operator==(rhs);
	}

	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) = 0;

	virtual int GetAlertnessState() const = 0;
	virtual void SetAlertnessState(int iAlertnessState) = 0;

	// Readability sounds
	virtual tSoundID GetReadabilitySoundID() const = 0;
	virtual void SetReadabilitySoundID(tSoundID readabilitySoundID) = 0;

	virtual bool HasReadabilitySoundFinished() const = 0;
	virtual void SetReadabilitySoundFinished(bool bReadabilitySoundFinished) = 0;
	//~ Readability sounds
};

#endif //__I_CACHED_AI_VALUES_H__
