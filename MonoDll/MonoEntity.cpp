#include "StdAfx.h"
#include "MonoEntity.h"
#include "MonoEntityPropertyHandler.h"

#include "Scriptbinds\Entity.h"

#include <IEntityClass.h>

#include <IMonoScriptSystem.h>
#include <IMonoAssembly.h>
#include <IMonoClass.h>
#include <IMonoConverter.h>

#include <MonoCommon.h>

CEntity::CEntity()
	: m_pScript(nullptr)
	, m_bInitialized(false)
{
}

CEntity::~CEntity()
{
	SAFE_RELEASE(m_pScript);
}

bool CEntity::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	pGameObject->EnablePrePhysicsUpdate( ePPU_Always );
	pGameObject->EnablePhysicsEvent( true, eEPE_OnPostStepImmediate );

	IEntity *pEntity = GetEntity();
	IEntityClass *pEntityClass = pEntity->GetClass();

	m_pScript = gEnv->pMonoScriptSystem->InstantiateScript(pEntityClass->GetName(), eScriptFlag_Entity);

	IMonoClass *pEntityInfoClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetClass("EntityInfo");

	m_pScript->CallMethod("InternalSpawn", pEntityInfoClass->BoxObject(&SMonoEntityInfo(pEntity)));

	int numProperties;
	auto pProperties = static_cast<CEntityPropertyHandler *>(pEntityClass->GetPropertyHandler())->GetQueuedProperties(pEntity->GetId(), numProperties);

	if(pProperties)
	{
		for(int i = 0; i < numProperties; i++)
		{
			auto queuedProperty = pProperties[i];

			SetPropertyValue(queuedProperty.propertyInfo, queuedProperty.value.c_str());
		}
	}

	m_bInitialized = true;

	if (!GetGameObject()->BindToNetwork())
		return false;

	return true;
}

void CEntity::ProcessEvent(SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LEVEL_LOADED:
		m_pScript->CallMethod("OnInit");
		break;
	case ENTITY_EVENT_RESET:
		m_pScript->CallMethod("OnReset", event.nParam[0]==1);
		break;
	case ENTITY_EVENT_COLLISION:
		{
			EventPhysCollision *pCollision = (EventPhysCollision *)event.nParam[0];

			EntityId targetId = 0;

			IEntity *pTarget = pCollision->iForeignData[0]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pCollision->pForeignData[0]:0;
			if(pTarget)
				targetId = pTarget->GetId();

			m_pScript->CallMethod("OnCollision", targetId, pCollision->pt, pCollision->vloc[0].GetNormalizedSafe(), pCollision->idmat[0], pCollision->n);
		}
		break;
	case ENTITY_EVENT_START_GAME:
		m_pScript->CallMethod("OnStartGame");
		break;
	case ENTITY_EVENT_START_LEVEL:
		m_pScript->CallMethod("OnStartLevel");
		break;
	case ENTITY_EVENT_ENTERAREA:
		m_pScript->CallMethod("OnEnterArea", (EntityId)event.nParam[0], (int)event.nParam[1], event.fParam[0]);
		break;
	case ENTITY_EVENT_MOVEINSIDEAREA:
		m_pScript->CallMethod("OnMoveInsideArea", (EntityId)event.nParam[0], (int)event.nParam[1], event.fParam[0]);
		break;
	case ENTITY_EVENT_LEAVEAREA:
		m_pScript->CallMethod("OnLeaveArea", (EntityId)event.nParam[0], (int)event.nParam[1], event.fParam[0]);
		break;
	case ENTITY_EVENT_ENTERNEARAREA:
		m_pScript->CallMethod("OnEnterNearArea", (EntityId)event.nParam[0], (int)event.nParam[1], event.fParam[0]);
		break;
	case ENTITY_EVENT_MOVENEARAREA:
		m_pScript->CallMethod("OnMoveNearArea", (EntityId)event.nParam[0], (int)event.nParam[1], event.fParam[0]);
		break;
	case ENTITY_EVENT_LEAVENEARAREA:
		m_pScript->CallMethod("OnLeaveNearArea", (EntityId)event.nParam[0], (int)event.nParam[1], event.fParam[0]);
		break;
	case ENTITY_EVENT_XFORM:
		m_pScript->CallMethod("OnMove");
		break;
	case ENTITY_EVENT_ATTACH:
		m_pScript->CallMethod("OnAttach", (EntityId)event.nParam[0]);
		break;
	case ENTITY_EVENT_DETACH:
		m_pScript->CallMethod("OnDetach", (EntityId)event.nParam[0]);
		break;
	case ENTITY_EVENT_DETACH_THIS:
		m_pScript->CallMethod("OnDetachThis", (EntityId)event.nParam[0]);
		break;
	}
}

void CEntity::FullSerialize(TSerialize ser)
{
	IEntity *pEntity = GetEntity();

	ser.BeginGroup("Properties");
	auto pPropertyHandler = static_cast<CEntityPropertyHandler *>(pEntity->GetClass()->GetPropertyHandler());
	for(int i = 0; i < pPropertyHandler->GetPropertyCount(); i++)
	{
		if(ser.IsWriting())
		{
			IEntityPropertyHandler::SPropertyInfo propertyInfo;
			pPropertyHandler->GetPropertyInfo(i, propertyInfo);

			ser.Value(propertyInfo.name, pPropertyHandler->GetProperty(pEntity, i));
		}
		else
		{
			IEntityPropertyHandler::SPropertyInfo propertyInfo;
			pPropertyHandler->GetPropertyInfo(i, propertyInfo);

			char *propertyValue = nullptr;
			ser.ValueChar(propertyInfo.name, propertyValue, 0);

			pPropertyHandler->SetProperty(pEntity, i, propertyValue);
		}
	}
	ser.EndGroup();
}

void CEntity::SetPropertyValue(IEntityPropertyHandler::SPropertyInfo propertyInfo, const char *value)
{
	m_pScript->CallMethod("SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
}

///////////////////////////////////////////////////
// Entity RMI's
///////////////////////////////////////////////////
CEntity::RMIParams::RMIParams(IMonoArray *pArray)
{
	length = pArray->GetSize();

	anyValues = new MonoAnyValue[length];

	for(int i = 0; i < length; i++)
		anyValues[i] = pArray->GetItem(i)->GetAnyValue();
}

void CEntity::RMIParams::SerializeWith(TSerialize ser)
{
	ser.Value("length", length);

	for(int i = 0; i < length; i++)
		anyValues[i].SerializeWith(ser);
}

IMPLEMENT_RMI(CEntity, SvScriptRMI)
{
	return true;
}

IMPLEMENT_RMI(CEntity, ClScriptRMI)
{
	return true;
}