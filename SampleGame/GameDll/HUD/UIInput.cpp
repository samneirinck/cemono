////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2010.
// -------------------------------------------------------------------------
//  File name:   UIInput.cpp
//  Version:     v1.00
//  Created:     17/9/2010 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UIInput.h"

#include "Game.h"
#include "GameActions.h"

TActionHandler<CUIInput> CUIInput::s_actionHandler;
SUIEventHelper<CUIInput> CUIInput::s_EventDispatcher;

CUIInput::CUIInput()
{
	if ( !gEnv->pFlashUI
		|| !g_pGame->GetIGameFramework() 
		|| !g_pGame->GetIGameFramework()->GetIActionMapManager()
		|| !gEnv->pSystem
		|| !gEnv->pSystem->GetISystemEventDispatcher() )
	{
		assert( false );
		return;
	}

	IActionMapManager* pAmMgr = g_pGame->GetIGameFramework()->GetIActionMapManager();
	pAmMgr->AddExtraActionListener( this );
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener( this );

	// set up the handlers
	if (s_actionHandler.GetNumHandlers() == 0)
	{
		#define ADD_HANDLER(action, func) s_actionHandler.AddHandler(actions.action, &CUIInput::func)
		const CGameActions& actions = g_pGame->Actions();

		ADD_HANDLER(ui_toggle_pause, OnActionTogglePause);

		ADD_HANDLER(ui_up, OnActionUp);
		ADD_HANDLER(ui_down, OnActionDown);
		ADD_HANDLER(ui_left, OnActionLeft);	
		ADD_HANDLER(ui_right, OnActionRight);

		ADD_HANDLER(ui_click, OnActionClick);	
		ADD_HANDLER(ui_back, OnActionBack);	

		#undef ADD_HANDLER
	}


	// ui events (receive events form ui)
	#define EVENT_NO_PARAM
	#define EVENT_ONE_PARAM(name, desc)  {__desc.Params.push_back(SUIParameterDesc(name, name, desc));}
	#define EVENT_TWO_PARAM(name1, desc1, name2, desc2)  EVENT_ONE_PARAM(name1, desc1) EVENT_ONE_PARAM(name2, desc2)
	#define EVENT_DYNAMIC_PARAM { __desc.IsDynamic = true; }

	#define REGISTER_EVENT(event, desc, params) { SUIEventDesc __desc( (#event), (#event), (desc) ); { params } \
	s_EventDispatcher.RegisterEvent( m_pUIEvents, __desc, &CUIInput::On##event ); }

	m_pUIEvents = gEnv->pFlashUI->CreateEventSystem( "System", IUIEventSystem::eEST_UI_TO_SYSTEM );

	REGISTER_EVENT(LoadLevel,	"Load a level",				EVENT_TWO_PARAM( "Level", "Name of the level", "Server", "If true, load as Server" ));
	REGISTER_EVENT(ReloadLevel,	"Reload current level",		EVENT_NO_PARAM);
	REGISTER_EVENT(SaveGame,	"Quicksave current game",	EVENT_ONE_PARAM( "Resume", "If true, game will be resumed if game was paused" ));
	REGISTER_EVENT(LoadGame,	"Quickload current game",	EVENT_ONE_PARAM( "Resume", "If true, game will be resumed if game was  paused" ));
	REGISTER_EVENT(PauseGame,	"Pause current game",		EVENT_NO_PARAM);
	REGISTER_EVENT(ResumeGame,	"Resume current game",		EVENT_NO_PARAM);
	REGISTER_EVENT(UnloadAllElements,	"Unloads all UI Elements (Skip Elements that are defined in the Array input)",		EVENT_DYNAMIC_PARAM);

	m_pUIEvents->RegisterListener( this );

	#undef REGISTER_EVENT

}

CUIInput::~CUIInput()
{
	if (   gEnv->pGame 
		&& gEnv->pGame->GetIGameFramework() 
		&& gEnv->pGame->GetIGameFramework()->GetIActionMapManager() )
	{
		IActionMapManager* pAmMgr = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
		pAmMgr->RemoveExtraActionListener( this );
	}
	if (   gEnv->pSystem
		&& gEnv->pSystem->GetISystemEventDispatcher())
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener( this );
	}
	if ( m_pUIEvents )
		m_pUIEvents->UnregisterListener(this);
}

void CUIInput::OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam )
{
	switch ( event )
	{
	case ESYSTEM_EVENT_GAME_PAUSED:
		EnablePauseFilter();
		break;
	case ESYSTEM_EVENT_GAME_RESUMED:
		DisablePauseFilter();
		break;
	}
}

void CUIInput::DisablePauseFilter()
{
	IActionMapManager* pAmMgr = g_pGame->GetIGameFramework()->GetIActionMapManager();
	if (pAmMgr)
	{
		pAmMgr->EnableFilter("only_ui", false);
	}
}

void CUIInput::EnablePauseFilter()
{
	IActionMapManager* pAmMgr = g_pGame->GetIGameFramework()->GetIActionMapManager();
	if (pAmMgr)
	{
		pAmMgr->Enable(true);
		pAmMgr->EnableFilter("only_ui", true);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Actions /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUIInput::OnAction( const ActionId& action, int activationMode, float value )
{
	s_actionHandler.Dispatch( this, 0, action, activationMode, value );
}


bool CUIInput::OnActionTogglePause(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if ( !gEnv->bMultiplayer && activationMode == eAAM_OnRelease && !gEnv->IsEditor() && g_pGame->GetIGameFramework()->IsGameStarted() )
	{
		bool isGamePaused = g_pGame->GetIGameFramework()->IsGamePaused();
		g_pGame->GetIGameFramework()->PauseGame( !isGamePaused, true );
	}
	return false;
}

#define SEND_CONTROLLER_EVENT(evt) 	if ( gEnv->pFlashUI ) \
	{ \
		switch (activationMode) \
		{ \
			case eAAM_OnPress: 	 gEnv->pFlashUI->DispatchControllerEvent( IUIElement::evt, IUIElement::eCIS_OnPress ); break; \
			case eAAM_OnRelease: gEnv->pFlashUI->DispatchControllerEvent( IUIElement::evt, IUIElement::eCIS_OnRelease ); break;\
		} \
	}


bool CUIInput::OnActionUp(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	SEND_CONTROLLER_EVENT(eCIE_Up);
	return false;
}

bool CUIInput::OnActionDown(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	SEND_CONTROLLER_EVENT(eCIE_Down);
	return false;
}

bool CUIInput::OnActionLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	SEND_CONTROLLER_EVENT(eCIE_Left);
	return false;
}

bool CUIInput::OnActionRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	SEND_CONTROLLER_EVENT(eCIE_Right);
	return false;
}

bool CUIInput::OnActionClick(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	SEND_CONTROLLER_EVENT(eCIE_Action);
	return false;
}

bool CUIInput::OnActionBack(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	SEND_CONTROLLER_EVENT(eCIE_Back);
	return false;
}

#undef SEND_CONTROLLER_EVENT

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// UI Events ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUIInput::OnEvent( const SUIEvent& event )
{
	s_EventDispatcher.Dispatch( this, event );
}

void CUIInput::OnLoadLevel( const SUIEvent& event )
{
	string mapname;
	bool isServer;
	if (event.args.GetArg(0, mapname), event.args.GetArg(1, isServer))
	{
		g_pGame->GetIGameFramework()->ExecuteCommandNextFrame(string().Format("map %s%s", mapname.c_str(), isServer ? " s" : ""));
		if ( g_pGame->GetIGameFramework()->IsGamePaused() )
			g_pGame->GetIGameFramework()->PauseGame(false, true);
	}
}

void CUIInput::OnReloadLevel( const SUIEvent& event )
{
	ILevel* pLevel = g_pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel();
	if (pLevel)
	{
		g_pGame->GetIGameFramework()->ExecuteCommandNextFrame(string().Format("map %s", pLevel->GetLevelInfo()->GetName()));
		if ( g_pGame->GetIGameFramework()->IsGamePaused() )
			g_pGame->GetIGameFramework()->PauseGame(false, true);
	}
}

void CUIInput::OnSaveGame( const SUIEvent& event )
{
	ILevel* pLevel = g_pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel();
	if (pLevel)
	{
		g_pGame->GetIGameFramework()->SaveGame(pLevel->GetLevelInfo()->GetPath(), true);
		bool bResume = false;
		event.args.GetArg( 0, bResume );
		if ( bResume && g_pGame->GetIGameFramework()->IsGamePaused() )
			g_pGame->GetIGameFramework()->PauseGame(false, true);
	}
}

void CUIInput::OnLoadGame( const SUIEvent& event )
{
	ILevel* pLevel = g_pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel();
	if (pLevel)
	{
		g_pGame->GetIGameFramework()->LoadGame(pLevel->GetLevelInfo()->GetPath(), true);
		bool bResume = false;
		event.args.GetArg( 0, bResume );
		if ( bResume && g_pGame->GetIGameFramework()->IsGamePaused() )
			g_pGame->GetIGameFramework()->PauseGame(false, true);
	}
}

void CUIInput::OnPauseGame( const SUIEvent& event )
{
	g_pGame->GetIGameFramework()->PauseGame(true, true);
}

void CUIInput::OnResumeGame( const SUIEvent& event )
{
	g_pGame->GetIGameFramework()->PauseGame(false, true);
}

void CUIInput::OnUnloadAllElements( const SUIEvent& event )
{
	if (!gEnv->pFlashUI) return;

	for ( int i = 0; i < gEnv->pFlashUI->GetUIElementCount(); ++i )
	{
		IUIElement* pElement = gEnv->pFlashUI->GetUIElement( i );
		bool bSkipElement = false;
		for ( int k = 0; k < event.args.GetArgCount(); ++k )
		{
			string arg;
			event.args.GetArg( k ,arg );
			if ( arg == pElement->GetName() )
			{
				bSkipElement = true;
				break;
			}
		}
		if ( !bSkipElement )
		{
			IUIElementIteratorPtr instances = pElement->GetInstances();
			while ( IUIElement* pInstance = instances->Next() )
			{
				pInstance->Unload();
			}
		}
	}
}


//----------------------
//
// here you can define your own actions, and map them to nodes to use in the flowgraphs for your UI
//
//----------------------
