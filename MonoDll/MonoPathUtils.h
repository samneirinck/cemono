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

class CMonoPathUtils
{
public:
	static string GetBinaryPath()
	{
#ifdef WIN64
		return "Bin64\\";
#endif

		return "Bin32\\";
	}

	static string GetEnginePath()
	{
		return "Engine\\";
	}

	static string GetMonoPath()
	{
		return GetEnginePath().append("Mono").append("\\");
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