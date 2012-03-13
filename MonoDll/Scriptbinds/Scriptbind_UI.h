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

struct SMonoUIParameterDesc {
	int				Type;
	mono::string	Name;
	mono::string	DisplayName;
	mono::string	Description;
};

struct SMonoUIEventDesc {
	int				Type;
	mono::string	Name;
	mono::string	DisplayName;
	mono::string	Description;

	mono::array		Params;
	bool			IsDynamic;
	mono::string	DynamicName;
	mono::string	DynamicDescription;
};

class CScriptbind_UI
	: public IMonoScriptBind	
{
public:
	class CUICallback : public IUIEventListener {
	protected:
		friend class CScriptbind_UI;
		string								Name;
		CScriptbind_UI*						Parent;
		IUIEventSystem*						System;
		IUIEventSystem::EEventSystemType	Type;
	public:
		CUICallback(const char* name, CScriptbind_UI* parent, IUIEventSystem*system, IUIEventSystem::EEventSystemType type);
		~CUICallback();
		const char* FindEvent(uint ID);
		int FindEvent(const char* pName);
		virtual void OnEvent(const SUIEvent& event);
	};
	typedef std::map<string, CUICallback*> TEventMap;
	typedef std::pair<string, CUICallback*> TEventMapPair;
	CUICallback* GetOrCreateSystem(const char* pName, IUIEventSystem::EEventSystemType type);
	bool SystemExists(const char* pName, IUIEventSystem::EEventSystemType type);
	CUICallback* FindSystem(const char* pName, IUIEventSystem::EEventSystemType type);
	void RemoveSystem(CUICallback* pCB);

	static CScriptbind_UI*	s_pInstance;
	CScriptbind_UI();
	~CScriptbind_UI();

	void OnReset();

	void OnEvent(const char* SystemName, const char* EventName, const SUIEvent& event);
	
	//Exposed to CryMono
	static int RegisterEvent(mono::string eventsystem, int direction, SMonoUIEventDesc desc);
	static bool RegisterToEventSystem(mono::string eventsystem, int type);
	static void UnregisterFromEventSystem(mono::string eventsystem, int type);
	static void SendEvent(mono::string eventsystem, int event, mono::array args);
	static void SendNamedEvent(mono::string eventsystem, mono::string event, mono::array args);

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "UI"; }
	// ~IMonoScriptBind

	IMonoClass *m_pUIClass;
	TEventMap	m_EventMapS2UI;
	TEventMap	m_EventMapUI2S;
};

#endif //__SCRIPTBIND_UI_H__