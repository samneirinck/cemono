#include "StdAfx.h"
#include "ScriptBind_PhysicalWorld.h"

#include <IEntitySystem.h>

#include <IMonoArray.h>
#include <IMonoObject.h>

CScriptBind_PhysicalWorld::CScriptBind_PhysicalWorld()
{
	REGISTER_METHOD(RayWorldIntersection);
}

int CScriptBind_PhysicalWorld::RayWorldIntersection(Vec3 origin, Vec3 dir, int objFlags, unsigned int flags, MonoRayHit &hit, int maxHits, mono::array skipEntities)
{
	IPhysicalEntity **pSkipEnts = NULL;
	std::vector<IPhysicalEntity *> physEnts;

	if(skipEntities)
	{
		IMonoArray *pSkipEntities = *skipEntities;

		for(int i = 0; i < pSkipEntities->GetSize(); i++)
		{
			if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pSkipEntities->GetItem(i)->Unbox<EntityId>()))
			{
				if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
					physEnts.push_back(pPhysEnt);
			}
		}

		pSkipEnts = new IPhysicalEntity*[physEnts.size()];

		for(int i = 0; i < physEnts.size(); i++)
			pSkipEnts[i] = physEnts[i];

		delete pSkipEntities;
	}

	ray_hit realHit;

	int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir, objFlags, flags, &realHit, maxHits, physEnts.size() > 0 ? pSkipEnts : NULL, physEnts.size());

	SAFE_DELETE_ARRAY(pSkipEnts);
	physEnts.clear();

	hit.bTerrain = realHit.bTerrain;

	// We should return physical entity id's really, but this isn't exposed yet.
	if(realHit.pCollider)
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