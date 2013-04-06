#include "StdAfx.h"
#include "Sound.h"

CScriptbind_Sound::CScriptbind_Sound()
{
	REGISTER_METHOD(SetGlobalParameter);

	REGISTER_METHOD(CreateSound);
	REGISTER_METHOD(CreateLineSound);
	REGISTER_METHOD(CreateSphereSound);

	REGISTER_METHOD(Play);
	REGISTER_METHOD(Stop);

	REGISTER_METHOD(SetPosition);
}

CScriptbind_Sound::~CScriptbind_Sound()
{
}

void CScriptbind_Sound::SetGlobalParameter(mono::string sGlobalParameter, const float fValue)
{
	gEnv->pSoundSystem->SetGlobalParameter(ToCryString(sGlobalParameter), fValue);
}

ISound *CScriptbind_Sound::CreateSound(mono::string sGroupAndSoundName, uint32 nFlags, uint32 nFlagsExtended)
{
	return gEnv->pSoundSystem->CreateSound(ToCryString(sGroupAndSoundName), nFlags, nFlagsExtended);
}

ISound *CScriptbind_Sound::CreateLineSound(mono::string sGroupAndSoundName, uint32 nFlags, uint32 nFlagsExtended, Vec3 &vStart, Vec3 &vEnd)
{
	return gEnv->pSoundSystem->CreateLineSound(ToCryString(sGroupAndSoundName), nFlags, nFlagsExtended, vStart, vEnd);
}

ISound *CScriptbind_Sound::CreateSphereSound(mono::string sGroupAndSoundName, uint32 nFlags, uint32 nFlagsExtended, float fRadius)
{
	return gEnv->pSoundSystem->CreateSphereSound(ToCryString(sGroupAndSoundName), nFlags, nFlagsExtended, fRadius);
}

void CScriptbind_Sound::Play(ISound *pSound, const float fVolumeScale, const bool bForceActiveState, const bool bSetRatio, IEntitySoundProxy *pEntitySoundProxy)
{
	pSound->Play(fVolumeScale, bForceActiveState, bSetRatio, pEntitySoundProxy);
}

void CScriptbind_Sound::Stop(ISound *pSound, const ESoundStopMode eStopMode)
{
	pSound->Stop(eStopMode);
}

void CScriptbind_Sound::SetPosition(ISound *pSound, Vec3 pos)
{
	pSound->SetPosition(pos);
}