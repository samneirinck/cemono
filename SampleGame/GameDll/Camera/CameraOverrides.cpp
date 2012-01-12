/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera Override Modes
-------------------------------------------------------------------------
History:
- 25:08:2008: Created by Jan Müller

*************************************************************************/

#include "StdAfx.h"
#include "CameraOverrides.h"
#include "CameraManager.h"
#include "Game.h"
#include "GameCVars.h"
#include "CameraFlight.h"
#include "Player.h"
#include "CameraInputHelper.h"

CCameraOverrides::CCameraOverrides() :
m_trackedEntityId(0),
m_fLookAtHOff(0.0f),
m_fTrackingDistance(0.5f),
m_camOverrideType(ECO_NONE),
m_fFadeOutCoverMode(0.0f)
{

}

void CCameraOverrides::Update()
{
	if(m_fFadeOutCoverMode > 0.0f)
	{
		if(m_camOverrideType != ECO_LOWCOVER)
		{
			m_fFadeOutCoverMode = 0.0f;
			m_camOverrideType &= ~ECO_LOWCOVER;
		}
		else
		{
			m_fFadeOutCoverMode -= gEnv->pTimer->GetFrameTime();
			//fade out current override
			CameraID curCam = g_pGame->GetCameraManager()->GetActiveCameraId();
			SCamModeSettings camSettings;
			bool foundCam = g_pGame->GetCameraManager()->GetCameraSettings(curCam, camSettings, true);
			m_overrideSettings.InterpolateTo(camSettings, m_fFadeOutCoverMode);
			//terminate
			if(m_fFadeOutCoverMode <= 0.0f)
				m_camOverrideType &= ~ECO_LOWCOVER;
		}
	}
}

void CCameraOverrides::SetTrackEntityOverride(bool active)
{
	if(!active)
	{
		m_camOverrideType &= ~ECO_TRACK_TARGET;
		return;
	}

	//disable override first to get real settings
	CameraID curCam = g_pGame->GetCameraManager()->GetActiveCameraId();
	SCamModeSettings camSettings;
	bool foundCam = g_pGame->GetCameraManager()->GetCameraSettings(curCam, camSettings);
	if(foundCam)
	{
		camSettings.dist = m_fTrackingDistance;
		camSettings.camType = ECT_CamRear;
		camSettings.posDamping = 0;
		camSettings.vOff = 0.0f;
		camSettings.hOff = 0.3f;
		SetCameraOverrideSettings(camSettings);
		m_camOverrideType |= ECO_TRACK_TARGET;
	}
}

void CCameraOverrides::SetLowCoverOverride(bool active)
{
	if(!active)
	{
		//m_camOverrideType &= ~ECO_LOWCOVER;
		m_fFadeOutCoverMode = 0.2f;
		return;
	}

	//disable override first to get real settings
	CameraID curCam = g_pGame->GetCameraManager()->GetActiveCameraId();
	SCamModeSettings camSettings;
	bool foundCam = g_pGame->GetCameraManager()->GetCameraSettings(curCam, camSettings);
	if(foundCam)
	{
		SCamModeSettings targetSettings = camSettings;
		targetSettings.dist = 4.f;
		targetSettings.vOff = -0.9f;
		targetSettings.hOff = 0.3f;

		if(!(m_camOverrideType & ECO_LOWCOVER))
			SetCameraOverrideSettings(camSettings);
		else
			GetCameraOverrideSettings(camSettings);

		//interpolate to target settings
		camSettings.InterpolateTo(targetSettings);

		SetCameraOverrideSettings(camSettings);
		m_camOverrideType |= ECO_LOWCOVER;
		m_fFadeOutCoverMode = 0.0f;
	}
}

void CCameraOverrides::SetLookAtOverride(const Vec3 &camPos, IEntity *pTarget, IEntity *pOrigin)
{
	if(m_camOverrideType & ECO_LOOK_AT && !pTarget)
	{
		//turn off
		CCameraFlight *pFlight = CCameraFlight::GetInstance();
		pFlight->SetFadeMode(eCFFM_INOUT, 0.2f, 0.0f, 0.8f, 1.0f);
		pFlight->SetProgress(0.81f);
		pFlight->SetFadeProgress(0.0f);
		pFlight->SetPaused(false);
		m_camOverrideType &= ~ECO_LOOK_AT;
	}
	else if(pTarget)
	{
		CameraID curCam = g_pGame->GetCameraManager()->GetActiveCameraId();
		SCamModeSettings camSettings;
		bool foundCam = g_pGame->GetCameraManager()->GetCameraSettings(curCam, camSettings);
		if(foundCam)
		{
			//get lookAt position at the center of the entity
			AABB bbox;
			pTarget->GetLocalBounds(bbox);
			Vec3 targetPos = pTarget->GetWorldPos();
			targetPos.z += bbox.GetCenter().z;

			//compute looking direction
			Vec3 camDir = targetPos - camPos;
			float targetDistance = camDir.len();
			if(targetDistance < 0.001f)
			{
				targetDistance = 0.001f;
				camDir = Vec3Constants<float>::fVec3_OneY;
			}
			else
				//normalize
				camDir = camDir / targetDistance;

			CCameraFlight *pFlight = CCameraFlight::GetInstance();
			CPlayer *pHero = CPlayer::GetHero();

			if(m_camOverrideType & ECO_LOOK_AT) // update
			{
				Vec3 vTargetDir = targetPos - pHero->GetEntity()->GetWorldPos();
				vTargetDir.NormalizeSafe();
				Vec3 vCrossDir = vTargetDir.Cross(camDir);
				if(m_fLookAtHOff == 0.0f)
				{
					if(vCrossDir.z < 0.0f) //init offset
						m_fLookAtHOffTarget = -1.0f;
					else if(vCrossDir.z >= 0.0f)
						m_fLookAtHOffTarget = 1.0f;
				}

				/*if((m_fLookAtHOffTarget > 0.0f && m_fLookAtHOff < m_fLookAtHOffTarget) ||
					(m_fLookAtHOffTarget < 0.0f && m_fLookAtHOff > m_fLookAtHOffTarget))
					m_fLookAtHOff += m_fLookAtHOffTarget * 2.0f * gEnv->pTimer->GetFrameTime();
				else
					m_fLookAtHOff = m_fLookAtHOffTarget;*/

				//set new ref dir/pos
				Vec3 vRight = camDir.Cross(Vec3Constants<float>::fVec3_OneZ);
				pFlight->SetRefDir(camDir, true);
				pFlight->SetRefPos(pHero->GetEntity()->GetWorldPos() - camDir * 2.5f + vRight * m_fLookAtHOffTarget);

				//scale fov based on distance to target
				/*float fFov = 45.0f;
				fFov *= min(1.0f, g_pGameCVars->cl_cam_lookAt_fov_threshold / targetDistance);
				fFov = max(20.0f, fFov);
				//set new fov
				camSettings.FOV = fFov;
				//set override with new settings
				SetCameraOverrideSettings(camSettings);*/
			}
			else	//start
			{
				std::vector<Vec3> camPositions;
				camPositions.push_back(Vec3(0.0f, 0.0f, 2.0f));
				camPositions.push_back(Vec3(0.0f, 0.0f, 2.0f));
				pFlight->SetCameraCourse(camPositions, camDir, true, pHero->GetEntity()->GetWorldPos());
				pFlight->SetMoveSpeed(0.1f);
				pFlight->SetMode(eCFM_SPLINE);
				pFlight->SetFadeMode(eCFFM_INOUT, 0.4f, 0.4f, 0.6f, 1.0f);
				pFlight->SetPaused(true);
				pFlight->SetRefDir(camDir, true);

				m_fLookAtHOff = 0.0f;
				m_camOverrideType |= ECO_LOOK_AT;
			}
		}
	}
}

void CCameraOverrides::SetZoomOverride(bool active, float fov, float fovDamping)
{
	if(!active)
	{
		m_camOverrideType &= ~ECO_ZOOM;
		return;
	}

	//disable override first to get real settings
	CameraID curCam = g_pGame->GetCameraManager()->GetActiveCameraId();
	SCamModeSettings camSettings;
	bool foundCam = g_pGame->GetCameraManager()->GetCameraSettings(curCam, camSettings, true);
	if(foundCam)
	{
		camSettings.FOV = fov;
		camSettings.fovDamping = fovDamping;
		SetCameraOverrideSettings(camSettings);
		m_camOverrideType |= ECO_ZOOM;
	}
}
