#include "StdAfx.h"
#include "ScriptBind_3DEngine.h"

CScriptBind_3DEngine::CScriptBind_3DEngine()
{
	REGISTER_EXPOSED_METHOD(GetTerrainElevation);
	REGISTER_EXPOSED_METHOD(GetTerrainSize);
	REGISTER_EXPOSED_METHOD(GetTerrainSectorSize);
}

float CScriptBind_3DEngine::GetTerrainElevation(int x, int y, bool includeOutdoorVoxels)
{
	return gEnv->p3DEngine->GetTerrainElevation((float)x, (float)y, includeOutdoorVoxels);
}

int CScriptBind_3DEngine::GetTerrainSize()
{
	return gEnv->p3DEngine->GetTerrainSize();
}

int CScriptBind_3DEngine::GetTerrainSectorSize()
{
	return gEnv->p3DEngine->GetTerrainSectorSize();
}