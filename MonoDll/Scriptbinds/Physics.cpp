#include "StdAfx.h"
#include "Physics.h"

#include <IEntitySystem.h>

#include <IMonoArray.h>
#include <IMonoObject.h>

CScriptbind_Physics::CScriptbind_Physics()
{
	REGISTER_METHOD(GetPhysicalEntity);

	REGISTER_METHOD(Physicalize);
	REGISTER_METHOD(Sleep);

	REGISTER_METHOD(AddImpulse);

	REGISTER_METHOD(GetVelocity);
	REGISTER_METHOD(SetVelocity);

	REGISTER_METHOD(RayWorldIntersection);
}

IPhysicalEntity *CScriptbind_Physics::GetPhysicalEntity(IEntity *pEntity)
{
	return pEntity->GetPhysics();
}

void CScriptbind_Physics::Physicalize(IEntity *pEntity, SMonoPhysicalizeParams params)
{
	// Unphysicalize
	{
		const Ang3 oldRotation = pEntity->GetWorldAngles();
		const Quat newRotation = Quat::CreateRotationZ( oldRotation.z );
		pEntity->SetRotation( newRotation );

		SEntityPhysicalizeParams pp;
		pp.type = PE_NONE;
		pEntity->Physicalize( pp );
	}
	// ~Unphysicalize

	SEntityPhysicalizeParams pp;

	pp.bCopyJointVelocities = params.copyJointVelocities;
	pp.density = params.density;
	pp.fStiffnessScale = params.stiffnessScale;
	pp.mass = params.mass;
	pp.nAttachToPart = params.attachToPart;
	pp.nLod = params.lod;
	pp.nSlot = params.slot;
	pp.type = params.type;

	if(params.attachToEntity.id != 0)
	{
		if(IPhysicalEntity *pPhysEnt = gEnv->pPhysicalWorld->GetPhysicalEntityById(params.attachToEntity.id))
			pp.pAttachToEntity = pPhysEnt;
	}

	pEntity->Physicalize(pp);

	if(IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics())
	{
		pe_action_awake awake;
		awake.bAwake = false;
		pPhysicalEntity->Action(&awake);

		pe_action_move actionMove;
		actionMove.dir = Vec3(0,0,0);
		pPhysicalEntity->Action(&actionMove);
	}
}

void CScriptbind_Physics::Sleep(IEntity *pEntity, bool sleep)
{
	IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics();
	if(!pPhysicalEntity)
		return;

	pe_action_awake awake;
	awake.bAwake = !sleep;

	pPhysicalEntity->Action(&awake);
}

void CScriptbind_Physics::AddImpulse(IEntity *pEntity, SMonoActionImpulse actionImpulse)
{
	pe_action_impulse impulse;

	impulse.angImpulse = actionImpulse.angImpulse;
	impulse.iApplyTime = actionImpulse.iApplyTime;
	impulse.impulse = actionImpulse.impulse;
	impulse.ipart = actionImpulse.ipart;
	impulse.iSource = actionImpulse.iSource;
	impulse.partid = actionImpulse.partid;
	impulse.point = actionImpulse.point;

	if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
		pPhysEnt->Action(&impulse);
}

Vec3 CScriptbind_Physics::GetVelocity(IEntity *pEntity)
{
	IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics();
	if(!pPhysicalEntity)
		return Vec3(ZERO);

	pe_status_dynamics sd;
	if(pPhysicalEntity->GetStatus(&sd) != 0)
		return sd.v;

	return Vec3(0, 0, 0);
}

void CScriptbind_Physics::SetVelocity(IEntity *pEntity, Vec3 vel)
{
	IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics();
	if(!pPhysicalEntity)
		return;

	pe_action_set_velocity asv;
	asv.v = vel;

	pPhysicalEntity->Action(&asv);
}

int CScriptbind_Physics::RayWorldIntersection(Vec3 origin, Vec3 dir, int objFlags, unsigned int flags, SMonoRayHit &monoHit, int maxHits, mono::object skipEntities)
{
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

		delete pSkipEntities;
	}

	IPhysicalEntity **pSkipEnts = new IPhysicalEntity*[physEnts.size()];

	for(int i = 0; i < physEnts.size(); i++)
		pSkipEnts[i] = physEnts[i];

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