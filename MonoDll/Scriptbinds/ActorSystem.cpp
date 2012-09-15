#include "StdAfx.h"
#include "ActorSystem.h"

#include <IGameFramework.h>

CActorSystem::CActorSystem()
{
	REGISTER_METHOD(GetPlayerHealth);
	REGISTER_METHOD(SetPlayerHealth);
	REGISTER_METHOD(GetPlayerMaxHealth);
	REGISTER_METHOD(SetPlayerMaxHealth);

	REGISTER_METHOD(IsPlayer);
	REGISTER_METHOD(IsClient);

	REGISTER_METHOD(GetActorInfoByChannelId);
	REGISTER_METHOD(GetActorInfoById);

	REGISTER_METHOD(CreateActor);
	REGISTER_METHOD(RemoveActor);

	REGISTER_METHOD(GetClientActorId);
}

SMonoActorInfo CActorSystem::GetActorInfoByChannelId(uint16 channelId)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActorByChannelId(channelId))
		return SMonoActorInfo(pActor);

	return SMonoActorInfo();
}

SMonoActorInfo CActorSystem::GetActorInfoById(EntityId id)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(id))
		return SMonoActorInfo(pActor);

	return SMonoActorInfo();
}

SMonoActorInfo CActorSystem::CreateActor(int channelId, mono::string name, mono::string className, Vec3 pos, Vec3 angles, Vec3 scale)
{
	if(gEnv->bServer)
	{
		if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->CreateActor(channelId, ToCryString(name), ToCryString(className), pos, Quat(Ang3(angles)), scale))
			return SMonoActorInfo(pActor);
	}

	return SMonoActorInfo();
}

void CActorSystem::RemoveActor(EntityId id)
{
	gEnv->pGameFramework->GetIActorSystem()->RemoveActor(id);
}

EntityId CActorSystem::GetClientActorId()
{
	return gEnv->pGameFramework->GetClientActorId();
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

bool CActorSystem::IsPlayer(IActor *pActor)
{
	return pActor->IsPlayer();
}

bool CActorSystem::IsClient(IActor *pActor)
{
	return pActor->IsClient();
}