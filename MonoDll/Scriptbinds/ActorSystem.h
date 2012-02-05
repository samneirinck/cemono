/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// ActorSystem scriptbind; used to get and set actor C++ properties.
//////////////////////////////////////////////////////////////////////////
// 06/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_ACTORSYSTEM_H__
#define __SCRIPTBIND_ACTORSYSTEM_H__

#include <IMonoScriptBind.h>

#include <MonoCommon.h>

class CScriptBind_ActorSystem : public IMonoScriptBind
{
public:
	CScriptBind_ActorSystem();
	~CScriptBind_ActorSystem() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "ActorSystem"; }
	// ~IMonoScriptBind

	MonoMethod(float, GetPlayerHealth, EntityId);
	MonoMethod(void, SetPlayerHealth, EntityId, float);
	MonoMethod(float, GetPlayerMaxHealth, EntityId);
	MonoMethod(void, SetPlayerMaxHealth, EntityId, float);

	MonoMethod(void, RegisterActorClass, mono::string, bool);
};

#endif //__SCRIPTBIND_ACTORSYSTEM_H__