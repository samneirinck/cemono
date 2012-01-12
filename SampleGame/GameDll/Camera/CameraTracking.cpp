/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera code dealing with obstacle avoidance.

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#include "StdAfx.h"
#include "CameraTracking.h"
#include "CameraRayScan.h"
#include "CameraInputHelper.h"
#include "IViewSystem.h"
#include "Player.h"
#include "CameraPolar.h"

//init statics
const static float g_fInterpolationRate = 9.0f;
const static float g_fInterpolationWeight = 0.1f;
const static float g_fAutoTrackObstacleDistance = 0.4f;
const static float g_fOffsetTrackingDistance = 0.3f;

CCameraTracking::CCameraTracking() :
m_pCamRayScan(NULL),
m_fFrameTime(0.001f),
m_fYawDelta(0.0f),
m_fPitchDelta(0.0f),
m_eLastDirYaw(eTD_LEFT),
m_eLastDirPitch(eTD_TOP),
m_fSpeed(0.0f),
m_fTimeCovered(0.0f),
m_bViewCovered(false),
m_vLastObstaclePos(ZERO),
m_fAutoRotateSpeed(0.0f)
{
	// Currently, this class is a static instance. This means that we
	// can't store pointers to entities that might disappear, like
	// things in the level
}

void CCameraTracking::SetCameraRayScan(CCameraRayScan *pRayScan)
{
	m_pCamRayScan = pRayScan;
	CRY_ASSERT(m_pCamRayScan);
	CRY_ASSERT(pRayScan);
}

bool CCameraTracking::Update(SViewParams &viewParams, float &fHOffObstacleStrength, const SCamModeSettings &camMode, const CPlayer &hero, bool bObstacleFound /* = false */)
{
	if(!g_pGameCVars->cl_cam_tracking || !m_pCamRayScan)
		return false;

	m_fFrameTime = max(g_fCamError, gEnv->pTimer->GetFrameTime());

	//in combat mode this function doesn't really avoid obstacles, it avoids clipping
	float fCombatModeWeight = 5.0f;

	//default angle and minimum
	const float fNow = gEnv->pTimer->GetFrameStartTime().GetSeconds();

	CCameraInputHelper *pCamHelper = hero.GetCameraInputHelper();
	CRY_ASSERT(pCamHelper);
	float fLastUserInput = pCamHelper->GetLastUserInputTime();
	//user input overrides auto-follow
	if(fNow - fLastUserInput < 0.5f)
		return false;

	bool bTrackingActive = camMode.camType == ECT_CamFollow && (camMode.collisionType == ECCT_CollisionTrack || camMode.collisionType == ECCT_CollisionTrackOrCut);

	//get current values
	Vec3 curCamDir = viewParams.position-viewParams.targetPos;
	m_curCamOrientation.Set(0.0f, 0.0f, 0.0f);
	CartesianToSpherical(curCamDir, m_curCamOrientation);
	curCamDir.Normalize();

	if(m_curCamOrientation.m_fDist < g_pGameCVars->cl_cam_min_distance)
		m_curCamOrientation.m_fDist = g_pGameCVars->cl_cam_min_distance;

	//work in 0 .. 2PI
	m_curCamOrientation.m_fYaw += gf_PI; 

	//if there is something in the way
	if(bObstacleFound)	
	{
		//re-start fadeout
		m_fTimeCovered = 0.5f;
		//set last obstacle pos
		m_vLastObstaclePos = viewParams.position;
		
		//scan obstacle
		if(!IdentifyObstacle(curCamDir, hero))
			return false;
	}
	else if(fabsf(m_fYawDelta) > g_fCamError || fabsf(m_fPitchDelta) > g_fCamError)
	{
		//if there is nothing in the way, fade out the movement

		//time based fade
		if(m_fTimeCovered > 0)
		{
			m_fTimeCovered = max(m_fTimeCovered - m_fFrameTime, 0.0f);

			//these interpolators should be time and not frame based
			m_fYawDelta = (g_fInterpolationRate * m_fYawDelta) * g_fInterpolationWeight;
			m_fPitchDelta = (g_fInterpolationRate * m_fPitchDelta) * g_fInterpolationWeight;
			m_fSpeed = (g_fInterpolationRate * m_fSpeed) * g_fInterpolationWeight;
		}
		else
		{
			m_fYawDelta = 0.0f;
			m_fPitchDelta = 0.0f;
			m_fSpeed = 0.0f;
		}
	}

	//apply delta rotation for obstacle avoidance
	if(fabsf(m_fYawDelta) > g_fCamError || fabsf(m_fPitchDelta) > g_fCamError)
	{
		if(bTrackingActive)
		{
			//set new yaw
			float newYaw = m_curCamOrientation.m_fYaw + m_fYawDelta;
			//re-align yaw
			//the camera direction is 90° off and flipped compared to entity space
			newYaw = (newYaw - gf_PI * 0.5f) * -1.0f;
			//set new pitch
			float newPitch = m_curCamOrientation.m_fPitch + m_fPitchDelta;

			if(g_pGameCVars->cl_cam_orbit != 0)
			{
				//pCamHelper->SetTrackingDelta(-m_fYawDelta, m_fPitchDelta);
				pCamHelper->SetYawDelta(-m_fYawDelta);
				pCamHelper->SetPitchDelta(m_fPitchDelta);
			}
			else
			{
				//apply yaw/pitch on camera
				//pCamHelper->SetInterpolationTarget(newYaw, newPitch, gf_PI, 0.1f, 0.0f);
				//this will always reset follow cam interpolation
				pCamHelper->SetYawDelta(m_fYawDelta);
				pCamHelper->SetPitchDelta(m_fPitchDelta);
			}
		}
		else
		{
			//in orbit mode we basically simulate user input
			//pCamHelper->SetTrackingDelta(-fCombatModeWeight*g_fYawDelta, fCombatModeWeight*g_fPitchDelta);

			//in cutting mode we offset the camera to avoid clipping
			float offsetStrength = 0.0f;
			float offsetSpeed = 2.0f;
			if(bObstacleFound)
			{
				offsetStrength = (m_fYawDelta < 0.0f)?-g_fOffsetTrackingDistance:g_fOffsetTrackingDistance;
				offsetSpeed = 0.5f;
			}

			fHOffObstacleStrength = InterpolateTo(fHOffObstacleStrength, offsetStrength, offsetSpeed);
		}

		//CryLogAlways("new yaw %f, yawDelta %f", newYaw, g_yawDelta);
		return true;
	}
	else
		UpdateAutoFollow(viewParams, hero);

	return false;
}

bool CCameraTracking::IdentifyObstacle(const Vec3 &vCamDir, const CPlayer &hero)
{
	//check player direction
	Vec3 newDir = -hero.GetEntity()->GetForwardDir();
	newDir.z += vCamDir.z;
	newDir.normalize();

	//compute rotation speed
	const float fHeroSpeedModifier = clamp(hero.GetActorStats()->speedFlat / 4.0f, 0.3f, 1.0f);
	const float fNewSpeed = g_pGameCVars->cl_cam_tracking_rotation_speed * m_fFrameTime * fHeroSpeedModifier;
	m_fSpeed = InterpolateTo(m_fSpeed, fNewSpeed, (fNewSpeed>m_fSpeed)?0.1f:0.3f); 
	//m_fSpeed = (g_fInterpolationRate * m_fSpeed + speed) * g_fInterpolationWeight;

	//get ray data from camera ray tests
	ray_hit *pRayHit = m_pCamRayScan->GetHit(eRAY_TOP_RIGHT);
	if(!pRayHit || pRayHit->dist == 0.0f)
		pRayHit = m_pCamRayScan->GetHit(eRAY_BOTTOM_RIGHT);
	bool bHitsRight = (pRayHit && pRayHit->dist > 0.0f);
	Vec3 dirRight = (pRayHit)?-(m_pCamRayScan->GetRayDir(eRAY_TOP_RIGHT)):Vec3(ZERO);

	//ray data left side
	pRayHit = m_pCamRayScan->GetHit(eRAY_TOP_LEFT);
	if(!pRayHit || pRayHit->dist == 0.0f)
		pRayHit = m_pCamRayScan->GetHit(eRAY_BOTTOM_LEFT);
	bool bHitsLeft = (pRayHit && pRayHit->dist > 0.0f);
	Vec3 dirLeft = (pRayHit)?-(m_pCamRayScan->GetRayDir(eRAY_TOP_LEFT)):Vec3(ZERO);

	//left or right
	if(bHitsRight ^ bHitsLeft)
	{
		//find rotation direction
		if(!bHitsRight && !bHitsLeft)
		{
			if(m_eLastDirYaw == eTD_LEFT) //continue last direction
				newDir = dirLeft;
			else
				newDir = dirRight;
		}
		else if(!bHitsRight)
		{
			m_eLastDirYaw = eTD_RIGHT;
			newDir = dirRight;
		}
		else
		{
			m_eLastDirYaw = eTD_LEFT;
			newDir = dirLeft;
		}

		//compute yaw/pitch for target position
		float newYaw = 0.0f;
		float newPitch = 0.0f;
		float newDist = 0.0f;
		CartesianToSpherical(newDir * m_curCamOrientation.m_fDist, newYaw, newPitch, newDist);

		newYaw += gf_PI;

		//now interpolate to target

		//compute delta yaw
		m_fYawDelta = (newYaw - m_curCamOrientation.m_fYaw) * m_fSpeed;
		if(m_eLastDirYaw == eTD_RIGHT && m_fYawDelta < 0.0f || m_eLastDirYaw == eTD_LEFT && m_fYawDelta > 0.0f)
			m_fYawDelta *= -1.0f;
	}

	//compute top/bottom rotation

	//ray data top side
	pRayHit = m_pCamRayScan->GetHit(eRAY_TOP_CENTER);
	bool bHitsTop = (pRayHit && pRayHit->dist > 0.0f)?true:false;
	Vec3 vDirTop = (pRayHit)?-(m_pCamRayScan->GetRayDir(eRAY_TOP_CENTER)):Vec3(ZERO);

	//ray data bottom side
	pRayHit = m_pCamRayScan->GetHit(eRAY_BOTTOM_CENTER);
	bool bHitsBottom = (pRayHit && pRayHit->dist > 0.0f)?true:false;
	Vec3 vDirBottom = (pRayHit)?-(m_pCamRayScan->GetRayDir(eRAY_BOTTOM_CENTER)):Vec3(ZERO);

	//top or bottom (if not left or right)
	if(g_pGameCVars->cl_cam_tracking_allow_pitch && (bHitsTop ^ bHitsBottom) && !(bHitsRight ^ bHitsLeft))
	{
		//find rotation direction
		if(!bHitsTop && !bHitsBottom)
		{
			if(m_eLastDirPitch == eTD_TOP) //continue last direction
				newDir = vDirTop;
			else
				newDir = vDirBottom;
		}
		else if(!bHitsBottom)
		{
			m_eLastDirPitch = eTD_BOTTOM;
			newDir = vDirBottom;
		}
		else
		{
			m_eLastDirPitch = eTD_TOP;
			newDir = vDirTop;
		}

		//compute yaw/pitch for target position
		float newYaw = 0.0f;
		float newPitch = 0.0f;
		float newDist = 0.0f; //newdist (raydist) will be ignored
		CartesianToSpherical(newDir, newYaw, newPitch, newDist);

		//compute delta pitch
		m_fPitchDelta = (newPitch - m_curCamOrientation.m_fPitch) * m_fSpeed * 10.0f;
	}

	//if all rays hit - don't bother!
	//this is a termination condition when the camera is pulled through geometry
	if(bHitsLeft & bHitsRight & bHitsBottom & bHitsTop)
	{
		if(m_bViewCovered)
		{
			//if obstacle behind player
			//if(g_rHit.dist > 0.0f)
			//this is a strange fix, but it's working better and is much cheaper than a raycast
			if(fabsf(m_fYawDelta) < 0.01f && fabsf(m_fPitchDelta) > 0.001f)
				return false;
		}
		m_bViewCovered = true;
	}
	else
		m_bViewCovered = false;

	return true;
}

void CCameraTracking::UpdateAutoFollow(const SViewParams &viewParams, const CPlayer &hero)
{
	if(g_pGameCVars->cl_cam_auto_follow_rate > 0.0f) //auto-tracking
	{
		//if there is an obstacle nearby, don't try to rotate into it
		float lastObstacleDist = (m_vLastObstaclePos - viewParams.position).len();
		if(lastObstacleDist < g_fAutoTrackObstacleDistance)
			return;

		if(hero.GetActorStats()->speedFlat < g_pGameCVars->cl_cam_auto_follow_movement_speed)
		{
			//only rotate when player moves
			m_fAutoRotateSpeed = 0.0f;
			return;
		}

		//get camera direction
		Vec3 camDir = -m_pCamRayScan->GetRayDir(eRAY_CENTER);
		camDir.z = 0.0f;
		camDir.Normalize();
		//get Player direction
		Vec3 heroDirection = (hero.GetAnimatedCharacter()->GetAnimLocation().q * FORWARD_DIRECTION);

		//compute angle between directions
		float dt = camDir.Dot(heroDirection);	
		dt = clamp(dt, -1.0f, 1.0f);
		float angle = cry_acosf(dt);

		//check angle being bigger than threshold
		if(angle > g_pGameCVars->cl_cam_auto_follow_threshold)
		{
			float moveSpeed = max(0.002f, gf_PI*m_fFrameTime*g_pGameCVars->cl_cam_auto_follow_rate);
			m_fAutoRotateSpeed = InterpolateTo(max(0.002f, m_fAutoRotateSpeed), moveSpeed, 0.2f);

			//compute rotation direction by taking height part of cross-prod
			float dirVal = camDir.x * heroDirection.y - camDir.y * heroDirection.x;

			CCameraInputHelper *const pCamHelper = hero.GetCameraInputHelper();
				CRY_ASSERT(pCamHelper);
			if(dirVal > 0) //rotate right
				pCamHelper->SetTrackingDelta(-m_fAutoRotateSpeed, 0.0f);
			else //rotate left
				pCamHelper->SetTrackingDelta(m_fAutoRotateSpeed, 0.0f);
		}
		else
			m_fAutoRotateSpeed = 0.0f;
	}
}