#include "StdAfx.h"
#include "Physics.h"

#include "MonoScriptSystem.h"

#include "MonoException.h"

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
	REGISTER_METHOD(GetDynamicsEntityStatus);
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

int CScriptbind_Physics::RayWorldIntersection(Vec3 origin, Vec3 dir, int objFlags, unsigned int flags, int maxHits, mono::object skipEntities, mono::object &hits)
{
	IPhysicalEntity **pSkipEnts = NULL;
	int numSkipEnts = 0;

	if(skipEntities)
	{
		IMonoArray *pSkipEntities = *skipEntities;
		numSkipEnts = pSkipEntities->GetSize();

		pSkipEnts = new IPhysicalEntity*[numSkipEnts];

		for(int i = 0; i < numSkipEnts; i++)
		{
			IMonoObject *pItem = pSkipEntities->GetItem(i);

#ifndef RELEASE
			if(!pItem)
				g_pScriptSystem->GetCryBraryAssembly()->GetException("CryEngine", "NullPointerException")->Throw();
#endif

			pSkipEnts[i] = pItem->Unbox<IPhysicalEntity *>();
		}
	}

	ray_hit *pHits = new ray_hit[maxHits];
	int numHits = gEnv->pPhysicalWorld->RayWorldIntersection(origin, dir, objFlags, flags, pHits, maxHits, pSkipEnts, numSkipEnts);

	SAFE_DELETE_ARRAY(pSkipEnts);

	if(numHits > 0)
	{
		IMonoClass *pRayHitClass = g_pScriptSystem->GetCryBraryAssembly()->GetClass("RaycastHit");

		IMonoArray *pRayHits = CreateMonoArray(numHits);//, pRayHitClass);
		for(int i = 0; i < numHits; i++)
			pRayHits->InsertMonoObject(pRayHitClass->BoxObject(&pHits[i]));

		hits = pRayHits->GetManagedObject();
	}

	delete[] pHits;

	return numHits;
}

mono::object CScriptbind_Physics::SimulateExplosion(pe_explosion explosion)
{
	gEnv->pPhysicalWorld->SimulateExplosion(&explosion);

	if(explosion.nAffectedEnts > 0)
	{
		IMonoArray *pAffectedEnts = CreateMonoArray(explosion.nAffectedEnts);

		for(int i = 0; i < explosion.nAffectedEnts; i++)
			pAffectedEnts->InsertNativePointer(explosion.pAffectedEnts[i]);

		return pAffectedEnts->GetManagedObject();
	}

	return NULL;
}

pe_status_living CScriptbind_Physics::GetLivingEntityStatus(IEntity *pEntity)
{
	pe_status_living status;
	if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
		pEntity->GetPhysics()->GetStatus(&status);

	return status;
}

pe_status_dynamics CScriptbind_Physics::GetDynamicsEntityStatus(IEntity *pEntity)
{
	pe_status_dynamics status;
	if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
		pEntity->GetPhysics()->GetStatus(&status);

	return status;
}