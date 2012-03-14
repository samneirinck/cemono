#include "StdAfx.h"
#include "ScriptBind_PhysicalWorld.h"

#include <IEntitySystem.h>

#include <IMonoArray.h>
#include <IMonoObject.h>

CScriptBind_PhysicalWorld::CScriptBind_PhysicalWorld()
{
	REGISTER_METHOD(RayWorldIntersection);
}

int CScriptBind_PhysicalWorld::RayWorldIntersection(Vec3 origin, Vec3 dir, int objFlags, unsigned int flags, MonoRayHit &monoHit, int maxHits, mono::array skipEntities)
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

	ray_hit hit;
	int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir, objFlags, flags, &hit, maxHits, pSkipEnts, physEnts.size());

	SAFE_DELETE_ARRAY(pSkipEnts);
	physEnts.clear();

	monoHit.bTerrain = hit.bTerrain;

	// We should return physical entity id's really, but this isn't exposed yet.
	//if(hit.pCollider)
		//monoHit.colliderId = gEnv->pPhysicalWorld->GetPhysicalEntityId(hit.pCollider);

	monoHit.dist = hit.dist;
	monoHit.foreignIdx = hit.foreignIdx;
	monoHit.idmatOrg = hit.idmatOrg;
	monoHit.iNode = hit.iNode;
	monoHit.ipart = hit.ipart;
	monoHit.iPrim = hit.iPrim;
	monoHit.n = hit.n;
	monoHit.partid = hit.partid;
	monoHit.pt = hit.pt;
	monoHit.surface_idx = hit.surface_idx;

	return numHits;
}