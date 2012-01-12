/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera Override Modes
When the camera settings are overridden, a completely different camera can
be active without interfering with the flowgraph at all.
-------------------------------------------------------------------------
History:
- 25:08:2008: Created by Jan Müller

*************************************************************************/

#ifndef CAMERA_OVERRIDES_H
#define CAMERA_OVERRIDES_H

#include "CameraManager.h"

enum ECamOverrideType
{
	ECO_NONE			= 0x00,

	ECO_LOOK_AT			= 0x01,
	ECO_TRACK_TARGET	= 0x02,
	ECO_ZOOM			= 0x04,
	ECO_LOWCOVER	= 0x08,
};

class CCameraOverrides
{

public:
	//**************************************
	CCameraOverrides();

	//updating camera fades
	void Update();

	//returns whether current camera gets overridden
	int GetCameraOverride() const { return m_camOverrideType/* != ECO_NONE*/; }
	//get override settings
	void GetCameraOverrideSettings(SCamModeSettings &settings) const { settings = m_overrideSettings; }
	//set override settings
	void SetCameraOverrideSettings(const SCamModeSettings &settings) { m_overrideSettings = settings; }

	//set tracked entity for tracking mode
	void SetTrackEntity(const EntityId id) { m_trackedEntityId = id; }
	EntityId GetTrackEntity() const { return m_trackedEntityId; }
	//set tracking distance
	void SetTrackDistance(const float dist) { m_fTrackingDistance = max(dist, 0.1f); }

	//run track entity override
	void SetTrackEntityOverride(bool active);

	// low cover temporary camera override
	void SetLowCoverOverride(bool active);

	//this sets a look-at-entity override, pTarget = NULL to reset
	void SetLookAtOverride(const Vec3 &camPos, IEntity *pTarget, IEntity *pOrigin = NULL);

	//this lets you adjust the zoom of the active camera
	void SetZoomOverride(bool active, float fov, float fovDamping);

private:

	//this overrides whatever camera is active with the given settings
	int									m_camOverrideType;
	SCamModeSettings		m_overrideSettings;

	//workaround way to fade out current override to normal settings
	float								m_fFadeOutCoverMode;

	//horizontal offset for look at override
	float								m_fLookAtHOff, m_fLookAtHOffTarget;

	//target tracked entity
	EntityId						m_trackedEntityId;
	float								m_fTrackingDistance;
};

#endif