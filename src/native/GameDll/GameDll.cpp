#include "StdAfx.h"
#include "Game.h"

#include <CryLibrary.h>
#include <platform_impl.h>


extern "C"
{
	GAME_API IGame* CreateGame(IGameFramework* pGameFramework)
	{
		ModuleInitISystem(pGameFramework->GetISystem());

		static char pGameBuffer[sizeof(CGame)];
		return new ((void*)pGameBuffer)CGame();
	}
}