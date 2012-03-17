#include "StdAfx.h"
#include "3DEngine.h"

CScriptbind_3DEngine::CScriptbind_3DEngine()
{
	REGISTER_METHOD(GetTerrainElevation);
	REGISTER_METHOD(GetTerrainSize);
	REGISTER_METHOD(GetTerrainSectorSize);
}

float CScriptbind_3DEngine::GetTerrainElevation(int x, int y, bool includeOutdoorVoxels)
{
	return gEnv->p3DEngine->GetTerrainElevation((float)x, (float)y, includeOutdoorVoxels);
}

int CScriptbind_3DEngine::GetTerrainSize()
{
	return gEnv->p3DEngine->GetTerrainSize();
}

int CScriptbind_3DEngine::GetTerrainSectorSize()
{
	return gEnv->p3DEngine->GetTerrainSectorSize();
}