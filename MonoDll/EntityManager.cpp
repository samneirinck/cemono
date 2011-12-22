#include "StdAfx.h"
#include "EntityManager.h"

#include "MonoScriptSystem.h"

#include "MonoObject.h"
#include "MonoEntityClass.h"
#include "MonoArray.h"
#include "MonoClass.h"

#include <IEntityClass.h>
#include <IMonoScript.h>

#include <IGameFramework.h>

std::vector<const char *> CEntityManager::m_monoEntityClasses = std::vector<const char *>();

CEntityManager::CEntityManager()
{
	REGISTER_METHOD(SpawnEntity);
	REGISTER_METHOD(RegisterEntityClass);
	REGISTER_METHOD(GetPropertyValue);
	REGISTER_METHOD(SetPropertyValue);

	REGISTER_METHOD(FindEntity);
	REGISTER_METHOD(GetEntitiesByClass);

	REGISTER_METHOD(SetWorldPos);
	REGISTER_METHOD(GetWorldPos);
	REGISTER_METHOD(SetWorldAngles);
	REGISTER_METHOD(GetWorldAngles);

	gEnv->pEntitySystem->AddSink(this, IEntitySystem::SinkEventSubscriptions::OnSpawn | IEntitySystem::SinkEventSubscriptions::OnRemove, 0);
}

CEntityManager::~CEntityManager()
{
	gEnv->pEntitySystem->RemoveSink(this);
}

void CEntityManager::OnSpawn(IEntity *pEntity,SEntitySpawnParams &params)
{
	const char *className = pEntity->GetClass()->GetName();

	if(!IsMonoEntity(className))
		return;
	
	int scriptId = gEnv->pMonoScriptSystem->InstantiateScript(EMonoScriptType_Entity, className);

	m_entityScripts.insert(TMonoEntityScripts::value_type(scriptId, pEntity->GetId()));

	CallMonoScript(scriptId, "Spawn", pEntity->GetId());
}

bool CEntityManager::OnRemove(IEntity *pEntity)
{
	if(!pEntity)
		return true;

	const char *entClass = pEntity->GetClass()->GetName();
	if(!IsMonoEntity(entClass))
		return true;

	//if(IMonoResult *pResult = IMonoScript::CallMonoScript(pScript, "OnRemove"))
		//return pResult->Unbox<bool>();

	CallMonoScript(GetScriptId(pEntity->GetId()), "OnRemove");

	IMonoArray *pArray = CreateMonoArray(2);
	pArray->InsertObject(CreateMonoObject<int>(GetScriptId(pEntity->GetId())));
	pArray->InsertString(entClass);
	static_cast<CMonoScriptSystem  *>(gEnv->pMonoScriptSystem)->GetManagerClass()->CallMethod("RemoveScriptInstance", pArray);
	
	return true;
}

bool CEntityManager::IsMonoEntity(const char *entityClassName)
{
	for(std::vector<const char *>::iterator it = m_monoEntityClasses.begin(); it != m_monoEntityClasses.end(); ++it)
	{
		if(!strcmp(entityClassName, (*it)))
			return true;
	}

	return false;
}

int CEntityManager::GetScriptId(EntityId entityId)
{
	for (TMonoEntityScripts::const_iterator it=m_entityScripts.begin(); it!=m_entityScripts.end(); ++it)
	{
		if(it->second==entityId)
			return it->first;
	}

	return -1;
}

EntityId CEntityManager::SpawnEntity(SMonoEntitySpawnParams &params, bool bAutoInit)
{
	return gEnv->pEntitySystem->SpawnEntity(params.Convert(), bAutoInit)->GetId();
}

bool CEntityManager::RegisterEntityClass(SMonoEntityRegisterParams params, MonoArray *Properties)
{
	CMonoArray *propertiesArray = new CMonoArray(Properties);

	int numProperties = propertiesArray->GetSize();
	std::vector<IEntityPropertyHandler::SPropertyInfo> properties;
	
	for	(int i = 0; i < propertiesArray->GetSize(); ++i)
	{
		SMonoEntityProperty monoProperty = propertiesArray->GetItem(i)->Unbox<SMonoEntityProperty>();

		IEntityPropertyHandler::SPropertyInfo propertyInfo;

		propertyInfo.name = ToCryString(monoProperty.name);
		propertyInfo.description = ToCryString(monoProperty.description);
		propertyInfo.editType = ToCryString(monoProperty.editType);
		propertyInfo.type = monoProperty.type;
		propertyInfo.limits.min = monoProperty.limits.min;
		propertyInfo.limits.max = monoProperty.limits.max;

		properties.push_back(propertyInfo);
	}

	IEntityClassRegistry::SEntityClassDesc entityClassDesc;	
	entityClassDesc.flags = params.Flags;
	entityClassDesc.sName = ToCryString(params.Name);
	entityClassDesc.sEditorHelper = ToCryString(params.EditorHelper);
	entityClassDesc.sEditorIcon = ToCryString(params.EditorIcon);
	
	CMonoEntityClass *entityClass = new CMonoEntityClass(entityClassDesc, ToCryString(params.Category), properties);

	m_monoEntityClasses.push_back(entityClassDesc.sName);
	return gEnv->pEntitySystem->GetClassRegistry()->RegisterClass(entityClass);
}

MonoString *CEntityManager::GetPropertyValue(EntityId entityId, MonoString *propertyName)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	IEntityPropertyHandler *pPropertyHandler = pEntity->GetClass()->GetPropertyHandler();

	return ToMonoString(pPropertyHandler->GetProperty(pEntity, 0));
	//return pPropertyHandler->GetProperty(pEntity, propertyName);
}

void CEntityManager::SetPropertyValue(EntityId entityId, MonoString *propertyName, MonoString *value)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	//CMonoEntityPropertyHandler *pPropertyHandler = static_cast<CMonoEntityPropertyHandler *>(pEntity->GetClass()->GetPropertyHandler());

	//pPropertyHandler->SetProperty(pEntity, propertyName, value);
}

EntityId CEntityManager::FindEntity(MonoString *name)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->FindEntityByName(ToCryString(name)))
		return pEntity->GetId();

	return -1;
}

MonoArray *CEntityManager::GetEntitiesByClass(MonoString *_class)
{
	const char *className = ToCryString(_class);
	std::vector<EntityId> m_entities;

	IEntityItPtr pIt = gEnv->pEntitySystem->GetEntityIterator();
	while(!pIt->IsEnd())
	{
		if(IEntity *pEntity = pIt->Next())
		{
			if(!strcmp(pEntity->GetClass()->GetName(), className))
				m_entities.push_back(pEntity->GetId());
		}
	}

	if(m_entities.size()<1)
		return NULL;

	CMonoArray *pArray = new CMonoArray(m_entities.size());
	for(std::vector<EntityId>::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
		pArray->InsertObject(CreateMonoObject<EntityId>(*it));

	return pArray->GetMonoArray();
}

void CEntityManager::SetWorldPos(EntityId id, Vec3 newPos)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

	pEntity->SetPos(newPos);
}

Vec3 CEntityManager::GetWorldPos(EntityId id)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return Vec3(ZERO);

	return pEntity->GetWorldPos();
}

void CEntityManager::SetWorldAngles(EntityId id, Vec3 newAngles)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return;

	pEntity->SetRotation(Quat(Ang3(newAngles)));
}

Vec3 CEntityManager::GetWorldAngles(EntityId id)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(id);
	if(!pEntity)
		return Vec3(ZERO);

	return Vec3(pEntity->GetWorldAngles());
}