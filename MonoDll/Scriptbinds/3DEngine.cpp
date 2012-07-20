#include "StdAfx.h"
#include "3DEngine.h"

CScriptbind_3DEngine::CScriptbind_3DEngine()
{
	REGISTER_METHOD(GetTerrainElevation);
	REGISTER_METHOD(GetTerrainZ);

	REGISTER_METHOD(GetTerrainSize);
	REGISTER_METHOD(GetTerrainSectorSize);
	REGISTER_METHOD(GetTerrainUnitSize);

	REGISTER_METHOD(SetTimeOfDay);
	REGISTER_METHOD(GetTimeOfDay);

	REGISTER_METHOD(GetTimeOfDayAdvancedInfo);
	REGISTER_METHOD(SetTimeOfDayAdvancedInfo);

	REGISTER_METHOD(SetTimeOfDayVariableValue);
	REGISTER_METHOD(SetTimeOfDayVariableValueColor);

	REGISTER_METHOD(CreateLightSource);
	REGISTER_METHOD(SetLightSourceParams);
	REGISTER_METHOD(GetLightSourceParams);
}

ILightSource *CScriptbind_3DEngine::CreateLightSource()
{
	ILightSource *pLightSource = gEnv->p3DEngine->CreateLightSource();

	return pLightSource;
}

void CScriptbind_3DEngine::SetLightSourceParams(ILightSource *pLightSource, MonoLightParams params)
{
	CDLight light;
	light.m_nLightStyle = params.lightStyle;
	light.m_Origin = params.origin;
	light.m_fLightFrustumAngle = params.lightFrustumAngle;
	light.m_fRadius = params.radius;
	light.m_Flags = params.flags;

	light.m_fCoronaScale = params.coronaScale;
	light.m_fCoronaDistSizeFactor = params.coronaDistSizeFactor;
	light.m_fCoronaDistIntensityFactor = params.coronaDistIntensityFactor;

	if(params.specularCubemap)
		light.SetSpecularCubemap(gEnv->pRenderer->EF_LoadTexture(ToCryString(params.specularCubemap)));
	if(params.diffuseCubemap)
		light.SetDiffuseCubemap(gEnv->pRenderer->EF_LoadTexture(ToCryString(params.diffuseCubemap)));

	light.SetLightColor(params.diffuseColor);
	light.SetSpecularMult(params.specularMultiplier);
	light.m_nPostEffect = params.postEffect;

	light.m_fHDRDynamic = params.hdrDynamic;

	light.m_fProjectorNearPlane = params.projectNearPlane;

	pLightSource->SetLightProperties(light);
}

MonoLightParams CScriptbind_3DEngine::GetLightSourceParams(ILightSource *pLightSource)
{
	MonoLightParams params;
	return params;
}

float CScriptbind_3DEngine::GetTerrainElevation(float x, float y, bool includeOutdoorVoxels)
{
	return gEnv->p3DEngine->GetTerrainElevation(x, y, includeOutdoorVoxels);
}

float CScriptbind_3DEngine::GetTerrainZ(int x, int y)
{
	return gEnv->p3DEngine->GetTerrainZ(x, y);
}

int CScriptbind_3DEngine::GetTerrainUnitSize()
{
	return gEnv->p3DEngine->GetHeightMapUnitSize();
}

int CScriptbind_3DEngine::GetTerrainSize()
{
	return gEnv->p3DEngine->GetTerrainSize();
}

int CScriptbind_3DEngine::GetTerrainSectorSize()
{
	return gEnv->p3DEngine->GetTerrainSectorSize();
}

void CScriptbind_3DEngine::Asplode(Vec3 pos, Vec3 dir, float minRadius, float maxRadius, float pressure)
{
	pe_explosion explosion;
	explosion.epicenter = pos;
	explosion.epicenterImp = pos;

	explosion.explDir = dir;

	explosion.r = minRadius;
	explosion.rmin = minRadius;
	explosion.rmax = maxRadius;

	if (explosion.rmax==0)
			explosion.rmax=0.0001f;

	explosion.impulsivePressureAtR = pressure;

	explosion.nGrow = 0;
	explosion.rminOcc = 0.07f;

	explosion.holeSize = 0.0f;
	explosion.nOccRes = explosion.rmax>50.0f ? 0:16;

	gEnv->pPhysicalWorld->SimulateExplosion(&explosion, 0, 0, ent_living|ent_rigid|ent_sleeping_rigid|ent_independent|ent_static|ent_delayed_deformations);
}

void CScriptbind_3DEngine::SetTimeOfDay(float hour, bool forceUpdate)
{
	gEnv->p3DEngine->GetTimeOfDay()->SetTime(hour, forceUpdate);
}

float CScriptbind_3DEngine::GetTimeOfDay()
{
	return gEnv->p3DEngine->GetTimeOfDay()->GetTime();
}

ITimeOfDay::SAdvancedInfo CScriptbind_3DEngine::GetTimeOfDayAdvancedInfo()
{
	ITimeOfDay::SAdvancedInfo info;
	gEnv->p3DEngine->GetTimeOfDay()->GetAdvancedInfo(info);

	return info;
}

void CScriptbind_3DEngine::SetTimeOfDayAdvancedInfo(ITimeOfDay::SAdvancedInfo advancedInfo)
{
	gEnv->p3DEngine->GetTimeOfDay()->SetAdvancedInfo(advancedInfo);
}

void CScriptbind_3DEngine::SetTimeOfDayVariableValue(ITimeOfDay::ETimeOfDayParamID id, float value)
{
	float valueArray[3];
	valueArray[0] = value;
	gEnv->p3DEngine->GetTimeOfDay()->SetVariableValue(id, valueArray);
}

void CScriptbind_3DEngine::SetTimeOfDayVariableValueColor(ITimeOfDay::ETimeOfDayParamID id, Vec3 value)
{
	float valueArray[3];
	valueArray[0] = value.x;
	valueArray[1] = value.x;
	valueArray[2] = value.x;

	gEnv->p3DEngine->GetTimeOfDay()->SetVariableValue(id, valueArray);
}