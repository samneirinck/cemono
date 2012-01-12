////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2010.
// -------------------------------------------------------------------------
//  File name:   UIInput.h
//  Version:     v1.00
//  Created:     17/9/2010 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIInput_H__
#define __UIInput_H__

#include "IActionMapManager.h"
#include <IFlashUI.h>

class CUIInput 
	: public IActionListener
	, public IUIEventListener
	, public ISystemEventListener
{
public:

	CUIInput();
	~CUIInput();

	// IActionListener
	virtual void OnAction( const ActionId& action, int activationMode, float value );
	// ~IActionListener

	// IUIEventListener
	virtual void OnEvent( const SUIEvent& event );
	// ~IUIEventListener

	// ISystemEventListener
	virtual void OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam );
	// ~ISystemEventListener

private:
	static TActionHandler<CUIInput>	s_actionHandler;
	static SUIEventHelper<CUIInput> s_EventDispatcher;
	IUIEventSystem* m_pUIEvents;

	// actions
	bool OnActionTogglePause(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionUp(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionDown(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionClick(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionBack(EntityId entityId, const ActionId& actionId, int activationMode, float value);


	// ui events
	void OnLoadLevel( const SUIEvent& event );
	void OnReloadLevel( const SUIEvent& event );
	void OnSaveGame( const SUIEvent& event );
	void OnLoadGame( const SUIEvent& event );
	void OnPauseGame( const SUIEvent& event );
	void OnResumeGame( const SUIEvent& event );
	void OnUnloadAllElements( const SUIEvent& event );


	void DisablePauseFilter();
	void EnablePauseFilter();
};


#endif