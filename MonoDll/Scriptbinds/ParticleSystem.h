///////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// CryENGINE particle system scriptbind
//////////////////////////////////////////////////////////////////////////
// 14/03/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/

#ifndef __SCRIPTBIND_PARTICLE_SYSTEM__
#define __SCRIPTBIND_PARTICLE_SYSTEM__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

struct IParticleManager;
struct IParticleEffect;

class CScriptbind_ParticleSystem : public IMonoScriptBind
{
public:
	CScriptbind_ParticleSystem();
	~CScriptbind_ParticleSystem() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "NativeParticleEffectMethods"; }
	virtual const char *GetNamespace() override { return "CryEngine.Engine.Particles.Native"; }
	// ~IMonoScriptBind

	// Externals
	static IParticleEffect *FindEffect(mono::string effectName, bool bLoadResources = true);

	static IParticleEmitter *Spawn(IParticleEffect *pEffect, bool independent, Vec3 pos, Vec3 dir, float scale);
	static void Remove(IParticleEffect *pEffect);
	static void LoadResources(IParticleEffect *pEffect);

	static SpawnParams GetParticleEmitterSpawnParams(IParticleEmitter *pEmitter);
	static void SetParticleEmitterSpawnParams(IParticleEmitter *pEmitter, SpawnParams &spawnParams);
	static IParticleEffect *GetParticleEmitterEffect(IParticleEmitter *pEmitter);
	// ~Externals

	static IParticleManager *m_pParticleManager;
};

#endif //__SCRIPTBIND_PARTICLE_SYSTEM__