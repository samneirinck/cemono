/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// StaticEntity scriptbind.
//////////////////////////////////////////////////////////////////////////
// 23/01/2013 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __LOGGING_BINDING_H__
#define __LOGGING_BINDING_H__

#include <IMonoAutoRegScriptBind.h>

#include <MonoCommon.h>

struct MovementRequest;
struct MonoPhysicalizationParams;

class CScriptBind_StaticEntity : public IMonoAutoRegScriptBind
{
public:
	CScriptBind_StaticEntity();
	virtual ~CScriptBind_StaticEntity() {}

protected:
	// IMonoScriptBind
	virtual void Release() { delete this; }

	virtual const char* GetClassName() { return "StaticEntity"; }
	// ~IMonoScriptBind

	MonoMethod(mono::string, GetPropertyValue, EntityId, mono::string);
	MonoMethod(void, SetPropertyValue, EntityId, mono::string, mono::string);

	MonoMethod(void, SetWorldPos, EntityId, Vec3);
	MonoMethod(Vec3, GetWorldPos, EntityId);
	MonoMethod(void, SetWorldAngles, EntityId, Vec3);
	MonoMethod(Vec3, GetWorldAngles, EntityId);

	MonoMethod(void, LoadObject, EntityId, mono::string, int);
	MonoMethod(void, LoadCharacter, EntityId, mono::string, int);

	MonoMethod(void, Physicalize, EntityId, MonoPhysicalizationParams);

	MonoMethod(void, CreateGameObjectForEntity, EntityId);
	MonoMethod(void, BindGameObjectToNetwork, EntityId);

	MonoMethod(mono::string, GetStaticObjectFilePath, EntityId, int);

	MonoMethod(void, AddMovement, EntityId, MovementRequest&);
	MonoMethod(Vec3, GetVelocity, EntityId);
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

#endif //__LOGGING_BINDING_H__