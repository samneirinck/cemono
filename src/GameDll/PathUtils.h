#ifndef __MONO_PATHUTILS_H__
#define __MONO_PATHUTILS_H__

#pragma once

#include "StdAfx.h"
#include "Properties.h"

class CPathUtils
{
public:
	static string GetCrysisWarsPath()
	{
		char path[256];
		CryGetCurrentDirectory(256, path);

		return string(path).append("\\");
	}

	static string GetModPath(bool fullPath = true)
	{
		return fullPath ? GetCrysisWarsPath().append(gEnv->pCryPak->GetModDir()) : gEnv->pCryPak->GetModDir();
	}

	static string GetAssemblyPath()
	{
		return GetModPath().append(ASSEMBLY_PATH).append("\\");
	}

	static string GetFGNodePath()
	{
		return GetModPath().append(NODE_PATH).append("\\");
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

#endif //__MONO_PATHUTILS_H__