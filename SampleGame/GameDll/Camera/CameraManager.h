/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera Manager.
-------------------------------------------------------------------------
History:
- 24:04:2008: Created by Nick Hesketh
- 08:2008: Continued by Jan Müller

*************************************************************************/

#ifndef __CAMERAMANAGER_H__
#define __CAMERAMANAGER_H__

class CCameraOverrides;
class CCameraView;

#include "CameraCommon.h"

enum ECamTypes
{
	ECT_CamOrbit=0,
	ECT_CamRear,
	ECT_CamRefDir,
	ECT_CamFollow,
	ECT_CamFirstPerson,
	ECT_CamUndefined
	// ECT_CamFollowCrumbs,ECT_CamFixed,ECT_CamFixedPan, ECT_CamFollowSpline
};

enum ECamCollisionTypes
{
	ECCT_CollisionCut=0,						// Cam cut pops cam in front of occluder.
	ECCT_CollisionTrackOrCut,			// Cam tracks round occluder, or cuts if needed.
	ECCT_CollisionTrack,
	ECCT_CollisionNone
	//ECCT_CollisionTransparency=4,		// Occluding objects made transparent.
	//ECCT_CollisionCull=5,						// Occluding objects culled from scene.
	//ECCT_CamPhysical=6,							// Camera is physicalised
};

class CYawPitchDampState
{
public:
	// Damping
	SSpherical spVel;				// damping velocity state

	// Maintaining damping direction as source and target angles move between quadrants.
	SSpherical spNewPrev;		// target last tick
	SSpherical spPrevPrev;	// source last tick
	int nNewWrap;						// nWraps of target. normalised into 0..1 range
	int nYawWrap;						// nWraps of source. normalised into 0..1 range
	bool bNeedReset;

	void Reset(const SSpherical &sph)
	{
		spNewPrev=sph;
		spPrevPrev=sph;
		spVel.Set(0.0f, 0.0f, 0.0f);
		nNewWrap=0;
		nYawWrap=0;
		bNeedReset=false;
	}
	void Reset()
	{
		SSpherical sph;
		Reset(sph);
	}

	CYawPitchDampState()
	{ Reset(); }

	CYawPitchDampState(const SSpherical& sph)
	{ Reset(sph); }
};

struct SCamRetainedState	// One instance of this for camera system
{
	int camIdLast;
	float cutHoldValue;
	float cutHoldDist;
	bool bNeedReset;

	float driverP4HoldValue;
	float driverP6HoldValue;

	CYawPitchDampState dampStateP3;

	SCamRetainedState() : camIdLast(0),cutHoldValue(0.0f),cutHoldDist(2.0f),bNeedReset(false),
		driverP4HoldValue(0),driverP6HoldValue(0)
	{ ; }
};

// Settings for follow cam modes.
struct SCamModeSettings	// Multiple instances. (One instance of this per camera definition).
{
	SCamModeSettings() :
		camType(ECT_CamUndefined),
		m_fYawApplyRate(0.0f),
		m_fPitchApplyRate(0.0f)
	{
	}

	//interpolate to the "variable" values of the given settings
	void InterpolateTo(SCamModeSettings &targetSettings, float interpolationTime = 1.0f);

	// mode
	ECamTypes camType;	// Orbit cam, Rear cam

	//interpolate these values ...
	float dist;
	float maxDist;
	float hOff;
	float vOff;
	float FOV;
	float pitchOff;
	//~interpolate

	float pitchLookDownPullBack;
	float pitchLookUpPushIn;
	float maxPitch;
	float minPitch;
	float maxYaw;
	Vec3	vRefDir;

	// damping + lookahead
	float kVel;		// lookahead
	float kDist;	// lookahead
	float posDamping;
	float angDamping;
	float fovDamping;

	// collision
	ECamCollisionTypes collisionType;	// cut, track and cut

	// rotation params
	float m_fYawApplyRate;
	float m_fPitchApplyRate;

	// state
	Vec3 vPos;
	Quat rotation;
	float pitch;
	float yaw;
};

typedef int CameraID;

class CCameraManager
{
public:
	CCameraManager();
	~CCameraManager();

	//update loop for camera system
	void Update();

	//can and set state of active camera
	void GetCameraState(SCamRetainedState &state) const;
	void SetCameraState(SCamRetainedState &state);
	void ResetCameraState() {m_camState.bNeedReset=true;}

	//choose active camera
	bool SetActiveCameraId(CameraID idCamera);
	CameraID GetActiveCameraId() const { return m_idActive; }
	CameraID GetLastCameraId() const { return m_idPrev; }
	CameraID FindCameraByType(ECamTypes eType) const;

	//camera id handling
	bool IsValidCameraId(CameraID idCamera) const;
	const char *GetCameraNameForId(CameraID idCamera) const;
	//retrieve camera type for id
	ECamTypes GetCameraType(CameraID idCamera);

	//setup camera
	CameraID AddCamera(const char *psCamName, const SCamModeSettings &settings);
	//get current camera settings
	bool GetCameraSettings(CameraID idCamera,SCamModeSettings &settings,bool bIgnoreOverride=false) const;
	//this sets up the camera and should usually come from flowgraph
	bool SetCameraSettings(CameraID idCamera,const SCamModeSettings &settings);

	//get actual camera view
	CCameraView *GetCamView() { return m_pCameraView; }

	//camera override functions
	CCameraOverrides *GetCamOverrides() { return m_pCamOverrides; }

	//returns true when camera changed and wasn't updated yet
	static bool ChangedCamera() { return g_bChangedCamera; }

	//"resets" camera
	void Reset();

	void LogActiveCameraSettings();

protected:
	struct SCamNode
	{
		SCamModeSettings settings;
		string sName;
	};
	typedef std::vector< SCamNode* > TCamNodeTable;

	TCamNodeTable m_camNodes;
	SCamRetainedState m_camState;
	CameraID m_idActive;
	CameraID m_idPrev;

	static bool g_bChangedCamera;

	//camera view
	CCameraView				*m_pCameraView;

	//camera overrides manager
	CCameraOverrides	*m_pCamOverrides;

	void AddNullCam();
	CameraID GetCameraIdForName(const char *psName) const;
};

//INLINES
inline ECamTypes CCameraManager::GetCameraType(CameraID idCamera)
{
	if(IsValidCameraId(idCamera))
		return m_camNodes[idCamera]->settings.camType;
	return ECT_CamUndefined;
}

#endif // ifndef __CAMERAMANAGER_H__