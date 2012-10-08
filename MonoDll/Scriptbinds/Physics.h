/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// PhysicalWorld scriptbind to implement necessary physical world methods,
// i.e. RayWorldIntersection.
//////////////////////////////////////////////////////////////////////////
// 13/01/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_PHYSICALWORLD__
#define __SCRIPTBIND_PHYSICALWORLD__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

struct SMonoRayHit
{
	float dist;
	int colliderId;
	int ipart;
	int partid;
	short surface_idx;
	short idmatOrg;	// original material index, not mapped with material mapping
	int foreignIdx;
	int iNode; // BV tree node that had the intersection; can be used for "warm start" next time
	Vec3 pt;
	Vec3 n;	// surface normal
	int bTerrain;	// global terrain hit
	int iPrim; // hit triangle index
};

struct SMonoPhysicalizeParams
{
	int type;

	int flagsOR;
	int flagsAND;

	int slot;

	float density;
	float mass;

	int lod;

	EntityId attachToEntity;

	int attachToPart;

	float stiffnessScale;

	bool copyJointVelocities;

	pe_player_dimensions playerDim;
	pe_player_dynamics playerDyn;
};

class CScriptbind_Physics : public IMonoScriptBind
{
public:
	CScriptbind_Physics();
	~CScriptbind_Physics() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "NativePhysicsMethods"; }
	// ~IMonoScriptBind

	static IPhysicalEntity *GetPhysicalEntity(IEntity *pEntity);

	static void Physicalize(IEntity *pEntity, SMonoPhysicalizeParams params);

	static void Sleep(IEntity *pEntity, bool sleep);

	static void AddImpulse(IEntity *pEntity, pe_action_impulse impulse);

	static Vec3 GetVelocity(IEntity *pEntity);
	static void SetVelocity(IEntity *pEntity, Vec3 vel);

	static int RayWorldIntersection(Vec3, Vec3, int, unsigned int, SMonoRayHit &, int, mono::object);

	static pe_action_impulse GetImpulseStruct()
	{
		pe_action_impulse impulse;
		return impulse;
	}

	static pe_player_dimensions GetPlayerDimensionsStruct()
	{
		pe_player_dimensions pd;
		return pd;
	}

	static pe_player_dynamics GetPlayerDynamicsStruct()
	{
		pe_player_dynamics pd;
		return pd;
	}
};

#endif //__SCRIPTBIND_PHYSICALWORLD__