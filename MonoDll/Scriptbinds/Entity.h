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
	static void AddMovement(IEntity *pEnt, MovementRequest&);

	static Vec3 GetVelocity(IEntity *pEnt);
	static void SetVelocity(IEntity *pEnt, Vec3);

	static void SetWorldTM(IEntity *pEnt, Matrix34 tm);
	static Matrix34 GetWorldTM(IEntity *pEnt);
	static void SetLocalTM(IEntity *pEnt, Matrix34 tm);
	static Matrix34 GetLocalTM(IEntity *pEnt);

	static mono::string GetMaterial(IEntity *pEnt);
	static void SetMaterial(IEntity *pEnt, mono::string material);

	static mono::string GetName(IEntity *pEnt);
	static void SetName(IEntity *pEnt, mono::string name);

	static EEntityFlags GetFlags(IEntity *pEnt);
	static void SetFlags(IEntity *pEnt, EEntityFlags flags);
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