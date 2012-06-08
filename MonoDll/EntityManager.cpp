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
{
	REGISTER_METHOD(SpawnEntity);
	REGISTER_METHOD(RemoveEntity);

	REGISTER_METHOD(RegisterEntityClass);

	REGISTER_METHOD(GetEntity);
	REGISTER_METHOD(FindEntity);
	REGISTER_METHOD(GetEntitiesByClass);
	REGISTER_METHOD(GetEntitiesInBox);

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

	REGISTER_METHOD(BreakIntoPieces);

	REGISTER_METHOD(CreateGameObjectForEntity);
	REGISTER_METHOD(GetStaticObjectFilePath);

	REGISTER_METHOD(SetWorldTM);
	REGISTER_METHOD(GetWorldTM);
	REGISTER_METHOD(SetLocalTM);
	REGISTER_METHOD(GetLocalTM);

	REGISTER_METHOD(GetName);
	REGISTER_METHOD(SetName);

	REGISTER_METHOD(GetFlags);
	REGISTER_METHOD(SetFlags);

	REGISTER_METHOD(GetAttachmentCount);
	REGISTER_METHOD(GetAttachmentMaterialByIndex);
	REGISTER_METHOD(SetAttachmentMaterialByIndex);

	REGISTER_METHOD(SetAttachmentMaterial);
	REGISTER_METHOD(GetAttachmentMaterial);

	gEnv->pEntitySystem->AddSink(this, IEntitySystem::OnBeforeSpawn | IEntitySystem::OnSpawn | IEntitySystem::OnRemove, 0);
}

CEntityManager::~CEntityManager()
{
	m_monoEntityClasses.clear();

	for(auto it = m_monoEntities.begin(); it != m_monoEntities.end(); ++it)
	{
		(*it).reset();

		it = m_monoEntities.erase(it);
	}

	if(gEnv->pEntitySystem)
		gEnv->pEntitySystem->RemoveSink(this);
}

bool CEntityManager::OnBeforeSpawn(SEntitySpawnParams &params)
{
	const char *className = params.pClass->GetName();

	if(!IsMonoEntity(className))
		return true;

	m_monoEntities.push_back(std::shared_ptr<CEntity>(new CEntity(params)));

	return true;
}

void CEntityManager::OnSpawn(IEntity *pEntity,SEntitySpawnParams &params)
{
	if(!IsMonoEntity(params.pClass->GetName()))
		return;

	// Id isn't available in OnBeforeSpawn while running Editor, so we rely on the GUID instead.
	if(auto monoEnt = GetMonoEntity(pEntity->GetGuid()))
		monoEnt->OnSpawn(pEntity, params);
}

bool CEntityManager::OnRemove(IEntity *pEntity)
{
	if(!pEntity)
		return true;

	const char *entClass = pEntity->GetClass()->GetName();
	if(!IsMonoEntity(entClass))
		return true;

	EntityId id = pEntity->GetId();
	bool result = true;

	if(IMonoClass *pEntityClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Entity"))
	{
		IMonoArray *pArgs = CreateMonoArray(1);
		pArgs->Insert(id);

		result = pEntityClass->CallMethod("InternalRemove", pArgs, true)->Unbox<bool>();

		SAFE_RELEASE(pArgs);
		SAFE_RELEASE(pEntityClass);
	}

	if(result)
	{
		for(TMonoEntities::iterator it = m_monoEntities.begin(); it != m_monoEntities.end(); ++it)
		{
			if((*it)->GetEntityId() == id)
			{
				(*it).reset();

				m_monoEntities.erase(it);
				break;
			}
		}
	}

	return result;
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

std::shared_ptr<CEntity> CEntityManager::GetMonoEntity(EntityGUID guid)
{
	for each(auto& monoEntity in m_monoEntities)
	{
		if(monoEntity->GetEntityGUID()==guid)
			return monoEntity;
	}

	return NULL;
}

IMonoClass *CEntityManager::GetScript(EntityId entityId, bool returnBackIfInvalid)
{
	if(auto& entity = GetMonoEntity(entityId))
		return entity->GetScript();

	if(returnBackIfInvalid)
		return m_monoEntities.back()->GetScript();

	return NULL;
}

bool CEntityManager::SpawnEntity(EntitySpawnParams monoParams, bool bAutoInit, SMonoEntityInfo &entityInfo)
{
	IEntityClass *pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(ToCryString(monoParams.sClass));
	if(pClass != NULL)
	{
		SEntitySpawnParams spawnParams;
		spawnParams.pClass = pClass;
		spawnParams.sName = ToCryString(monoParams.sName);

		spawnParams.nFlags = monoParams.flags;
		spawnParams.vPosition = monoParams.pos;
		spawnParams.qRotation = Quat(Ang3(monoParams.rot));
		spawnParams.vScale = monoParams.scale;

		if(IEntity *pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams, bAutoInit))
		{
			entityInfo.pEntity = pEntity;
			entityInfo.id = pEntity->GetId();

			return true;
		}
	}

	return false;
}

bool CEntityManager::RegisterEntityClass(EntityRegisterParams params, mono::array Properties)
{
	const char *className = ToCryString(params.Name);
	if(gEnv->pEntitySystem->GetClassRegistry()->FindClass(className))
	{
		CryLog("Aborting registration of entity class %s, a class with the same name already exists", className); 
		return false;
	}

	std::vector<IEntityPropertyHandler::SPropertyInfo> properties;
	if(Properties != NULL)
	{
		IMonoArray *propertiesArray = *Properties;

		int numProperties = propertiesArray->GetSize();
	
		for	(int i = 0; i < numProperties; ++i)
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
	}

	IEntityClassRegistry::SEntityClassDesc entityClassDesc;	
	entityClassDesc.flags = params.Flags;
	entityClassDesc.sName = className;
	entityClassDesc.editorClassInfo.sCategory = ToCryString(params.Category);

	if(params.EditorHelper != NULL)
		entityClassDesc.editorClassInfo.sHelper = ToCryString(params.EditorHelper);
	if(params.EditorIcon != NULL)
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

mono::array CEntityManager::GetEntitiesInBox(AABB bbox, int objTypes)
{
	IPhysicalEntity **pEnts = NULL;

	int numEnts = gEnv->pPhysicalWorld->GetEntitiesInBox(bbox.min, bbox.max, pEnts, objTypes);
	IMonoArray *pEntities = CreateMonoArray(numEnts);

	for(int i = 0; i < numEnts; i++)
		pEntities->Insert(gEnv->pPhysicalWorld->GetPhysicalEntityId(pEnts[i]));

	return pEntities->GetMonoArray();
}

mono::string CEntityManager::GetPropertyValue(IEntity *pEnt, mono::string propertyName)
{
	IEntityPropertyHandler *pPropertyHandler = pEnt->GetClass()->GetPropertyHandler();

	return ToMonoString(pPropertyHandler->GetProperty(pEnt, 0));
}

void CEntityManager::SetWorldTM(IEntity *pEntity, Matrix34 tm)
{
	pEntity->SetWorldTM(tm);
}

Matrix34 CEntityManager::GetWorldTM(IEntity *pEntity)
{
	return pEntity->GetWorldTM();
}

void CEntityManager::SetLocalTM(IEntity *pEntity, Matrix34 tm)
{
	pEntity->SetLocalTM(tm);
}

Matrix34 CEntityManager::GetLocalTM(IEntity *pEntity)
{
	return pEntity->GetLocalTM();
}

AABB CEntityManager::GetWorldBoundingBox(IEntity *pEntity)
{
	AABB boundingBox;
	pEntity->GetWorldBounds(boundingBox);

	return boundingBox;
}

AABB CEntityManager::GetBoundingBox(IEntity *pEntity)
{
	AABB boundingBox;
	pEntity->GetLocalBounds(boundingBox);

	return boundingBox;
}

void CEntityManager::SetPos(IEntity *pEntity, Vec3 newPos)
{
	pEntity->SetPos(newPos);
}

Vec3 CEntityManager::GetPos(IEntity *pEntity)
{
	return pEntity->GetPos();
}

void CEntityManager::SetWorldPos(IEntity *pEntity, Vec3 newPos)
{
	pEntity->SetWorldTM(Matrix34::Create(pEntity->GetScale(), pEntity->GetWorldRotation(), newPos));
}

Vec3 CEntityManager::GetWorldPos(IEntity *pEntity)
{
	return pEntity->GetWorldPos();
}

void CEntityManager::SetRotation(IEntity *pEntity, Quat newAngles)
{
	pEntity->SetRotation(newAngles);
}

Quat CEntityManager::GetRotation(IEntity *pEntity)
{
	return pEntity->GetRotation();
}

void CEntityManager::SetWorldRotation(IEntity *pEntity, Quat newAngles)
{
	pEntity->SetWorldTM(Matrix34::Create(pEntity->GetScale(), newAngles, pEntity->GetWorldPos()));
}

Quat CEntityManager::GetWorldRotation(IEntity *pEntity)
{
	return pEntity->GetWorldRotation();
}

void CEntityManager::LoadObject(IEntity *pEntity, mono::string fileName, int slot)
{
	pEntity->SetStatObj(gEnv->p3DEngine->LoadStatObj(ToCryString(fileName)), slot, true);
}

void CEntityManager::LoadCharacter(IEntity *pEntity, mono::string fileName, int slot)
{
	pEntity->LoadCharacter(slot, ToCryString(fileName));
}

EEntitySlotFlags CEntityManager::GetSlotFlags(IEntity *pEntity, int slot)
{
	return (EEntitySlotFlags)pEntity->GetSlotFlags(slot);
}

void CEntityManager::SetSlotFlags(IEntity *pEntity, int slot, EEntitySlotFlags slotFlags)
{
	pEntity->SetSlotFlags(slot, slotFlags);
}

void CEntityManager::BreakIntoPieces(IEntity *pEntity, int slot, int piecesSlot, IBreakableManager::BreakageParams breakageParams)
{
	gEnv->pEntitySystem->GetBreakableManager()->BreakIntoPieces(pEntity, slot, piecesSlot, breakageParams);
}

void CEntityManager::CreateGameObjectForEntity(IEntity *pEntity)
{
	IGameObject *pGameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(pEntity->GetId());
	if(!pGameObject)
		return;

	if(auto& entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetMonoEntity(pEntity->GetId()))
		entity->RegisterGameObject(pGameObject);
}

void CEntityManager::BindGameObjectToNetwork(IEntity *pEntity)
{
	if(auto& entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetMonoEntity(pEntity->GetId()))
		entity->GetGameObject()->BindToNetwork();
}

mono::string CEntityManager::GetStaticObjectFilePath(IEntity *pEntity, int slot)
{
	if(IStatObj *pStatObj = pEntity->GetStatObj(slot))
		return ToMonoString(pStatObj->GetFilePath());

	return ToMonoString("");
}

mono::string CEntityManager::GetName(IEntity *pEntity)
{
	return ToMonoString(pEntity->GetName());
}

void CEntityManager::SetName(IEntity *pEntity, mono::string name)
{
	pEntity->SetName(ToCryString(name));
}

EEntityFlags CEntityManager::GetFlags(IEntity *pEntity)
{
	return (EEntityFlags)pEntity->GetFlags();
}

void CEntityManager::SetFlags(IEntity *pEntity, EEntityFlags flags)
{
	pEntity->SetFlags(flags);
}

int CEntityManager::GetAttachmentCount(IEntity *pEnt)
{
	if(auto pCharacter = pEnt->GetCharacter(0))
	{
		if(auto pAttachmentManager = pCharacter->GetIAttachmentManager())
			return pAttachmentManager->GetAttachmentCount();
	}

	return 0;
}

IAttachmentManager *GetAttachmentManager(IEntity *pEntity)
{
	if(auto pCharacter = pEntity->GetCharacter(0))
		return pCharacter->GetIAttachmentManager();

	return NULL;
}

IMaterial *CEntityManager::GetAttachmentMaterialByIndex(IEntity *pEnt, int index)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByIndex(index))
			return pAttachment->GetIAttachmentObject()->GetMaterial();
	}

	return NULL;
}

IMaterial *CEntityManager::GetAttachmentMaterial(IEntity *pEnt, mono::string attachmentName)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByName(ToCryString(attachmentName)))
		{
			if(auto pAttachmentObject = pAttachment->GetIAttachmentObject())
				return pAttachmentObject->GetMaterial();
		}
	}

	return NULL;
}

void CEntityManager::SetAttachmentMaterialByIndex(IEntity *pEnt, int index, IMaterial *pMaterial)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByIndex(index))
			pAttachment->GetIAttachmentObject()->SetMaterial(pMaterial);
	}
}

void CEntityManager::SetAttachmentMaterial(IEntity *pEnt, mono::string attachmentName, IMaterial *pMaterial)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByName(ToCryString(attachmentName)))
			return pAttachment->GetIAttachmentObject()->SetMaterial(pMaterial);;
	}
}