/********************************************************************
CryGame Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------

-------------------------------------------------------------------------
History:
- 3:4:2009   Extracted from AIReadabilityManager.h by Alex McCarthy

*********************************************************************/


#ifndef __READABILITY_SOUND_H__
#define __READABILITY_SOUND_H__

#include <IEntity.h>

struct SReadabilitySoundEntry
{
	string	m_fileName;
	bool		voice;
	int			m_probability;

	SReadabilitySoundEntry()
		: m_fileName(), m_probability(1), voice(false) {}
	SReadabilitySoundEntry(string& fileName, int probability, bool voice)
		: m_fileName(fileName), m_probability(probability), voice(voice) {}

	void Serialize(TSerialize ser)
	{
		ser.Value("m_fileName", m_fileName, 'stab');
		ser.Value("voice", voice, 'bool');
		// Don't serialize m_probability, since it's not needed on the client
	}
};

struct SReadabilitySoundParams
{
	SReadabilitySoundParams()
		: m_soundEntry(), m_bPlaySoundAtActorTarget(false), m_bStopPreviousSound(false)
	{
	}

	SReadabilitySoundParams(const SReadabilitySoundEntry& soundEntry,
		const Vec3& vSoundPosition, bool bPlaySoundAtActorTarget, bool bStopPreviousSound)
		: m_soundEntry(soundEntry), m_bPlaySoundAtActorTarget(bPlaySoundAtActorTarget)
		, m_vSoundPosition(vSoundPosition), m_bStopPreviousSound(bStopPreviousSound)
	{
	}

	SReadabilitySoundEntry m_soundEntry;
	Vec3 m_vSoundPosition;
	bool m_bPlaySoundAtActorTarget;
	bool m_bStopPreviousSound;

	void Serialize(TSerialize ser)
	{
		ser.Value("vSoundPosition", m_vSoundPosition, 'wrld');
		m_soundEntry.Serialize(ser);
		ser.Value("bPlaySoundAtActorTarget", m_bPlaySoundAtActorTarget, 'bool');
		ser.Value("bStopPreviousSound", m_bStopPreviousSound, 'bool');
	}
};

#endif //__READABILITY_SOUND_H__
