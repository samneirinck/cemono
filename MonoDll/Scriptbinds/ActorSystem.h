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

	SMonoActorInfo(IActor *pIActor)
		: pActor(pIActor)
	{
		pEntity = pIActor->GetEntity();
		id = pEntity->GetId();
	}

	SMonoActorInfo(EntityId entId, int chId)
		: id(entId)
		, channelId(chId) {}

	IEntity *pEntity;
	IActor *pActor;
	EntityId id;
	int channelId;
};

enum EMonoActorType
{
	EMonoActorType_Managed,
	EMonoActorType_Native,
	EMonoActorType_Any
};

class CActorSystem 
	: public IMonoScriptBind
	, public IEntitySystemSink
{
public:
	CActorSystem();
	~CActorSystem() {}

	// IEntitySystemSink
	virtual bool OnBeforeSpawn(SEntitySpawnParams &params) { return true; }
	virtual void OnSpawn(IEntity *pEntity,SEntitySpawnParams &params) {}
	virtual bool OnRemove(IEntity *pEntity) { return true; }
	virtual void OnReused( IEntity *pEntity, SEntitySpawnParams &params) {}
	virtual void OnEvent(IEntity *pEntity, SEntityEvent &event) {}
	// ~IEntitySystemSink

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "NativeActorMethods"; }
	// ~IMonoScriptBind

	static bool IsMonoActor(const char *actorClassName, EMonoActorType type = EMonoActorType_Any);

	// externals
	static float GetPlayerHealth(IActor *pActor);
	static void SetPlayerHealth(IActor *pActor, float);
	static float GetPlayerMaxHealth(IActor *pActor);
	static void SetPlayerMaxHealth(IActor *pActor, float);

	static SMonoActorInfo GetActorInfoByChannelId(uint16 channelId);
	static SMonoActorInfo GetActorInfoById(EntityId id);

	static void RegisterActorClass(mono::string name, bool isNative);
	static SMonoActorInfo CreateActor(mono::object actor, int channelId, mono::string name, mono::string className, Vec3 pos, Quat rot, Vec3 scale);
	static void RemoveActor(EntityId id);

	static EntityId GetClientActorId();

	typedef std::map<const char *, bool> TActorClasses;
	static TActorClasses m_monoActorClasses;
};

#endif //__SCRIPTBIND_ACTORSYSTEM_H__