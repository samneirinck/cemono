/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script Binding for GameRules

-------------------------------------------------------------------------
History:
- 23:2:2006   18:30 : Created by Márcio Martins

*************************************************************************/
#ifndef __SCRIPTBIND_GAMERULES_H__
#define __SCRIPTBIND_GAMERULES_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IMonoScriptBind.h>
#include <MonoCommon.h>

class CGameRules;
class CActor;
struct IGameFramework;
struct ISystem;

class CScriptBind_GameRules : public IMonoScriptBind
{
public:
	CScriptBind_GameRules(ISystem *pSystem, IGameFramework *pGameFramework);
	virtual ~CScriptBind_GameRules();

	// <title SpawnPlayer>
	// Syntax: GameRules.SpawnPlayer( int, mono::string, mono::string, className, Vec3, Vec3 )
	// Arguments:
	//		channelId	- Channel identifier.
	//		name		- Player name.
	//		className	- Name of the player class.
	//		pos			- Playerition.
	//		angles		- Player angle.
	// Description:
	//		Spawns a player.
	MonoMethod(EntityId, SpawnPlayer, int, mono::string, mono::string, Vec3, Vec3);
	// <title ChangePlayerClass>
	// Syntax: GameRules.ChangePlayerClass( int, mono::string, className )
	// Arguments:
	//		channelId - Channel identifier.
	//		className - Name of the new class for the player.
	// Description:
	//		Changes the player class.
	//int, ChangePlayerClass, int, mono::string, className);
	// <title RevivePlayer>
	// Syntax: GameRules.RevivePlayer( EntityId, Vec3, Vec3, int, bool )
	// Arguments:
	//		playerId		- Player identifier.
	//		pos				- Playerition.
	//		angles			- Player angle.
	//		teamId			- Team identifier.
	//		clearInventory	- True to clean the inventory, false otherwise. 
	// Description:
	//		Revives the player.
	MonoMethod(void, RevivePlayer, EntityId, Vec3, Vec3, int, bool);

	// IMonoScriptBind
	virtual void Release() { delete this; }

	virtual const char *GetClassName() { return "GameRules"; }
	// ~IMonoScriptBind

private:
	void RegisterMethods();

	static CGameRules *GetGameRules();
	static CActor *GetActor(EntityId id);
};

#endif //__SCRIPTBIND_GAMERULES_H__