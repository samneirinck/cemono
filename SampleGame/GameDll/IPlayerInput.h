#ifndef __IPLAYERINPUT_H__
#define __IPLAYERINPUT_H__

#pragma once

#include "IAgent.h" // for EStance
#include "ISerialize.h"
#include "IGameObject.h"
// PLAYERPREDICTION
#include "Network/SerializeDirHelper.h"
// ~PLAYERPREDICTION

struct SSerializedPlayerInput
{
	uint8 stance;
	uint8 bodystate;
	Vec3 deltaMovement;
	Vec3 lookDirection;
	// PLAYERPREDICTION
	Vec3 bodyDirection;
	// ~PLAYERPREDICTION
	bool sprint;
	bool leanl;
	bool leanr;
	bool aiming;
	bool usinglookik;
	bool allowStrafing;
	// PLAYERPREDICTION
	bool isDirty;
	// ~PLAYERPREDICTION
	float pseudoSpeed;
	// PLAYERPREDICTION
	Vec3 position;
	uint8 physCounter;
	// ~PLAYERPREDICTION
	float movementValue;

	SSerializedPlayerInput() :
		stance((uint8)STANCE_NULL),
		bodystate(0),
		deltaMovement(ZERO),
		// PLAYERPREDICTION
		position(ZERO),
		// ~PLAYERPREDICTION
		lookDirection(FORWARD_DIRECTION),
		// PLAYERPREDICTION
		bodyDirection(FORWARD_DIRECTION),
		// ~PLAYERPREDICTION
		sprint(false),
		leanl(false),
		leanr(false),
		aiming(false),
		usinglookik(false),
		allowStrafing(true),
		// PLAYERPREDICTION
		isDirty(false),
		// ~PLAYERPREDICTION
		pseudoSpeed(0.0f),
		// PLAYERPREDICTION
		physCounter(0),
		// ~PLAYERPREDICTION
		movementValue(0.0f)
	{
	}

	void Serialize( TSerialize ser )
	{
		// PLAYERPREDICTION
		ser.Value( "stance", stance, 'stnc' );
		//ser.Value( "bodystate", bodystate, 'bdst');
		// note: i'm not sure what some of these parameters mean, but i copied them from the defaults in serpolicy.h
		// however, the rounding mode for this value must ensure that zero gets sent as a zero, not anything else, or things break rather badly
		ser.Value( "deltaMovement", deltaMovement, 'dMov' );
		//ser.Value( "lookDirection", lookDirection, 'dir0' );
		SerializeDirHelper(ser, lookDirection, 'pYaw', 'pElv');
		ser.Value( "sprint", sprint, 'bool' );
		ser.Value( "position", position, 'wrld' );
		ser.Value( "physcounter", physCounter, 'ui4');
		ser.Value( "leanl", leanl, 'bool' );
		ser.Value( "leanr", leanr, 'bool' );
		//ser.Value( "aiming", aiming, 'bool' );
		//ser.Value( "usinglookik", usinglookik, 'bool' );
		//ser.Value( "allowStrafing", allowStrafing, 'bool' );
		ser.Value( "pseudoSpeed", pseudoSpeed, 'unit' );
		ser.Value( "movementValue", movementValue, 'sone');
		//ser.Value("ActionMap", actionMap, NSerPolicy::A_JumpyValue(0.0f, 127.0f, 7));
		// ~PLAYERPREDICTION
	}
};

class CCameraInputHelper;
struct IPlayerInput
{
	// PLAYERPREDICTION
	static const EEntityAspects INPUT_ASPECT = eEA_GameClientD;
	// ~PLAYERPREDICTION

	enum EInputType
	{
		PLAYER_INPUT,
		NETPLAYER_INPUT,
		AI_INPUT,
		DEDICATED_INPUT,
	};

	virtual ~IPlayerInput() {};

	virtual void PreUpdate() = 0;
	virtual void Update() = 0;
	virtual void PostUpdate() = 0;

	virtual void OnAction( const ActionId& action, int activationMode, float value ) = 0;

	virtual void SetState( const SSerializedPlayerInput& input ) = 0;
	virtual void GetState( SSerializedPlayerInput& input ) = 0;

	virtual void Reset() = 0;
	virtual void DisableXI(bool disabled) = 0;

	virtual EInputType GetType() const = 0;

	virtual void GetMemoryUsage(ICrySizer * s) const = 0;

	//this helper computes mode transitions and camera pitch
	virtual CCameraInputHelper *GetCameraInputHelper() const { return NULL; }

	virtual uint32 GetMoveButtonsState() const = 0;
	virtual uint32 GetActions() const = 0;

	virtual void SerializeSaveGame( TSerialize ser ) {};
};

#endif
