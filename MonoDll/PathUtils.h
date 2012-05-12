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

#include <Windows.h>
#undef GetTempPath

class PathUtils
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

	static string GetTempPath()
	{
		TCHAR tempPath[MAX_PATH];
		GetTempPathA(MAX_PATH, tempPath);

		string cryMonoTempDir = string(tempPath) + string("CryMono//");

		DWORD attribs = GetFileAttributesA(cryMonoTempDir.c_str());
		if(attribs == INVALID_FILE_ATTRIBUTES || attribs | FILE_ATTRIBUTE_DIRECTORY)
			CryCreateDirectory(cryMonoTempDir.c_str(), NULL);

		return cryMonoTempDir.c_str();
	}
};

#endif //__MONO_PATH_UTILS_H__