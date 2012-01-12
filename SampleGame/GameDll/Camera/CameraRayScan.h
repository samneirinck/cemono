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

#ifndef CAMERA_RAY_SCAN
#define CAMERA_RAY_SCAN

#include "RayCastQueue.h"

enum ECameraRays
{
	eRAY_CENTER = 0,
	eRAY_TOP_LEFT,
	eRAY_TOP_RIGHT,
	eRAY_BOTTOM_LEFT,
	eRAY_BOTTOM_RIGHT,
	eRAY_TOP_CENTER,
	eRAY_BOTTOM_CENTER,
	eNUM_RAYS
};

const QueuedRayID INVALID_RAY_ID = 0;
typedef Functor2<const uint32&, const RayCastResult&> RayCastResultCallback;

class CCameraRayScan
{
public:
	CCameraRayScan();
	~CCameraRayScan();

	void Reset();

	//update all rays
	void ShootRays(const Vec3 &rayPos, const Vec3 &rayDir, IPhysicalEntity **pSkipEnts = NULL, int numSkipEnts = 0);
	//send one ray and save in pHit (deferred)
	QueuedRayID ShootRay(const Vec3 &rayPos, const Vec3 &rayDir, int objTypes = g_objTypes, int geomFlags = g_geomFlags, IPhysicalEntity **pSkipEnts = NULL, int numSkipEnts = 0);
	//get current hit
	ray_hit *GetHit(ECameraRays nr = eNUM_RAYS);
	//get ray dir for hit
	const Vec3&	GetRayDir(ECameraRays nr) const;

	const RayCastResult* GetExternalHit(const QueuedRayID& queuedId) const;
	void RemoveExternalHit(const QueuedRayID& queuedId);

private:
	void ShootRayInt(ECameraRays camRay, const Vec3 &rayPos, const Vec3 &rayDir, const float& len, IPhysicalEntity **pSkipEnts, int numSkipEnts);
	void OnRayCastResult(const QueuedRayID &rayID, const RayCastResult &result);

private:
	static const int g_objTypes;
	static const int g_geomFlags;

	struct SRayCastInfo
	{
		SRayCastInfo() : rayID(INVALID_RAY_ID), hashit(false) {}
		QueuedRayID rayID;
		ray_hit hit;
		Vec3 dir;
		bool hashit;
	};
	SRayCastInfo m_rayInfo[eNUM_RAYS];

	typedef std::map<QueuedRayID, RayCastResult> TRayCastResultMap;
	TRayCastResultMap m_ExternalRayCasts;
};

#endif