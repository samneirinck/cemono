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
#undef GetClassName

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

	static string GetTempPath()
	{
		TCHAR tempPath[MAX_PATH];
		GetTempPathA(MAX_PATH, tempPath);

		string cryMonoTempDir = string(tempPath) + string("CryMono//");

		DWORD attribs = GetFileAttributesA(cryMonoTempDir.c_str());
		if(attribs == INVALID_FILE_ATTRIBUTES || attribs | FILE_ATTRIBUTE_DIRECTORY)
			CryCreateDirectory(cryMonoTempDir.c_str(), nullptr);

		return cryMonoTempDir.c_str();
	}
};

#endif //__MONO_PATH_UTILS_H__