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

struct MonoRayHit
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

class CScriptbind_PhysicalWorld : public IMonoScriptBind
{
public:
	CScriptbind_PhysicalWorld();
	~CScriptbind_PhysicalWorld() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "PhysicalWorld"; }
	// ~IMonoScriptBind

	static int RayWorldIntersection(Vec3, Vec3, int, unsigned int, MonoRayHit &, int, mono::array);
};

#endif //__SCRIPTBIND_PHYSICALWORLD__