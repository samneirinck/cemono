#include "StdAfx.h"
#include "Scriptbinds\Entity.h"

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

std::vector<const char *> CScriptbind_Entity::m_monoEntityClasses = std::vector<const char *>();

CScriptbind_Entity::CScriptbind_Entity()
{
	REGISTER_METHOD(SpawnEntity);
	REGISTER_METHOD(RemoveEntity);

	REGISTER_METHOD(RegisterEntityClass);

	REGISTER_METHOD(GetEntity);
	REGISTER_METHOD(FindEntity);
	REGISTER_METHOD(GetEntitiesByClass);
	REGISTER_METHOD(GetEntitiesInBox);

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

	REGISTER_METHOD(SetVisionParams);
	REGISTER_METHOD(SetHUDSilhouettesParams);

	gEnv->pEntitySystem->AddSink(this, IEntitySystem::OnSpawn | IEntitySystem::OnRemove, 0);
}

bool CScriptbind_Entity::IsMonoEntity(const char *className)
{
	for each(auto entityClass in m_monoEntityClasses)
	{
		if(!strcmp(entityClass, className))
			return true;
	}

	return false;
}

void CScriptbind_Entity::OnSpawn(IEntity *pEntity,SEntitySpawnParams &params)
{
	const char *className = params.pClass->GetName();
	if(!IsMonoEntity(className))
		return;

	auto gameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(pEntity->GetId());
	gameObject->ActivateExtension(className);
}

bool CScriptbind_Entity::OnRemove(IEntity *pIEntity)
{
	const char *className = pIEntity->GetClass()->GetName();
	if(!IsMonoEntity(className))
		return true;

	if(IMonoClass *pEntityClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Entity"))
	{
		IMonoArray *pArgs = CreateMonoArray(1);
		pArgs->Insert(pIEntity->GetId());

		auto result = pEntityClass->CallMethod("InternalRemove", pArgs, true)->Unbox<bool>();

		SAFE_RELEASE(pArgs);
		SAFE_RELEASE(pEntityClass);

		if(!result)
			return false;
	}

	return true;
}

struct SMonoEntityCreator
	: public IGameObjectExtensionCreatorBase
{
	virtual IGameObjectExtension *Create() { return new CEntity(); }
	virtual void GetGameObjectExtensionRMIData(void **ppRMI, size_t *nCount) { return CEntity::GetGameObjectExtensionRMIData(ppRMI, nCount); }
};

bool CScriptbind_Entity::RegisterEntityClass(EntityRegisterParams params, mono::array Properties)
{
	const char *className = ToCryString(params.Name);
	if(gEnv->pEntitySystem->GetClassRegistry()->FindClass(className))
	{
		MonoWarning("Aborting registration of entity class %s, a class with the same name already exists", className); 
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

	m_monoEntityClasses.push_back(className);

	bool result = gEnv->pEntitySystem->GetClassRegistry()->RegisterClass(new CEntityClass(entityClassDesc, properties));

	static SMonoEntityCreator creator;
	gEnv->pGameFramework->GetIGameObjectSystem()->RegisterExtension(className, &creator, NULL);

	return result;
}

bool CScriptbind_Entity::SpawnEntity(EntitySpawnParams monoParams, bool bAutoInit, SMonoEntityInfo &entityInfo)
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

void CScriptbind_Entity::RemoveEntity(EntityId id)
{
	gEnv->pEntitySystem->RemoveEntity(id);
}

IEntity *CScriptbind_Entity::GetEntity(EntityId id)
{
	return gEnv->pEntitySystem->GetEntity(id);
}

EntityId CScriptbind_Entity::FindEntity(mono::string name)
{
	if(IEntity *pEntity = gEnv->pEntitySystem->FindEntityByName(ToCryString(name)))
		return pEntity->GetId();

	return 0;
}

mono::array CScriptbind_Entity::GetEntitiesByClass(mono::string _class)
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

mono::array CScriptbind_Entity::GetEntitiesInBox(AABB bbox, int objTypes)
{
	IPhysicalEntity **pEnts = NULL;

	int numEnts = gEnv->pPhysicalWorld->GetEntitiesInBox(bbox.min, bbox.max, pEnts, objTypes);
	IMonoArray *pEntities = CreateMonoArray(numEnts);

	for(int i = 0; i < numEnts; i++)
		pEntities->Insert(gEnv->pPhysicalWorld->GetPhysicalEntityId(pEnts[i]));

	return pEntities->GetMonoArray();
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

void CScriptbind_Entity::BreakIntoPieces(IEntity *pEntity, int slot, int piecesSlot, IBreakableManager::BreakageParams breakageParams)
{
	gEnv->pEntitySystem->GetBreakableManager()->BreakIntoPieces(pEntity, slot, piecesSlot, breakageParams);
}

mono::string CScriptbind_Entity::GetStaticObjectFilePath(IEntity *pEntity, int slot)
{
	if(IStatObj *pStatObj = pEntity->GetStatObj(slot))
		return ToMonoString(pStatObj->GetFilePath());

	return ToMonoString("");
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

int CScriptbind_Entity::GetAttachmentCount(IEntity *pEnt)
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

IMaterial *CScriptbind_Entity::GetAttachmentMaterialByIndex(IEntity *pEnt, int index)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByIndex(index))
			return pAttachment->GetIAttachmentObject()->GetMaterial();
	}

	return NULL;
}

IMaterial *CScriptbind_Entity::GetAttachmentMaterial(IEntity *pEnt, mono::string attachmentName)
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

void CScriptbind_Entity::SetAttachmentMaterialByIndex(IEntity *pEnt, int index, IMaterial *pMaterial)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByIndex(index))
			pAttachment->GetIAttachmentObject()->SetMaterial(pMaterial);
	}
}

void CScriptbind_Entity::SetAttachmentMaterial(IEntity *pEnt, mono::string attachmentName, IMaterial *pMaterial)
{
	if(auto pAttachmentManager = GetAttachmentManager(pEnt))
	{
		if(auto pAttachment = pAttachmentManager->GetInterfaceByName(ToCryString(attachmentName)))
			return pAttachment->GetIAttachmentObject()->SetMaterial(pMaterial);;
	}
}

void CScriptbind_Entity::SetVisionParams(IEntity *pEntity, float r, float g, float b, float a)
{
	IEntityRenderProxy *pRenderProxy = static_cast<IEntityRenderProxy *>(pEntity->GetProxy(ENTITY_PROXY_RENDER));
	if(!pRenderProxy)
		return;

	pRenderProxy->SetVisionParams(r, g, b, a);
}

void CScriptbind_Entity::SetHUDSilhouettesParams(IEntity *pEntity, float r, float g, float b, float a)
{
	IEntityRenderProxy *pRenderProxy = static_cast<IEntityRenderProxy *>(pEntity->GetProxy(ENTITY_PROXY_RENDER));
	if(!pRenderProxy)
		return;

	pRenderProxy->SetVisionParams(r, g, b, a);
}