/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script Binding for Game

-------------------------------------------------------------------------
History:
- 14:08:2006   11:30 : Created by AlexL
*************************************************************************/
#ifndef __SCRIPTBIND_GAME_H__
#define __SCRIPTBIND_GAME_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IScriptSystem.h>
#include <ScriptHelpers.h>

struct IGameFramework;
struct ISystem;

class CScriptBind_Game :
	public CScriptableBase
{
public:
	CScriptBind_Game(ISystem *pSystem, IGameFramework *pGameFramework);
	virtual ~CScriptBind_Game();

protected:
	// <title PauseGame>
	// Syntax: Game.PauseGame( bool pause )
	// Arguments:
	//		pause - True to pause the game, false to resume.
	// Description:
	//		Pauses the game.
	int PauseGame(IFunctionHandler *pH, bool pause);

	// <title QueryBattleStatus>
	// Syntax: Game.QueryBattleStatus()
	// Description:
	//		Queries battle status, range from 0 (quiet) to 1 (full combat).
	int	QueryBattleStatus(IFunctionHandler *pH);

	// <title GetNumLightsActivated>
	// Syntax: Game.GetNumLightsActivated()
	// Description:
	//		Gets the number of activated lights.
	int GetNumLightsActivated(IFunctionHandler *pH);

	// <title IsMountedWeaponUsableWithTarget>
	// Syntax: Game.IsMountedWeaponUsableWithTarget()
	// Description:
	//		Checks if the mounted weapon is usable with the target.
	int IsMountedWeaponUsableWithTarget(IFunctionHandler *pH);

	// <title IsPlayer>
	// Syntax: Game.IsPlayer()
	// Description:
	//		Checks if the given entity is player.
	int IsPlayer(IFunctionHandler *pH, ScriptHandle entityId);

private:
	void RegisterGlobals();
	void RegisterMethods();

	ISystem						*m_pSystem;
	IScriptSystem			*m_pSS;
	IGameFramework		*m_pGameFW;
};

#endif //__SCRIPTBIND_GAME_H__