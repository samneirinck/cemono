#include "StdAfx.h"
#include "ParticleSystem.h"

#include <IParticles.h>

IParticleManager *CScriptbind_ParticleSystem::m_pParticleManager = NULL;

CScriptbind_ParticleSystem::CScriptbind_ParticleSystem()
{
	m_pParticleManager = gEnv->p3DEngine->GetParticleManager();

	REGISTER_METHOD(FindEffect);
	REGISTER_METHOD(Spawn);
}

IParticleEffect *CScriptbind_ParticleSystem::FindEffect(mono::string effectName, bool bLoadResources)
{
	return m_pParticleManager->FindEffect(ToCryString(effectName), "CScriptbind_ParticleSystem::FindEffect", bLoadResources);
}

void CScriptbind_ParticleSystem::Spawn(IParticleEffect *pEffect, bool independent, Vec3 pos, Vec3 dir, float scale)
{
	pEffect->Spawn(independent, IParticleEffect::ParticleLoc(pos, dir, scale));
}