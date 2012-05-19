/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// The entity manager handles spawning, removing and storing of mono
// entities.
//////////////////////////////////////////////////////////////////////////
// 21/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "MonoCommon.h"
#include <IMonoArray.h>

#include <IMonoEntityManager.h>
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

	SEntitySpawnParams Convert()
	{
		SEntitySpawnParams params;

		params.sName = ToCryString(sName);
		params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(ToCryString(sClass));

		params.vPosition = pos;
		params.qRotation = Quat((Ang3)rot);
		params.vScale = scale;

		params.nFlags = flags;

		return params;
	}
};

struct EntityRegisterParams
{
	mono::string Name;
	mono::string Category;

	mono::string EditorHelper;
	mono::string EditorIcon;

	EEntityClassFlags Flags;
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
	SMonoEntityInfo() { }

	SMonoEntityInfo(IEntity *pEnt)
		: pEntity(pEnt)
	{
		id = mono::entityId(pEnt->GetId());
	}

	SMonoEntityInfo(IEntity *pEnt, EntityId entId)
		: pEntity(pEnt)
	{
		id = mono::entityId(entId);
	}

	IEntity *pEntity;
	mono::entityId id;
};

class CEntityManager 
	: public IMonoEntityManager
	, public IMonoScriptBind
	, public IEntitySystemSink
{

public:
	CEntityManager();
	~CEntityManager();

	// IEntitySystemSink
	virtual bool OnBeforeSpawn( SEntitySpawnParams &params );
	virtual void OnSpawn( IEntity *pEntity,SEntitySpawnParams &params );
	virtual bool OnRemove( IEntity *pEntity );
	virtual void OnReused( IEntity *pEntity, SEntitySpawnParams &params ) {}
	virtual void OnEvent( IEntity *pEntity, SEntityEvent &event ) {}
	// ~IEntitySystemSink

	// IMonoEntityManager
	virtual IMonoClass *GetScript(EntityId entityId, bool returnBackIfInvalid = false) override;
	// ~IMonoEntityManager

	std::shared_ptr<CEntity> GetMonoEntity(EntityId entityId);
	std::shared_ptr<CEntity> GetMonoEntity(EntityGUID entityGUID);
	bool IsMonoEntity(const char *entityClassName);

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "EntityBase"; }
	// ~IMonoScriptBind

	// ScriptBinds
	static SMonoEntityInfo SpawnEntity(EntitySpawnParams, bool);
	static void RemoveEntity(EntityId);

	static IEntity *GetEntity(EntityId id);

	static bool RegisterEntityClass(EntityRegisterParams, mono::array);

	static EntityId FindEntity(mono::string);
	static mono::array GetEntitiesByClass(mono::string);

	static bool EntityExists(EntityId);

	static mono::string GetPropertyValue(IEntity *pEnt, mono::string);

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

	static void Physicalize(IEntity *pEnt, MonoPhysicalizationParams);
	
	static void Sleep(IEntity *pEnt, bool);

	static void BreakIntoPieces(IEntity *pEnt, int, int, IBreakableManager::BreakageParams);

	static void CreateGameObjectForEntity(IEntity *pEnt);
	static void BindGameObjectToNetwork(IEntity *pEnt);

	static mono::string GetStaticObjectFilePath(IEntity *pEnt, int);

	static void AddImpulse(IEntity *pEnt, ActionImpulse);

	static Vec3 GetVelocity(IEntity *pEnt);
	static void SetVelocity(IEntity *pEnt, Vec3);

	static void SetWorldTM(IEntity *pEnt, Matrix34 tm);
	static Matrix34 GetWorldTM(IEntity *pEnt);
	static void SetLocalTM(IEntity *pEnt, Matrix34 tm);
	static Matrix34 GetLocalTM(IEntity *pEnt);

	static mono::string GetName(IEntity *pEnt);
	static void SetName(IEntity *pEnt, mono::string name);

	static EEntityFlags GetFlags(IEntity *pEnt);
	static void SetFlags(IEntity *pEnt, EEntityFlags flags);
	/// End direct entity calls

	// ~ScriptBinds
	
	typedef std::vector<std::shared_ptr<CEntity>> TMonoEntities;
	static TMonoEntities m_monoEntities;

	static std::vector<const char *> m_monoEntityClasses;

	int m_refs;
};

struct ActionImpulse
{
	Vec3 impulse;
	Vec3 angImpulse;	// optional
	Vec3 point; // point of application, in world CS, optional 
	int partid;	// receiver part identifier
	int ipart; // alternatively, part index can be used
	int iApplyTime; // 0-apply immediately, 1-apply before the next time step, 2-apply after the next time step
	int iSource; // reserved for internal use
};

struct MonoPhysicalizationParams
{
	struct MonoPlayerDynamics
	{
		float kInertia;
		float kInertiaAccel;
		float kAirControl;
		float kAirResistance;
		Vec3 gravity;
		float nodSpeed;
		bool swimming;
		float mass;
		int surface_idx;
		float minSlideAngle;
		float maxClimbAngle;
		float maxJumpAngle;
		float minFallAngle;
		float maxVelGround;
		float timeImpulseRecover;
		int collTypes;
		EntityId livingEntToIgnore;
		bool active;
		int iRequestedTime;
	};

	struct MonoPlayerDimensions
	{
	    float heightPivot;
	    float heightEye;
	    Vec3 sizeCollider;
	    float heightCollider;
	    float headRadius;
	    float heightHead;
	    Vec3 dirUnproj;
	    float maxUnproj;
	    bool useCapsule;
	};

	int type;

	int slot;

	float density;
	float mass;
	
	float stiffnessScale;

	MonoPlayerDimensions playerDimensions;
	MonoPlayerDynamics playerDynamics;
};

#endif //__ENTITY_MANAGER_H__