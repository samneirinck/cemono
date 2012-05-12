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

class CScriptbind_UI;

struct SMonoUIParameterDesc 
{
	SUIParameterDesc::EUIParameterType			Type;

	mono::string	Name;
	mono::string	DisplayName;
	mono::string	Description;
};

struct SMonoUIEventDesc 
{
	int				Type;
	mono::string	Name;
	mono::string	DisplayName;
	mono::string	Description;

	mono::array		Params;
	bool			IsDynamic;
	mono::string	DynamicName;
	mono::string	DynamicDescription;
};

class CUICallback : public IUIEventListener
{
public:
	CUICallback(const char *name, CScriptbind_UI *pParent, IUIEventSystem *pUIEventSystem, IUIEventSystem::EEventSystemType type);
	~CUICallback();

	// IUIEventListener
	virtual void OnEvent(const SUIEvent& event);
	// ~IUIEventListener


	const char *FindEvent(uint ID);
	int FindEvent(const char *name);

	IUIEventSystem::EEventSystemType GetEventSystemType() { return m_type; }
	string GetEventSystemName() { return m_name; }

	IUIEventSystem *GetEventSystem() { return m_pSystem; }

protected:
	string								m_name;

	CScriptbind_UI						*m_pParent;
	IUIEventSystem						*m_pSystem;

	IUIEventSystem::EEventSystemType	m_type;
};

class CScriptbind_UI
	: public IMonoScriptBind
{
	typedef std::map<string, CUICallback *> TEventMap;

public:

	CScriptbind_UI();
	~CScriptbind_UI();

	CUICallback *GetOrCreateSystem(const char *name, IUIEventSystem::EEventSystemType type);
	bool SystemExists(const char *name, IUIEventSystem::EEventSystemType type);
	CUICallback *FindSystem(const char *name, IUIEventSystem::EEventSystemType type);
	void RemoveSystem(CUICallback *pCB);

	static CScriptbind_UI *s_pInstance;

	void OnEvent(const char *systemName, const char *eventName, const SUIEvent& event);
	
	//Exposed to CryMono
	static int RegisterEvent(mono::string eventsystem, IUIEventSystem::EEventSystemType direction, SMonoUIEventDesc desc);
	static bool RegisterToEventSystem(mono::string eventsystem, IUIEventSystem::EEventSystemType type);
	static void UnregisterFromEventSystem(mono::string eventsystem, IUIEventSystem::EEventSystemType type);
	static void SendEvent(mono::string eventsystem, int event, mono::array args);
	static void SendNamedEvent(mono::string eventsystem, mono::string event, mono::array args);

	void Reset();

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "UI"; }
	// ~IMonoScriptBind

	TEventMap	m_EventMapS2UI;
	TEventMap	m_EventMapUI2S;

	IMonoClass							*m_pUIClass;
};

#endif //__SCRIPTBIND_UI_H__