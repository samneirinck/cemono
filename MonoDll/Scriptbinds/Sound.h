/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2013.
//////////////////////////////////////////////////////////////////////////
// CryENGINE Sound scriptbind
//////////////////////////////////////////////////////////////////////////
// 04/04/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_SOUND_H__
#define __SCRIPTBIND_SOUND_H__

#include <MonoCommon.h>
#include <IMonoScriptbind.h>

class CScriptbind_Sound
	: public IMonoScriptBind
{
public:
	CScriptbind_Sound();
	~CScriptbind_Sound();

	// IMonoScriptbind
	virtual const char *GetClassName() { return "NativeSoundMethods"; }
	// ~IMonoScriptbind

	static void SetGlobalParameter(mono::string sGlobalParameter, const float fValue);

	static ISound *CreateSound(mono::string sGroupAndSoundName, uint32 nFlags, uint32 nFlagsExtended = 0);
	static ISound *CreateLineSound(mono::string sGroupAndSoundName, uint32 nFlags, uint32 nFlagsExtended, Vec3 &vStart, Vec3 &VEnd);
	static ISound *CreateSphereSound(mono::string sGroupAndSoundName, uint32 nFlags, uint32 nFlagsExtended, float fRadius);

	static void Play(ISound *pSound, const float fVolumeScale=1.0f, const bool bForceActiveState=true, const bool bSetRatio=true, IEntitySoundProxy *pEntitySoundProxy=NULL);
	static void Stop(ISound *pSound, const ESoundStopMode eStopMode=ESoundStopMode_EventFade);

	static void SetPosition(ISound *pSound, Vec3 pos);
};

#endif __SCRIPTBIND_SOUND_H__