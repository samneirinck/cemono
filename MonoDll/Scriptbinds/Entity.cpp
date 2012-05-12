#include "StdAfx.h"
#include "Entity.h"

#include "MonoEntity.h"

CScriptbind_Entity::CScriptbind_Entity()
{
	REGISTER_METHOD(GetPropertyValue);

	REGISTER_METHOD(SetPos);
	REGISTER_METHOD(GetPos);
	REGISTER_METHOD(SetWorldPos);
	REGISTER_METHOD(GetWorldPos);

	REGISTER_METHOD(SetRotation);
	REGISTER_METHOD(GetRotation);
	REGISTER_METHOD(SetWorldRotation);
	REGISTER_METHOD(GetWorldRotation);

	REGISTER_METHOD(LoadObject);
	REGISTER_METHOD(LoadCharacter);

	REGISTER_METHOD(GetBoundingBox);
	REGISTER_METHOD(GetWorldBoundingBox);

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

mono::string CScriptbind_Entity::GetPropertyValue(IEntity *pEnt, mono::string propertyName)
{
	IEntityPropertyHandler *pPropertyHandler = pEnt->GetClass()->GetPropertyHandler();

	return ToMonoString(pPropertyHandler->GetProperty(pEnt, 0));
}

void CScriptbind_Entity::SetWorldTM(IEntity *pEntity, Matrix34 tm)
{
	pEntity->SetWorldTM(tm);
}

Matrix34 CScriptbind_Entity::GetWorldTM(IEntity *pEntity)
{
	return pEntity->GetWorldTM();
}

void CScriptbind_Entity::SetLocalTM(IEntity *pEntity, Matrix34 tm)
{
	pEntity->SetLocalTM(tm);
}

Matrix34 CScriptbind_Entity::GetLocalTM(IEntity *pEntity)
{
	return pEntity->GetLocalTM();
}

AABB CScriptbind_Entity::GetWorldBoundingBox(IEntity *pEntity)
{
	AABB boundingBox;
	pEntity->GetWorldBounds(boundingBox);

	return boundingBox;
}

AABB CScriptbind_Entity::GetBoundingBox(IEntity *pEntity)
{
	AABB boundingBox;
	pEntity->GetLocalBounds(boundingBox);

	return boundingBox;
}

void CScriptbind_Entity::SetPos(IEntity *pEntity, Vec3 newPos)
{
	pEntity->SetPos(newPos);
}

Vec3 CScriptbind_Entity::GetPos(IEntity *pEntity)
{
	return pEntity->GetPos();
}

void CScriptbind_Entity::SetWorldPos(IEntity *pEntity, Vec3 newPos)
{
	pEntity->SetWorldTM(Matrix34::Create(pEntity->GetScale(), pEntity->GetWorldRotation(), newPos));
}

Vec3 CScriptbind_Entity::GetWorldPos(IEntity *pEntity)
{
	return pEntity->GetWorldPos();
}

void CScriptbind_Entity::SetRotation(IEntity *pEntity, Quat newAngles)
{
	pEntity->SetRotation(newAngles);
}

Quat CScriptbind_Entity::GetRotation(IEntity *pEntity)
{
	return pEntity->GetRotation();
}

void CScriptbind_Entity::SetWorldRotation(IEntity *pEntity, Quat newAngles)
{
	pEntity->SetWorldTM(Matrix34::Create(pEntity->GetScale(), newAngles, pEntity->GetWorldPos()));
}

Quat CScriptbind_Entity::GetWorldRotation(IEntity *pEntity)
{
	return pEntity->GetWorldRotation();
}

void CScriptbind_Entity::LoadObject(IEntity *pEntity, mono::string fileName, int slot)
{
	pEntity->SetStatObj(gEnv->p3DEngine->LoadStatObj(ToCryString(fileName)), slot, true);
}

void CScriptbind_Entity::LoadCharacter(IEntity *pEntity, mono::string fileName, int slot)
{
	pEntity->LoadCharacter(slot, ToCryString(fileName));
}

EEntitySlotFlags CScriptbind_Entity::GetSlotFlags(IEntity *pEntity, int slot)
{
	return (EEntitySlotFlags)pEntity->GetSlotFlags(slot);
}

void CScriptbind_Entity::SetSlotFlags(IEntity *pEntity, int slot, EEntitySlotFlags slotFlags)
{
	pEntity->SetSlotFlags(slot, slotFlags);
}

void CScriptbind_Entity::Physicalize(IEntity *pEntity, MonoPhysicalizationParams params)
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

void CScriptbind_Entity::Sleep(IEntity *pEntity, bool sleep)
{
	if(IPhysicalEntity *pPhysicalEntity = pEntity->GetPhysics())
	{
		pe_action_awake awake;
		awake.bAwake = !sleep;

		pPhysicalEntity->Action(&awake);
	}
}

void CScriptbind_Entity::BreakIntoPieces(IEntity *pEntity, int slot, int piecesSlot, IBreakableManager::BreakageParams breakageParams)
{
	gEnv->pEntitySystem->GetBreakableManager()->BreakIntoPieces(pEntity, slot, piecesSlot, breakageParams);
}

void CScriptbind_Entity::CreateGameObjectForEntity(IEntity *pEntity)
{
	IGameObject *pGameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(pEntity->GetId());
	if(!pGameObject)
		return;

	if(auto& entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetMonoEntity(pEntity->GetId()))
		entity->RegisterGameObject(pGameObject);
}

void CScriptbind_Entity::BindGameObjectToNetwork(IEntity *pEntity)
{
	if(auto& entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetMonoEntity(pEntity->GetId()))
		entity->GetGameObject()->BindToNetwork();
}

mono::string CScriptbind_Entity::GetStaticObjectFilePath(IEntity *pEntity, int slot)
{
	if(IStatObj *pStatObj = pEntity->GetStatObj(slot))
		return ToMonoString(pStatObj->GetFilePath());

	return ToMonoString("");
}

void CScriptbind_Entity::AddImpulse(IEntity *pEntity, ActionImpulse actionImpulse)
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

void CScriptbind_Entity::AddMovement(IEntity *pEntity, MovementRequest &movementRequest)
{
	if(auto &entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetMonoEntity(pEntity->GetId()))
		entity->AddMovement(movementRequest);
}

Vec3 CScriptbind_Entity::GetVelocity(IEntity *pEntity)
{
	if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
	{
		pe_status_dynamics sd;
		if(pPhysEnt->GetStatus(&sd) != 0)
			return sd.v;
	}

	return Vec3(0,0,0);
}

void CScriptbind_Entity::SetVelocity(IEntity *pEntity, Vec3 vel)
{
	if(IPhysicalEntity *pPhysEnt = pEntity->GetPhysics())
	{
		pe_action_set_velocity asv;
		asv.v = vel;

		pPhysEnt->Action(&asv);
	}
}

mono::string CScriptbind_Entity::GetMaterial(IEntity *pEntity)
{
	const char *material = "";

	if(IMaterial *pMaterial = pEntity->GetMaterial())
		material = pMaterial->GetName();
	
	return ToMonoString(material);
}

void CScriptbind_Entity::SetMaterial(IEntity *pEntity, mono::string material)
{
	if(IMaterial *pMaterial = gEnv->p3DEngine->GetMaterialManager()->FindMaterial(ToCryString(material)))
		pEntity->SetMaterial(pMaterial);
}

mono::string CScriptbind_Entity::GetName(IEntity *pEntity)
{
	return ToMonoString(pEntity->GetName());
}

void CScriptbind_Entity::SetName(IEntity *pEntity, mono::string name)
{
	pEntity->SetName(ToCryString(name));
}

EEntityFlags CScriptbind_Entity::GetFlags(IEntity *pEntity)
{
	return (EEntityFlags)pEntity->GetFlags();
}

void CScriptbind_Entity::SetFlags(IEntity *pEntity, EEntityFlags flags)
{
	pEntity->SetFlags(flags);
}