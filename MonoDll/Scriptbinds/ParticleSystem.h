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
	virtual const char *GetClassName() override { return "ParticleEffect"; }
	// ~IMonoScriptBind

	// Externals
	static IParticleEffect *FindEffect(mono::string effectName, bool bLoadResources = true);

	static void Spawn(IParticleEffect *pEffect, bool independent, Vec3 pos, Vec3 dir, float scale);
	// ~Externals

	static IParticleManager *m_pParticleManager;
};

#endif //__SCRIPTBIND_PARTICLE_SYSTEM__