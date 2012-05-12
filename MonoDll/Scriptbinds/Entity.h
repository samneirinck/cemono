/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Entity scriptbind.
//////////////////////////////////////////////////////////////////////////
// 23/01/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_ENTITY_H__
#define __SCRIPTBIND_ENTITY_H__

#include <IMonoScriptBind.h>

#include <MonoCommon.h>

#include <IEntitySystem.h>
#include <IBreakableManager.h>

struct MovementRequest;
struct MonoPhysicalizationParams;
struct ActionImpulse;

class CScriptbind_Entity : public IMonoScriptBind
{
public:
	CScriptbind_Entity();
	virtual ~CScriptbind_Entity() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "EntityBase"; }
	// ~IMonoScriptBind

	static mono::string GetPropertyValue(EntityId, mono::string);
	static void SetPropertyValue(EntityId, mono::string, mono::string);

	static void SetWorldPos(EntityId, Vec3);
	static Vec3 GetWorldPos(EntityId);
	static void SetRotation(EntityId, Quat);
	static Quat GetRotation(EntityId);

	static AABB GetBoundingBox(EntityId, int slot);

	static void LoadObject(EntityId, mono::string, int);
	static void LoadCharacter(EntityId, mono::string, int);

	static EEntitySlotFlags GetSlotFlags(EntityId, int);
	static void SetSlotFlags(EntityId, int, EEntitySlotFlags);

	static void Physicalize(EntityId, MonoPhysicalizationParams);
	
	static void Sleep(EntityId, bool);

	static void BreakIntoPieces(EntityId, int, int, IBreakableManager::BreakageParams);

	static void CreateGameObjectForEntity(EntityId);
	static void BindGameObjectToNetwork(EntityId);

	static mono::string GetStaticObjectFilePath(EntityId, int);

	static void AddImpulse(EntityId, ActionImpulse);
	static void AddMovement(EntityId, MovementRequest&);

	static Vec3 GetVelocity(EntityId);
	static void SetVelocity(EntityId, Vec3);

	static void SetWorldTM(EntityId id, Matrix34 tm);
	static Matrix34 GetWorldTM(EntityId id);
	static void SetLocalTM(EntityId id, Matrix34 tm);
	static Matrix34 GetLocalTM(EntityId id);

	static mono::string GetMaterial(EntityId id);
	static void SetMaterial(EntityId id, mono::string material);

	static mono::string GetName(EntityId id);
	static void SetName(EntityId id, mono::string name);

	static EEntityFlags GetFlags(EntityId id);
	static void SetFlags(EntityId id, EEntityFlags flags);
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

#endif //__SCRIPTBIND_ENTITY_H__