#include "StdAfx.h"
#include "MonoEntity.h"

#include <IMonoScriptSystem.h>
#include <IMonoConverter.h>
#include <IMonoObject.h>

CEntity::CEntity(IMonoClass *pScriptClass)
	: m_pScriptClass(pScriptClass)
	, m_pGameObject(NULL)
	, m_pAnimatedCharacter(NULL)
{
}

CEntity::~CEntity()
{
	if(m_pAnimatedCharacter != NULL)
		m_pGameObject->ReleaseExtension("AnimatedCharacter");

	SAFE_RELEASE(m_pScriptClass);
}

void CEntity::OnSpawn(EntityId id)
{
	m_entityId = id;

#define ADD_EVENTLISTENER(event) gEnv->pEntitySystem->AddEntityEventListener(id, event, this);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEVEL_LOADED);
	ADD_EVENTLISTENER(ENTITY_EVENT_RESET);
	ADD_EVENTLISTENER(ENTITY_EVENT_COLLISION);
	//ADD_EVENTLISTENER(ENTITY_EVENT_ONHIT);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_GAME);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_LEVEL);
	ADD_EVENTLISTENER(ENTITY_EVENT_ENTERAREA);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEAVEAREA);
#undef ADD_EVENTLISTENER

	IMonoClass *pEntityInfoClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("EntityInfo");

	CallMonoScript<void>(m_pScriptClass, "InternalSpawn", gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(pEntityInfoClass, &SMonoEntityInfo(gEnv->pEntitySystem->GetEntity(id))));
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

void CEntity::AddMovement(const MovementRequest &request)
{
	SCharacterMoveRequest moveRequest;
	moveRequest.type = request.type;

	moveRequest.velocity = request.velocity;

	if(!m_pAnimatedCharacter)
		m_pAnimatedCharacter = static_cast<IAnimatedCharacter *>(m_pGameObject->AcquireExtension("AnimatedCharacter"));

	m_pAnimatedCharacter->AddMovement(moveRequest);
}