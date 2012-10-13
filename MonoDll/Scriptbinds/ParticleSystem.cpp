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
}

IParticleEffect *CScriptbind_ParticleSystem::FindEffect(mono::string effectName, bool bLoadResources)
{
	return m_pParticleManager->FindEffect(ToCryString(effectName), "CScriptbind_ParticleSystem::FindEffect", bLoadResources);
}

void CScriptbind_ParticleSystem::Spawn(IParticleEffect *pEffect, bool independent, Vec3 pos, Vec3 dir, float scale)
{
	pEffect->Spawn(independent, IParticleEffect::ParticleLoc(pos, dir, scale));
}

void CScriptbind_ParticleSystem::Remove(IParticleEffect *pEffect)
{
	m_pParticleManager->DeleteEffect(pEffect);
}

void CScriptbind_ParticleSystem::LoadResources(IParticleEffect *pEffect)
{
	pEffect->LoadResources();
}