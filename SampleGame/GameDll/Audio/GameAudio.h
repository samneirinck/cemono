/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 
	Header for G04 centralized Game Audio code

-------------------------------------------------------------------------
History:
- 11:08:2008: Created by Tomas Neumann

*************************************************************************/
#include DEVIRTUALIZE_HEADER_FIX(GameAudio.h)

#ifndef __AUDIO_GAMEAUDIO_H__
#define __AUDIO_GAMEAUDIO_H__

//-----------------------------------------------------------------------------------------------------

class CSoundMoods;
class CBattleStatus;
class CScriptBind_GameAudio;
struct IEntity;

//-----------------------------------------------------------------------------------------------------

enum EGameAudioSounds
{
	EGameAudioSound_NOSOUND = 0,
	EGameAudioSound_LAST
};

enum EGameAudioSignals
{
	EGameAudioSignal_NOSIGNAL = 0,
	EGameAudioSignal_LAST
};

struct SGameSound
{
	tSoundID ID;
	bool bLooping;
	bool b3D;
	//int nMassIndex;
	//int nSpeedIndex;
	//int nStrengthIndex;
};

UNIQUE_IFACE struct IGameAudio
{
	virtual void Serialize(TSerialize ser) = 0;
	virtual void Update() = 0;
	virtual void PlaySound(EGameAudioSounds eSound, IEntity *pEntity = NULL, float param = 1.0f, bool stopSound = false) = 0;
	virtual void SendSignal(EGameAudioSignals eSignal) = 0;
};

class CGameAudio : public IGameAudio
{

public:
	CGameAudio();
	~CGameAudio();

	void Serialize(TSerialize ser);
	void Update();

	CSoundMoods* GetSoundMoods() { return m_pSoundMoods; }
	CBattleStatus* GetBattleStatus() { return m_pBattleStatus; }

	// Play a Sound
	void PlaySound(EGameAudioSounds eSound, IEntity *pEntity = NULL, float param = 1.0f, bool stopSound = false);
	void SendSignal(EGameAudioSignals eSignal);

private:

	void Init();

	CSoundMoods *m_pSoundMoods;
	CBattleStatus *m_pBattleStatus;

	SGameSound m_sounds[EGameAudioSound_LAST];

	CScriptBind_GameAudio *m_pScriptBindGameAudio;

};

//-----------------------------------------------------------------------------------------------------

extern CGameAudio *g_pGameAudio;

#define SAFE_GAMEAUDIO_FUNC(func)\
	{	if(g_pGameAudio) g_pGameAudio->func; }

#define SAFE_GAMEAUDIO_SOUNDMOODS_FUNC(func)\
	{	if(g_pGameAudio && g_pGameAudio->GetSoundMoods()) g_pGameAudio->GetSoundMoods()->func; }

#define SAFE_GAMEAUDIO_BATTLESTATUS_FUNC(func)\
	{	if(g_pGameAudio && g_pGameAudio->GetBattleStatus()) g_pGameAudio->GetBattleStatus()->func; }

#define SAFE_GAMEAUDIO_BATTLESTATUS_FUNC_RET(func)\
	((g_pGameAudio && g_pGameAudio->GetBattleStatus()) ? g_pGameAudio->GetBattleStatus()->func : NULL)


#endif

//-----------------------------------------------------------------------------------------------------
