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

#include "StdAfx.h"
#include "CameraView.h"
#include "Game.h"
#include "CameraFlight.h"
#include "Actor.h"
#include "CameraInputHelper.h"
#include "Player.h"
#include "CameraOverrides.h"
#include "CameraRayScan.h"

//static
static const float g_fModeTransitionSlow = 0.3f;
static const float g_fModeTransitionFast = 0.05f;
static const float g_fFirstPersonCamDistance = 0.5f;

CCameraView::CCameraView(IEntity *pTarget) :
m_pTarget(NULL),
m_fFrameTime(0.0f),
m_pCamHelper(NULL),
m_vTargetPosition(ZERO),
m_bModeTransition(false),
m_fTransitionTimeout(0.0f)
{
	SetTarget(pTarget);
	//setup scanning and tracking
	m_pCamRayScan = new CCameraRayScan();
	m_camTracking.SetCameraRayScan(m_pCamRayScan);
}

CCameraView::~CCameraView()
{
	SAFE_DELETE(m_pCamRayScan);
}

void CCameraView::Reset()
{
	//initialize the camera view
	IViewSystem *pViewSystem = g_pGame->GetIGameFramework()->GetIViewSystem();
	IView *pView = pViewSystem->GetActiveView();
	CRY_ASSERT(pView);
	m_lastViewParams = *(pView->GetCurrentParams());

	//the player can still change camera angles etc.
	CPlayer *pHero = CPlayer::GetHero();
	CRY_ASSERT(pHero);


	m_pCamHelper = pHero->GetCameraInputHelper();
	CRY_ASSERT(m_pCamHelper);

	m_bModeTransition = false;
	m_fTransitionTimeout = 0.0f;

	m_curPolar.SetPitch(1.f);
}

void CCameraView::SetTarget(IEntity *pTarget)
{
	if(m_pTarget == pTarget)
		return;

	m_pTarget = pTarget;
	m_vTargetOffset = Vec3Constants<float>::fVec3_Zero;
	m_vTargetPosition = Vec3Constants<float>::fVec3_Zero;

	//no target -> nothing to do
	if(!pTarget)
		return;

	//new target, fresh start
	Reset();

	//set target position
	m_vTargetPosition = m_pTarget->GetWorldPos() + m_vTargetOffset;

	//compute offset for actor (usually head height)
	IActor *pTargetActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(pTarget->GetId());
	if(pTargetActor)
	{
		CActor *pTempActor = static_cast<CActor*> (pTargetActor);
		const SStanceInfo *pStanceInfo = pTempActor->GetStanceInfo(pTempActor->GetStance());
		CRY_ASSERT(pStanceInfo);
		m_vTargetOffset.z = pStanceInfo->viewOffset.z;
	}
}

void CCameraView::UpdateSettings(SViewParams &viewParams)
{
	//cache some values for interpolation
	//float fLastDistance = m_curSettings.dist;

	ECamTypes eOldType = m_curSettings.camType;
	//get camera settings
	CCameraManager *pCamMan = g_pGame->GetCameraManager();
	CameraID iCurCam = pCamMan->GetActiveCameraId();
	pCamMan->GetCameraSettings(iCurCam, m_curSettings);

	if(eOldType != m_curSettings.camType)
	{
		m_bModeTransition = true;
		m_fTransitionTimeout = 1.0f;
	}

	//pre-calculations
	viewParams.fov = InterpolateTo(m_lastViewParams.fov, DEG2RAD(m_curSettings.FOV), 0.5f);

	//get frame time
	m_fFrameTime = max(gEnv->pTimer->GetFrameTime(), 0.0001f);

	// add cam offsets
 	m_curSettings.dist = g_pGameCVars->cl_cam_orbit_distance;
// 	Vec3 camOffset(Vec3Constants<float>::fVec3_Zero);
	Vec3 camOffset = viewParams.rotation.GetColumn1().Cross(Vec3Constants<float>::fVec3_OneZ);
	camOffset.NormalizeFast();
	camOffset *= g_pGameCVars->cl_cam_orbit_offsetX;
	camOffset.z = g_pGameCVars->cl_cam_orbit_offsetZ;

	//update/interpolation position
	m_vTargetPosition = m_pTarget->GetWorldPos() + m_vTargetOffset + camOffset; //InterpolateTo(m_vTargetPosition, m_pTarget->GetWorldPos() + m_vTargetOffset, 0.1f);
}

void CCameraView::Update(SViewParams &viewParams)
{
	//valid target ?
	if(m_pTarget == NULL)
		return;

	//get latest settings
	UpdateSettings(viewParams);

	//force first person
	if(g_pGameCVars->cl_fake_first_person)
		m_curSettings.camType = ECT_CamFirstPerson;

//	m_curSettings.camType = ECT_CamFollow;

	//update camera mode
	switch(m_curSettings.camType)
	{
	case ECT_CamRear:
		UpdateRearMode();
		break;
	case ECT_CamFollow:
		UpdateFollowMode();
		break;
	case ECT_CamRefDir:
		UpdateRefDirMode();
		break;
	case ECT_CamOrbit:
		UpdateOrbitMode();
		break;
	case ECT_CamFirstPerson:
		UpdateFirstPersonMode();
		break;
	default:
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "Unhandled camera mode active.");
		break;
	}

	//write update results to viewParams
	WriteViewParams(viewParams);

	//update mode transition if necessary
	RunModeTransition(viewParams);

	//update rayscan, distance checks and camera tracking
	RunScanAndTrack(viewParams);

	//update collisions

	//write back view matrix
	Matrix33 matCam = Matrix33(viewParams.rotation);
	CPlayer::GetHero()->m_camViewMtxFinal = matCam;
	//write back to cam helper
	if (m_pCamHelper)
	{
		m_pCamHelper->SetYaw(m_curPolar.GetYaw());
		m_pCamHelper->SetPitch(m_curPolar.GetPitch());
	}

	//automatic flights
	UpdateCameraFlight(viewParams);

	//update "last params"
	m_lastViewParams = viewParams;
}

void CCameraView::UpdateFollowMode()
{
	//get direction from user input
	//Vec3 vDir = SphericalToCartesian(m_pCamHelper->GetYaw(), m_pCamHelper->GetPitch(), 1.0f);

	//get ideal distance and compute new distance
	float fIdealDistance = (m_curSettings.dist + m_curSettings.maxDist) * 0.5f;
	float fDistTolerance = max(m_curSettings.maxDist - m_curSettings.dist, 0.01f) * 0.3f;
	float fMinDistance = (m_curSettings.dist < m_lastViewParams.dist)? m_curSettings.dist : m_lastViewParams.dist;

	//Dir to target
	Vec3 vDir = m_vTargetPosition - m_lastViewParams.position;
	float fFollowDistance = vDir.len();
	if(fFollowDistance < g_fCamError)
	{
		fFollowDistance = 1.0f;
		vDir = Vec3Constants<float>::fVec3_OneY;
	}
	vDir /= fFollowDistance; //normalize

	fFollowDistance = m_curSettings.dist;

	//compute and interpolate follow distance
	if(fabsf(fFollowDistance - fIdealDistance) > fDistTolerance)
	{
		fFollowDistance = InterpolateTo(fFollowDistance, fIdealDistance, 0.5f);
		fFollowDistance = clamp(fFollowDistance, fMinDistance, m_curSettings.maxDist);
	}

	//interpolate rotation
	//m_curPolar

	//add user input
	if (m_pCamHelper)
		m_curPolar.Set(m_curPolar.GetYaw() + m_pCamHelper->RetrieveYawDelta(), m_curPolar.GetPitch() + m_pCamHelper->RetrievePitchDelta());
}

void CCameraView::UpdateOrbitMode()
{
	//get direction from user input
	if (m_pCamHelper)
		m_curPolar.Set(m_curPolar.GetYaw() + m_pCamHelper->RetrieveYawDelta(), m_curPolar.GetPitch() + m_pCamHelper->RetrievePitchDelta());
}

void CCameraView::UpdateRefDirMode()
{
	//get "position - direction" from the system, but looking direction from flowgraph
	m_curPolar.Set(m_curSettings.vRefDir);
}

void CCameraView::UpdateRearMode()
{
	//compute camera direction
	const Quat &qRotation = m_pTarget->GetRotation();
	Vec3 vCamDir = qRotation.GetColumn1();
	//add player input pitch
	m_curPolar.Set(vCamDir);
	if (m_pCamHelper)
		m_curPolar.SetPitch(m_pCamHelper->GetPitch());
}

void CCameraView::UpdateFirstPersonMode()
{
	//overwrite settings -> fix this in the flowgraph when adding the mode!
	m_curSettings.dist = 0.05f;
	m_curSettings.hOff = 0.0f;
	m_curSettings.vOff = 0.15f;

	//compute camera direction
	const Quat &qRotation = m_pTarget->GetRotation();
	Vec3 vCamDir = qRotation.GetColumn1();

	//move camera a little in front of the target
	m_vTargetPosition += vCamDir * g_fFirstPersonCamDistance;

	//add player input pitch
	m_curPolar.Set(vCamDir);
	if (m_pCamHelper)
		m_curPolar.SetPitch(m_pCamHelper->GetPitch());
}

void CCameraView::WriteViewParams(SViewParams &viewParams)
{
	//get direction from polar
	const Vec3 &vDir = m_curPolar.GetDir();

	//compute final camera position
	viewParams.position = m_vTargetPosition - vDir * m_curSettings.dist;
	//set final camera rotation
	viewParams.rotation = Quat::CreateRotationVDir(vDir, 0.0f);

	//add offsets
	viewParams.position.z += m_curSettings.vOff;
	Vec3 vRight = vDir.Cross(Vec3Constants<float>::fVec3_OneZ);
	viewParams.position += vRight * m_curSettings.hOff;

	//set distance
	viewParams.dist = m_curSettings.dist;
}

void CCameraView::UpdateCameraFlight(SViewParams &viewParams)
{
	CCameraOverrides *pCamOverride = g_pGame->GetCameraManager()->GetCamOverrides();
	pCamOverride->SetLookAtOverride(viewParams.position, NULL, NULL);

	//camera flights overwrite the normal camera
	CCameraFlight::GetInstance()->UpdateFlight(viewParams);
}

void CCameraView::RunModeTransition(SViewParams &viewParams)
{
	if(!m_bModeTransition)
		return;

	//update time-limit
	m_fTransitionTimeout -= m_fFrameTime;
	if(m_fTransitionTimeout < 0.1f)
		m_fTransitionTimeout = 0.1f;

	//compute transition speed
	float fDist = max(0.01f, (m_lastViewParams.position - viewParams.position).len());
	float fTransitionTime = 1.0f - clamp(fDist / 5.0f, 0.0f, 1.0f);
	fTransitionTime = max(g_fModeTransitionFast, g_fModeTransitionSlow * fTransitionTime);
	//after timeout, catch up instantly (0 transition time)
	fTransitionTime *= m_fTransitionTimeout;

	//fade values
	viewParams.position = InterpolateTo(m_lastViewParams.position, viewParams.position, fTransitionTime);
	viewParams.dist = (m_vTargetPosition - viewParams.position).len();

	//fov is always interpolated (on update)
	//viewParams.fov = InterpolateTo(m_lastViewParams.fov, viewParams.fov, fTransitionTime);

	//terminate
	if(fDist < 0.02f)
	{
		m_bModeTransition = false;
		return;
	}
}

void CCameraView::RunScanAndTrack(SViewParams &viewParams)
{
	//get actual view data
	const Vec3 &vCamDir = viewParams.rotation.GetColumn1();
	const Vec3 &vCamTarget = viewParams.position + vCamDir * viewParams.dist;

	//update rayscan

	//strange bugs are happening since living entities are being hit
	IPhysicalEntity *pTargetPhysics = m_pTarget->GetPhysics();
	m_pCamRayScan->ShootRays(vCamTarget, -vCamDir * viewParams.dist, &pTargetPhysics, 1);
	ray_hit *pHit = m_pCamRayScan->GetHit();

	//was there a collision ?
	bool bCollision = (pHit != 0 && pHit->dist > 0.0f && pHit->dist < viewParams.dist);

	//camera tracking
	if(m_curSettings.collisionType == ECCT_CollisionTrack || m_curSettings.collisionType == ECCT_CollisionTrackOrCut)
	{
		float hOffTemp = 0.0f;
		//this shouldn't need the hero to update -> refactor camera tracker
		const CPlayer &hero = *(CPlayer::GetHero());
		bool bCollisionAvoided = m_camTracking.Update(viewParams, hOffTemp, m_curSettings, hero, bCollision);
// 		bCollision = bCollision & !bCollisionAvoided;
	}

	Vec3 vDestPos(viewParams.position);
	if(bCollision)
	{
		//avoid collision
		vDestPos = vCamTarget - vCamDir * pHit->dist;
	}

	// smooth slide
	if (g_pGameCVars->cl_cam_orbit_slide > 0 && (!pHit || !pHit->bTerrain))
	{
		Vec3 vDir(vCamDir);
		vDir *= ((m_lastViewParams.position + (vDestPos - m_lastViewParams.position) * g_pGameCVars->cl_cam_orbit_slidespeed * 10.f * viewParams.frameTime) - viewParams.position).GetLengthFast();
		viewParams.position += vDir;
	}
	else
	{
		viewParams.position = vDestPos;
	}
}