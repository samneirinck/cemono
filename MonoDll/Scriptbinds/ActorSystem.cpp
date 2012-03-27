#include "StdAfx.h"
#include "ActorSystem.h"

#include <IGameFramework.h>
#include <IActorSystem.h>

CActorSystem::CActorSystem()
{
	// TODO: Use the CallbackHandler for this.
	REGISTER_METHOD(GetPlayerHealth);
	REGISTER_METHOD(SetPlayerHealth);
	REGISTER_METHOD(GetPlayerMaxHealth);
	REGISTER_METHOD(SetPlayerMaxHealth);

	REGISTER_METHOD(GetEntityIdForChannelId);
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
	//gEnv->pGameFramework->RegisterFactory(ToCryString(className), (CActorClass *)0, isAI, (CActorClass *)0);
}

float CActorSystem::GetPlayerHealth(EntityId playerId)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		return pActor->GetHealth();

	return 0.0f;
}

void CActorSystem::SetPlayerHealth(EntityId playerId, float newHealth)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		pActor->SetHealth(newHealth);
}

float CActorSystem::GetPlayerMaxHealth(EntityId playerId)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		return pActor->GetMaxHealth();

	return 0.0f;
}

void CActorSystem::SetPlayerMaxHealth(EntityId playerId, float newMaxHealth)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		pActor->SetMaxHealth(newMaxHealth);
}