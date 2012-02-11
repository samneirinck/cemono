#include "StdAfx.h"
#include "MonoEntity.h"

#include <IMonoScriptSystem.h>
#include <IMonoConverter.h>
#include <IMonoObject.h>

CMonoEntity::CMonoEntity(int scriptId)
	: m_scriptId(scriptId)
	, m_pGameObject(NULL)
	, m_pAnimatedCharacter(NULL)
{
}

CMonoEntity::~CMonoEntity()
{
	if(m_pAnimatedCharacter != NULL)
		m_pGameObject->ReleaseExtension("AnimatedCharacter");

	gEnv->pMonoScriptSystem->RemoveScriptInstance(m_scriptId);
}

void CMonoEntity::OnSpawn(EntityId id)
{
	m_entityId = id;

#define ADD_EVENTLISTENER(event) gEnv->pEntitySystem->AddEntityEventListener(id, event, this);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEVEL_LOADED);
	ADD_EVENTLISTENER(ENTITY_EVENT_RESET);
	ADD_EVENTLISTENER(ENTITY_EVENT_COLLISION);
	ADD_EVENTLISTENER(ENTITY_EVENT_ONHIT);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_GAME);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_LEVEL);
	ADD_EVENTLISTENER(ENTITY_EVENT_ENTERAREA);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEAVEAREA);
#undef ADD_EVENTLISTENER

	CallMonoScript<void>(m_scriptId, "InternalSpawn", m_entityId);
}

void CMonoEntity::OnEntityEvent(IEntity *pEntity,SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LEVEL_LOADED:
		{
			CallMonoScript<void>(m_scriptId, "OnInit");
		}
		break;
	case ENTITY_EVENT_RESET:
		{
			CallMonoScript<void>(m_scriptId, "OnReset", event.nParam[0]==1);
		}
		break;
	case ENTITY_EVENT_COLLISION:
		{
			EventPhysCollision *pCollision = (EventPhysCollision *)event.nParam[0];

			IEntity *pTarget = pCollision->iForeignData[0]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pCollision->pForeignData[0]:0;
			if(pTarget && pTarget->GetId()!=m_entityId)
			{
				Vec3 dir = pCollision->vloc[0].GetNormalizedSafe();

				// uint targetId, Vec3 hitPt, Vec3 dir, short materialId, Vec3 contactNormal);
				CallMonoScript<void>(m_scriptId, "OnCollision", pTarget->GetId(), pCollision->pt, dir, pCollision->idmat[0], pCollision->n);
			}
		}
		break;
	case ENTITY_EVENT_ONHIT:
		{
			CallMonoScript<void>(m_scriptId, "OnHit");
		}
		break;
	case ENTITY_EVENT_START_GAME:
		{
			CallMonoScript<void>(m_scriptId, "OnStartGame");
		}
		break;
	case ENTITY_EVENT_START_LEVEL:
		{
			CallMonoScript<void>(m_scriptId, "OnStartLevel");
		}
		break;
	case ENTITY_EVENT_ENTERAREA:
		{
			CallMonoScript<void>(m_scriptId, "OnEnterArea", (EntityId)event.nParam[0], (EntityId)event.nParam[2]);
		}
		break;
	case EEntityEvent::ENTITY_EVENT_LEAVEAREA:
		{
			CallMonoScript<void>(m_scriptId, "OnLeaveArea", (EntityId)event.nParam[0], (EntityId)event.nParam[2]);
		}
		break;
	}
}

void CMonoEntity::AddMovement(const MovementRequest &request)
{
	SCharacterMoveRequest moveRequest;
	moveRequest.type = request.type;

	moveRequest.velocity = request.velocity;

	if(!m_pAnimatedCharacter)
		m_pAnimatedCharacter = static_cast<IAnimatedCharacter *>(m_pGameObject->AcquireExtension("AnimatedCharacter"));

	m_pAnimatedCharacter->AddMovement(moveRequest);
}