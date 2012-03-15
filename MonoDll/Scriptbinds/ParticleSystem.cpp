#include "StdAfx.h"
#include "ParticleSystem.h"

#include <IParticles.h>

IParticleManager *CScriptbind_ParticleSystem::m_pParticleManager = NULL;
CScriptbind_ParticleSystem::TParticleEffectsMap CScriptbind_ParticleSystem::m_particleEffects = CScriptbind_ParticleSystem::TParticleEffectsMap();

CScriptbind_ParticleSystem::CScriptbind_ParticleSystem()
{
	m_pParticleManager = gEnv->p3DEngine->GetParticleManager();

	REGISTER_METHOD(FindEffect);
	REGISTER_METHOD(Spawn);
}

int CScriptbind_ParticleSystem::FindEffect(mono::string effectName, bool bLoadResources)
{
	if(IParticleEffect *pEffect = m_pParticleManager->FindEffect(ToCryString(effectName), "CScriptbind_ParticleSystem::FindEffect", bLoadResources))
	{
		int index = m_particleEffects.size();
		m_particleEffects.insert(TParticleEffectsMap::value_type(index, pEffect));

		return index;
	}

	return -1;
}

void CScriptbind_ParticleSystem::Spawn(int id, bool independent, Vec3 pos, Vec3 dir, float scale)
{
	m_particleEffects[id]->Spawn(independent, IParticleEffect::ParticleLoc(pos, dir, scale));
}