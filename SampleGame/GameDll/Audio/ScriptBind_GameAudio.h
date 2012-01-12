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
#ifndef __SCRIPTBIND_GAMEAUDIO_H__
#define __SCRIPTBIND_GAMEAUDIO_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IScriptSystem.h>
#include <ScriptHelpers.h>


struct ISystem;
class CGameAudio;

class CScriptBind_GameAudio : public CScriptableBase
{
public:
	CScriptBind_GameAudio(CGameAudio *pGameAudio);
	virtual ~CScriptBind_GameAudio();

protected:
	//int SetMusicState(IFunctionHandler *pH, float fIntensity, float fBoredom);

	// <title SendSignal>
	// Syntax: GameAudio.SendSignal( int eGameAudioSignal )
	// Arguments:
	//		eGameAudioSignal - Signal to be sent.
	// Description:
	//		Sends the signal for the game audio.
	int SendSignal(IFunctionHandler *pH, int eGameAudioSignal);

	//int StartLogic(IFunctionHandler *pH);
	//int StopLogic(IFunctionHandler *pH);
	//int PauseGame(IFunctionHandler *pH, bool pause);

private:
	void RegisterGlobals();
	void RegisterMethods();

	ISystem						*m_pSystem;
	IScriptSystem			*m_pSS;
	CGameAudio       *m_pGameAudio;
};

#endif //__SCRIPTBIND_GAME_H__