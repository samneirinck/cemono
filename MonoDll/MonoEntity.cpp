#include "StdAfx.h"
#include "MonoEntity.h"
#include "MonoEntityPropertyHandler.h"

#include "MonoScriptSystem.h"
#include "Scriptbinds\Entity.h"

#include <IEntityClass.h>

#include <IMonoScriptSystem.h>
#include <IMonoAssembly.h>
#include <IMonoClass.h>
#include <IMonoConverter.h>

#include <MonoCommon.h>

CMonoEntityExtension::CMonoEntityExtension()
	: m_pScript(nullptr)
	, m_bInitialized(false)
	, m_pAnimatedCharacter(nullptr)
{
}

CMonoEntityExtension::~CMonoEntityExtension()
{
	if (m_pAnimatedCharacter)
	{
		IGameObject *pGameObject = GetGameObject();
		pGameObject->ReleaseExtension("AnimatedCharacter");
	}
}

bool CMonoEntityExtension::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	pGameObject->EnablePhysicsEvent( true, eEPE_OnPostStepImmediate );

	if (!GetGameObject()->BindToNetwork())
		return false;

	IEntity *pEntity = GetEntity();
	IEntityClass *pEntityClass = pEntity->GetClass();

	m_pScript = g_pScriptSystem->InstantiateScript(pEntityClass->GetName(), eScriptFlag_Entity);

	IMonoClass *pEntityInfoClass = g_pScriptSystem->GetCryBraryAssembly()->GetClass("EntityInitializationParams", "CryEngine.Native");

	SMonoEntityInfo entityInfo(pEntity);

	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->InsertMonoObject(pEntityInfoClass->BoxObject(&entityInfo));

	g_pScriptSystem->InitializeScriptInstance(m_pScript, pArgs);

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

	return true;
}

void CMonoEntityExtension::PostInit(IGameObject *pGameObject)
{
	Reset(false);
}

void CMonoEntityExtension::Reset(bool enteringGamemode)
{
	if(m_pAnimatedCharacter)
		m_pAnimatedCharacter->ResetState();
	else if(m_pAnimatedCharacter = static_cast<IAnimatedCharacter *>(GetGameObject()->QueryExtension("AnimatedCharacter")))
		m_pAnimatedCharacter->ResetState();
}

void CMonoEntityExtension::ProcessEvent(SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LEVEL_LOADED:
		m_pScript->CallMethod("OnInit");
		break;
	case ENTITY_EVENT_RESET:
		{
			bool enterGamemode = event.nParam[0]==1;

			if(!enterGamemode && GetEntity()->GetFlags() & ENTITY_FLAG_NO_SAVE)
			{
				gEnv->pEntitySystem->RemoveEntity(GetEntityId());
				return;
			}

			m_pScript->CallMethod("OnEditorReset", enterGamemode);

			Reset(enterGamemode);
		}
		break;
	case ENTITY_EVENT_COLLISION:
		{
			EventPhysCollision *pCollision = (EventPhysCollision *)event.nParam[0];

			EntityId targetId = 0;

			IEntity *pTarget = pCollision->iForeignData[1]==PHYS_FOREIGN_ID_ENTITY ? (IEntity*)pCollision->pForeignData[1]:0;
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
		m_pScript->CallMethod("OnMove", (EEntityXFormFlags)event.nParam[0]);
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
	case ENTITY_EVENT_PREPHYSICSUPDATE:
		m_pScript->CallMethod("OnPrePhysicsUpdate");
		break;
	}
}

void CMonoEntityExtension::PostUpdate(float frameTime)
{
	m_pScript->CallMethod("OnPostUpdate");
}

void CMonoEntityExtension::FullSerialize(TSerialize ser)
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

	ser.BeginGroup("ManagedEntity");

	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->InsertNativePointer(&ser);

	m_pScript->GetClass()->InvokeArray(m_pScript->GetManagedObject(), "InternalFullSerialize", pArgs);

	ser.EndGroup();
}

bool CMonoEntityExtension::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags)
{
	ser.BeginGroup("ManagedEntity");

	void *params[4];
	params[0] = &ser;
	params[1] = &aspect;
	params[2] = &profile;
	params[3] = &flags;

	m_pScript->GetClass()->Invoke(m_pScript->GetManagedObject(), "InternalNetSerialize", params, 4);

	ser.EndGroup();

	return true;
}

void CMonoEntityExtension::PostSerialize()
{
	m_pScript->CallMethod("PostSerialize");
}

void CMonoEntityExtension::SetPropertyValue(IEntityPropertyHandler::SPropertyInfo propertyInfo, const char *value)
{
	if(value != nullptr)
		m_pScript->CallMethod("SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
}

///////////////////////////////////////////////////
// Entity RMI's
///////////////////////////////////////////////////
CMonoEntityExtension::RMIParams::RMIParams(IMonoArray *pArray, const char *funcName, EntityId target)
	: methodName(funcName)
	, targetId(target)
	, pArgs(pArray)
{
}

void CMonoEntityExtension::RMIParams::SerializeWith(TSerialize ser)
{
	int length = pArgs ? pArgs->GetSize() : 0;
	ser.Value("length", length);

	ser.Value("methodName", methodName);
	ser.Value("targetId", targetId, 'eid');

	if(length > 0)
	{
		if(ser.IsWriting())
		{
			for(int i = 0; i < length; i++)
				pArgs->GetItem(i)->GetAnyValue().SerializeWith(ser);
		}
		else
		{
			pArgs = g_pScriptSystem->GetScriptDomain()->CreateArray(length);

			for(int i = 0; i < length; i++)
			{
				MonoAnyValue value;
				value.SerializeWith(ser);
				pArgs->InsertAny(value);
			}
		}
	}
}

IMPLEMENT_RMI(CMonoEntityExtension, SvScriptRMI)
{
	IMonoClass *pEntityClass = g_pScriptSystem->GetCryBraryAssembly()->GetClass("Entity");

	IMonoArray *pNetworkArgs = CreateMonoArray(3);
	pNetworkArgs->Insert(ToMonoString(params.methodName.c_str()));
	pNetworkArgs->Insert(params.pArgs);
	pNetworkArgs->Insert(params.targetId);

	pEntityClass->InvokeArray(nullptr, "OnRemoteInvocation", pNetworkArgs);

	return true;
}

IMPLEMENT_RMI(CMonoEntityExtension, ClScriptRMI)
{
	IMonoClass *pEntityClass = g_pScriptSystem->GetCryBraryAssembly()->GetClass("Entity");

	IMonoArray *pNetworkArgs = CreateMonoArray(3);
	pNetworkArgs->Insert(ToMonoString(params.methodName.c_str()));
	pNetworkArgs->Insert(params.pArgs);
	pNetworkArgs->Insert(params.targetId);

	pEntityClass->InvokeArray(nullptr, "OnRemoteInvocation", pNetworkArgs);

	return true;
}