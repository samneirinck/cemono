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
		return GetModPath().append(ASSEMBLY_PATH).append("\\");
	}

	static string GetLibPath()
	{
		return GetModPath().append(MONO_LIB_PATH).append("\\");
	}

	static string GetConfigPath()
	{
		return GetModPath().append(MONO_CONFIG_PATH).append("\\");
	}
};