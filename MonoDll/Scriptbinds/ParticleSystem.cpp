#include "StdAfx.h"
#include "ParticleSystem.h"

#include <IParticles.h>

IParticleManager *CScriptbind_ParticleSystem::m_pParticleManager = nullptr;

CScriptbind_ParticleSystem::CScriptbind_ParticleSystem()
{
	m_pParticleManager = gEnv->p3DEngine->GetParticleManager();

	REGISTER_METHOD(FindEffect);
	REGISTER_METHOD(Spawn);
	REGISTER_METHOD(Remove);
	REGISTER_METHOD(LoadResources);

	REGISTER_METHOD(GetParticleEmitterSpawnParams);
	REGISTER_METHOD(SetParticleEmitterSpawnParams);
	REGISTER_METHOD(GetParticleEmitterEffect);
}

IParticleEffect *CScriptbind_ParticleSystem::FindEffect(mono::string effectName, bool bLoadResources)
{
	return m_pParticleManager->FindEffect(ToCryString(effectName), "CScriptbind_ParticleSystem::FindEffect", bLoadResources);
}

IParticleEmitter *CScriptbind_ParticleSystem::Spawn(IParticleEffect *pEffect, bool independent, Vec3 pos, Vec3 dir, float scale)
{
	return pEffect->Spawn(independent, IParticleEffect::ParticleLoc(pos, dir, scale));
}

void CScriptbind_ParticleSystem::Remove(IParticleEffect *pEffect)
{
	m_pParticleManager->DeleteEffect(pEffect);
}

void CScriptbind_ParticleSystem::LoadResources(IParticleEffect *pEffect)
{
	pEffect->LoadResources();
}

SpawnParams CScriptbind_ParticleSystem::GetParticleEmitterSpawnParams(IParticleEmitter *pEmitter)
{
	SpawnParams params;
	pEmitter->GetSpawnParams(params);

	return params;
}

void CScriptbind_ParticleSystem::SetParticleEmitterSpawnParams(IParticleEmitter *pEmitter, SpawnParams &spawnParams)
{
	pEmitter->SetSpawnParams(spawnParams);
}

IParticleEffect *CScriptbind_ParticleSystem::GetParticleEmitterEffect(IParticleEmitter *pEmitter)
{
	return const_cast<IParticleEffect *>(pEmitter->GetEffect());
}