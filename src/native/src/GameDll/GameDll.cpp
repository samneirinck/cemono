#include "StdAfx.h"

#include "Game.h"
#include "GameStartup.h"
#include "EditorGame.h"


extern "C"
{
	GAME_API IGame *CreateGame(IGameFramework* pGameFramework)
	{
		ModuleInitISystem(pGameFramework->GetISystem(),"CryGame");

		static char pGameBuffer[sizeof(CGame)];
		return new ((void*)pGameBuffer) CGame();
	}

	GAME_API IGameStartup *CreateGameStartup()
	{
		static char gameStartup_buffer[sizeof(CGameStartup)];
		return new ((void*)gameStartup_buffer) CGameStartup();
	}
	GAME_API IEditorGame *CreateEditorGame()
	{
		// TODO
		return NULL;
	}
}