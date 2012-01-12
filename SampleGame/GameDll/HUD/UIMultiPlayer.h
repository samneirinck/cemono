////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UIMultiPlayer.h
//  Version:     v1.00
//  Created:     26/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIMultiPlayer_H__
#define __UIMultiPlayer_H__

#include <IFlashUI.h>

class CUIMultiPlayer
	: public IUIEventListener
{
public:
	CUIMultiPlayer();
	~CUIMultiPlayer();

	// IUIEventListener
	virtual void OnEvent( const SUIEvent& event );
	// ~IUIEventListener

	// ui functions
	void EnteredGame();
	void PlayerJoined(EntityId playerid, const string& name);
	void PlayerLeaved(EntityId playerid, const string& name);
	void PlayerKilled(EntityId playerid, EntityId shooterid);

private:
	static SUIEventHelper<CUIMultiPlayer> s_EventDispatcher;
	IUIEventSystem* m_pUIEvents;
	IUIEventSystem* m_pUIFunctions;


	enum EUIEvent
	{
		EUIE_EnteredGame,
		EUIE_PlayerJoined,
		EUIE_PlayerLeaved,
		EUIE_PlayerKilled,
	};

	std::map< EUIEvent, uint > m_EventMap;

	std::vector< EntityId > m_Players;
private:

// 	// ui events
 	void RequestPlayers( const SUIEvent& event );
};


#endif // __UIMultiPlayer_H__