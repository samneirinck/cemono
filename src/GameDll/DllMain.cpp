/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
Description: Dll Entry point to capture dll instance handle (needed for loading of embedded resources)

-------------------------------------------------------------------------
History:
- 01:11:2007: Created by Marco Koegler
- 16:04:2009: Game class shutdown FIX - James-Ryan
*************************************************************************/

#include "StdAfx.h"
#if defined(WIN32)
#include <windows.h>

void* g_hInst = 0;

BOOL APIENTRY DllMain ( HINSTANCE hInst, DWORD reason, LPVOID reserved )
{
	if ( reason == DLL_PROCESS_ATTACH )
		g_hInst = hInst;
	else if ( reason == DLL_PROCESS_DETACH)
		g_hInst = NULL;
	
	return TRUE;
}
#endif