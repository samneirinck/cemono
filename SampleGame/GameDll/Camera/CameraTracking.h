/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera code dealing with obstacle avoidance and automatic movement.

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#ifndef CAMERA_OBSTACLE_TRACKING
#define CAMERA_OBSTACLE_TRACKING

#include "CameraCommon.h"

//forward declarations
class CCameraRayScan;
class CPlayer;
struct SViewParams;
struct SCamModeSettings;

enum ETrackDirection
{
	eTD_LEFT = 0,
	eTD_RIGHT,
	eTD_TOP = 0,
	eTD_BOTTOM
};

//tracking class
class CCameraTracking
{
public:
	//the tracking needs the camera ray scan as input data
	CCameraTracking();

	//the tracking needs the camera ray scan as input data
	void SetCameraRayScan(CCameraRayScan *pRayScan);

	//avoid obstacles in the camera view by overwriting position and rotation in viewParams
	bool Update(SViewParams &viewParams, float &fHOffObstacleStrength, const SCamModeSettings &camMode, const CPlayer &hero, const bool bObstacleFound = false);

private:

	//use the raycasts to identify the obstacle type
	bool IdentifyObstacle(const Vec3 &vCamDir, const CPlayer &hero);

	//the camera is automatically rotating in player direction
	void UpdateAutoFollow(const SViewParams &viewParams, const CPlayer &hero);

	//the camera ray data
	CCameraRayScan			*m_pCamRayScan;

	//current camera orientation
	SSpherical m_curCamOrientation;
	//target rotation
	float	m_fYawDelta;
	float	m_fPitchDelta;
	//last rotation direction and speed for smoothing
	ETrackDirection	m_eLastDirYaw;
	ETrackDirection	m_eLastDirPitch;
	float	m_fSpeed;
	float	m_fTimeCovered;
	bool	m_bViewCovered;
	//camera position at last obstacle
	Vec3	m_vLastObstaclePos;
	//speed at which the camera is rotating in player direction
	float m_fAutoRotateSpeed;

	//current frameTime
	float	m_fFrameTime;
};

#endif