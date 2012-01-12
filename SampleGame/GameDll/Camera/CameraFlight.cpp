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

#include "StdAfx.h"
#include "CameraFlight.h"
#include "CameraCommon.h"
#include "IViewSystem.h"
#include "IEntitySystem.h"
#include "IGameFramework.h"
#include "Game.h"
#include "CameraManager.h"
#include "CameraView.h"

//statics
const static uint32 NUM_OF_RESERVED_ENTRIES = 10;
const static float	RAY_SCAN_DISTANCE = 3.0f;

CCameraFlight::CCameraFlight() :
m_fFlightSpeed(1.0f),
m_fMoveSpeedMps(4.0f)
{
	m_cameraCourse.reserve(NUM_OF_RESERVED_ENTRIES);
	m_tempCourseA.reserve(NUM_OF_RESERVED_ENTRIES);
	m_tempCourseB.reserve(NUM_OF_RESERVED_ENTRIES);

	Reset();
}

CCameraFlight::~CCameraFlight()
{
}

CCameraFlight* CCameraFlight::GetInstance()
{
	static CCameraFlight *g_camFlight = new CCameraFlight();
	return g_camFlight;
}

void CCameraFlight::Reset()
{
	m_cameraCourse.clear();
	SetMoveSpeed();
	m_fFlightProgress = 0.0f;
	m_eMovementMode = eCFM_NONE;
	m_pRefEnt = NULL;
	m_eFadeMode = eCFFM_NONE;
	m_vRefPos = Vec3Constants<float>::fVec3_Zero;
	m_vRefPos2 = Vec3Constants<float>::fVec3_Zero;
	m_vRefDir = Vec3Constants<float>::fVec3_OneY;
	m_vLookingDirection = Vec3Constants<float>::fVec3_OneY;
	m_vTargetFadePos = Vec3Constants<float>::fVec3_Zero;
	m_bPaused = false;
	m_bUseRefDir = false;
	m_fCourseLengthOverwrite = 0.0f;
	m_fFadeTime = 0.0f;
	m_qFadeOrientation.SetIdentity();
	m_fFadeProgress = 0.0f;
	m_eState = eCFS_NONE;
}

void CCameraFlight::InitFlight(const Vec3 &vRefPos, IEntity *pRefEntity)
{
	//data is relative to this spot
	m_vRefPos = vRefPos;
	m_vRefPos2 = Vec3Constants<float>::fVec3_Zero;
	m_pRefEnt = pRefEntity;
	//progress and fading
	m_fFlightProgress = 0.0f;
	m_fCourseLengthOverwrite = 0.0f;
	m_vTargetFadePos = m_vRefPos;
	m_fFadeProgress = 0.0f;
	m_bUseRefDir = false;
	m_eState = eCFS_NONE;

	//update course speed
	SetMoveSpeed(m_fMoveSpeedMps);
}

void CCameraFlight::SetCameraCourse(const std::vector<SCameraFlightPoint> &cPositions, const Vec3 &vRefPos, IEntity *pRefEntity)
{
	CRY_ASSERT(cPositions.size());

	const int iNumPoints = cPositions.size();
	m_tempCourseA.resize(iNumPoints);
	m_tempCourseB.resize(iNumPoints);

	m_cameraCourse = cPositions;
	if(iNumPoints == 2)
		m_cameraCourse.push_back(m_cameraCourse[1]);

	InitFlight(vRefPos, pRefEntity);
}

void CCameraFlight::SetCameraCourse(const std::vector<Vec3> &cPositions, const Vec3 &vLookAt, bool bLookAtRelative, const Vec3 &vRefPos, IEntity *pRefEntity)
{
	CRY_ASSERT(cPositions.size() > 1);

	const int iNumPoints = cPositions.size();
	m_cameraCourse.resize(iNumPoints);
	m_tempCourseA.resize(iNumPoints);
	m_tempCourseB.resize(iNumPoints);

	for(int i = 0; i < iNumPoints; ++i)
	{
		const Vec3 &vPos = cPositions[i];
		m_cameraCourse[i].m_vCamPos = vPos;
		m_cameraCourse[i].m_vCamLookAt = (bLookAtRelative)?vPos+vLookAt:vLookAt;
	}

	//this is a linear "track"
	if(m_cameraCourse.size() == 2)
		m_cameraCourse.push_back(m_cameraCourse[1]);

	InitFlight(vRefPos, pRefEntity);
}

void CCameraFlight::SetMoveSpeed(float fMpS /* = 4.0f */)
{
	CRY_ASSERT(fMpS > 0.0f);
	fMpS = max(fMpS, g_fCamError);

	if(m_cameraCourse.size() < 3)
	{
		m_fFlightSpeed = 1.0f;
		return;
	}

	//collect flight distance
	float fMoveDistance = max(0.01f, GetCourseLength());

	//compute speed per second
	float fFlightTime = fMoveDistance / fMpS;
	m_fFlightSpeed = 1.0f / fFlightTime;
	m_fMoveSpeedMps = fMpS;
}

void CCameraFlight::UpdateFlight(SViewParams &viewParams)
{
	if(m_eMovementMode != eCFM_FREE_FLIGHT &&
		(m_fFlightProgress >= 1.0f || m_eMovementMode == eCFM_NONE || m_cameraCourse.size() < 3))
	{
		//update free fly point while not in free fly
		m_freeFlyPoint.m_vCamPos = viewParams.position;
		m_freeFlyPoint.m_vCamLookAt = viewParams.position + Vec3Constants<float>::fVec3_OneY;
		m_eState = eCFS_NONE;
		//nothing else to do
		return;
	}

	m_eState = eCFS_RUNNING;

	//update ref pos
	if(m_pRefEnt)
		m_vRefPos = m_pRefEnt->GetWorldPos();
	//if refPos2 is set, find middle
	if(m_vRefPos2.len2() > 0.0f)
		m_vRefPos = (m_vRefPos + m_vRefPos2) * 0.5f;

	//find target
	SCameraFlightPoint targetPoint = SCameraFlightPoint();

	switch(m_eMovementMode)
	{
	case eCFM_FREE_FLIGHT:
		targetPoint = m_freeFlyPoint;
		break;
	case eCFM_SPLINE:
		targetPoint = GetSplinePoint(m_fFlightProgress);
		break;
	case eCFM_LINE:
		targetPoint = GetTrackPoint(m_fFlightProgress);
		break;
	default:
		break;
	}

	//compute new dir/pos
	m_vLookingDirection = targetPoint.m_vCamLookAt - targetPoint.m_vCamPos;
	if(m_bUseRefDir)
	{
		m_vLookingDirection = m_vRefDir;
		m_qFadeOrientation = Quat::CreateRotationVDir(m_vLookingDirection, 0.0f);
	}
	m_vLookingDirection.NormalizeSafe();
	Quat qTempDirection = Quat::CreateRotationVDir(m_vLookingDirection, 0.0f);
	Vec3 vTempPos = targetPoint.m_vCamPos;

	bool bFading = false;

	//compute fading
	if(m_eMovementMode != eCFM_FREE_FLIGHT)
	{
		if(m_fFlightProgress > m_fFadeOutTime && (m_eFadeMode == eCFFM_OUT || m_eFadeMode == eCFFM_INOUT))
		{
			//fade position
			m_fFadeProgress = InterpolateTo(m_fFadeProgress, 1.0f, m_fFadeTime);
			m_vTargetFadePos = vTempPos * (1.0f - m_fFadeProgress) + viewParams.position * m_fFadeProgress;
			//fade orientation
			qTempDirection = Quat_tpl<float>::CreateNlerp(m_qFadeOrientation, viewParams.rotation, m_fFadeProgress);
			if(m_fFadeProgress < 0.998f)
			{
				bFading = true;
				m_eState = eCFS_FADE_OUT;
			}
		}
		else if(m_fFlightProgress < m_fFadeInTime && (m_eFadeMode == eCFFM_IN || m_eFadeMode == eCFFM_INOUT))
		{
			//fade position
			m_fFadeProgress = InterpolateTo(m_fFadeProgress, 1.0f, m_fFadeTime);
			m_vTargetFadePos = viewParams.position * (1.0f - m_fFadeProgress) + vTempPos * m_fFadeProgress;
			//fade orientation
			qTempDirection = Quat_tpl<float>::CreateNlerp(viewParams.rotation, qTempDirection, m_fFadeProgress);
			if(m_fFadeProgress < 0.998f)
			{
				bFading = true;
				m_eState = eCFS_FADE_IN;
			}
		}
		else
		{
			m_vTargetFadePos = vTempPos;
			//m_vTargetFadeLookAt = targetPoint.m_vCamLookAt;
			m_qFadeOrientation = qTempDirection;
			m_fFadeProgress = 0.0f;
			m_eState = eCFS_RUNNING;
		}
	}
	else
	{
		m_vTargetFadePos = vTempPos;
	}

	//update dir
	m_vLookingDirection = qTempDirection.GetColumn1();

	//raycast to prevent clipping during flight
	if(m_eMovementMode != eCFM_FREE_FLIGHT)
		DetectCollisions();

	//set position and rotation to viewparams
	viewParams.rotation = qTempDirection;
	viewParams.position = m_vTargetFadePos;//InterpolateTo(m_vTargetFadePos, viewParams.position, 1.0f);

	//progress flight
	if(m_eMovementMode != eCFM_FREE_FLIGHT && !bFading)
	{
		if(m_bPaused && m_fFlightProgress < 0.2f)
		{
			m_fFlightProgress += gEnv->pTimer->GetFrameTime() * m_fFlightSpeed;
			m_fFlightProgress = min(0.2f, m_fFlightProgress);
		}
		else if (!m_bPaused)
			m_fFlightProgress += gEnv->pTimer->GetFrameTime() * m_fFlightSpeed;
	}
}

SCameraFlightPoint CCameraFlight::GetTrackPoint(float t)
{
	//clamp to 0..1 range
	CRY_ASSERT( t >= 0.0f && t <= 1.0f );
	t = clamp(t, 0.0f, 1.0f);

	float fLength = GetCourseLength();
	float fPositionOffset = fLength * t;

	CRY_ASSERT(!m_cameraCourse.empty());
	std::vector<SCameraFlightPoint>::const_iterator iter = m_cameraCourse.begin();
	// iterate to the second position
	++iter;
	std::vector<SCameraFlightPoint>::const_iterator end = m_cameraCourse.end();

	float fMoveDistance = 0.0f;
	float fLastMoveDistance = 0.0f;
	Vec3 vTempPos = m_cameraCourse[0].m_vCamPos;
	Vec3 vLastLookAt = m_cameraCourse[0].m_vCamLookAt;
	//compute track position for t
	for(; iter != end; ++iter)
	{
		fLastMoveDistance = fMoveDistance;
		Vec3 vEdge = iter->m_vCamPos - vTempPos;
		float fEdgeDistance = vEdge.len();
		fMoveDistance += fEdgeDistance;
		if(fMoveDistance > fPositionOffset)
		{
			float fEdgeDelta = fPositionOffset - fLastMoveDistance;
			Vec3 vLookDelta = (iter->m_vCamLookAt - vLastLookAt).normalized();
			return SCameraFlightPoint(vTempPos + fEdgeDelta * vEdge.normalized(), vLastLookAt + fEdgeDelta * vLookDelta) + m_vRefPos;
		}

		//update last Pos
		vLastLookAt = iter->m_vCamLookAt;
		vTempPos = iter->m_vCamPos;
	}
	//all points are relative to m_vRefPos (which can be ZERO)
	return m_cameraCourse[0] + m_vRefPos;
}

SCameraFlightPoint CCameraFlight::GetSplinePoint(float t)
{
	if(m_cameraCourse.size() < 3)
		return SCameraFlightPoint();

	//clamp to 0..1 range
	CRY_ASSERT( t >= 0.0f && t <= 1.0f );
	t = clamp(t, 0.0f, 1.0f);

	m_tempCourseA = m_cameraCourse;
	m_tempCourseB.clear();

	//generate spline points
	do
	{
		for(int p = 0; p < m_tempCourseA.size()-1; ++p)
		{
			m_tempCourseB.push_back(m_tempCourseA[p]*(1.0f-t) + m_tempCourseA[p+1]*t);
		}
		m_tempCourseA = m_tempCourseB;
		m_tempCourseB.clear();
	}
	while (m_tempCourseA.size() > 1);

	//all points are relative to m_vRefPos (which can be ZERO)
	return m_tempCourseA[0] + m_vRefPos;
}

float CCameraFlight::GetCourseLength() const
{
	if(m_fCourseLengthOverwrite > 0.0f)
		return m_fCourseLengthOverwrite;

	float fMoveDistance = 0.0f;
	Vec3 vTempPos = m_cameraCourse[0].m_vCamPos;

	CRY_ASSERT(!m_cameraCourse.empty());
	std::vector<SCameraFlightPoint>::const_iterator iter = m_cameraCourse.begin();
	// iterate to the second position
	++iter;

	std::vector<SCameraFlightPoint>::const_iterator end = m_cameraCourse.end();
	for(; iter != end; ++iter)
	{
		fMoveDistance += (iter->m_vCamPos - vTempPos).len();
		vTempPos = iter->m_vCamPos;
	}

	return fMoveDistance;
}

void CCameraFlight::DetectCollisions()
{
	primitives::sphere	sph;
	sph.r				= 0.15f;
	sph.center	= m_vTargetFadePos + m_vLookingDirection * RAY_SCAN_DISTANCE;
	CCameraRayScan *pRayScan = g_pGame->GetCameraManager()->GetCamView()->GetCamRayScan();
	if (m_RayId == INVALID_RAY_ID)
	{
		m_RayId = pRayScan->ShootRay(sph.center, -m_vLookingDirection * RAY_SCAN_DISTANCE, ent_all & ~(ent_living | ent_independent | ent_rigid), geom_colltype0);
	}
	const RayCastResult* pRayRes = pRayScan->GetExternalHit(m_RayId);
	const ray_hit *pHit = pRayRes->hitCount > 0 ? &pRayRes->hits[0] : NULL;
	static int iNumHits = 0;
	if(pHit && pHit->dist > 0.0f)
	{
		iNumHits++;
		if(iNumHits > 2)
		{
			bool bIgnore = false;
			IPhysicalEntity *pPhysEntity = pHit->pCollider;
			if(pPhysEntity)
			{
				int iForeignData = pPhysEntity->GetiForeignData();
				if (iForeignData == PHYS_FOREIGN_ID_STATIC)
				{
					//check whether the hit rendernode is "vegetation"
					void *pForeignData = pPhysEntity->GetForeignData(PHYS_FOREIGN_ID_STATIC);
					IRenderNode * pRN = (IRenderNode*)pForeignData;
					if(pRN && pRN->GetRenderNodeType() == eERType_Vegetation)
						bIgnore = true;
				}
				else if(iForeignData == PHYS_FOREIGN_ID_ENTITY)
				{
					IEntity *pEntity = gEnv->pEntitySystem->GetEntityFromPhysics(pPhysEntity);
					if(pEntity && (pEntity == m_pRefEnt || pEntity->GetId() == LOCAL_PLAYER_ENTITY_ID))
						bIgnore = true;
				}
				else if(iForeignData == -1)
					bIgnore = true;
			}

			if(!bIgnore)
				m_vTargetFadePos = m_vTargetFadePos + m_vLookingDirection * max(0.0f, (RAY_SCAN_DISTANCE - pHit->dist));
			else
				iNumHits = 0;
		}
	}
	else
		iNumHits = 0;

	if (pRayRes)
	{
		pRayScan->RemoveExternalHit(m_RayId);
		m_RayId = INVALID_RAY_ID;
	}
}