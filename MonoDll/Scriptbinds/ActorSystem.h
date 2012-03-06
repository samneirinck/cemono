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

	static float GetPlayerHealth(EntityId);
	static void SetPlayerHealth(EntityId, float);
	static float GetPlayerMaxHealth(EntityId);
	static void SetPlayerMaxHealth(EntityId, float);

	static EntityId GetEntityIdForChannelId(uint16 channelId);
	static void RemoveActor(EntityId id);

	static void RegisterActorClass(mono::string, bool);
};

#endif //__SCRIPTBIND_ACTORSYSTEM_H__