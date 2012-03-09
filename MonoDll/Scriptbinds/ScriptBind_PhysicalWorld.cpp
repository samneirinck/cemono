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
	IPhysicalEntity **pSkipEnts = NULL;
	IMonoArray *pSkipEntities = NULL;

	if(skipEntities != NULL)
	{
		pSkipEntities = *skipEntities;
		pSkipEnts = new IPhysicalEntity*[pSkipEntities->GetSize()];
		for(int i = 0; i < pSkipEntities->GetSize(); i++)
			pSkipEnts[i] = gEnv->pPhysicalWorld->GetPhysicalEntityById(pSkipEntities->GetItem(i)->Unbox<EntityId>());

		delete pSkipEntities;
	}

	ray_hit realHit;

	int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir, objFlags, flags, &realHit, maxHits, pSkipEntities ? pSkipEnts : NULL, pSkipEntities ? pSkipEntities->GetSize() : 0);

	SAFE_DELETE_ARRAY(pSkipEnts);

	hit.bTerrain = realHit.bTerrain;

	// We should return physical entity id's really, but this isn't exposed yet.
	if(realHit.pCollider != NULL && realHit.pCollider->GetType() != PE_STATIC && realHit.pCollider->GetType() != PE_NONE)
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