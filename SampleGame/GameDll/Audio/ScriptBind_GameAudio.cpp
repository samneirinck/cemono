/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script Binding for G04 GameAudio

-------------------------------------------------------------------------
History:
- 18:08:2008 : Created by TomasN
*************************************************************************/
#include "StdAfx.h"
#include "ScriptBind_GameAudio.h"
#include "GameAudio.h"
//#include "../CryAction/IAnimationGraph.h"
//#include "Game.h"
//#include "Menus/FlashMenuObject.h"


//------------------------------------------------------------------------
CScriptBind_GameAudio::CScriptBind_GameAudio(CGameAudio *pGameAudio)
{
	m_pSystem			= gEnv->pSystem;
	m_pSS					= gEnv->pScriptSystem;
	m_pGameAudio = pGameAudio;

	Init(m_pSS, m_pSystem);
	SetGlobalName("GameAudio");

	RegisterMethods();
	RegisterGlobals();
}

//------------------------------------------------------------------------
CScriptBind_GameAudio::~CScriptBind_GameAudio()
{
}

//------------------------------------------------------------------------
void CScriptBind_GameAudio::RegisterGlobals()
{
	m_pSS->SetGlobalValue("GAMEAUDIOSIGNAL_NOSOUND", EGameAudioSignal_NOSIGNAL);
	//m_pSS->SetGlobalValue("GAMEAUDIOSIGNAL_Menu_Activate", EGameAudioSignal_Menu_Activate);
	//m_pSS->SetGlobalValue("GAMEAUDIOSIGNAL_Menu_Deactivate", EGameAudioSignal_Menu_Deactivate);
	m_pSS->SetGlobalValue("GAMEAUDIOSIGNAL_Last", EGameAudioSignal_LAST);

}

//------------------------------------------------------------------------
void CScriptBind_GameAudio::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_GameAudio::

SCRIPT_REG_TEMPLFUNC(SendSignal, "EGameAudioSignals");

#undef SCRIPT_REG_CLASSNAME
}

//------------------------------------------------------------------------
int CScriptBind_GameAudio::SendSignal(IFunctionHandler *pH, int EGameAudioSignals)
{
	if (m_pGameAudio)
	{
		switch(EGameAudioSignals)
		{
		case EGameAudioSignal_NOSIGNAL:
			m_pGameAudio->SendSignal(EGameAudioSignal_NOSIGNAL);
			break;
		//case EGameAudioSignal_Menu_Activate:
		//	m_pGameAudio->SendSignal(EGameAudioSignal_Menu_Activate);
		//	break;
		//case EGameAudioSignal_Menu_Deactivate:
		//	m_pGameAudio->SendSignal(EGameAudioSignal_Menu_Deactivate);
		//	break;
		case EGameAudioSignal_LAST:
			break;
		default:

			break;
		}

	}

	return pH->EndFunction();
}

