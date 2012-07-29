/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// 3DEngine scriptbind
//////////////////////////////////////////////////////////////////////////
// 13/01/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_3DENGINE__
#define __SCRIPTBIND_3DENGINE__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

#include "Renderer.h"

struct MonoLightParams;

class CScriptbind_3DEngine : public IMonoScriptBind
{
public:
	CScriptbind_3DEngine();
	~CScriptbind_3DEngine() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "Native3DEngineMethods"; }
	// ~IMonoScriptBind

	static float GetTerrainElevation(float x, float y, bool includeVoxels);
	static float GetTerrainZ(int x, int y);

	static int GetTerrainSize();
	static int GetTerrainSectorSize();
	static int GetTerrainUnitSize();

	static void Asplode(Vec3 pos, Vec3 dir, float minRadius, float maxRadius, float pressure);

	static void SetTimeOfDay(float hour, bool forceUpdate);
	static float GetTimeOfDay();

	static ITimeOfDay::SAdvancedInfo GetTimeOfDayAdvancedInfo();
	static void SetTimeOfDayAdvancedInfo(ITimeOfDay::SAdvancedInfo advancedInfo);

	static void SetTimeOfDayVariableValue(ITimeOfDay::ETimeOfDayParamID id, float value);
	static void SetTimeOfDayVariableValueColor(ITimeOfDay::ETimeOfDayParamID id, Vec3 value);

	static ILightSource *CreateLightSource();
	static void SetLightSourceParams(ILightSource *pLightSource, MonoLightParams params);
	static MonoLightParams GetLightSourceParams(ILightSource *pLightSource);

	static void SetLightSourceMatrix(ILightSource *pLightSource, Matrix34 matrix);
	static Matrix34 GetLightSourceMatrix(ILightSource *pLightSource);
};

struct MonoLightParams
{
	int lightStyle;
	Vec3 origin;
	float lightFrustumAngle;
	float radius;
	uint32 flags;

	float coronaScale;
	float coronaDistSizeFactor;
	float coronaDistIntensityFactor;

	mono::string specularCubemap;
	mono::string diffuseCubemap;

	ColorF diffuseColor;
	float specularMultiplier;
	int postEffect;

	float hdrDynamic;

	float projectNearPlane;
};

#endif //__SCRIPTBIND_3DENGINE__