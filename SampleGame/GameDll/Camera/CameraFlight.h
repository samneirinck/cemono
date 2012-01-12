/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera flight moves the camera on a spline course
or freely in player control.

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#ifndef CAMERA_FLIGHT_H
#define CAMERA_FLIGHT_H

#include "CameraRayScan.h"

struct SViewParams;

enum ECameraFlightMode
{
	eCFM_LINE,						//in line mode the camera follows a line track
	eCFM_SPLINE,					//in spline mode the camera follows a spline track
	eCFM_FREE_FLIGHT,			//in free flight mode the camera is controlled by the player
	eCFM_NONE
};

enum ECameraFlightFadeMode
{
	eCFFM_IN,							//fade in only
	eCFFM_INOUT,					//fade in and out
	eCFFM_OUT,						//fade out only
	eCFFM_NONE
};

enum ECameraFlightState
{
	eCFS_NONE,
	eCFS_FADE_IN,					//fading in
	eCFS_FADE_OUT,				//fading out
	eCFS_RUNNING					//flight active (get progress for details)
};

struct SCameraFlightPoint
{
	Vec3	m_vCamPos;
	Vec3	m_vCamLookAt;
	SCameraFlightPoint(const Vec3 &pos, const Vec3 &lookAt) : m_vCamPos(pos), m_vCamLookAt(lookAt)
	{}

	SCameraFlightPoint() : m_vCamPos(ZERO), m_vCamLookAt(ZERO,1.0f,ZERO)
	{}

	//scale both vectors/positions
	SCameraFlightPoint operator*(float fScalar)
	{
		SCameraFlightPoint resultPoint(*this);
		resultPoint.m_vCamPos *= fScalar;
		resultPoint.m_vCamLookAt *= fScalar;
		return resultPoint;
	}

	ILINE friend SCameraFlightPoint operator*(float fScalar, const SCameraFlightPoint &flightPoint)
	{
		SCameraFlightPoint resultPoint(flightPoint);
		resultPoint.m_vCamPos *= fScalar;
		resultPoint.m_vCamLookAt *= fScalar;
		return resultPoint;
	}

	//add both vectors/positions
	SCameraFlightPoint operator+(const SCameraFlightPoint &flightPoint)
	{
		SCameraFlightPoint resultPoint(*this);
		resultPoint.m_vCamPos += flightPoint.m_vCamPos;
		resultPoint.m_vCamLookAt += flightPoint.m_vCamLookAt;
		return resultPoint;
	}

	//scale both vectors/positions
	SCameraFlightPoint operator+(const Vec3 &vFlightOffset)
	{
		SCameraFlightPoint resultPoint(*this);
		resultPoint.m_vCamPos += vFlightOffset;
		resultPoint.m_vCamLookAt += vFlightOffset;
		return resultPoint;
	}

	ILINE friend SCameraFlightPoint operator+(const Vec3 &vFlightOffset, const SCameraFlightPoint &flightPoint)
	{
		SCameraFlightPoint resultPoint(flightPoint);
		resultPoint.m_vCamPos += vFlightOffset;
		resultPoint.m_vCamLookAt += vFlightOffset;
		return resultPoint;
	}
};

//************************************************************************
class CCameraFlight
{
public:
	//get cam flight instance
	static CCameraFlight* GetInstance();

	//reset, stop current flight
	void Reset();

	//camera flight ongoing
	bool IsCameraFlightActive() const { return (m_fFlightProgress > 0.0f && m_fFlightProgress < 1.0f); }

	//setup a camera course
	//if vRefPos is set, the positions are treated relative to it
	//when pRefEntity is set, it's world position overwrites the vRefPos
	//if pRefEnt is set, it's overwriting vRevPos every frame

	//camera flight with lookAt saved inside/per positions
	void SetCameraCourse(const std::vector<SCameraFlightPoint> &cPositions, const Vec3 &vRefPos = Vec3Constants<float>::fVec3_Zero, IEntity *pRefEntity = NULL);
	//camera flight with explicit "lookAt", direction can be relative to the positions
	void SetCameraCourse(const std::vector<Vec3> &cPositions, const Vec3 &vLookAt, bool bLookAtRelative = false, const Vec3 &vRefPos = Vec3Constants<float>::fVec3_Zero, IEntity *pRefEntity = NULL);

	//change the reference position all values are relative to (reference entity overwrites first position when set)
	void SetRefPos(const Vec3 &vRefPos, const Vec3 &vRefPos2 = Vec3Constants<float>::fVec3_Zero, const IEntity *pRefEntity = NULL);

	//set ref dir override
	void SetRefDir(const Vec3 &vRefDir, bool bUseRefDir = true);

	//change the movement speed of the camera
	void SetMoveSpeed(float fMpS = 4.0f);

	//change the movement type
	void SetMode(ECameraFlightMode eMode) { m_eMovementMode = eMode; }
	//get the movement type
	ECameraFlightMode GetMode() { return m_eMovementMode; }

	//controls fading in and out of camera flights
	//fFadeTime is the time in seconds the fade takes to finish, it's interrupted if the fade progress is finished before that
	//fFadeInEnd is the flight progress in % (0.0f - 1.0f) where the fade-in (if available) is finished
	//fFadeOutStart "" "" where the fade-out begins
	//fFadeDistanceOverride can be used to overwrite the actual course length (for example 1.0 instead of 0.1 in meter) to avoid artifacts during fading on short distances
	void SetFadeMode(ECameraFlightFadeMode eMode, float fFadeTime = 0.1f, float fFadeInEnd = 0.2f, float fFadeOutStart = 0.8f, float fFadeDistanceOverride = 0.0f);

	//(re)set fade progress (for example forward fade to start/end)
	void SetFadeProgress(float fFade) { m_fFadeProgress = clamp(fFade, 0.0f, 1.0f); }
	float GetFadeProgress() const { return m_fFadeProgress; }

	//set position and direction for free fly
	void SetFreeFlyPos(const SCameraFlightPoint &pos) { m_freeFlyPoint = pos; }
	//get position and lookAt for free fly
	const SCameraFlightPoint& GetFreeFlyPos() { return m_freeFlyPoint; }

	//pause flight
	void SetPaused(bool bPaused) { m_bPaused = bPaused; }
	bool GetPaused() const { return m_bPaused; }

	//get current ECameraFlightState
	ECameraFlightState GetState() const { return m_eState; }

	//get current dir of camera during a fligth
	Vec3 GetLookingDirection() { return m_vLookingDirection; }

	//flight progress between 0 and 1.0f (in percent)
	float GetProgress() const { return m_fFlightProgress; }
	//setting the progress means jumping to a certain position of a running flight
	void SetProgress(float fProgress) { m_fFlightProgress = clamp(fProgress, 0.0f, 1.0f); }

	//update the current track flight and overwrite the given viewParams
	void UpdateFlight(SViewParams &viewParams);

private:
	//this is a singleton
	CCameraFlight();
	~CCameraFlight();

	//init flight settings
	void InitFlight(const Vec3 &vRefPos, IEntity *pRefEntity);

	//retrieve a camera position for a certain timeFrame [0.0f, 1.0f] on a spline curve
	SCameraFlightPoint GetSplinePoint(float t);
	//retrieve a camera position for a certain timeFrame [0.0f, 1.0f]
	SCameraFlightPoint GetTrackPoint(float t);

	//compute length of course
	float GetCourseLength() const;

	//detect and handle collisions
	void DetectCollisions();

	//flight progression
	float		m_fFlightProgress;
	//movement speed per second of the camera
	float		m_fFlightSpeed;
	float		m_fMoveSpeedMps;
	//reference point for relative movement
	Vec3		m_vRefPos;
	Vec3		m_vRefPos2;
	//override reference direction
	Vec3		m_vRefDir;
	//reference entity for relative movement
	const IEntity *m_pRefEnt;

	//temp looking direction during flight
	Vec3		m_vLookingDirection;

	//list of spline course anchor points
	std::vector<SCameraFlightPoint> m_cameraCourse;
	//temp lists to work on
	std::vector<SCameraFlightPoint> m_tempCourseA, m_tempCourseB;

	//camera movement mode
	ECameraFlightMode m_eMovementMode;

	//camera fade mode
	ECameraFlightFadeMode m_eFadeMode;
	float									m_fFadeTime, m_fFadeInTime, m_fFadeOutTime;
	float									m_fFadeProgress;
	float									m_fCourseLengthOverwrite;
	//camera fading data
	Vec3							m_vTargetFadePos;
	Quat							m_qFadeOrientation;

	//free fly point
	SCameraFlightPoint m_freeFlyPoint;

	//pause mode
	bool							m_bPaused;
	//use override direction
	bool							m_bUseRefDir;

	//flight state
	ECameraFlightState m_eState;

	//camera rayscan to detect collision
	QueuedRayID m_RayId;
};

//INLINES

ILINE void CCameraFlight::SetFadeMode(ECameraFlightFadeMode eMode, float fFadeTime, float fFadeInEnd, float fFadeOutStart, float fFadeDistanceOverride)
{
	m_eFadeMode = eMode;
	m_fFadeTime = max(0.001f, fFadeTime);
	m_fFadeInTime = clamp(fFadeInEnd, 0.0f, .9f);
	m_fFadeOutTime = clamp(fFadeOutStart, 0.1f, 1.0f);
	m_fCourseLengthOverwrite = fFadeDistanceOverride;
}

ILINE void CCameraFlight::SetRefPos(const Vec3 &vRefPos, const Vec3 &vRefPos2 /* = Vec3Constants<float>::fVec3_Zero */, const IEntity *pRefEntity)
{
	m_vRefPos = vRefPos;
	m_vRefPos2 = vRefPos2;
	m_pRefEnt = pRefEntity;
}

ILINE void CCameraFlight::SetRefDir(const Vec3 &vRefDir, bool bUseRefDir /* = true */)
{
	m_vRefDir = vRefDir;
	m_bUseRefDir = bUseRefDir;
}

#endif
