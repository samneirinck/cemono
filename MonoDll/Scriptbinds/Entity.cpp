#include "StdAfx.h"
#include "Entity.h"

#include "MonoEntity.h"

CScriptbind_Entity::CScriptbind_Entity()
{
	REGISTER_METHOD(GetPropertyValue);
	REGISTER_METHOD(SetPropertyValue);

	REGISTER_METHOD(SetWorldPos);
	REGISTER_METHOD(GetWorldPos);
	REGISTER_METHOD(SetRotation);
	REGISTER_METHOD(GetRotation);

	REGISTER_METHOD(LoadObject);
	REGISTER_METHOD(LoadCharacter);

	REGISTER_METHOD(GetBoundingBox);

	REGISTER_METHOD(GetSlotFlags);
	REGISTER_METHOD(SetSlotFlags);

	REGISTER_METHOD(Physicalize);
	REGISTER_METHOD(Sleep);
	REGISTER_METHOD(BreakIntoPieces);

	REGISTER_METHOD(CreateGameObjectForEntity);
	REGISTER_METHOD(GetStaticObjectFilePath);

	REGISTER_METHOD(AddImpulse);
	REGISTER_METHOD(AddMovement);

	REGISTER_METHOD(GetVelocity);
	REGISTER_METHOD(SetVelocity);

	REGISTER_METHOD(SetWorldTM);
	REGISTER_METHOD(GetWorldTM);
	REGISTER_METHOD(SetLocalTM);
	REGISTER_METHOD(GetLocalTM);

	REGISTER_METHOD(GetMaterial);
	REGISTER_METHOD(SetMaterial);

	REGISTER_METHOD(GetName);
	REGISTER_METHOD(SetName);

	REGISTER_METHOD(GetFlags);
	REGISTER_METHOD(SetFlags);
}

mono::string CScriptbind_Entity::GetPropertyValue(EntityId entityId, mono::string propertyName)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId))
	{
		IEntityPropertyHandler *pPropertyHandler = pEntity->GetClass()->GetPropertyHandler();

		return ToMonoString(pPropertyHandler->GetProperty(pEntity, 0));
	}

	return ToMonoString("");
}

void CScriptbind_Entity::SetWorldTM(EntityId id, Matrix34 tm)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		pEntity->SetWorldTM(tm);
}

Matrix34 CScriptbind_Entity::GetWorldTM(EntityId id)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		return pEntity->GetWorldTM();

	return Matrix34(IDENTITY);
}

void CScriptbind_Entity::SetLocalTM(EntityId id, Matrix34 tm)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		pEntity->SetLocalTM(tm);
}

Matrix34 CScriptbind_Entity::GetLocalTM(EntityId id)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		return pEntity->GetLocalTM();

	return Matrix34(IDENTITY);
}

AABB CScriptbind_Entity::GetBoundingBox(EntityId entityId, int slot)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId))
	{
		return pEntity->GetStatObj(slot)->GetAABB();
	}

	return AABB(ZERO);
}

void CScriptbind_Entity::SetPropertyValue(EntityId entityId, mono::string propertyName, mono::string value)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	//CEntityPropertyHandler *pPropertyHandler = static_cast<CEntityPropertyHandler *>(pEntity->GetClass()->GetPropertyHandler());

	//pPropertyHandler->SetProperty(pEntity, propertyName, value);
}

void CScriptbind_Entity::SetWorldPos(EntityId id, Vec3 newPos)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

	pEntity->SetPos(newPos);
}

Vec3 CScriptbind_Entity::GetWorldPos(EntityId id)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return Vec3(ZERO);

	return pEntity->GetWorldPos();
}

void CScriptbind_Entity::SetRotation(EntityId id, Quat newAngles)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

	pEntity->SetRotation(newAngles);
}

Quat CScriptbind_Entity::GetRotation(EntityId id)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return Quat(ZERO);

	return pEntity->GetRotation();
}

void CScriptbind_Entity::LoadObject(EntityId entityId, mono::string fileName, int slot)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	if(!pEntity)
		return;

	pEntity->SetStatObj(gEnv->p3DEngine->LoadStatObj(ToCryString(fileName)), slot, true);
}

void CScriptbind_Entity::LoadCharacter(EntityId entityId, mono::string fileName, int slot)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	if(!pEntity)
		return;

	pEntity->LoadCharacter(slot, ToCryString(fileName));
}

EEntitySlotFlags CScriptbind_Entity::GetSlotFlags(EntityId id, int slot)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		return (EEntitySlotFlags)pEntity->GetSlotFlags(slot);

	return (EEntitySlotFlags)0;
}

void CScriptbind_Entity::SetSlotFlags(EntityId id, int slot, EEntitySlotFlags slotFlags)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		pEntity->SetSlotFlags(slot, slotFlags);
}

void CScriptbind_Entity::Physicalize(EntityId id, MonoPhysicalizationParams params)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

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

	pp.type = params.type;
	pp.nSlot = params.slot;
	pp.mass = params.mass;
	pp.nFlagsOR = pef_monitor_poststep;
	pp.fStiffnessScale = params.stiffnessScale;

	if(IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics())
	{
		Ang3 rot(pEntity->GetWorldAngles());
		pEntity->SetRotation(Quat::CreateRotationZ(rot.z));

		SEntityPhysicalizeParams nop;
		nop.type = PE_NONE;
		pEntity->Physicalize(nop);
	}

	pEntity->Physicalize(pp);

	if(IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics())
	{
		pe_action_awake awake;
		awake.bAwake=0;

		pPhysicalEntity->Action(&awake);

		pe_action_move actionMove;
		actionMove.dir = Vec3(0,0,0);
		pPhysicalEntity->Action(&actionMove);
	}
}

void CScriptbind_Entity::Sleep(EntityId entityId, bool sleep)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId))
	{
		if(IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics())
		{
			pe_action_awake awake;
			awake.bAwake = !sleep;

			pPhysicalEntity->Action(&awake);
		}
	}
}

void CScriptbind_Entity::BreakIntoPieces(EntityId entityId, int slot, int piecesSlot, IBreakableManager::BreakageParams breakageParams)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId))
		gEnv->pEntitySystem->GetBreakableManager()->BreakIntoPieces(pEntity, slot, piecesSlot, breakageParams);
}

void CScriptbind_Entity::CreateGameObjectForEntity(EntityId id)
{
	IGameObject *pGameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(id);
	if(!pGameObject)
		return;

	if(auto& entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetEntity(id))
		entity->RegisterGameObject(pGameObject);
}

void CScriptbind_Entity::BindGameObjectToNetwork(EntityId id)
{
	if(auto& entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetEntity(id))
		entity->GetGameObject()->BindToNetwork();
}

mono::string CScriptbind_Entity::GetStaticObjectFilePath(EntityId id, int slot)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(pEntity)
	{
		if(IStatObj *pStatObj = pEntity->GetStatObj(slot))
			return ToMonoString(pStatObj->GetFilePath());
	}

	return ToMonoString("");
}

void CScriptbind_Entity::AddImpulse(EntityId id, ActionImpulse actionImpulse)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
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
}

void CScriptbind_Entity::AddMovement(EntityId id, MovementRequest &movementRequest)
{
	if(auto &entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetEntity(id))
		entity->AddMovement(movementRequest);
}

Vec3 CScriptbind_Entity::GetVelocity(EntityId id)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
	{
		if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
		{
			pe_status_dynamics sd;
			if(pPhysEnt->GetStatus(&sd) != 0)
				return sd.v;
		}
	}

	return Vec3(0,0,0);
}

void CScriptbind_Entity::SetVelocity(EntityId id, Vec3 vel)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
	{
		if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
		{
			pe_action_set_velocity asv;
			asv.v = vel;

			pPhysEnt->Action(&asv);
		}
	}
}

mono::string CScriptbind_Entity::GetMaterial(EntityId id)
{
	const char *material = "";

	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
	{
		if(IMaterial *pMaterial = pEntity->GetMaterial())
			material = pMaterial->GetName();
	}
	
	return ToMonoString(material);
}

void CScriptbind_Entity::SetMaterial(EntityId id, mono::string material)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
	{
		if(IMaterial *pMaterial = gEnv->p3DEngine->GetMaterialManager()->FindMaterial(ToCryString(material)))
			pEntity->SetMaterial(pMaterial);
	}
}

mono::string CScriptbind_Entity::GetName(EntityId id)
{
	const char *name = "";

	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		name = pEntity->GetName();

	return ToMonoString(name);
}

void CScriptbind_Entity::SetName(EntityId id, mono::string name)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		pEntity->SetName(ToCryString(name));
}

EEntityFlags CScriptbind_Entity::GetFlags(EntityId id)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		return (EEntityFlags)pEntity->GetFlags();

	return (EEntityFlags)0;
}

void CScriptbind_Entity::SetFlags(EntityId id, EEntityFlags flags)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id))
		pEntity->SetFlags(flags);
}