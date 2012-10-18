#include "StdAfx.h"
#include "ActorSystem.h"

#include "Actor.h"

#include <IGameFramework.h>

CActorSystem::TActorClasses CActorSystem::m_monoActorClasses = TActorClasses();

CActorSystem::CActorSystem()
{
	REGISTER_METHOD(GetPlayerHealth);
	REGISTER_METHOD(SetPlayerHealth);
	REGISTER_METHOD(GetPlayerMaxHealth);
	REGISTER_METHOD(SetPlayerMaxHealth);

	REGISTER_METHOD(GetActorInfoByChannelId);
	REGISTER_METHOD(GetActorInfoById);

	REGISTER_METHOD(RegisterActorClass);
	REGISTER_METHOD(CreateActor);
	REGISTER_METHOD(RemoveActor);

	REGISTER_METHOD(GetClientActorId);
}

bool CActorSystem::IsMonoActor(const char *actorClassName, EMonoActorType type)
{
	for each(auto classPair in m_monoActorClasses)
	{
		if(!strcmp(classPair.first, actorClassName))
			return (type == EMonoActorType_Any || classPair.second ? type == EMonoActorType_Native : EMonoActorType_Managed);
	}

	return false;
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

void CActorSystem::RegisterActorClass(mono::string name, bool isNative)
{
	const char *className = ToCryString(name);

	if(!isNative)
		gEnv->pGameFramework->RegisterFactory(className, (CActor *)0, false, (CActor *)0);

	m_monoActorClasses.insert(TActorClasses::value_type(className, isNative));
}

SMonoActorInfo CActorSystem::CreateActor(mono::object actor, int channelId, mono::string name, mono::string className, Vec3 pos, Quat rot, Vec3 scale)
{
	const char *sClassName = ToCryString(className);

	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->CreateActor(channelId, ToCryString(name), sClassName, pos, rot, scale))
	{
		if(IsMonoActor(sClassName, EMonoActorType_Managed))
			static_cast<CActor *>(pActor)->SetScript(*actor);

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