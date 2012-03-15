#include "StdAfx.h"
#include "ParticleSystem.h"

#include <IParticles.h>

IParticleManager *CScriptbind_ParticleSystem::m_pParticleManager = NULL;
CScriptbind_ParticleSystem::TParticleEffectsMap CScriptbind_ParticleSystem::m_particleEffects = CScriptbind_ParticleSystem::TParticleEffectsMap();

CScriptbind_ParticleSystem::CScriptbind_ParticleSystem()
{
	m_pParticleManager = gEnv->p3DEngine->GetParticleManager();

	REGISTER_METHOD(FindEffect);
}

int CScriptbind_ParticleSystem::FindEffect(mono::string effectName, bool bLoadResources)
{
	if(IParticleEffect *pEffect = m_pParticleManager->FindEffect(ToCryString(effectName), "CScriptbind_ParticleSystem::FindEffect", bLoadResources))
	{
		int index = m_particleEffects.size();
		m_particleEffects.insert(TParticleEffectsMap::value_type(pEffect, index));

		return index;
	}

	return -1;
}
