///////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// CryENGINE UI system scriptbind
//////////////////////////////////////////////////////////////////////////
// 22/03/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __UI_H__
#define __UI_H__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

#include <IFlashUI.h>

typedef void (*UICallback)(void);

class CUI 
	: public IMonoScriptBind
	, public IUIEventListener
{
public:
	CUI();
	~CUI();

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "UI"; }
	// ~IMonoScriptBind

	// IUIEventListener
	virtual void OnEvent(const SUIEvent& event);
	// ~IUIEventListener

	static IUIEventSystem *CreateEventSystem(mono::string name, IUIEventSystem::EEventSystemType eventType);

	static void RegisterFunction(IUIEventSystem *pEventSystem, mono::string name, mono::string desc, mono::array inputs, UICallback callback);
	static void RegisterEvent(IUIEventSystem *pEventSystem, mono::string name, mono::string desc, mono::array outputs);

private:
	typedef std::map<unsigned int, UICallback> TCallbacks;
	static TCallbacks m_functionCallbacks;

	typedef std::vector<unsigned int> TEvents;
	static TEvents m_events;

	static CUI *m_pUI;
};

#endif //__UI_H__