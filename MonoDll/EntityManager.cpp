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

CEntityManager::CEntityManager()
	: m_refs(0)
{
	REGISTER_METHOD(SpawnEntity);
	REGISTER_METHOD(RegisterEntityClass);
	
	REGISTER_METHOD(FindEntity);
	REGISTER_METHOD(GetEntitiesByClass);

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

	m_monoEntities.push_back(new CMonoEntity(gEnv->pMonoScriptSystem->InstantiateScript(EMonoScriptType_Entity, className)));

	return true;
}

void CEntityManager::OnSpawn(IEntity *pEntity,SEntitySpawnParams &params)
{
	const char *className = params.pClass->GetName();

	if(!IsMonoEntity(className))
		return;

	m_monoEntities.back()->OnSpawn(pEntity->GetId());
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
			int scriptId = (*it)->GetScriptId();
			if(IMonoObject *pResult = CallMonoScript(scriptId, "OnRemove"))
			{
				bool result = pResult->Unbox<bool>();

				SAFE_DELETE(pResult);
				return result;
			}
	
			m_monoEntities.erase(it);
			break;
		}
	}

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

CMonoEntity *CEntityManager::GetEntity(EntityId entityId)
{
	for (TMonoEntities::const_iterator it=m_monoEntities.begin(); it!=m_monoEntities.end(); ++it)
	{
		if((*it)->GetEntityId()==entityId)
			return (*it);
	}

	return NULL;
}

int CEntityManager::GetScriptId(EntityId entityId, bool returnBackIfInvalid)
{
	if(CMonoEntity *pEntity = GetEntity(entityId))
		return pEntity->GetScriptId();

	if(returnBackIfInvalid)
		return m_monoEntities.back()->GetScriptId();

	return -1;
}

EntityId CEntityManager::SpawnEntity(EntitySpawnParams &params, bool bAutoInit)
{
	return gEnv->pEntitySystem->SpawnEntity(params.Convert(), bAutoInit)->GetId();
}

bool CEntityManager::RegisterEntityClass(EntityRegisterParams params, mono::array Properties)
{
	if(gEnv->pEntitySystem->GetClassRegistry()->FindClass(ToCryString(params.Name)))
		return false;

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

EntityId CEntityManager::FindEntity(mono::string name)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->FindEntityByName(ToCryString(name)))
		return pEntity->GetId();

	return -1;
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