#ifndef __ENTITY_EVENT_HANDLING_H__
#define __ENTITY_EVENT_HANDLING_H__

#include <IEntity.h>
#include <IMonoObject.h>

/// <summary>
/// Helper method for making sure that we handle events in the same way for both entities and actors.
/// </summary>
inline void HandleEntityEvent(SEntityEvent &event, IEntity *pEntity, IMonoObject *pScript)
{
	switch(event.event)
	{
	case ENTITY_EVENT_LEVEL_LOADED:
		pScript->CallMethod("OnInit");
		break;
	case ENTITY_EVENT_RESET:
		{
			bool enterGamemode = event.nParam[0]==1;

			if(!enterGamemode && pEntity->GetFlags() & ENTITY_FLAG_NO_SAVE)
			{
				gEnv->pEntitySystem->RemoveEntity(pEntity->GetId());
				return;
			}

			pScript->CallMethod("OnEditorReset", enterGamemode);
		}
		break;
	case ENTITY_EVENT_COLLISION:
		{
			EventPhysCollision *pCollision = (EventPhysCollision *)event.nParam[0];

			SMonoColliderInfo source = SMonoColliderInfo(pCollision, 0);
			SMonoColliderInfo target = SMonoColliderInfo(pCollision, 1);

			IMonoClass *pColliderInfoClass = g_pScriptSystem->GetCryBraryAssembly()->GetClass("ColliderInfo");

			IMonoArray *pArgs = CreateMonoArray(6);

			pArgs->InsertMonoObject(pColliderInfoClass->BoxObject(&source));
			pArgs->InsertMonoObject(pColliderInfoClass->BoxObject(&target));

			pArgs->Insert(pCollision->pt);
			pArgs->Insert(pCollision->n);

			pArgs->Insert(pCollision->penetration);
			pArgs->Insert(pCollision->radius);

			pScript->GetClass()->InvokeArray(pScript->GetManagedObject(), "OnCollision", pArgs);
			SAFE_RELEASE(pArgs);
		}
		break;
	case ENTITY_EVENT_START_GAME:
		pScript->CallMethod("OnStartGame");
		break;
	case ENTITY_EVENT_START_LEVEL:
		pScript->CallMethod("OnStartLevel");
		break;
	case ENTITY_EVENT_ENTERAREA:
		pScript->CallMethod("OnEnterArea", (EntityId)event.nParam[0], (int)event.nParam[1], (EntityId)event.nParam[2]);
		break;
	case ENTITY_EVENT_MOVEINSIDEAREA:
		pScript->CallMethod("OnMoveInsideArea", (EntityId)event.nParam[0], (int)event.nParam[1], (EntityId)event.nParam[2]);
		break;
	case ENTITY_EVENT_LEAVEAREA:
		pScript->CallMethod("OnLeaveArea", (EntityId)event.nParam[0], (int)event.nParam[1], (EntityId)event.nParam[2]);
		break;
	case ENTITY_EVENT_ENTERNEARAREA:
		pScript->CallMethod("OnEnterNearArea", (EntityId)event.nParam[0], (int)event.nParam[1], (EntityId)event.nParam[2]);
		break;
	case ENTITY_EVENT_MOVENEARAREA:
		pScript->CallMethod("OnMoveNearArea", (EntityId)event.nParam[0], (int)event.nParam[1], (EntityId)event.nParam[2], event.fParam[0]);
		break;
	case ENTITY_EVENT_LEAVENEARAREA:
		pScript->CallMethod("OnLeaveNearArea", (EntityId)event.nParam[0], (int)event.nParam[1], (EntityId)event.nParam[2]);
		break;
	case ENTITY_EVENT_XFORM:
		pScript->CallMethod("OnMove", (EEntityXFormFlags)event.nParam[0]);
		break;
	case ENTITY_EVENT_ATTACH:
		pScript->CallMethod("OnAttach", (EntityId)event.nParam[0]);
		break;
	case ENTITY_EVENT_DETACH:
		pScript->CallMethod("OnDetach", (EntityId)event.nParam[0]);
		break;
	case ENTITY_EVENT_DETACH_THIS:
		pScript->CallMethod("OnDetachThis", (EntityId)event.nParam[0]);
		break;
	case ENTITY_EVENT_PREPHYSICSUPDATE:
		pScript->CallMethod("OnPrePhysicsUpdate");
		break;
	}
}

#endif // __ENTITY_EVENT_HANDLING_H__