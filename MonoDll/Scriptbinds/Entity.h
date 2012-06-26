/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// The entity manager handles spawning, removing and storing of mono
// entities.
//////////////////////////////////////////////////////////////////////////
// 21/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_ENTITY_H__
#define __SCRIPTBIND_ENTITY_H__

#include "MonoCommon.h"
#include <IMonoArray.h>

#include <IMonoScriptBind.h>

#include <mono\mini\jit.h>

#include <IEntitySystem.h>
#include <IBreakableManager.h>
#include <IAnimatedCharacter.h>

struct IMonoScript;
class CEntity;

struct MovementRequest;
struct MonoPhysicalizationParams;
struct ActionImpulse;

struct MovementRequest
{
	ECharacterMoveType type;

	Vec3 velocity;
};

struct EntitySpawnParams
{
	mono::string sName;
	mono::string sClass;

	Vec3 pos;
	Vec3 rot;
	Vec3 scale;

	EEntityFlags flags;
};

struct SEntityRegistrationParams
{
	mono::string Name;
	mono::string Category;

	mono::string EditorHelper;
	mono::string EditorIcon;

	EEntityClassFlags Flags;

	mono::object Properties;
};

struct SMonoEntityProperty
{
	mono::string name;
	mono::string description;
	mono::string editType;

	IEntityPropertyHandler::EPropertyType type;
	uint32 flags;

	IEntityPropertyHandler::SPropertyInfo::SLimits limits;
};

struct SMonoEntityInfo
{
	SMonoEntityInfo(IEntity *pEnt)
		: pEntity(pEnt)
	{
		if(pEnt != nullptr)
			id = pEnt->GetId();
		else
			id = 0;
	}

	SMonoEntityInfo(IEntity *pEnt, EntityId entId)
		: pEntity(pEnt)
		, id(entId)
	{
	}

	IEntity *pEntity;
	EntityId id;
};

class CScriptbind_Entity 
	: public IMonoScriptBind
	, public IEntitySystemSink
{

public:
	CScriptbind_Entity();
	~CScriptbind_Entity() {}

	// IEntitySystemSink
	virtual bool OnBeforeSpawn(SEntitySpawnParams &params) { return true; }
	virtual void OnSpawn(IEntity *pEntity,SEntitySpawnParams &params);
	virtual bool OnRemove(IEntity *pEntity);
	virtual void OnReused( IEntity *pEntity, SEntitySpawnParams &params) {}
	virtual void OnEvent(IEntity *pEntity, SEntityEvent &event) {}
	// ~IEntitySystemSink

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "EntityBase"; }
	// ~IMonoScriptBind

	bool IsMonoEntity(const char *className);

	// Scriptbinds
	static bool SpawnEntity(EntitySpawnParams, bool, SMonoEntityInfo &entityInfo);
	static void RemoveEntity(EntityId);

	static IEntity *GetEntity(EntityId id);

	static bool RegisterEntityClass(SEntityRegistrationParams);

	static EntityId FindEntity(mono::string);
	static mono::object GetEntitiesByClass(mono::string);
	static mono::object GetEntitiesInBox(AABB bbox, int objTypes);

	static void SetWorldPos(IEntity *pEnt, Vec3);
	static Vec3 GetWorldPos(IEntity *pEnt);
	static void SetPos(IEntity *pEnt, Vec3);
	static Vec3 GetPos(IEntity *pEnt);

	static void SetWorldRotation(IEntity *pEnt, Quat);
	static Quat GetWorldRotation(IEntity *pEnt);
	static void SetRotation(IEntity *pEnt, Quat);
	static Quat GetRotation(IEntity *pEnt);

	static AABB GetBoundingBox(IEntity *pEnt);
	static AABB GetWorldBoundingBox(IEntity *pEnt);

	static void LoadObject(IEntity *pEnt, mono::string, int);
	static void LoadCharacter(IEntity *pEnt, mono::string, int);

	static EEntitySlotFlags GetSlotFlags(IEntity *pEnt, int);
	static void SetSlotFlags(IEntity *pEnt, int, EEntitySlotFlags);

	static void BreakIntoPieces(IEntity *pEnt, int, int, IBreakableManager::BreakageParams);

	static mono::string GetStaticObjectFilePath(IEntity *pEnt, int);

	static void SetWorldTM(IEntity *pEnt, Matrix34 tm);
	static Matrix34 GetWorldTM(IEntity *pEnt);
	static void SetLocalTM(IEntity *pEnt, Matrix34 tm);
	static Matrix34 GetLocalTM(IEntity *pEnt);

	static mono::string GetName(IEntity *pEnt);
	static void SetName(IEntity *pEnt, mono::string name);

	static EEntityFlags GetFlags(IEntity *pEnt);
	static void SetFlags(IEntity *pEnt, EEntityFlags flags);

	static int GetAttachmentCount(IEntity *pEnt);
	static IMaterial *GetAttachmentMaterialByIndex(IEntity *pEnt, int index);
	static void SetAttachmentMaterialByIndex(IEntity *pEnt, int index, IMaterial *pMaterial);

	static IMaterial *GetAttachmentMaterial(IEntity *pEnt, mono::string name);
	static void SetAttachmentMaterial(IEntity *pEnt, mono::string attachmentName, IMaterial *pMaterial);

	static void SetVisionParams(IEntity *pEntity, float r, float g, float b, float a);
	static void SetHUDSilhouettesParams(IEntity *pEntity, float r, float g, float b, float a);
	// ~Scriptbinds

	static std::vector<const char *> m_monoEntityClasses;
};

#endif //__SCRIPTBIND_ENTITY_H__