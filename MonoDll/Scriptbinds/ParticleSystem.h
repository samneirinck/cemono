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

class CScriptbind_ParticleSystem : public IMonoScriptBind
{
public:
	CScriptbind_ParticleSystem();
	~CScriptbind_ParticleSystem() {}

protected:

	// Externals
	// ~Externals

	// IMonoScriptBind
	virtual const char *GetClassName() override { return "ParticleSystem"; }
	// ~IMonoScriptBind
};

#endif //__SCRIPTBIND_PARTICLE_SYSTEM__