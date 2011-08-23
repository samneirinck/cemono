#pragma once

#include "StdAfx.h"

#define MONO_LIB_FOLDER "lib"
#define MONO_CONFIG_FOLDER "etc"

class MonoPathUtils
{
public:
	static string GetEnginePath()
	{
		//char path[256];
		//CryGetCurrentDirectory(256, path);

		//return string(path).append("\\");

		return string("Engine\\");
	}

	static string GetAssemblyPath()
	{
		//return GetCemonoPath().append(ASSEMBLY_PATH).append("\\");
		return "";
	}

	static string GetFGNodePath()
	{
		//return GetModPath().append(NODE_PATH).append("\\");
		return "";
	}

	static string GetCemonoPath()
	{
		return GetEnginePath().append("cemono").append("\\");
	}

	static string GetLibPath()
	{
		return GetCemonoPath().append(MONO_LIB_FOLDER).append("\\");
	}

	static string GetCemonoLibPath()
	{
		return GetLibPath().append("cemono").append("\\");
	}

	static string GetConfigPath()
	{
		return GetCemonoPath().append(MONO_CONFIG_FOLDER).append("\\");
	}
};