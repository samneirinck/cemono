/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 
	Header for G04 Audio BattleStatus 

-------------------------------------------------------------------------
History:
- 11:08:2008: Created by Tomas Neumann based on MarcoC code in Game02 HUD

*************************************************************************/
#ifndef __AUDIO_BATTLESTATUS_H__
#define __AUDIO_BATTLESTATUS_H__

//-----------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------


class CBattleStatus
{

public:
	CBattleStatus();
	~CBattleStatus();

	//	Increases the battle status
	void	TickBattleStatus(float fValue);

	void	Update();
	
	//	Queries the battle status, 0=no battle, 1=full battle
	float	QueryBattleStatus(); 
	float GetBattleRange();

	void Serialize(TSerialize ser);

private:

	float	m_fBattleStatus;
	float m_fBattleStatusDelay;	

};

//-----------------------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------------------
