#include "StdAfx.h"
#include "3DEngine.h"

CScriptbind_3DEngine::CScriptbind_3DEngine()
{
	REGISTER_METHOD(GetTerrainElevation);
	REGISTER_METHOD(GetTerrainZ);

	REGISTER_METHOD(GetTerrainSize);
	REGISTER_METHOD(GetTerrainSectorSize);
	REGISTER_METHOD(GetTerrainUnitSize);
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