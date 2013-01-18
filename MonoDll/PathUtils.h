/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Contains various path utils used for Mono startup.
//////////////////////////////////////////////////////////////////////////
// 20/11/2011 : Created by Filip 'i59' Lundgren (based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_PATH_UTILS_H__
#define __MONO_PATH_UTILS_H__

#define MONO_CONFIG_PATH "etc"
#define MONO_LIB_PATH "lib"

class PathUtils
{
public:
	static string GetBinaryPath(bool force32 = false)
	{
#ifdef WIN64
		if(!force32)
			return "Bin64\\Plugins\\CryMono\\";
#endif

		return "Bin32\\Plugins\\CryMono\\";
	}

	static string GetEnginePath()
	{
		return "Engine\\";
	}

	static string GetMonoPath()
	{
		return GetBinaryPath(true).append("Mono").append("\\");
	}

	static string GetConfigPath()
	{
		return GetMonoPath().append(MONO_CONFIG_PATH).append("\\");
	}

	static string GetLibPath()
	{
		return GetMonoPath().append(MONO_LIB_PATH).append("\\");
	}
};

#endif //__MONO_PATH_UTILS_H__