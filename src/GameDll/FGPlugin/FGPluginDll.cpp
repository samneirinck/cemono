/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// FGPluginDll.cpp
//
// Purpose: Flowgraph Plugin DLL entry point
//
// History:
//	- 5/31/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FGPlugin\FGPluginGame.h"
#include <CryLibrary.h>
#include <platform_impl.h>

extern "C"
{
	GAME_API IGame *CreateGame(IGameFramework* pGameFramework)
	{
		ModuleInitISystem(pGameFramework->GetISystem());
		static char pGameBuffer[sizeof(CFGPluginGame)];
		return new ((void*)pGameBuffer) CFGPluginGame();
	}
}
