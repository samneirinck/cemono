/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera input helper, refactored from Camera code in HeroInput
-------------------------------------------------------------------------
History:
- 13:08:2008	Created by Jan Müller
- 21:08_2008  Modified by Jan Müller : camera pitch/yaw are fully managed in this class

*************************************************************************/

#ifndef __CAMERA_INPUT_HELPER_H__
#define __CAMERA_INPUT_HELPER_H__

#include "GameCVars.h"

//pitch scale constant for non-follow mode
static const float CAMERA_PITCH_SCALE = 2.5f;

class CPlayer;
class CPlayerInput;

class CCameraInputHelper
{
	//only the view should access the deltaYaw/Pitch
	friend class CHeroView;
	friend class CPlayerView;
	friend class CCameraView;

public:
	CCameraInputHelper(CPlayer *pHero, CPlayerInput *pHeroInput);
	~CCameraInputHelper();

	//updates camera controls
	void UpdateCameraInput(Ang3 &deltaRotation, const float frameTimeClamped, const float frameTimeNormalised);
	//computes and sets final pitch input
	void PostUpdate(const float fFrameTime, const float fDeltaPitch);
	//reset inputs
	void Reset();

	//time of the last user yaw/pitch change
	float GetLastUserInputTime() const { return m_fLastUserInput; }

	//is the camera interpolating to a target ?
	bool HasInterpolationTarget() const { return m_bInterpolationTargetActive; }
	//snap camera direction (in nav mode) to player direction
	void SnapToPlayerDir();
	//interpolate camera dir to target yaw and pitch with speedInRad/second
	void SetInterpolationTarget(float yaw, float pitch, float speedInRad = gf_PI*2.0f, float timeOut = 5.0f, float maxError = 0.2f);

	//return current camera pitch in rad
	float GetPitch() const { return m_fPitch; }
	//there is a delta-pitch (in follow mode)
	bool HasPitchChanged() const { return m_bPitchModified; }
	//overwrite pitch
	void SetPitch(const float pitch) { m_fPitch = pitch; }
	//overwrite pitch delta, causing turn
	void SetPitchDelta(const float pitch);

	//return current camera yaw in rad
	float GetYaw() const { return m_fYaw; }
	//there is a delta-yaw (in follow mode)
	bool HasYawChanged() const { return m_bYawModified; }
	//set current yaw 
	void SetYaw(const float yaw);
	//overwrite yaw delta, causing turn
	void SetYawDelta(const float yaw);

	//force a certain camera direction
	void ForceCameraDirection(float fYaw, float fPitch, float fTime);
	//get forced camera direction
	bool GetForcedDirection(float &fYaw, float &fPitch);
	//force overwrite direction active
	bool HasForceDirection() const { return m_fForceSettings > 0.0f; }

	//set tracking delta for non-follow cam
	void SetTrackingDelta(const float deltaYaw, const float deltaPitch);

	//cap angle to -pi .. pi
	void ClampPiRange(float &angleRad) const;

	//controls turning combat mode on/off
	void CombatModeChanged();

private:

	//get current input pitch delta (and remove modified flag)
	float RetrievePitchDelta();
	//get current input yaw delta (and remove modified flag)
	float RetrieveYawDelta();

	//update "interpolate to target"
	bool UpdateTargetInterpolation();

	//update pitch and yaw input
	void UpdatePitchYawDriver(float stickMag, float frameTimeNormalised);

	//hero
	CPlayer				*m_pHero;
	//owner/HeroInput
	CPlayerInput	*m_pHeroInput;

	//this is the interpolated pitch input
	float				m_fPitch;
	float				m_fTrackingPitchDelta;
	float				m_fInputPitchDelta;
	bool				m_bPitchModified;
	//this is the interpolated yaw input
	float				m_fYaw;
	float				m_fTrackingYawDelta;
	float				m_fInputYawDelta;
	bool				m_bYawModified;
	//when did the user change the yaw/pitch the last time
	float				m_fLastUserInput;
	//this is for target interpolation
	float				m_fInterpolationTargetYaw;
	float				m_fInterpolationTargetPitch;
	float				m_fInterpolationTargetSpeed;
	float				m_fInterpolationTargetTimeout;
	float				m_fInterpolationTargetMaxError;
	bool				m_bInterpolationTargetActive;
	//nav/combat mode change
	bool				m_bNavCombatModeChanged;
	//auto tracking switch
	bool				m_bAutoTracking;
	//force overwrite
	float				m_fForceYaw;
	float				m_fForcePitch;
	float				m_fForceSettings;
};

// INLINES

inline float CCameraInputHelper::RetrievePitchDelta()
{ 
	m_bPitchModified = false;
	float fDelta = m_fInputPitchDelta;
	m_fInputPitchDelta = 0.0f;
	return fDelta; 
}

inline float CCameraInputHelper::RetrieveYawDelta()
{ 
	m_bYawModified = false;
	float fDelta = m_fInputYawDelta;
	m_fInputYawDelta = 0.0f;
	return fDelta; 
}

inline void CCameraInputHelper::SetPitchDelta(const float pitch)
{
	m_fInputPitchDelta = pitch;
	m_bPitchModified = true;
}

inline void CCameraInputHelper::SetYawDelta(const float yaw)
{
	m_fInputYawDelta = yaw;
	m_bYawModified = true;
}

inline void CCameraInputHelper::SetTrackingDelta(const float deltaYaw, const float deltaPitch)
{
	m_fTrackingYawDelta = deltaYaw;
	m_fTrackingPitchDelta = deltaPitch;
}

inline void CCameraInputHelper::ClampPiRange(float &angleRad) const
{
	if(angleRad > gf_PI)
		angleRad = -(gf_PI2 - angleRad);
	else if(angleRad < -gf_PI)
		angleRad = gf_PI2 + angleRad;
}

inline void CCameraInputHelper::ForceCameraDirection(float fYaw, float fPitch, float fTime)
{
	m_fForceYaw = fYaw;
	m_fForcePitch = fPitch;
	m_fForceSettings = fTime;
}

inline bool CCameraInputHelper::GetForcedDirection(float &fYaw, float &fPitch)
{
	if(m_fForceSettings > 0.0f)
	{
		fYaw = m_fForceYaw;
		fPitch = m_fForcePitch;
		return true;
	}
	return false;
}

#endif