/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 
	G04 centralized Game Audio code 
	
-------------------------------------------------------------------------
History:
- 11:08:2008: Created by Tomas Neumann

*************************************************************************/
#include "StdAfx.h"

//#include "Game.h"
//#include "GameCVars.h"
#include "GameAudio.h"
#include "BattleStatus.h"
#include "SoundMoods.h"
#include "IGame.h"
#include "IGameFramework.h"
#include "IMusicSystem.h"
#include "ScriptBind_GameAudio.h"

//-----------------------------------------------------------------------------------------------------

CGameAudio *g_pGameAudio = 0;

CGameAudio::CGameAudio() : m_pSoundMoods(0), m_pBattleStatus(0)
{
	g_pGameAudio = this;
	m_pScriptBindGameAudio = new CScriptBind_GameAudio(this);

	Init();
}

//-----------------------------------------------------------------------------------------------------

CGameAudio::~CGameAudio()
{
	if (m_pSoundMoods)
		delete m_pSoundMoods;

	if (m_pBattleStatus)
		delete m_pBattleStatus;

	SAFE_DELETE(m_pScriptBindGameAudio);
}

//-----------------------------------------------------------------------------------------------------

void CGameAudio::Init()
{		
	for(int i = 0; i < EGameAudioSound_LAST; ++i)
	{
		m_sounds[i].ID = INVALID_SOUNDID;
		m_sounds[i].bLooping = false;
		m_sounds[i].b3D = false;
		//m_sounds[i].nMassIndex = -1;
		//m_sounds[i].nSpeedIndex = -1;
		//m_sounds[i].nStrengthIndex = -1;
	}

	if (!m_pSoundMoods)
		m_pSoundMoods = new CSoundMoods();

	
	if (!m_pBattleStatus)
		m_pBattleStatus = new CBattleStatus();

}

//-----------------------------------------------------------------------------------------------------

void CGameAudio::Update()
{		
	if (m_pSoundMoods)
		m_pSoundMoods->Update();


	if (m_pBattleStatus)
		m_pBattleStatus->Update();

}
//-----------------------------------------------------------------------------------------------------

void CGameAudio::Serialize(TSerialize ser)
{
	if (m_pSoundMoods)
		m_pSoundMoods->Serialize(ser);

	if (m_pBattleStatus)
		m_pBattleStatus->Serialize(ser);
}

// add enum of sound you want to play, and add command to game code at appropiate spot
void CGameAudio::PlaySound(EGameAudioSounds eSound, IEntity *pEntity, float param, bool stopSound)
{
	int soundFlag = 0; //localActor will get 2D sounds
	ESoundSemantic eSemantic = eSoundSemantic_None;
	ISound *pSound = NULL;
	bool	setParam = false;
	static string soundName;
	soundName.resize(0);

	switch(eSound)
	{
	case EGameAudioSound_NOSOUND:
		break;
	case EGameAudioSound_LAST:
		break;
	default:
		break;
	}

	//if(!force3DSound && m_pOwner == m_pGameFramework->GetClientActor() && !m_pOwner->IsThirdPerson() && soundName.size())
	//{
	//	if (bAppendPostfix)
	//		soundName.append("_fp");
	//}

	IEntitySoundProxy* pSoundProxy = pEntity ? (IEntitySoundProxy*)pEntity->CreateProxy(ENTITY_PROXY_SOUND) : NULL;

	if (soundName.size())		//get / create or stop sound
	{
		if(m_sounds[eSound].ID != INVALID_SOUNDID)
		{
			if (pSoundProxy)
				pSound = pSoundProxy->GetSound(m_sounds[eSound].ID);
			else
				pSound = gEnv->pSoundSystem->GetSound(m_sounds[eSound].ID);

			if (stopSound)
			{
				if(pSound)
					pSound->Stop();

				m_sounds[eSound].ID = INVALID_SOUNDID;
				return;
			}
		}

		if (!pSound && !stopSound)
		{
			pSound = gEnv->pSoundSystem->CreateSound(soundName, soundFlag);

			if (pSound)
			{
				pSound->SetSemantic(eSemantic);
				//float fTemp = 0.0f;
				m_sounds[eSound].ID = pSound->GetId();
				m_sounds[eSound].bLooping = (pSound->GetFlags() & FLAG_SOUND_LOOP) != 0;
				m_sounds[eSound].b3D = (pSound->GetFlags() & FLAG_SOUND_3D) != 0;
				//m_sounds[eSound].nMassIndex = pSound->GetParam("mass", &fTemp, false);
				//m_sounds[eSound].nSpeedIndex = pSound->GetParam("speed", &fTemp, false);
				//m_sounds[eSound].nStrengthIndex = pSound->GetParam("strength", &fTemp, false);
			}
		}
	}

	if ( pSound )		//set params and play
	{
		//pSound->SetPosition(m_pOwner->GetEntity()->GetWorldPos());

		if(setParam)
		{
			//if (m_sounds[eSound].nMassIndex != -1)
			//	pSound->SetParam(m_sounds[sound].nMassIndex, param);

			//if (m_sounds[eSound].nSpeedIndex != -1)
			//	pSound->SetParam(m_sounds[sound].nSpeedIndex, param);

			//if (m_sounds[eSound].nStrengthIndex != -1)
			//	pSound->SetParam(m_sounds[sound].nStrengthIndex, param);
		}

		if(!(m_sounds[eSound].bLooping && pSound->IsPlaying()))
		{
			if (pSoundProxy)
				pSoundProxy->PlaySound(pSound);
			else
				pSound->Play();
		}
	}
}

// add enum of signal you want to play, and add command to game code at appropiate spot
void CGameAudio::SendSignal(EGameAudioSignals eSignal)
{
/*
	int soundFlag = 0; //localActor will get 2D sounds
	ESoundSemantic eSemantic = eSoundSemantic_None;
	ISound *pSound = NULL;
	bool	setParam = false;
	bool	force3DSound = false;
	bool	bAppendPostfix=true;
	static string soundName;
	soundName.resize(0);

	switch(eSignal)
	{
	case EGameAudioSignal_NOSIGNAL:
		break;
	default:
		break;
	}

*/
}