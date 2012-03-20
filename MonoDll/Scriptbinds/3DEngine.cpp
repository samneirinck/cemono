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