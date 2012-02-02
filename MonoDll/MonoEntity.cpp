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
	ADD_EVENTLISTENER(ENTITY_EVENT_ONHIT);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_GAME);
	ADD_EVENTLISTENER(ENTITY_EVENT_START_LEVEL);
	ADD_EVENTLISTENER(ENTITY_EVENT_ENTERAREA);
	ADD_EVENTLISTENER(ENTITY_EVENT_LEAVEAREA);
#undef ADD_EVENTLISTENER

	CallMonoScript(m_scriptId, "InternalSpawn", m_entityId);
}

void CMonoEntity::OnEntityEvent(IEntity *pEntity,SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LEVEL_LOADED:
		{
			CallMonoScript(m_scriptId, "OnInit");
		}
		break;
	case ENTITY_EVENT_RESET:
		{
			CallMonoScript(m_scriptId, "OnReset", event.nParam[0]==1);
		}
		break;
	case ENTITY_EVENT_ONHIT:
		{
			//CallMonoScript(m_scriptId, "OnHit");
		}
		break;
	case ENTITY_EVENT_START_GAME:
		{
			CallMonoScript(m_scriptId, "OnStartGame");
		}
		break;
	case ENTITY_EVENT_START_LEVEL:
		{
			CallMonoScript(m_scriptId, "OnStartLevel");
		}
		break;
	case ENTITY_EVENT_ENTERAREA:
		{
			CallMonoScript(m_scriptId, "OnEnterArea", (EntityId)event.nParam[0], (EntityId)event.nParam[2]);
		}
		break;
	case EEntityEvent::ENTITY_EVENT_LEAVEAREA:
		{
			CallMonoScript(m_scriptId, "OnLeaveArea", (EntityId)event.nParam[0], (EntityId)event.nParam[2]);
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