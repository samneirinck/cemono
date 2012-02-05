#include "StdAfx.h"
#include "ScriptBind_PhysicalWorld.h"

#include <IMonoArray.h>
#include <IMonoObject.h>

CScriptBind_PhysicalWorld::CScriptBind_PhysicalWorld()
{
	REGISTER_METHOD(RayWorldIntersection);
}

int CScriptBind_PhysicalWorld::RayWorldIntersection(Vec3 origin, Vec3 dir, int objFlags, unsigned int flags, MonoRayHit &hit, int maxHits, mono::array skipEntities)
{
	ray_hit realHit;

	IMonoArray *pSkipEntities = *skipEntities;
	IPhysicalEntity **pSkipEnts = new IPhysicalEntity*[pSkipEntities->GetSize()];
	for(int i = 0; i < pSkipEntities->GetSize(); i++)
		pSkipEnts[i] = gEnv->pPhysicalWorld->GetPhysicalEntityById(pSkipEntities->GetItem(i)->Unbox<EntityId>());

	int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir, objFlags, flags, &realHit, maxHits, pSkipEnts);

	delete[] pSkipEnts;
	delete pSkipEntities;

	hit.bTerrain = realHit.bTerrain;
	hit.colliderId = gEnv->pPhysicalWorld->GetPhysicalEntityId(realHit.pCollider);
	hit.dist = realHit.dist;
	hit.foreignIdx = realHit.foreignIdx;
	hit.idmatOrg = realHit.idmatOrg;
	hit.iNode = realHit.iNode;
	hit.ipart = realHit.ipart;
	hit.iPrim = realHit.iPrim;
	hit.n = realHit.n;
	hit.partid = realHit.partid;
	hit.pt = realHit.pt;
	hit.surface_idx = realHit.surface_idx;

	return numHits;
}