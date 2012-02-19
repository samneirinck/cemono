/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// StaticEntity scriptbind.
//////////////////////////////////////////////////////////////////////////
// 23/01/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_STATICENTITY_H__
#define __SCRIPTBIND_STATICENTITY_H__

#include <IMonoScriptBind.h>

#include <MonoCommon.h>

struct MovementRequest;
struct MonoPhysicalizationParams;

class CScriptBind_StaticEntity : public IMonoScriptBind
{
public:
	CScriptBind_StaticEntity();
	virtual ~CScriptBind_StaticEntity() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "StaticEntity"; }
	// ~IMonoScriptBind

	static mono::string GetPropertyValue(EntityId, mono::string);
	static void SetPropertyValue(EntityId, mono::string, mono::string);

	static void SetWorldPos(EntityId, Vec3);
	static Vec3 GetWorldPos(EntityId);
	static void SetWorldAngles(EntityId, Vec3);
	static Vec3 GetWorldAngles(EntityId);

	static void LoadObject(EntityId, mono::string, int);
	static void LoadCharacter(EntityId, mono::string, int);

	static void Physicalize(EntityId, MonoPhysicalizationParams);

	static void CreateGameObjectForEntity(EntityId);
	static void BindGameObjectToNetwork(EntityId);

	static mono::string GetStaticObjectFilePath(EntityId, int);

	static void AddMovement(EntityId, MovementRequest&);
	static Vec3 GetVelocity(EntityId);
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

#endif //__SCRIPTBIND_STATICENTITY_H__