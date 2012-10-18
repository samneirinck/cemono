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

	REGISTER_METHOD(SimulateExplosion);

	REGISTER_METHOD(GetLivingEntityStatus);

	REGISTER_METHOD(GetImpulseStruct);
	REGISTER_METHOD(GetPlayerDimensionsStruct);
	REGISTER_METHOD(GetPlayerDynamicsStruct);
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

	pp.nFlagsOR = params.flagsOR;
	pp.nFlagsAND = params.flagsAND;

	if(params.attachToEntity != 0)
	{
		if(IPhysicalEntity *pPhysEnt = gEnv->pPhysicalWorld->GetPhysicalEntityById(params.attachToEntity))
			pp.pAttachToEntity = pPhysEnt;
	}

	if(pp.type == PE_LIVING)
	{
		pp.pPlayerDimensions = &params.playerDim;
		pp.pPlayerDynamics = &params.playerDyn;
	}

	pEntity->Physicalize(pp);
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

void CScriptbind_Physics::AddImpulse(IEntity *pEntity, pe_action_impulse actionImpulse)
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

int CScriptbind_Physics::RayWorldIntersection(Vec3 origin, Vec3 dir, int objFlags, unsigned int flags, ray_hit &hit, int maxHits, mono::object skipEntities)
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

	hit = ray_hit();
	int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir, objFlags, flags, &hit, maxHits, pSkipEnts, physEnts.size());

	SAFE_DELETE_ARRAY(pSkipEnts);
	physEnts.clear();

	return numHits;
}

void CScriptbind_Physics::SimulateExplosion(pe_explosion &explosion)
{
	gEnv->pPhysicalWorld->SimulateExplosion(&explosion);
}

pe_status_living CScriptbind_Physics::GetLivingEntityStatus(IEntity *pEntity)
{
	pe_status_living status;
	if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
		pEntity->GetPhysics()->GetStatus(&status);

	return status;
}