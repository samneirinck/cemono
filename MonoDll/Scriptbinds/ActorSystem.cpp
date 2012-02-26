#include "StdAfx.h"
#include "ActorSystem.h"

#include <IGameFramework.h>
#include <IActorSystem.h>

CScriptBind_ActorSystem::CScriptBind_ActorSystem()
{
	// TODO: Use the CallbackHandler for this.
	REGISTER_METHOD(GetPlayerHealth);
	REGISTER_METHOD(SetPlayerHealth);
	REGISTER_METHOD(GetPlayerMaxHealth);
	REGISTER_METHOD(SetPlayerMaxHealth);

	REGISTER_METHOD(RegisterActorClass);
}

void CScriptBind_ActorSystem::RegisterActorClass(mono::string className, bool isAI)
{
	//gEnv->pGameFramework->RegisterFactory(ToCryString(className), (CActorClass *)0, isAI, (CActorClass *)0);
}

float CScriptBind_ActorSystem::GetPlayerHealth(EntityId playerId)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		return pActor->GetHealth();

	return 0.0f;
}

void CScriptBind_ActorSystem::SetPlayerHealth(EntityId playerId, float newHealth)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		pActor->SetHealth(newHealth);
}

float CScriptBind_ActorSystem::GetPlayerMaxHealth(EntityId playerId)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		return pActor->GetMaxHealth();

	return 0.0f;
}

void CScriptBind_ActorSystem::SetPlayerMaxHealth(EntityId playerId, float newMaxHealth)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		pActor->SetMaxHealth(newMaxHealth);
}