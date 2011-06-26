#pragma once

#include "StdAfx.h"

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
		return GetCrysisWarsPath().append(gEnv->pCryPak->GetModDir()).append("\\");
	}

	static string GetCemonoPath()
	{
		return GetCrysisWarsPath().append("cemono").append("\\");
	}

	static string GetConfigurationPath()
	{
		return GetCemonoPath().append("etc").append("\\");
	}

	static string GetAssemblyPath()
	{
		return GetCemonoPath().append("lib").append("\\");
	}

	static string GetCemonoAssemblyPath()
	{
		return GetAssemblyPath().append("cemono").append("\\");
	}

	static string GetModGameLogicPath()
	{
		return GetModPath().append("GameLogic").append("\\");
	}
};

