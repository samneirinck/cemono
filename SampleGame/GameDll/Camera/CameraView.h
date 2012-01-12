/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera view -> compute camera position and orientation

-------------------------------------------------------------------------
History:
- 01:2009 : Created By Jan Müller

*************************************************************************/

#ifndef CAMERA_VIEW_H
#define CAMERA_VIEW_H

//forward declarations
class CCameraInputHelper;
class CCameraRayScan;

#include "IViewSystem.h"
#include "CameraManager.h"
#include "CameraPolar.h"
#include "CameraTracking.h"

//view main class
class CCameraView
{

public:
	//pTarget can be NULL
	CCameraView(IEntity *pTarget);
	~CCameraView();

	//set camera target, also resets camera view
	void SetTarget(IEntity *pTarget);

	//reset camera view
	void Reset();

	//update writes new camera position / direction in viewParams
	void Update(SViewParams &viewParams);

	//get mode settings of last view update
	SCamModeSettings *GetLastMode() { return &m_curSettings; }

	CCameraRayScan* GetCamRayScan() const { return m_pCamRayScan; }

private:
	//get settings from camera manager
	void UpdateSettings(SViewParams &viewParams);

	//follow cam tries to follow the player around
	void UpdateFollowMode();
	//orbit mode rotates around the player and keeps direction
	void UpdateOrbitMode();
	//ref mode looks in a design controlled direction
	void UpdateRefDirMode();
	//rear cam stays always behind the player (combat mode)
	void UpdateRearMode();
	//first person cam stays in front of the target
	void UpdateFirstPersonMode();

	//write new settings to viewParams
	void WriteViewParams(SViewParams &viewParams);

	//update automatic camera flights
	void UpdateCameraFlight(SViewParams &viewParams);

	//interpolate / transition between two modes
	void RunModeTransition(SViewParams &viewParams);

	//rays scan for collisions and ideal camera position (after normal update)
	void RunScanAndTrack(SViewParams &viewParams);

	//target Entity
	IEntity *m_pTarget;
	Vec3		m_vTargetPosition;
	//offset vector for camera Target
	Vec3		m_vTargetOffset;

	//user input
	CCameraInputHelper *m_pCamHelper;

	//rayscan
	CCameraRayScan	*m_pCamRayScan;
	//camera tracker
	CCameraTracking	m_camTracking;

	//current camera setting
	SCamModeSettings m_curSettings;

	//current frame time
	float m_fFrameTime;

	//trigger for mode transition
	bool m_bModeTransition;
	float m_fTransitionTimeout;

	//current polar coordinates
	CCameraPolar	m_curPolar;

	//last viewParams
	SViewParams m_lastViewParams;
};

#endif 

