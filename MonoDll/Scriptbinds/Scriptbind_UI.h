/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Basic UI scriptbind to allow creation and invoking of FG UI events
// via Mono.
//////////////////////////////////////////////////////////////////////////
// 11/03/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_UI_H__
#define __SCRIPTBIND_UI_H__

#include <IMonoScriptBind.h>
#include <MonoCommon.h>

#include <IFlashUI.h>

class CScriptbind_UI
	: public IMonoScriptBind
	, public IUIEventListener
{
public:
	CScriptbind_UI();
	~CScriptbind_UI() {}

	void OnReset();

	// IUIEventListener
	virtual void OnEvent( const SUIEvent& event );
	// ~IUIEventListener

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "UI"; }
	// ~IMonoScriptBind

	IMonoClass *m_pUIClass;
};

#endif //__SCRIPTBIND_UI_H__