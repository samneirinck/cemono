#pragma once

#include "StdAfx.h"
#include "MonoProperties.h"

class CMonoPathUtils
{
public:
	static string GetCrysisWarsPath()
	{
		char path[256];
		CryGetCurrentDirectory(256, path);

		return string(path).append("\\");
	}

	static string GetModPath()
	{
		return GetCrysisWarsPath().append(gEnv->pCryPak->GetModDir());
	}

	static string GetAssemblyPath()
	{
		CryLogAlways(GetModPath().append(ASSEMBLY_PATH).append("\\"));
		return GetModPath().append(ASSEMBLY_PATH).append("\\");
	}

	static string GetConfigPath()
	{
		CryLogAlways(GetModPath());
		CryLogAlways(GetModPath().append(CONFIG_PATH).append("\\"));
		return GetModPath().append(CONFIG_PATH).append("\\");
	}
};