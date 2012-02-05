#include "StdAfx.h"
#include "ScriptBind_StaticEntity.h"

#include "MonoEntity.h"

CScriptBind_StaticEntity::CScriptBind_StaticEntity()
{
	REGISTER_METHOD(GetPropertyValue);
	REGISTER_METHOD(SetPropertyValue);

	REGISTER_METHOD(SetWorldPos);
	REGISTER_METHOD(GetWorldPos);
	REGISTER_METHOD(SetWorldAngles);
	REGISTER_METHOD(GetWorldAngles);

	REGISTER_METHOD(LoadObject);
	REGISTER_METHOD(LoadCharacter);
	REGISTER_METHOD(Physicalize);

	REGISTER_METHOD(CreateGameObjectForEntity);
	REGISTER_METHOD(GetStaticObjectFilePath);

	REGISTER_METHOD(AddMovement);
	REGISTER_METHOD(GetVelocity);
}

mono::string CScriptBind_StaticEntity::GetPropertyValue(EntityId entityId, mono::string propertyName)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	IEntityPropertyHandler *pPropertyHandler = pEntity->GetClass()->GetPropertyHandler();

	return (mono::string)ToMonoString(pPropertyHandler->GetProperty(pEntity, 0));
	//return pPropertyHandler->GetProperty(pEntity, propertyName);
}

void CScriptBind_StaticEntity::SetPropertyValue(EntityId entityId, mono::string propertyName, mono::string value)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	//CMonoEntityPropertyHandler *pPropertyHandler = static_cast<CMonoEntityPropertyHandler *>(pEntity->GetClass()->GetPropertyHandler());

	//pPropertyHandler->SetProperty(pEntity, propertyName, value);
}

void CScriptBind_StaticEntity::SetWorldPos(EntityId id, Vec3 newPos)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

	pEntity->SetPos(newPos);
}

Vec3 CScriptBind_StaticEntity::GetWorldPos(EntityId id)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return Vec3(ZERO);

	return pEntity->GetWorldPos();
}

void CScriptBind_StaticEntity::SetWorldAngles(EntityId id, Vec3 newAngles)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

	pEntity->SetRotation(Quat(Ang3(newAngles)));
}

Vec3 CScriptBind_StaticEntity::GetWorldAngles(EntityId id)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return Vec3(ZERO);

	return Vec3(pEntity->GetWorldAngles());
}

void CScriptBind_StaticEntity::LoadObject(EntityId entityId, mono::string fileName, int slot)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	if(!pEntity)
		return;

	pEntity->SetStatObj(gEnv->p3DEngine->LoadStatObj(ToCryString(fileName)), slot, true);
}

void CScriptBind_StaticEntity::LoadCharacter(EntityId entityId, mono::string fileName, int slot)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	if(!pEntity)
		return;

	pEntity->LoadCharacter(slot, ToCryString(fileName));
}

void CScriptBind_StaticEntity::Physicalize(EntityId id, MonoPhysicalizationParams params)
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

	pEntity->Physicalize(pp);
}

void CScriptBind_StaticEntity::CreateGameObjectForEntity(EntityId id)
{
	IGameObject *pGameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(id);
	if(!pGameObject)
		return;

	if(CMonoEntity *pEntity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetEntity(id))
		pEntity->RegisterGameObject(pGameObject);
}

void CScriptBind_StaticEntity::BindGameObjectToNetwork(EntityId id)
{
	if(CMonoEntity *pEntity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetEntity(id))
		pEntity->GetGameObject()->BindToNetwork();
}

mono::string CScriptBind_StaticEntity::GetStaticObjectFilePath(EntityId id, int slot)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(pEntity)
	{
		if(IStatObj *pStatObj = pEntity->GetStatObj(slot))
			return ToMonoString(pStatObj->GetFilePath());
	}

	return ToMonoString("");
}

void CScriptBind_StaticEntity::AddMovement(EntityId id, MovementRequest &movementRequest)
{
	if(CMonoEntity *pEntity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetEntity(id))
		pEntity->AddMovement(movementRequest);
}

Vec3 CScriptBind_StaticEntity::GetVelocity(EntityId id)
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