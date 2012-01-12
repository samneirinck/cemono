////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UISettings.h
//  Version:     v1.00
//  Created:     10/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UISettings_H__
#define __UISettings_H__

#include <IFlashUI.h>
#include <IGameFramework.h>

class CUISettings
	: public IUIEventListener
	, public IGameFrameworkListener
{
public:
	CUISettings();
	~CUISettings();

	// IUIEventListener
	virtual void OnEvent( const SUIEvent& event );
	// ~IUIEventListener

	//IGameFrameworkListener
	virtual void OnPostUpdate(float fDeltaTime);
	virtual void OnSaveGame(ISaveGame* pSaveGame) {};
	virtual void OnLoadGame(ILoadGame* pLoadGame) {};
	virtual void OnLevelEnd(const char* nextLevel) {};
	virtual void OnActionEvent(const SActionEvent& event) {};

	//~IGameFrameworkListener

private:
	static SUIEventHelper<CUISettings> s_EventDispatcher;
	IUIEventSystem* m_pUIEvents;
	IUIEventSystem* m_pUIFunctions;

	ICVar* m_pRXVar;
	ICVar* m_pRYVar;
	ICVar* m_pFSVar;
	ICVar* m_pMusicVar;
	ICVar* m_pSFxVar;
	ICVar* m_pVideoVar;

	enum EUIEvent
	{
		EUIE_ResolutionChanged,
		EUIE_SoundSettingsChanged,
		EUIE_OnGetResolutions
	};

	std::map< EUIEvent, uint > m_EventMap;

	typedef std::vector< std::pair<int,int> > TResolutions;
	TResolutions m_Resolutions;

private:
	// ui functions
	void SendResolutions();
	void SendResChange();
	void SendSoundChange();

	// ui events
	void OnSetResolution( const SUIEvent& event );
	void OnSetSoundSettings( const SUIEvent& event );
	void OnGetResolutions( const SUIEvent& event );
};


#endif // __UISettings_H__