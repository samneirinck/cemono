////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UISettings.cpp
//  Version:     v1.00
//  Created:     10/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UISettings.h"

SUIEventHelper<CUISettings> CUISettings::s_EventDispatcher;

CUISettings::CUISettings()
	: m_pUIEvents(NULL)
	, m_pUIFunctions(NULL)
{
	if (!gEnv->pFlashUI) return;

	// CVars
	m_pRXVar = gEnv->pConsole->GetCVar("r_Width");
	m_pRYVar = gEnv->pConsole->GetCVar("r_Height");
	m_pFSVar = gEnv->pConsole->GetCVar("r_Fullscreen");
	m_pMusicVar = gEnv->pConsole->GetCVar("s_MusicVolume");
	m_pSFxVar = gEnv->pConsole->GetCVar("s_SFXVolume");
	m_pVideoVar = gEnv->pConsole->GetCVar("sys_flash_video_soundvolume");
	assert(m_pRXVar && m_pRYVar && m_pFSVar && m_pMusicVar && m_pSFxVar && m_pVideoVar);
	if (!(m_pRXVar && m_pRYVar && m_pFSVar && m_pMusicVar && m_pSFxVar && m_pVideoVar)) return;


	m_Resolutions.push_back(std::make_pair(1024,768));
	m_Resolutions.push_back(std::make_pair(1280,720));
	m_Resolutions.push_back(std::make_pair(1280,1050));
	m_Resolutions.push_back(std::make_pair(1680,1050));
	m_Resolutions.push_back(std::make_pair(1920,1080));

	// event descriptions
	SUIEventDesc evtResChanged("ResChanged", "OnResChanged", "Triggered on resolution change");
	evtResChanged.Params.push_back(SUIParameterDesc("ResX", "ResX", "Screen X resolution"));
	evtResChanged.Params.push_back(SUIParameterDesc("ResY", "ResY", "Screen Y resolution"));
	evtResChanged.Params.push_back(SUIParameterDesc("FullScreen", "FullScreen", "Fullscreen"));

	SUIEventDesc evtSoundChanged("SoundChanged", "OnSoundChanged", "Triggered if sound volume changed");
	evtSoundChanged.Params.push_back(SUIParameterDesc("Music", "Music", "Music volume"));
	evtSoundChanged.Params.push_back(SUIParameterDesc("SFx", "SFx", "SFx volume"));
	evtSoundChanged.Params.push_back(SUIParameterDesc("Video", "Video", "Video volume"));

	SUIEventDesc evtGetRes("OnResolutions", "OnResolutions", "Triggered if resolutions were requested.\n Will return comma separated string (UI array) with all resolutions (x1,y1,x2,y2,...)", true);


	// events to send from this class to UI flowgraphs
	m_pUIFunctions = gEnv->pFlashUI->CreateEventSystem("Settings", IUIEventSystem::eEST_SYSTEM_TO_UI);
	m_EventMap[EUIE_ResolutionChanged] = m_pUIFunctions->RegisterEvent(evtResChanged);
	m_EventMap[EUIE_SoundSettingsChanged] = m_pUIFunctions->RegisterEvent(evtSoundChanged);
	m_EventMap[EUIE_OnGetResolutions] = m_pUIFunctions->RegisterEvent(evtGetRes);

	// override descriptions
	evtResChanged.sDisplayName = "SetResolution";
	evtResChanged.sDesc = "Call this to set resolution";
	evtSoundChanged.sDisplayName = "SetSound";
	evtSoundChanged.sDesc = "Call this to set sound settings";
	evtGetRes.sDisplayName = "GetResolutionList";
	evtGetRes.sDesc = "Trigger this node to get all resolutions. This will trigger the \"Events:Settings:OnResolutions\" node.";
	evtGetRes.IsDynamic = false;

	// events that can be sent from UI flowgraphs to this class
	m_pUIEvents = gEnv->pFlashUI->CreateEventSystem("Settings", IUIEventSystem::eEST_UI_TO_SYSTEM);
	s_EventDispatcher.RegisterEvent(m_pUIEvents, evtResChanged, &CUISettings::OnSetResolution);
	s_EventDispatcher.RegisterEvent(m_pUIEvents, evtSoundChanged, &CUISettings::OnSetSoundSettings);
	s_EventDispatcher.RegisterEvent(m_pUIEvents, evtGetRes, &CUISettings::OnGetResolutions);
	m_pUIEvents->RegisterListener(this);


	// register as IGameFrameworkListener
	gEnv->pGame->GetIGameFramework()->RegisterListener(this, "CUISettings", FRAMEWORKLISTENERPRIORITY_HUD);
}

CUISettings::~CUISettings()
{
	if (m_pUIEvents) m_pUIEvents->UnregisterListener(this);
	if (gEnv->pGame && gEnv->pGame->GetIGameFramework()) gEnv->pGame->GetIGameFramework()->UnregisterListener(this);
}

void CUISettings::OnPostUpdate(float fDeltaTime)
{
	static int rX = m_pRXVar->GetIVal();
	static int rY = m_pRYVar->GetIVal();
	static int fs = m_pFSVar->GetIVal();
	if (rX != m_pRXVar->GetIVal() || rY != m_pRYVar->GetIVal() || fs != m_pFSVar->GetIVal())
	{
		SendResChange();
		rX = m_pRXVar->GetIVal();
		rY = m_pRYVar->GetIVal();
		fs = m_pFSVar->GetIVal();
	}

	static float music = m_pMusicVar->GetFVal();
	static float sfx = m_pSFxVar->GetFVal();
	static float video = m_pVideoVar->GetFVal();
	if (music != m_pMusicVar->GetFVal() || sfx != m_pSFxVar->GetFVal() || video != m_pVideoVar->GetFVal())
	{
		SendSoundChange();
		music = m_pMusicVar->GetFVal();
		sfx = m_pSFxVar->GetFVal();
		video = m_pVideoVar->GetFVal();
	}
}

// ui events
void CUISettings::OnEvent( const SUIEvent& event )
{
	s_EventDispatcher.Dispatch( this, event );
}

void CUISettings::OnSetResolution( const SUIEvent& event )
{
#if !defined(PS3) && !defined(XENON)
	int resX, resY;
	bool fs;
	if (event.args.GetArg(0, resX) && event.args.GetArg(1, resY) && event.args.GetArg(2, fs))
	{
		m_pRXVar->Set(resX);
		m_pRYVar->Set(resY);
		m_pFSVar->Set(fs);
		return;
	}
	assert(false);
#endif
}

void CUISettings::OnSetSoundSettings( const SUIEvent& event )
{
	float music, sfx, video;
	if (event.args.GetArg(0, music) && event.args.GetArg(1, sfx) && event.args.GetArg(2, video))
	{
		m_pMusicVar->Set(music);
		m_pSFxVar->Set(sfx);
		m_pVideoVar->Set(video);
		return;
	}
	assert(false);
}

void CUISettings::OnGetResolutions( const SUIEvent& event )
{
	SendResolutions();
	SendResChange();
	SendSoundChange();
}

// ui functions
void CUISettings::SendResolutions()
{
	SUIArguments args;
	for (TResolutions::iterator it = m_Resolutions.begin(); it != m_Resolutions.end(); ++it)
	{
		args.AddArgument(it->first);
		args.AddArgument(it->second);
	}
	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_OnGetResolutions], args));
}

void CUISettings::SendResChange()
{
	SUIArguments args;
	args.AddArgument(m_pRXVar->GetIVal());
	args.AddArgument(m_pRYVar->GetIVal());
	args.AddArgument(m_pFSVar->GetIVal() != 0);
	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_ResolutionChanged], args));
}

void CUISettings::SendSoundChange()
{
	SUIArguments args;
	args.AddArgument(m_pMusicVar->GetFVal());
	args.AddArgument(m_pSFxVar->GetFVal());
	args.AddArgument(m_pVideoVar->GetFVal());
	m_pUIFunctions->SendEvent(SUIEvent(m_EventMap[EUIE_SoundSettingsChanged], args));
}