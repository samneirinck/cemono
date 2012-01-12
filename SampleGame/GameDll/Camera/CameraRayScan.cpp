/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Manager for camera raycasts

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#include "StdAfx.h"
#include "CameraRayScan.h"
#include "Game.h"

//this is used to make sure the camera has some extra space inside the rayscan range
const static float RAY_SCAN_BUFFER_SCALE = 1.1f;
const static float RAY_SCAN_OFFSET_DISTANCE = 0.2f;
//raycast settings
const int CCameraRayScan::g_objTypes = (ent_all | ent_water) & ~(ent_living | ent_independent | ent_rigid);
const int CCameraRayScan::g_geomFlags = geom_colltype0|geom_colltype_player|rwi_stop_at_pierceable;
//raycast vectors
const static Vec3 g_vRayUpOffset(0,0,0.1f);
const static Vec3 g_vRayUpDir(0,0,0.1f);

CCameraRayScan::CCameraRayScan()
{
	Reset();
}

CCameraRayScan::~CCameraRayScan()
{
}

void CCameraRayScan::OnRayCastResult(const QueuedRayID &rayID, const RayCastResult &result)
{
	for(int i = 0; i < eNUM_RAYS; ++i)
	{
		if (m_rayInfo[i].rayID == rayID)
		{
			m_rayInfo[i].rayID = INVALID_RAY_ID;
			m_rayInfo[i].hashit = result.hitCount > 0;
			if (result.hitCount > 0)
				m_rayInfo[i].hit = result.hits[0];
			return;
		}
	}
	assert(m_ExternalRayCasts.find(rayID) == m_ExternalRayCasts.end());
	m_ExternalRayCasts[rayID] = result;
}

void CCameraRayScan::Reset()
{
	//clear results
	for(int i = 0; i < eNUM_RAYS; ++i)
	{
		m_rayInfo[i].rayID = INVALID_RAY_ID;
		m_rayInfo[i].hashit = false;
	}
	m_ExternalRayCasts.clear();
}

ray_hit * CCameraRayScan::GetHit(ECameraRays nr)
{
	if(nr >= 0 && nr < eNUM_RAYS)
		return m_rayInfo[nr].hashit ? &(m_rayInfo[nr].hit) : NULL;

	int nClosesHit = 0;
	float dist = 9999.f;
	for(int i = 0; i < eNUM_RAYS; ++i)
	{
		if(m_rayInfo[i].hashit)
		{
			ray_hit& hit = m_rayInfo[i].hit;
			if(m_rayInfo[i].hit.dist < dist)
			{
				dist = m_rayInfo[i].hit.dist;
				nClosesHit = i;
			}
		}
	}
	return m_rayInfo[nClosesHit].hashit ? &(m_rayInfo[nClosesHit].hit) : NULL;
}

const Vec3& CCameraRayScan::GetRayDir(ECameraRays nr) const
{
	if(nr >= 0 && nr < eNUM_RAYS)
		return m_rayInfo[nr].dir;
	return Vec3Constants<float>::fVec3_Zero;
}

const RayCastResult* CCameraRayScan::GetExternalHit(const QueuedRayID& queuedId) const
{
	TRayCastResultMap::const_iterator it = m_ExternalRayCasts.find(queuedId);
	if (it != m_ExternalRayCasts.end())
		return &it->second;
	return NULL;
}

void CCameraRayScan::RemoveExternalHit(const QueuedRayID& queuedId)
{
	TRayCastResultMap::iterator it = m_ExternalRayCasts.find(queuedId);
	if (it != m_ExternalRayCasts.end())
		m_ExternalRayCasts.erase(it);
}

void CCameraRayScan::ShootRays(const Vec3 &rayPos, const Vec3 &rayDir, IPhysicalEntity **pSkipEnts, int numSkipEnts)
{

	//shoot rays for all ray_hits
	const Vec3 dirNorm = rayDir.normalized();
	const Vec3 right = dirNorm.Cross(Vec3Constants<float>::fVec3_OneZ);
	const Vec3 rightOff = right * 0.15f;
	const Vec3 rightDir = right * 0.15f;
	const float len = rayDir.len() * RAY_SCAN_BUFFER_SCALE; //add some distance to be sure that the view is free

	const Vec3 rayPos2 = rayPos + (dirNorm * RAY_SCAN_OFFSET_DISTANCE); //move the rays away from the head to prevent clipping

	//center ray
	Vec3 tempPos = rayPos2;
	Vec3 tempDir = dirNorm;
	ShootRayInt(eRAY_CENTER, tempPos, tempDir, len, pSkipEnts, numSkipEnts);

	tempDir = (dirNorm - rightDir + g_vRayUpDir).normalized();
	tempPos = rayPos2 - rightOff + g_vRayUpOffset;
	ShootRayInt(eRAY_TOP_LEFT, tempPos, tempDir, len, pSkipEnts, numSkipEnts);

	tempDir = (dirNorm + rightDir + g_vRayUpDir).normalized();
	tempPos = rayPos2 + rightOff + g_vRayUpOffset;
	ShootRayInt(eRAY_TOP_RIGHT, tempPos, tempDir, len, pSkipEnts, numSkipEnts);

	tempDir = (dirNorm - rightDir - g_vRayUpDir).normalized();
	tempPos = rayPos2 - rightOff - g_vRayUpOffset;
	ShootRayInt(eRAY_BOTTOM_LEFT, tempPos, tempDir, len, pSkipEnts, numSkipEnts);

	tempDir = (dirNorm + rightDir - g_vRayUpDir).normalized();
	tempPos = rayPos2 + rightOff - g_vRayUpOffset;
	ShootRayInt(eRAY_BOTTOM_RIGHT, tempPos, tempDir, len, pSkipEnts, numSkipEnts);

	tempDir = (dirNorm + g_vRayUpDir).normalized();
	tempPos = rayPos2 + g_vRayUpOffset * 2.0f;
	ShootRayInt(eRAY_TOP_CENTER, tempPos, tempDir, len, pSkipEnts, numSkipEnts);

	tempDir = (dirNorm - g_vRayUpDir).normalized();
	tempPos = rayPos2 - g_vRayUpOffset * 2.0f;
	ShootRayInt(eRAY_BOTTOM_CENTER, tempPos, tempDir, len, pSkipEnts, numSkipEnts);
}

void CCameraRayScan::ShootRayInt(ECameraRays camRay, const Vec3 &rayPos, const Vec3 &rayDir, const float& len, IPhysicalEntity **pSkipEnts, int numSkipEnts)
{
	if (camRay < eNUM_RAYS)
	{
		if (m_rayInfo[camRay].rayID == INVALID_RAY_ID)
		{
			m_rayInfo[camRay].dir = rayDir;
			m_rayInfo[camRay].rayID = ShootRay(rayPos, rayDir * len, g_objTypes, g_geomFlags, pSkipEnts, numSkipEnts);
		}
	}
}

QueuedRayID CCameraRayScan::ShootRay(const Vec3 &rayPos, const Vec3 &rayDir, int objTypes /*= g_objTypes*/, int geomFlags /*= g_geomFlags*/, IPhysicalEntity **pSkipEnts /*= NULL*/, int numSkipEnts /*= 0*/)
{
	return g_pGame->GetRayCaster().Queue(RayCastRequest::MediumPriority,
		RayCastRequest(rayPos, rayDir, objTypes, geomFlags, pSkipEnts, numSkipEnts),
		functor(*this, &CCameraRayScan::OnRayCastResult));
}
