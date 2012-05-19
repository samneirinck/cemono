#include "StdAfx.h"
#include "MonoEntity.h"

#include <IMonoScriptSystem.h>
#include <IMonoConverter.h>
#include <IMonoObject.h>


CEntity::CEntity(SEntitySpawnParams &spawnParams)
	: m_pGameObject(NULL)
	, m_pScriptClass(NULL)
	, m_entityGUID(spawnParams.guid)
{
}

CEntity::~CEntity()
{
	SAFE_RELEASE(m_pScriptClass);
}

void CEntity::OnSpawn(IEntity *pEntity, SEntitySpawnParams &spawnParams)
{
	m_entityId = spawnParams.id;

#define ADD_EVENTLISTENER(event) gEnv->pEntitySystem->AddEntityEventListener(m_entityId, event, this);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEVEL_LOADED);
	ADD_EVENTLISTENER(ENTITY_EVENT_RESET);
	ADD_EVENTLISTENER(ENTITY_EVENT_COLLISION);
	//ADD_EVENTLISTENER(ENTITY_EVENT_ONHIT);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_GAME);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_LEVEL);
	ADD_EVENTLISTENER(ENTITY_EVENT_ENTERAREA);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEAVEAREA);
#undef ADD_EVENTLISTENER

	m_pScriptClass = gEnv->pMonoScriptSystem->InstantiateScript(spawnParams.pClass->GetName(), eScriptType_Entity);

	IMonoClass *pEntityInfoClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("EntityInfo");

	CallMonoScript<void>(m_pScriptClass, "InternalSpawn", gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(pEntityInfoClass, &SMonoEntityInfo(pEntity, m_entityId)));

	for each(auto propertyCall in m_propertyQueue)
		CallMonoScript<void>(m_pScriptClass, "SetPropertyValue", propertyCall.propertyInfo.name, propertyCall.propertyInfo.type, propertyCall.value);

	m_propertyQueue.clear();

	if(auto pMaterial = gEnv->p3DEngine->GetMaterialManager()->FindMaterial("Objects/Brushy/Trees/Ash/tree_ash_autumn"))
		pEntity->SetMaterial(pMaterial);
}

void CEntity::OnEntityEvent(IEntity *pEntity,SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LEVEL_LOADED:
		m_pScriptClass->CallMethod("OnInit");
		break;
	case ENTITY_EVENT_RESET:
		CallMonoScript<void>(m_pScriptClass, "OnReset", event.nParam[0]==1);
		break;
	case ENTITY_EVENT_COLLISION:
		{
			EventPhysCollision *pCollision = (EventPhysCollision *)event.nParam[0];

			EntityId targetId = 0;

			IEntity *pTarget = pCollision->iForeignData[0]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pCollision->pForeignData[0]:0;
			if(pTarget)
				targetId = pTarget->GetId();

			CallMonoScript<void>(m_pScriptClass, "OnCollision", targetId, pCollision->pt, pCollision->vloc[0].GetNormalizedSafe(), pCollision->idmat[0], pCollision->n);
		}
		break;/*
	case ENTITY_EVENT_ONHIT:
		{
			CallMonoScript<void>(m_scriptId, "OnHit");
		}
		break;*/
	case ENTITY_EVENT_START_GAME:
		m_pScriptClass->CallMethod("OnStartGame");
		break;
	case ENTITY_EVENT_START_LEVEL:
		m_pScriptClass->CallMethod("OnStartLevel");
		break;
	case ENTITY_EVENT_ENTERAREA:
		CallMonoScript<void>(m_pScriptClass, "OnEnterArea", (EntityId)event.nParam[0], (EntityId)event.nParam[2]);
		break;
	case ENTITY_EVENT_LEAVEAREA:
		CallMonoScript<void>(m_pScriptClass, "OnLeaveArea", (EntityId)event.nParam[0], (EntityId)event.nParam[2]);
		break;
	}
}

void CEntity::SetPropertyValue(IEntityPropertyHandler::SPropertyInfo propertyInfo, const char *value)
{
	if(IsSpawned())
		CallMonoScript<void>(m_pScriptClass, "SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
	else
		m_propertyQueue.push_back(SQueuedProperty(propertyInfo, value));
}