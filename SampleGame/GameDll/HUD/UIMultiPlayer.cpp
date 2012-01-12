////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UIMultiPlayer.cpp
//  Version:     v1.00
//  Created:     26/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UIMultiPlayer.h"

#include <IEntitySystem.h>
#include <IGame.h>
#include <IGameFramework.h>
#include <IActorSystem.h>

SUIEventHelper<CUIMultiPlayer> CUIMultiPlayer::s_EventDispatcher;
#define UNDEF_ENTITY_NAME "<Undefined Entity>"

CUIMultiPlayer::CUIMultiPlayer()
	: m_pUIEvents(NULL)
	, m_pUIFunctions(NULL)
{
	if (!gEnv->pFlashUI) return;


	// event descriptions
	SUIEventDesc evtEnteredGame("EnteredGame", "EnteredGame", "");

	SUIEventDesc evtPlayerJoined("PlayerJoined", "PlayerJoined", "");
	evtPlayerJoined.Params.push_back(SUIParameterDesc("ID", "ID", ""));
	evtPlayerJoined.Params.push_back(SUIParameterDesc("Name", "Name", ""));

	SUIEventDesc evtPlayerLeaved("PlayerLeaved", "PlayerLeaved", "");
	evtPlayerLeaved.Params.push_back(SUIParameterDesc("ID", "ID", ""));
	evtPlayerLeaved.Params.push_back(SUIParameterDesc("Name", "Name", ""));

	SUIEventDesc evtPlayerKilled("PlayerKilled", "PlayerKilled", "");
	evtPlayerKilled.Params.push_back(SUIParameterDesc("ID", "ID", ""));
	evtPlayerKilled.Params.push_back(SUIParameterDesc("Name", "Name", ""));
	evtPlayerKilled.Params.push_back(SUIParameterDesc("ShooterID", "ShooterID", ""));
	evtPlayerKilled.Params.push_back(SUIParameterDesc("ShooterName", "ShooterName", ""));


	// events to send from this class to UI flowgraphs
	m_pUIFunctions = gEnv->pFlashUI->CreateEventSystem("MP", IUIEventSystem::eEST_SYSTEM_TO_UI);
	
	m_EventMap[EUIE_EnteredGame] = m_pUIFunctions->RegisterEvent(evtEnteredGame);
	m_EventMap[EUIE_PlayerJoined] = m_pUIFunctions->RegisterEvent(evtPlayerJoined);
 	m_EventMap[EUIE_PlayerLeaved] = m_pUIFunctions->RegisterEvent(evtPlayerLeaved);
 	m_EventMap[EUIE_PlayerKilled] = m_pUIFunctions->RegisterEvent(evtPlayerKilled);

	// event descriptions
	SUIEventDesc evtRequestPlayers("GetPlayers", "GetPlayers", "");

	// events that can be sent from UI flowgraphs to this class
	m_pUIEvents = gEnv->pFlashUI->CreateEventSystem("MP", IUIEventSystem::eEST_UI_TO_SYSTEM);
 	s_EventDispatcher.RegisterEvent(m_pUIEvents, evtRequestPlayers, &CUIMultiPlayer::RequestPlayers);
	m_pUIEvents->RegisterListener(this);
}

CUIMultiPlayer::~CUIMultiPlayer()
{
	if (m_pUIEvents) m_pUIEvents->UnregisterListener(this);
}

// ui events
void CUIMultiPlayer::OnEvent( const SUIEvent& event )
{
	s_EventDispatcher.Dispatch( this, event );
}


// ui functions
void CUIMultiPlayer::EnteredGame()
{
	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_EnteredGame], SUIArguments()));
}

void CUIMultiPlayer::PlayerJoined(EntityId playerid, const string& name)
{
	if (gEnv->pGame->GetIGameFramework()->GetClientActorId() == playerid)
		return;

	SUIArguments args;
	args.AddArgument(playerid);
	args.AddArgument(name);

	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_PlayerJoined], args));
}

void CUIMultiPlayer::PlayerLeaved(EntityId playerid, const string& name)
{
	if (gEnv->pGame->GetIGameFramework()->GetClientActorId() == playerid)
		return;

	SUIArguments args;
	args.AddArgument(playerid);
	args.AddArgument(name);

	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_PlayerLeaved], args));
}

void CUIMultiPlayer::PlayerKilled(EntityId playerid, EntityId shooterid)
{
	SUIArguments args;

	IEntity* pEntity = gEnv->pEntitySystem->GetEntity(playerid);
	assert(pEntity);
	IEntity* pShooterEntity = gEnv->pEntitySystem->GetEntity(shooterid);
	assert(pShooterEntity);

	args.AddArgument(playerid);
	args.AddArgument(pEntity ? string(pEntity->GetName()) : string(UNDEF_ENTITY_NAME));
	args.AddArgument(shooterid);
	args.AddArgument(pShooterEntity ? string(pShooterEntity->GetName()) : string("undefined"));

	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_PlayerKilled], args));
}

void CUIMultiPlayer::RequestPlayers( const SUIEvent& event )
{
	IActorIteratorPtr actors = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->CreateActorIterator();
	while (IActor* pActor = actors->Next())
	{
		if (pActor->IsPlayer())
			PlayerJoined(pActor->GetEntityId(), pActor->GetEntity()->GetName());
	}
}