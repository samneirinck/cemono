#include "StdAfx.h"
#include "MonoEntity.h"
#include "MonoEntityPropertyHandler.h"

#include "Scriptbinds\Entity.h"

#include <IEntityClass.h>

#include <IMonoScriptSystem.h>
#include <IMonoAssembly.h>
#include <IMonoConverter.h>

#include <MonoCommon.h>

CEntity::CEntity()
	: m_pScriptClass(NULL)
	, m_bInitialized(false)
{
}

CEntity::~CEntity()
{
	SAFE_RELEASE(m_pScriptClass);
}

bool CEntity::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	pGameObject->EnablePrePhysicsUpdate( ePPU_Always );
	pGameObject->EnablePhysicsEvent( true, eEPE_OnPostStepImmediate );

	IEntity *pEntity = GetEntity();
	IEntityClass *pEntityClass = pEntity->GetClass();

	m_pScriptClass = gEnv->pMonoScriptSystem->InstantiateScript(pEntityClass->GetName(), eScriptType_Entity);

	IMonoClass *pEntityInfoClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("EntityInfo");

	CallMonoScript<void>(m_pScriptClass, "InternalSpawn", gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(pEntityInfoClass, &SMonoEntityInfo(pEntity)));

	int numProperties;
	auto pProperties = static_cast<CEntityPropertyHandler *>(pEntityClass->GetPropertyHandler())->GetQueuedProperties(pEntity->GetId(), numProperties);

	if(pProperties)
	{
		for(int i = 0; i < numProperties; i++)
		{
			auto queuedProperty = pProperties[i];

			CryLogAlways("Setting property %s with value %s on entity %i", queuedProperty.propertyInfo.name, queuedProperty.value.c_str(), pEntity->GetId());
			CallMonoScript<void>(m_pScriptClass, "SetPropertyValue", queuedProperty.propertyInfo.name, queuedProperty.propertyInfo.type, queuedProperty.value.c_str());
		}
	}

	m_bInitialized = true;

	return true;
}

void CEntity::ProcessEvent(SEntityEvent &event)
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
	CallMonoScript<void>(m_pScriptClass, "SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
}