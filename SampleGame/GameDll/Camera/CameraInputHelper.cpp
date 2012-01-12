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

*************************************************************************/

#include "StdAfx.h"
#include "CameraInputHelper.h"
#include "Player.h"
#include "PlayerInput.h"
#include "Game.h"
#include "CameraOverrides.h"
#include "CameraFlight.h"
#include "CameraView.h"

//Statics
const static float INTERPOLATION_TARGET_MAX_ERROR = 0.2f;
const static float INTERPOLATION_SLOWDOWN_DIST = 0.6f;
const static float INTERPOLATION_PITCH_REL_SPEED = 0.25f;

//**************************************
CCameraInputHelper::CCameraInputHelper(CPlayer *pHero, CPlayerInput *pHeroInput) :
m_pHero(pHero),
m_pHeroInput(pHeroInput)
{
	CRY_ASSERT(m_pHero);
	CRY_ASSERT(m_pHeroInput);
	Reset();
}

CCameraInputHelper::~CCameraInputHelper()
{

}

void CCameraInputHelper::PostUpdate(float fFrameTime, float fDeltaPitch)
{
	//invert pitch in new camera system 
	if(g_pGameCVars->cl_cam_orbit != 0)
	{
		fDeltaPitch *= -1.0f;
		if(fabsf(fDeltaPitch) > g_fCamError)
		{
			float pitch = m_fPitch;//g_pGameCVars->cl_tpvPitchNav;
			pitch += fDeltaPitch*(fFrameTime/0.1f)*CAMERA_PITCH_SCALE;
			pitch = clamp(pitch, 0.5f, 2.5f);
			m_fPitch=pitch;
		}
	}
	else
	{
		float pitch=m_fPitch;//g_pGameCVars->cl_tpvPitchNav;
		pitch+=fDeltaPitch*(fFrameTime/0.1f)*CAMERA_PITCH_SCALE;
		pitch=clamp(pitch, g_pGameCVars->cl_cam_PitchMin, g_pGameCVars->cl_cam_PitchMax);
		m_fPitch=pitch;
	}
}

void CCameraInputHelper::Reset()
{
	m_fYaw = 0.0f;
	m_fPitch = gf_PI*0.5f;
	m_fInputPitchDelta = 0.0f;
	m_fInputYawDelta = 0.0f;
	//reset interpolation
	m_fInterpolationTargetYaw = 0.0f;
	m_fInterpolationTargetPitch = 0.0f;
	m_bInterpolationTargetActive = false;
	m_fInterpolationTargetTimeout = 0.0f;
	//reset timer
	m_fLastUserInput = 0.0f;
	m_bAutoTracking = false;
	m_bNavCombatModeChanged = false;
	//reset tracking
	m_fTrackingYawDelta = 0.0f;
	m_fTrackingPitchDelta = 0.0f;
	//make sure reset is distributed
	m_bYawModified = true;
	m_bPitchModified = true;

	//make sure the direction is fixed
	SSpherical tmpSpherical;
	CartesianToSpherical(m_pHero->GetEntity()->GetForwardDir(), tmpSpherical);
	ForceCameraDirection(tmpSpherical.m_fYaw, tmpSpherical.m_fPitch, 0.3f);
}

void CCameraInputHelper::UpdateCameraInput(Ang3 &deltaRotation, const float frameTimeClamped, const float frameTimeNormalised)
{
	// Use new control scheme,
	// or, if we're just entering/leaving it then adjust camera/character orientations as appropriate.

	//--- Translate left stick into a deltaRotation.z (and a move direction)

	Vec3 vcStick=Vec3(m_pHeroInput->m_moveStickLR, m_pHeroInput->m_moveStickUD,0);	// Stick vector in view space	// -m_moveStickLR removes flicker, but inverses direction..

	static Vec3 vcStickPrev=Vec3(0,1,0);

	float stickMag=vcStick.len();

	if(stickMag>0.001f)
		vcStick*=1/stickMag;
	else
		vcStick=vcStickPrev;	// should hold previous value/direction unless we're in transition.

	vcStickPrev=vcStick;

	Matrix33 camViewMtx = m_pHero->m_camViewMtxFinal;
	//in flight mode the actual camera matrix changes
	//the mtxFinal itself stays untouched, so that the camera can move back after the flight
	if(CCameraFlight::GetInstance()->IsCameraFlightActive())
	{
		Vec3 vCamDir = CCameraFlight::GetInstance()->GetLookingDirection();
		camViewMtx.SetRotationVDir(vCamDir, 0.0f);
	}
	
	const Matrix34& entityWorldTM = m_pHero->GetEntity()->GetWorldTM();
	Vec3 vwEntPos = entityWorldTM.GetTranslation();
	Vec3 vwDrawFrom = vwEntPos+Vec3(0,0,0.8f);
	Vec3 vwEntityFront = entityWorldTM.GetColumn1();
	Vec3 vwEntityRight = entityWorldTM.GetColumn0();
	Vec3 vCamRef = camViewMtx*Vec3(0,1,0);
	Vec3 vwStick = camViewMtx*vcStick;	// Stick vector in world space
	Vec3 vwfStick = Vec3FlattenXY(vwStick);

	Vec3 vwfEntityFront=Vec3FlattenXY(vwEntityFront);
	Vec3 vwfEntityRight=Vec3FlattenXY(vwEntityRight);
	float dotRight=vwfStick.Dot(vwfEntityRight);
	float dotFront=vwfStick.Dot(vwfEntityFront);
	dotFront = clamp(dotFront,-1.0f,1.0f);

	bool enteredNavFromCombat = false; //(m_bNavCombatModeChanged && !bIsCombatMode);

	//--- Just moved into nav mode from combat
	if(enteredNavFromCombat)
	{
		// so set directions appropriately.

		float dotFront2=(vCamRef).Dot(Vec3(0,1,0));
		float dotRight2=(camViewMtx*Vec3(1,0,0)).Dot(Vec3(0,1,0));
		float yaw = 0.0f;

		dotFront2 = clamp(dotFront2,-1.0f,1.0f);
		if(dotRight2>=0)
		{
			yaw=-cry_acosf(dotFront2);
		}
		else
		{
			yaw=cry_acosf(dotFront2);
		}

		m_fYaw = yaw;
	}

	//this is the actual entity rotation (yaw movement)
	CCameraOverrides *pCamOverride = g_pGame->GetCameraManager()->GetCamOverrides();
	int eCamOverrideMode = pCamOverride->GetCameraOverride();

	//the ECO_LOWCOVER fading is a workaround, causing problems when inputs are changed during the fade
	//in combat mode all inputs have to be passed through to synchronize the camera direction in other modes
	if((stickMag>0.01f && eCamOverrideMode != ECO_LOWCOVER))
	{
		if(dotRight>=0)
		{
			deltaRotation.z=-cry_acosf(dotFront);
		}
		else
		{
			deltaRotation.z=cry_acosf(dotFront);
		}
	}
	else
		deltaRotation.z=0;

	float maxDeltaRot = 7.85f; // safety value //g_pGameCVars->cl_nav_SprintMaxTurnRate;
	Limit( maxDeltaRot, 0.f, gf_PI * 10.f ); // limit to 3600 degrees per second, i.e. 'pretty fast'
	maxDeltaRot *= frameTimeClamped;	

	// In Nav mode movement is always in the characters forward direction.
	UpdatePitchYawDriver(stickMag, frameTimeNormalised);

	m_bNavCombatModeChanged=false;
}

void CCameraInputHelper::UpdatePitchYawDriver(float stickMag, float frameTimeNormalised)
{
	SCVars* pGameCVars = g_pGameCVars;

	//normal update
	CCameraManager *pCamMan = g_pGame->GetCameraManager();
	SCamModeSettings settings;
	pCamMan->GetCameraSettings(pCamMan->GetActiveCameraId(), settings);

	if (m_pHero->m_stats.flyMode == 0)
	{
		m_pHeroInput->m_deltaMovement=Vec3(0,1,0)*stickMag;
	}

	//camera flights overwrite everything
	CCameraFlight *pFlight = CCameraFlight::GetInstance();
	bool bFlightActive = pFlight->IsCameraFlightActive() || (pFlight->GetPaused() && pFlight->GetState() != eCFS_NONE);
	if(bFlightActive && !(pFlight->GetState() == eCFS_FADE_OUT && pFlight->GetFadeProgress() > 0.8f))
		return;

	//force settings overwrite inputs
	if(m_fForceSettings > 0.0f)
	{
		m_fYaw = m_fForceYaw;
		m_fPitch = m_fForcePitch;
		m_fForceSettings -= gEnv->pTimer->GetFrameTime();
		return;
	}

	// nav stick:
	static float g_fYawRotSpeed = 0.0f;
	static float g_fPitchRotSpeed = 0.0f;

	float fNewYawSpeed = m_pHeroInput->m_cameraStickLR * gf_PI * g_pGameCVars->cl_cam_rotation_speed;
	float fNewPitchSpeed = m_pHeroInput->m_cameraStickUD * gf_PI * g_pGameCVars->cl_cam_rotation_speed;

	//interpolate rotation acceleration
	if(g_pGameCVars->cl_cam_rotation_enable_acceleration)
	{

		if(fNewYawSpeed == 0.0f)
			g_fYawRotSpeed = 0.0f;
		else
		{
			g_fYawRotSpeed = InterpolateTo(g_fYawRotSpeed, fNewYawSpeed, g_pGameCVars->cl_cam_rotation_acceleration_time_yaw);
			if(g_fYawRotSpeed < 0.0f)
				g_fYawRotSpeed = min(g_fYawRotSpeed, -0.01f);
			else
				g_fYawRotSpeed = max(g_fYawRotSpeed, 0.01f);
		}
		if(fNewPitchSpeed == 0.0f)
			g_fPitchRotSpeed = 0.0f;
		else
		{
			g_fPitchRotSpeed = InterpolateTo(g_fPitchRotSpeed, fNewPitchSpeed, g_pGameCVars->cl_cam_rotation_acceleration_time_pitch);
			if(g_fPitchRotSpeed < 0.0f)
				g_fPitchRotSpeed = min(g_fPitchRotSpeed, -0.002f);
			else
				g_fPitchRotSpeed = max(g_fPitchRotSpeed, 0.002f);
		}
	}
	else
	{
		g_fYawRotSpeed = fNewYawSpeed;
		g_fPitchRotSpeed = fNewPitchSpeed;
	}

	//set delta yaw/pitch
	float deltaYawBase = g_fYawRotSpeed * frameTimeNormalised;
	deltaYawBase *= settings.m_fYawApplyRate; //apply yaw rate modifier
	float deltaPitchBase = g_fPitchRotSpeed * frameTimeNormalised;
	deltaPitchBase *= settings.m_fPitchApplyRate; //apply pitch rate modifier
	bool bHasYaw = abs(m_pHeroInput->m_cameraStickLR)>0.001f;
	bool bHasPitch = abs(m_pHeroInput->m_cameraStickUD)>0.001f;
	bool bAutoTracking = false;

	if(bHasYaw || bHasPitch)
		m_fLastUserInput = gEnv->pTimer->GetFrameStartTime().GetSeconds();

	//interpolate camera to target ***************
	if(UpdateTargetInterpolation())
		return;

	//if(settings.camType != ECT_CamFollow)
	{
		//handle auto rotation (simulated input)
		if(!bHasYaw && fabsf(m_fTrackingYawDelta) > 0.001f)
		{
			deltaYawBase = m_fTrackingYawDelta;
			m_fTrackingYawDelta = 0.0f;
			bAutoTracking = true;
		}
		if(!bHasPitch && fabsf(m_fTrackingPitchDelta) > 0.001f)
		{
			deltaPitchBase = m_fTrackingPitchDelta;
			m_fTrackingPitchDelta = 0.0f;
			bAutoTracking = true;
		}
	}

	if(g_pGameCVars->cl_cam_orbit != 0)
	{
		CCameraView *pCamView = g_pGame->GetCameraManager()->GetCamView();
		ECamTypes eLastType = pCamView->GetLastMode()->camType;
		if(eLastType != ECT_CamRear && eLastType != ECT_CamFirstPerson)
		{
			SetYawDelta(RetrieveYawDelta() - deltaYawBase);
			SetPitchDelta(RetrievePitchDelta() - deltaPitchBase);
		}
	}
	else if(!CCameraManager::ChangedCamera())
	{
		const float frameTime = max(gEnv->pTimer->GetFrameTime(),FLT_EPSILON);

		//add dampening to the rotation input
		static float yawDeltaDamped = 0.0f;
		float dampeningStrength = max(1.0f, pGameCVars->cl_cam_yaw_input_inertia / frameTime);

		yawDeltaDamped = yawDeltaDamped * (dampeningStrength-1.0f) + deltaYawBase;
		yawDeltaDamped /= dampeningStrength;
		if(bAutoTracking || bAutoTracking != m_bAutoTracking || fabsf(yawDeltaDamped) < fabsf(deltaYawBase))
			yawDeltaDamped = deltaYawBase;

		//wait for pull
		if(!m_bYawModified)
			m_fInputYawDelta = yawDeltaDamped;
		else
			m_fInputYawDelta += yawDeltaDamped;
		m_fYaw += yawDeltaDamped;
		if(fabsf(yawDeltaDamped) > 0.002f)
			m_bYawModified = true;

		//else
		//	m_bYawModified = false;

		//add dampening to the rotation input
		static float pitchDamped = 0.0f;
		dampeningStrength = max(1.0f, pGameCVars->cl_cam_pitch_input_inertia / frameTime);
		pitchDamped = pitchDamped * (dampeningStrength-1.0f) + deltaPitchBase;
		pitchDamped /= dampeningStrength;
		if(bAutoTracking != m_bAutoTracking || fabsf(pitchDamped) < fabsf(deltaPitchBase))
			pitchDamped = deltaPitchBase;

		//compute new pitch
		if(!m_bPitchModified)
			m_fInputPitchDelta = pitchDamped;
		else
			m_fInputPitchDelta += pitchDamped;
		if(fabsf(pitchDamped) > 0.002f)
			m_bPitchModified = true;
		//else
		//	m_bPitchModified = false;
		m_fPitch += pitchDamped;

		//clamp yaw to -pi .. pi
		ClampPiRange(m_fYaw);

		//clamp pitch
		m_fPitch = clamp(m_fPitch, pGameCVars->cl_cam_PitchMin, pGameCVars->cl_cam_PitchMax);
	}

	m_bAutoTracking = bAutoTracking;
}

// All actions and bindings that can be invoked in one of the combat/nav modes, but not the other(s)
// should be reset here, since the corresponding key-up or 0 value action won't be processed in the other mode(s).
void CCameraInputHelper::CombatModeChanged()
{
	m_bNavCombatModeChanged=true;
	//m_pHeroInput->OnActionMoveStickLR(0,"",0,m_pHeroInput->m_moveStickLR);
	//m_pHeroInput->OnActionCameraStickLR(0,"",0,m_pHeroInput->m_cameraStickLR);
	//OnActionAttack1("attack1",2,0);
	m_pHero->CActor::OnAction("attack1", 2, 0);

	SCVars* pGameCVars = g_pGameCVars;

	pGameCVars->cl_tpvHOffCombatBias = 0;
	pGameCVars->cl_tpvVOffCombatBias = 0;
	m_fTrackingYawDelta = 0.0f;
	m_fTrackingPitchDelta = 0.0f;

	bool bSetNavPitch=pGameCVars->cl_tpvNavSwitchVCentreEnabled != 0;

	if(bSetNavPitch)
	{
		float navPitch=pGameCVars->cl_tpvNavSwitchVCentreValue;
		m_fPitch=navPitch;
	}
}

void CCameraInputHelper::SnapToPlayerDir()
{
	//get target direction (player dir)
	Vec3 dir = m_pHero->GetEntity()->GetForwardDir();
	SSpherical sph;
	CartesianToSpherical(dir, sph);
	//snap camera direction (in nav mode) to player direction
	//the camera direction is 90° off and flipped compared to entity space
	sph.m_fYaw -= gf_PI * 0.5f;
	sph.m_fYaw *= -1.0f;
	SetInterpolationTarget(sph.m_fYaw, GetPitch());
}

void CCameraInputHelper::SetInterpolationTarget(float yaw, float pitch, float speedInRad, float timeOut, float maxError)
{
	SCamModeSettings settings;
	int curCam = g_pGame->GetCameraManager()->GetActiveCameraId();
	g_pGame->GetCameraManager()->GetCameraSettings(curCam, settings);
	if(settings.camType != ECT_CamFollow)
		return; //interpolation works in follow mode only at the moment

	speedInRad = clamp(speedInRad, .02f, gf_PI*10.0f);

	m_bInterpolationTargetActive = true;
	ClampPiRange(yaw);
	m_fInterpolationTargetYaw = yaw;
	m_fInterpolationTargetPitch = clamp(pitch, g_pGameCVars->cl_cam_PitchMin, g_pGameCVars->cl_cam_PitchMax);
	m_fInterpolationTargetSpeed = speedInRad;
	m_fInterpolationTargetTimeout = timeOut; //5 seconds timeout
	m_fInterpolationTargetMaxError = maxError;
}

bool CCameraInputHelper::UpdateTargetInterpolation()
{
	if(!m_bInterpolationTargetActive || HasForceDirection())
		return false;

	//check timeout just in case
	m_fInterpolationTargetTimeout -= gEnv->pTimer->GetFrameTime();
	if(m_fInterpolationTargetTimeout < 0.0f)
	{
		m_bInterpolationTargetActive = false;
		return true;
	}

	//work in 0 .. 2pi range
	float targetYaw = m_fInterpolationTargetYaw + gf_PI;
	float curYaw = GetYaw() + gf_PI;

	//compute yaw delta
	float yawDelta = targetYaw - curYaw;
	if(yawDelta > gf_PI)
		yawDelta = -(gf_PI2 - yawDelta);
	else if(yawDelta < -gf_PI)
		yawDelta = gf_PI2 + yawDelta;

	//compute pitch delta
	float pitchDelta = m_fInterpolationTargetPitch - GetPitch();

	//termination
	float absYawDelta = fabsf(yawDelta);
	float absPitchDelta = fabsf(pitchDelta);
	if(absYawDelta < m_fInterpolationTargetMaxError && absPitchDelta < m_fInterpolationTargetMaxError)
	{
		m_bInterpolationTargetActive = false;
		return true;
	}

	//compute safe rotation speed
	float interpolationSpeed = m_fInterpolationTargetSpeed * gEnv->pTimer->GetFrameTime();
	if(absYawDelta < INTERPOLATION_SLOWDOWN_DIST && absYawDelta > INTERPOLATION_TARGET_MAX_ERROR)
		interpolationSpeed *= max(0.02f, absYawDelta / INTERPOLATION_SLOWDOWN_DIST);
	else if(absPitchDelta < INTERPOLATION_SLOWDOWN_DIST && absPitchDelta > INTERPOLATION_TARGET_MAX_ERROR)
		interpolationSpeed *= max(0.02f, absPitchDelta / INTERPOLATION_SLOWDOWN_DIST);
	//scale yaw and pitch delta
	if(absYawDelta > interpolationSpeed)
		yawDelta = ((yawDelta < 0.0f)?-1.0f:1.0f) * interpolationSpeed;
	if(absPitchDelta > interpolationSpeed)
		pitchDelta = ((pitchDelta < 0.0f)?-1.0f:1.0f) * interpolationSpeed * INTERPOLATION_PITCH_REL_SPEED;

	//set deltas
	if(absYawDelta >= INTERPOLATION_TARGET_MAX_ERROR)
		SetYawDelta(yawDelta);
	if(absPitchDelta >= INTERPOLATION_TARGET_MAX_ERROR)
		SetPitchDelta(pitchDelta);

	return true;
}

void CCameraInputHelper::SetYaw(const float yaw) 
{ 
	m_fYaw = yaw;
	//clamp yaw range
	if(g_pGameCVars->cl_cam_orbit == 0)
		ClampPiRange(m_fYaw);
}