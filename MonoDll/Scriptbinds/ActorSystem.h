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

#include <IActorSystem.h>

struct SMonoActorInfo
{
	SMonoActorInfo() : id(0) {}

	IEntity *pEntity;
	IActor *pActor;
	EntityId id;
};

class CActorSystem : public IMonoScriptBind
{
public:
	CActorSystem();
	~CActorSystem() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "Actor"; }
	// ~IMonoScriptBind

	static float GetPlayerHealth(IActor *pActor);
	static void SetPlayerHealth(IActor *pActor, float);
	static float GetPlayerMaxHealth(IActor *pActor);
	static void SetPlayerMaxHealth(IActor *pActor, float);

	static EntityId GetEntityIdForChannelId(uint16 channelId);

	static mono::object CreateActor(int channelId, mono::string name, mono::string className, Vec3 pos, Vec3 angles, Vec3 scale);
	static void RemoveActor(EntityId id);

	static EntityId GetClientActor();

	static void RegisterActorClass(mono::string, bool);
};

#endif //__SCRIPTBIND_ACTORSYSTEM_H__