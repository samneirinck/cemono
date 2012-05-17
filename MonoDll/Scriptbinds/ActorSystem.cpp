#include "StdAfx.h"
#include "ActorSystem.h"

#include "Actor.h"

#include <IGameFramework.h>

CActorSystem::CActorSystem()
{
	REGISTER_METHOD(GetPlayerHealth);
	REGISTER_METHOD(SetPlayerHealth);
	REGISTER_METHOD(GetPlayerMaxHealth);
	REGISTER_METHOD(SetPlayerMaxHealth);

	REGISTER_METHOD(GetEntityIdForChannelId);

	REGISTER_METHOD(CreateActor);
	REGISTER_METHOD(RemoveActor);

	REGISTER_METHOD(GetClientActor);

	REGISTER_METHOD(RegisterActorClass);
}

EntityId CActorSystem::GetEntityIdForChannelId(uint16 channelId)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActorByChannelId(channelId))
		return pActor->GetEntityId();

	return 0;
}

mono::object CActorSystem::CreateActor(int channelId, mono::string name, mono::string className, Vec3 pos, Vec3 angles, Vec3 scale)
{
	SMonoActorInfo actorInfo = SMonoActorInfo();

	if(gEnv->bServer)
	{
		if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->CreateActor(channelId, ToCryString(name), ToCryString(className), pos, Quat(Ang3(angles)), scale))
		{
			actorInfo.pActor = pActor;
			actorInfo.pEntity = pActor->GetEntity();
			actorInfo.id = pActor->GetEntityId();
		}
	}

	IMonoClass *pActorInfoClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("ActorInfo");
	return gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(pActorInfoClass, &actorInfo)->GetMonoObject();
}

void CActorSystem::RemoveActor(EntityId id)
{
	gEnv->pGameFramework->GetIActorSystem()->RemoveActor(id);
}

EntityId CActorSystem::GetClientActor()
{
	return gEnv->pGameFramework->GetClientActorId();
}

void CActorSystem::RegisterActorClass(mono::string className, bool isAI)
{
	if(gEnv->pGameFramework->GetIGameObjectSystem()->GetID(ToCryString(className)) != IGameObjectSystem::InvalidExtensionID)
		return; // already registered

	gEnv->pGameFramework->RegisterFactory(ToCryString(className), (CActor *)0, (isAI), (CActor *)0);
}

float CActorSystem::GetPlayerHealth(IActor *pActor)
{
	return pActor->GetHealth();
}

void CActorSystem::SetPlayerHealth(IActor *pActor, float newHealth)
{
	pActor->SetHealth(newHealth);
}

float CActorSystem::GetPlayerMaxHealth(IActor *pActor)
{
	return pActor->GetMaxHealth();
}

void CActorSystem::SetPlayerMaxHealth(IActor *pActor, float newMaxHealth)
{
	pActor->SetMaxHealth(newMaxHealth);
}