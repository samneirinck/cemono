#include "StdAfx.h"
#include "EntityManager.h"

#include <MonoEntity.h>
#include "MonoEntityClass.h"

#include "MonoScriptSystem.h"

#include "MonoObject.h"
#include "MonoArray.h"
#include "MonoClass.h"

#include <IEntityClass.h>

#include <IGameObjectSystem.h>
#include <IGameObject.h>
#include <IGameFramework.h>

std::vector<const char *> CEntityManager::m_monoEntityClasses = std::vector<const char *>();
CEntityManager::TMonoEntities CEntityManager::m_monoEntities = CEntityManager::TMonoEntities();

CEntityManager::CEntityManager()
	: m_refs(0)
{
	REGISTER_METHOD(SpawnEntity);
	REGISTER_METHOD(RemoveEntity);

	REGISTER_METHOD(RegisterEntityClass);

	REGISTER_METHOD(GetEntity);

	REGISTER_METHOD(FindEntity);
	REGISTER_METHOD(GetEntitiesByClass);

	REGISTER_METHOD(EntityExists);

	gEnv->pEntitySystem->AddSink(this, IEntitySystem::OnBeforeSpawn | IEntitySystem::OnSpawn | IEntitySystem::OnRemove, 0);
}

CEntityManager::~CEntityManager()
{
	gEnv->pEntitySystem->RemoveSink(this);
}

bool CEntityManager::OnBeforeSpawn(SEntitySpawnParams &params)
{
	const char *className = params.pClass->GetName();

	if(!IsMonoEntity(className))
		return true;

	m_monoEntities.push_back(std::shared_ptr<CEntity>(new CEntity(gEnv->pMonoScriptSystem->InstantiateScript(className))));

	return true;
}

void CEntityManager::OnSpawn(IEntity *pEntity,SEntitySpawnParams &params)
{
	const char *className = params.pClass->GetName();

	if(!IsMonoEntity(className))
		return;

	EntityId id = pEntity->GetId();
	if(id == 0)
	{
		CryLogAlways("Failed to spawn entity %s of class %s", params.sName, params.pClass ? params.pClass->GetName() : "[Invalid Class]");
		m_monoEntities.erase(std::remove(m_monoEntities.begin(), m_monoEntities.end(), m_monoEntities.back()), m_monoEntities.end());
	}

	m_monoEntities.back()->OnSpawn(id);
}

bool CEntityManager::OnRemove(IEntity *pEntity)
{
	if(!pEntity)
		return true;

	const char *entClass = pEntity->GetClass()->GetName();
	if(!IsMonoEntity(entClass))
		return true;

	for(TMonoEntities::iterator it = m_monoEntities.begin(); it != m_monoEntities.end(); ++it)
	{
		if((*it)->GetEntityId()==pEntity->GetId())
		{
			bool doRemove = CallMonoScript<bool>((*it)->GetScriptId(), "InternalRemove");

			m_monoEntities.erase(it);
			
			return doRemove;
		}
	}

	return true;
}

void CEntityManager::RemoveEntity(EntityId id)
{
	for(TMonoEntities::iterator it = m_monoEntities.begin(); it != m_monoEntities.end(); ++it)
	{
		if((*it)->GetEntityId()==id)
			m_monoEntities.erase(it);
	}

	gEnv->pEntitySystem->RemoveEntity(id);
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

std::shared_ptr<CEntity> CEntityManager::GetMonoEntity(EntityId entityId)
{
	for each(auto& monoEntity in m_monoEntities)
	{
		if(monoEntity->GetEntityId()==entityId)
			return monoEntity;
	}

	return NULL;
}

int CEntityManager::GetScriptId(EntityId entityId, bool returnBackIfInvalid)
{
	if(auto& entity = GetMonoEntity(entityId))
		return entity->GetScriptId();

	if(returnBackIfInvalid)
		return m_monoEntities.back()->GetScriptId();

	return -1;
}

SMonoEntityInfo CEntityManager::SpawnEntity(EntitySpawnParams params, bool bAutoInit)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->SpawnEntity(params.Convert(), bAutoInit))
		return SMonoEntityInfo(pEntity);

	return SMonoEntityInfo();
}

bool CEntityManager::RegisterEntityClass(EntityRegisterParams params, mono::array Properties)
{
	const char *className = ToCryString(params.Name);
	if(gEnv->pEntitySystem->GetClassRegistry()->FindClass(className))
	{
		CryLogAlways("Aborting registration of entity class %s, a class with the same name already exists", className); 
		return false;
	}

	CScriptArray *propertiesArray = new CScriptArray(Properties);

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
	entityClassDesc.sName = className;
	entityClassDesc.editorClassInfo.sCategory = ToCryString(params.Category);
	entityClassDesc.editorClassInfo.sHelper = ToCryString(params.EditorHelper);
	entityClassDesc.editorClassInfo.sIcon = ToCryString(params.EditorIcon);
	
	CEntityClass *entityClass = new CEntityClass(entityClassDesc, properties);

	m_monoEntityClasses.push_back(entityClassDesc.sName);

	return gEnv->pEntitySystem->GetClassRegistry()->RegisterClass(entityClass);
}

IEntity *CEntityManager::GetEntity(EntityId id)
{
	return gEnv->pEntitySystem->GetEntity(id);
}

EntityId CEntityManager::FindEntity(mono::string name)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->FindEntityByName(ToCryString(name)))
		return pEntity->GetId();

	return 0;
}

bool CEntityManager::EntityExists(EntityId entityId)
{
	if(entityId == 0)
		return false;

	return gEnv->pEntitySystem->GetEntity(entityId) != NULL;
}

mono::array CEntityManager::GetEntitiesByClass(mono::string _class)
{
	const char *className = ToCryString(_class);
	std::vector<EntityId> classEntities;

	IEntityItPtr pIt = gEnv->pEntitySystem->GetEntityIterator();
	while(!pIt->IsEnd())
	{
		if(IEntity *pEntity = pIt->Next())
		{
			if(!strcmp(pEntity->GetClass()->GetName(), className))
				classEntities.push_back(pEntity->GetId());
		}
	}

	if(classEntities.size()<1)
		return NULL;

	IMonoArray *pArray = CreateMonoArray(classEntities.size());
	for(std::vector<EntityId>::iterator it = classEntities.begin(); it != classEntities.end(); ++it)
		pArray->Insert(*it);

	return *pArray;
}