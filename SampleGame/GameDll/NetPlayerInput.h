#ifndef __NETPLAYERINPUT_H__
#define __NETPLAYERINPUT_H__

#pragma once

#include "IPlayerInput.h"

class CPlayer;

class CNetPlayerInput : public IPlayerInput
{
public:
	CNetPlayerInput( CPlayer * pPlayer );

	// IPlayerInput
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();

	virtual void SetState( const SSerializedPlayerInput& input );
	virtual void GetState( SSerializedPlayerInput& input );

	virtual void OnAction(  const ActionId& action, int activationMode, float value  ) {};

	virtual void Reset();
	virtual void DisableXI(bool disabled);
	// PLAYERPREDICTION
	virtual void ClearXIMovement() {};
	// ~PLAYERPREDICTION
	
	virtual void GetMemoryUsage(ICrySizer * s) const {s->Add(*this);}

	virtual EInputType GetType() const
	{
		return NETPLAYER_INPUT;
	};

	ILINE virtual uint32 GetMoveButtonsState() const { return 0; }
	ILINE virtual uint32 GetActions() const { return 0; }

	// PLAYERPREDICTION
	virtual float GetLastRegisteredInputTime() const { return 0.0f; }
	virtual void SerializeSaveGame( TSerialize ser ) {}
	// ~IPlayerInput

	void GetDesiredVel(const Vec3 &pos, Vec3 &vel) const;

	ILINE bool HasReceivedUpdate() const
	{
		return (m_lastUpdate.GetValue() != 0);
	}
	// ~PLAYERPREDICTION

protected:
	CPlayer * m_pPlayer;
	SSerializedPlayerInput m_curInput;
	// PLAYERPREDICTION
	float CalculatePseudoSpeed() const;

	void DoSetState( const SSerializedPlayerInput& input );
	void UpdateInterpolation();
	void UpdateMoveRequest();

	void InitialiseInterpolation(f32 netPosDist, const Vec3 &desPosOffset, const Vec3 &desiredVelocity, const CTimeValue	&curTime);
	void UpdateErrorSnap(const Vec3 &entPos, const Vec3 &desiredPos, f32 netPosDist, const Vec3 &desPosOffset, const CTimeValue &curTime);


	CTimeValue m_lastUpdate;

	Vec3		m_predictedPosition;

	Vec3		m_lookDir;

	Vec3  m_initialDir;
	CTimeValue	m_netLastUpdate;     	 // Time we last received information

	float		m_netDesiredSpeed;       // Speed
	float		m_netLerpSpeed;          // Cur Lerp speed
	bool		m_passedNetPos;					// Remote has passed the last synched network position
	bool		m_passedPredictionPos;	// Remote has passed the last synched network position
	bool		m_newInterpolation;

	Vec3				m_breadCrumb;
	CTimeValue	m_nextBCTime;     	 
	float				m_blockedTime;
	// ~PLAYERPREDICTION
};


#endif
