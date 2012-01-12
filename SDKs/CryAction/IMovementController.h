/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Control pipe between AI and game objects.
  
 -------------------------------------------------------------------------
  History:
  - 29:11:2005: Created by Craig Tiller

*************************************************************************/

#ifndef __IMOVEMENTCONTROLLER_H__
#define __IMOVEMENTCONTROLLER_H__

#pragma once

#include "IAnimationGraph.h"
#include "IAgent.h" // For enums

// Note:
// IMPORTANT: wherever "Target" is mentioned in this file, we talk
// about a target position, *not* direction.
struct SActorTargetParams
{
	enum type_uninitialized { UNINITIALIZED };

	SActorTargetParams() : 
		location(ZERO), 
		direction(FORWARD_DIRECTION), 
		vehicleSeat(0), 
		speed(-1.0f), 
		directionTolerance(0.0f),
		startArcAngle(0.0f),
		startWidth(0.0f),
		signalAnimation(true), 
		projectEnd(false),
		navSO(false),
		stance(STANCE_NULL), 
		pQueryStart(NULL), 
		pQueryEnd(NULL),
  	triggerUser(eAGTU_AI)
	{
	}

	//non-default constructor to allow CMovementRequest to avoid unnecessary initialization of
	//	vars in SActorTargetParams
	ILINE SActorTargetParams(type_uninitialized) {}

	Vec3 location; // target location
	Vec3 direction;
	string vehicleName;
	int vehicleSeat;
	float speed;
	float directionTolerance; // allowed direction tolerance (radians)
	float startArcAngle; // arc angle used to bend the starting line (radians)
	float startWidth; // width of the starting line
	bool signalAnimation; // true if we're to send a signal at a target, false if we're to set an action
	bool projectEnd;
	bool navSO;
	string animation;
	EStance stance;
	TAnimationGraphQueryID * pQueryStart;
	TAnimationGraphQueryID * pQueryEnd;
	EAnimationGraphTriggerUser triggerUser;
};

class CMovementRequest
{
public:
	ILINE CMovementRequest()
		: m_flags(0),
		m_actorTarget(SActorTargetParams::UNINITIALIZED)
	{
	}

	// was anything set at all in this request?`
	bool IsEmpty()
	{
		return m_flags == 0;
	}

	void SetAlertness(int alertness)
	{
		m_alertness = alertness;
	}

	ILINE int GetAlertness()
	{
		return m_alertness;
	}

	ILINE bool AlertnessChanged()
	{
		bool ret(m_alertnessLast!=m_alertness);
		m_alertnessLast = m_alertness;
		return ret;
	}

	void SetActorTarget( const SActorTargetParams& params )
	{
		m_actorTarget = params;
		SetFlag(eMRF_ActorTarget);
		ClearFlag(eMRF_RemoveActorTarget);
	}

	void ClearActorTarget()
	{
		ClearFlag(eMRF_ActorTarget);
		SetFlag(eMRF_RemoveActorTarget);
	}

	void SetLookTarget( const Vec3& position, float importance = 1.0f )
	{
		m_lookTarget = position;
		m_lookImportance = importance;
		SetFlag(eMRF_LookTarget);
		ClearFlag(eMRF_RemoveLookTarget);
	}

	void ClearLookTarget()
	{
		SetFlag(eMRF_RemoveLookTarget);
		ClearFlag(eMRF_LookTarget);
	}

	void SetLookStyle( ELookStyle eLookStyle )
	{
		m_eLookStyle = eLookStyle;
		SetFlag(eMRF_LookStyle);
		// MTJ: Might set remove flag, but I don't think we need one
	}

	void ClearLookStyle()
	{
		ClearFlag(eMRF_LookStyle);
		m_eLookStyle = (ELookStyle) -1; // Just to help debugging
		// MTJ: Might set remove flag, but I don't think we need one
	}

	bool HasLookStyle()
	{
		return CheckFlag(eMRF_LookStyle);
	}

	ELookStyle GetLookStyle()
	{
		CRY_ASSERT( HasLookStyle() );
		return m_eLookStyle;
	}

	void SetAimTarget( const Vec3& position )
	{
		m_aimTarget = position;
		SetFlag(eMRF_AimTarget);
		ClearFlag(eMRF_RemoveAimTarget);
	}

	void ClearAimTarget()
	{
		ClearFlag(eMRF_AimTarget);
		SetFlag(eMRF_RemoveAimTarget);
	}

	void SetBodyTarget( const Vec3& position )
	{
		m_bodyTarget = position;
		SetFlag(eMRF_BodyTarget);
		ClearFlag(eMRF_RemoveBodyTarget);
	}

	void ClearBodyTarget()
	{
		ClearFlag(eMRF_BodyTarget);
		SetFlag(eMRF_RemoveBodyTarget);
	}

	void SetFireTarget( const Vec3& position )
	{
		m_fireTarget = position;
		SetFlag(eMRF_FireTarget);
		ClearFlag(eMRF_RemoveFireTarget);
	}

	void ClearFireTarget()
	{
		ClearFlag(eMRF_FireTarget);
		SetFlag(eMRF_RemoveFireTarget);
	}

	void SetLean( float lean )
	{
		m_desiredLean = lean;
		SetFlag(eMRF_DesiredLean);
		ClearFlag(eMRF_RemoveDesiredLean);
	}

	void ClearLean()
	{
		ClearFlag(eMRF_DesiredLean);
		SetFlag(eMRF_RemoveDesiredLean);
	}

	void SetPeekOver( float peekOver )
	{
		m_desiredPeekOver = peekOver;
		SetFlag(eMRF_DesiredPeekOver);
		ClearFlag(eMRF_RemoveDesiredPeekOver);
	}

	void ClearPeekOver()
	{
		ClearFlag(eMRF_DesiredPeekOver);
		SetFlag(eMRF_RemoveDesiredPeekOver);
	}


	void SetDesiredSpeed( float speed )
	{
		m_desiredSpeed = speed;
		SetFlag(eMRF_DesiredSpeed);
		ClearFlag(eMRF_RemoveDesiredSpeed);
	}

	void ClearDesiredSpeed()
	{
		ClearFlag(eMRF_DesiredSpeed);
		SetFlag(eMRF_RemoveDesiredSpeed);
	}

	void SetPseudoSpeed( float speed )
	{
		m_pseudoSpeed = speed;
		SetFlag(eMRF_PseudoSpeed);
		ClearFlag(eMRF_RemovePseudoSpeed);
	}

	void ClearPseudoSpeed()
	{
		ClearFlag(eMRF_PseudoSpeed);
		SetFlag(eMRF_RemovePseudoSpeed);
	}

	void SetPrediction( const SPredictedCharacterStates& prediction)
	{
		m_prediction = prediction;
		SetFlag(eMRF_Prediction);
	}

	const SPredictedCharacterStates& GetPrediction() const
	{
		CRY_ASSERT( HasPrediction() );
		return m_prediction;
	}

	void ClearPrediction()
	{
		ClearFlag(eMRF_Prediction);
		SetFlag(eMRF_RemovePrediction);
	}

	void AddDeltaMovement( const Vec3& direction )
	{
		if (!CheckFlag(eMRF_DeltaMovement))
			m_deltaMovement = direction;
		else
			m_deltaMovement += direction;
		SetFlag(eMRF_DeltaMovement);
	}

	void RemoveDeltaMovement()
	{
		ClearFlag(eMRF_DeltaMovement);
	}

	void AddDeltaRotation( const Ang3& rotation )
	{
		if (!CheckFlag(eMRF_DeltaRotation))
			m_deltaRotation = rotation;
		else
			m_deltaRotation += rotation;
		SetFlag(eMRF_DeltaRotation);
	}

	void RemoveDeltaRotation()
	{
		ClearFlag(eMRF_DeltaRotation);
	}

	void SetDistanceToPathEnd( float dist )
	{
		m_distanceToPathEnd = dist;
	}

	void ClearDistanceToPathEnd()
	{
		m_distanceToPathEnd = -1.0f;
	}

	void SetDistanceOffFromPath(float dist)
	{
		m_distanceOffFromPath = dist;
	}

	void ClearDistanceOffFromPath()
	{
		m_distanceOffFromPath = -1.0f;
	}

	void SetJump()
	{
		SetFlag(eMRF_Jump);
	}

	void ClearJump()
	{
		ClearFlag(eMRF_Jump);
	}

	void SetAllowStrafing()
	{
		SetFlag(eMRF_AllowStrafing);
	}

	void SetAllowStrafing( bool allow )
	{
		if (allow)
			SetAllowStrafing();
		else
			ClearAllowStrafing();
	}

	void ClearAllowStrafing()
	{
		ClearFlag(eMRF_AllowStrafing);
	}

	bool AllowStrafing() const
	{
		return CheckFlag(eMRF_AllowStrafing);
	}

	void SetStance( EStance stance )
	{
		m_stance = stance;
		SetFlag(eMRF_Stance);
		ClearFlag(eMRF_RemoveStance);
	}

	void ClearStance()
	{
		ClearFlag(eMRF_Stance);
		SetFlag(eMRF_RemoveStance);
	}

	void SetMoveTarget( const Vec3& pos )
	{
		m_moveTarget = pos;
		SetFlag(eMRF_MoveTarget);
		ClearFlag(eMRF_RemoveMoveTarget);
	}

	void ClearMoveTarget()
	{
		ClearFlag(eMRF_MoveTarget);
		SetFlag(eMRF_RemoveMoveTarget);
	}

	void SetInflectionPoint(const Vec3& pos)
	{
		m_inflectionPoint = pos;
		SetFlag(eMRF_InflectionPoint);
		ClearFlag(eMRF_RemoveInflectionPoint);
	}

	void ClearInflectionPoint()
	{
		ClearFlag(eMRF_InflectionPoint);
		SetFlag(eMRF_RemoveInflectionPoint);
	}

	void SetContext(unsigned int context)
	{
		m_context = context;
		SetFlag(eMRF_Context);
		ClearFlag(eMRF_RemoveContext);
	}

	void ClearContext()
	{
		ClearFlag(eMRF_Context);
		SetFlag(eMRF_RemoveContext);
	}

	void SetDesiredBodyDirectionAtTarget( const Vec3& target )
	{
		m_desiredBodyDirectionAtTarget = target;
		SetFlag(eMRF_DesiredBodyDirectionAtTarget);
		ClearFlag(eMRF_RemoveDesiredBodyDirectionAtTarget);
	}

	void ClearDesiredBodyDirectionAtTarget()
	{
		ClearFlag(eMRF_DesiredBodyDirectionAtTarget);
		SetFlag(eMRF_RemoveDesiredBodyDirectionAtTarget);
	}

	void SetForcedNavigation( const Vec3& pos )
	{
		m_forcedNavigation = pos;
		SetFlag(eMRF_ForcedNavigation);
		ClearFlag(eMRF_RemoveForcedNavigation);
	}

	void ClearForcedNavigation()
	{
		m_forcedNavigation = ZERO;
		ClearFlag(eMRF_ForcedNavigation);
		SetFlag(eMRF_RemoveForcedNavigation);
	}

	bool HasForcedNavigation() const
	{
		return CheckFlag(eMRF_ForcedNavigation);
	}

	bool HasLookTarget() const
	{
		return CheckFlag(eMRF_LookTarget);
	}

	bool RemoveLookTarget() const
	{
		return CheckFlag(eMRF_RemoveLookTarget);
	}

	const Vec3& GetLookTarget() const
	{
		CRY_ASSERT( HasLookTarget() );
		return m_lookTarget;
	}

	float GetLookImportance() const
	{
		CRY_ASSERT( HasLookTarget() );
		return m_lookImportance;
	}

	bool HasAimTarget() const
	{
		return CheckFlag(eMRF_AimTarget);
	}

	bool RemoveAimTarget() const
	{
		return CheckFlag(eMRF_RemoveAimTarget);
	}

	const Vec3& GetAimTarget() const
	{
		CRY_ASSERT( HasAimTarget() );
		return m_aimTarget;
	}

	bool HasBodyTarget() const
	{
		return CheckFlag(eMRF_BodyTarget);
	}

	bool RemoveBodyTarget() const
	{
		return CheckFlag(eMRF_RemoveBodyTarget);
	}

	const Vec3& GetBodyTarget() const
	{
		CRY_ASSERT( HasBodyTarget() );
		return m_bodyTarget;
	}

	bool HasFireTarget() const
	{
		return CheckFlag(eMRF_FireTarget);
	}

	bool RemoveFireTarget() const
	{
		return CheckFlag(eMRF_RemoveFireTarget);
	}

	const Vec3& GetFireTarget() const
	{
		CRY_ASSERT( HasFireTarget() );
		return m_fireTarget;
	}

	bool HasDesiredSpeed() const
	{
		return CheckFlag(eMRF_DesiredSpeed);
	}

	bool RemoveDesiredSpeed() const
	{
		return CheckFlag(eMRF_RemoveDesiredSpeed);
	}

	float GetDesiredSpeed() const
	{
		CRY_ASSERT( HasDesiredSpeed() );
		return m_desiredSpeed;
	}

	bool HasPseudoSpeed() const
	{
		return CheckFlag(eMRF_PseudoSpeed);
	}

	bool RemovePseudoSpeed() const
	{
		return CheckFlag(eMRF_RemovePseudoSpeed);
	}

	bool HasPrediction() const
	{
		return CheckFlag(eMRF_Prediction);
	}

	bool RemovePrediction() const
	{
		return CheckFlag(eMRF_RemovePrediction);
	}

	float GetPseudoSpeed() const
	{
		CRY_ASSERT( HasPseudoSpeed() );
		return m_pseudoSpeed;
	}

	bool ShouldJump() const
	{
		return CheckFlag(eMRF_Jump);
	}

	bool HasStance() const
	{
		return CheckFlag(eMRF_Stance);
	}

	bool RemoveStance() const
	{
		return CheckFlag(eMRF_RemoveStance);
	}

	EStance GetStance() const
	{
		CRY_ASSERT( HasStance() );
		return m_stance;
	}

	bool HasMoveTarget() const
	{
		return CheckFlag(eMRF_MoveTarget);
	}

	bool RemoveMoveTarget() const
	{
		return CheckFlag(eMRF_RemoveMoveTarget);
	}

	const Vec3& GetMoveTarget() const
	{
		CRY_ASSERT( HasMoveTarget() );
		return m_moveTarget;
	}

	bool HasInflectionPoint() const
	{
		return CheckFlag(eMRF_InflectionPoint);
	}

	bool RemoveInflectionPoint() const
	{
		return CheckFlag(eMRF_RemoveInflectionPoint);
	}

	const Vec3& GetInflectionPoint() const
	{
		CRY_ASSERT( HasInflectionPoint() );
		return m_inflectionPoint;
	}

	bool HasDesiredBodyDirectionAtTarget() const
	{
		return CheckFlag(eMRF_DesiredBodyDirectionAtTarget);
	}

	bool RemoveDesiredBodyDirectionAtTarget() const
	{
		return CheckFlag(eMRF_RemoveDesiredBodyDirectionAtTarget);
	}

	const Vec3& GetDesiredBodyDirectionAtTarget() const
	{
		assert(HasDesiredBodyDirectionAtTarget());
		return m_desiredBodyDirectionAtTarget;
	}

	bool HasContext() const
	{
		return CheckFlag(eMRF_Context);
	}

	bool RemoveContext() const
	{
		return CheckFlag(eMRF_RemoveContext);
	}

	const unsigned int GetContext() const
	{
		CRY_ASSERT( HasContext() );
		return m_context;
	}


	const Vec3& GetForcedNavigation() const
	{
		CRY_ASSERT( HasForcedNavigation() );
		return m_forcedNavigation;
	}

	ILINE float GetDistanceToPathEnd() const
	{
		return m_distanceToPathEnd;
	}

	ILINE float GetDistanceOffFromPath() const
	{
		return m_distanceOffFromPath;
	}

	void SetNoAiming()
	{
		SetFlag(eMRF_NoAiming);
	}

	bool HasNoAiming() const
	{
		return CheckFlag(eMRF_NoAiming);
	}

	bool HasDeltaMovement() const
	{
		return CheckFlag(eMRF_DeltaMovement);
	}

	const Vec3& GetDeltaMovement() const
	{
		CRY_ASSERT( HasDeltaMovement() );
		return m_deltaMovement;
	}

	bool HasDeltaRotation() const
	{
		return CheckFlag(eMRF_DeltaRotation);
	}

	const Ang3& GetDeltaRotation() const
	{
		CRY_ASSERT( HasDeltaRotation() );
		return m_deltaRotation;
	}

	bool HasLean() const
	{
		return CheckFlag(eMRF_DesiredLean);
	}

	float GetLean() const
	{
		CRY_ASSERT( HasLean() );
		return m_desiredLean;
	}

	bool RemoveLean() const
	{
		return CheckFlag(eMRF_RemoveDesiredLean);
	}

	bool HasPeekOver() const
	{
		return CheckFlag(eMRF_DesiredPeekOver);
	}

	float GetPeekOver() const
	{
		CRY_ASSERT( HasPeekOver() );
		return m_desiredPeekOver;
	}

	bool RemovePeekOver() const
	{
		return CheckFlag(eMRF_RemoveDesiredPeekOver);
	}

	bool RemoveActorTarget() const
	{
		return CheckFlag(eMRF_RemoveActorTarget);
	}

	bool HasActorTarget() const
	{
		return CheckFlag(eMRF_ActorTarget);
	}

	const SActorTargetParams& GetActorTarget() const
	{
		CRY_ASSERT(CheckFlag(eMRF_ActorTarget));
		return m_actorTarget;
	}

private:

	// Márcio: Changed this from an enum since some compilers don't support 64bit enums
	// and we had ran out of bits!
	typedef uint64 MovementRequestFlags;
	
	// do we have some parameter
	static const uint64 eMRF_LookTarget							= 1ULL << 0;
	static const uint64 eMRF_AimTarget							= 1ULL << 1;
	static const uint64 eMRF_DesiredSpeed						= 1ULL << 2;
	static const uint64 eMRF_Stance									= 1ULL << 3;
	static const uint64 eMRF_MoveTarget							= 1ULL << 4;
	static const uint64 eMRF_DeltaMovement					= 1ULL << 5;
	static const uint64 eMRF_DeltaRotation					= 1ULL << 6;
	static const uint64 eMRF_LookStyle							= 1ULL << 7;
	static const uint64 eMRF_DesiredLean						= 1ULL << 8;
	static const uint64 eMRF_DesiredPeekOver				= 1ULL << 9;
	static const uint64 eMRF_ActorTarget						= 1ULL << 10;
	static const uint64 eMRF_FireTarget							= 1ULL << 11;
	static const uint64 eMRF_PseudoSpeed						= 1ULL << 12;
	static const uint64 eMRF_Prediction							= 1ULL << 13;
	static const uint64 eMRF_ForcedNavigation				= 1ULL << 14;
	static const uint64 eMRF_BodyTarget							= 1ULL << 15;
  static const uint64 eMRF_InflectionPoint				= 1ULL << 16;
	static const uint64 eMRF_Context								= 1ULL << 17;
	static const uint64 eMRF_DesiredBodyDirectionAtTarget	= 1ULL << 18;

	// other flags
	static const uint64 eMRF_Jump										= 1ULL << 19;
	static const uint64 eMRF_NoAiming								= 1ULL << 20;
	static const uint64 eMRF_AllowStrafing					= 1ULL << 21;

	// remove parameter
	static const uint64 eMRF_RemoveLookTarget				= eMRF_LookTarget				<< 31;
	static const uint64 eMRF_RemoveAimTarget				= eMRF_AimTarget				<< 31;
	static const uint64 eMRF_RemoveDesiredSpeed			= eMRF_DesiredSpeed			<< 31;
	static const uint64 eMRF_RemoveStance						= eMRF_Stance						<< 31;
	static const uint64 eMRF_RemoveMoveTarget				= eMRF_MoveTarget				<< 31;
	static const uint64 eMRF_RemoveDesiredLean			= eMRF_DesiredLean			<< 31;
	static const uint64 eMRF_RemoveDesiredPeekOver	= eMRF_DesiredPeekOver	<< 31;
	static const uint64 eMRF_RemoveActorTarget			= eMRF_ActorTarget			<< 31;
	static const uint64 eMRF_RemoveFireTarget				= eMRF_FireTarget				<< 31;
	static const uint64 eMRF_RemovePseudoSpeed			= eMRF_PseudoSpeed			<< 31;
	static const uint64 eMRF_RemovePrediction				= eMRF_Prediction				<< 31;
	static const uint64 eMRF_RemoveForcedNavigation	= eMRF_ForcedNavigation << 31;
	static const uint64 eMRF_RemoveBodyTarget				= eMRF_BodyTarget				<< 31;
	static const uint64 eMRF_RemoveInflectionPoint	= eMRF_InflectionPoint	<< 31;
	static const uint64 eMRF_RemoveContext					= eMRF_Context					<< 31;
	static const uint64 eMRF_RemoveDesiredBodyDirectionAtTarget	= eMRF_DesiredBodyDirectionAtTarget	<< 31;

	ILINE void ClearFlag( MovementRequestFlags flag )
	{
		m_flags &= ~flag;
	}
	ILINE void SetFlag( MovementRequestFlags flag )
	{
		m_flags |= flag;
	}
	ILINE bool CheckFlag( MovementRequestFlags flag ) const
	{
		return (m_flags & flag) != 0;
	}

	uint64 m_flags;
	Vec3 m_lookTarget;
	Vec3 m_aimTarget;
	Vec3 m_bodyTarget;
	Vec3 m_fireTarget;
	float m_desiredSpeed;
	float m_lookImportance;
	float	m_distanceToPathEnd;
	float m_distanceOffFromPath;
	EStance m_stance;
	Vec3 m_moveTarget;
	Vec3 m_inflectionPoint;								// Estimated position of the next move target after reaching the current move target
	Vec3 m_desiredBodyDirectionAtTarget;
	Vec3 m_deltaMovement;
	Ang3 m_deltaRotation;
	float m_desiredLean;
	float m_desiredPeekOver;
	SActorTargetParams m_actorTarget;
	int	m_alertness;
	int m_alertnessLast;
	float m_pseudoSpeed;
	SPredictedCharacterStates	m_prediction;
	Vec3 m_forcedNavigation;
	ELookStyle m_eLookStyle;
	unsigned int m_context;
};

struct SStanceState
{
	SStanceState() :
		pos(ZERO),
		entityDirection(FORWARD_DIRECTION),
		animationBodyDirection(ZERO),
		upDirection(0,0,1),
		weaponPosition(ZERO),
		aimDirection(FORWARD_DIRECTION),
		fireDirection(FORWARD_DIRECTION),
		eyePosition(ZERO),
		eyeDirection(FORWARD_DIRECTION),
		lean(0.0f),
		peekOver(0.0f),
		m_StanceSize  (Vec3Constants<float>::fVec3_Zero, Vec3Constants<float>::fVec3_Zero),
		m_ColliderSize(Vec3Constants<float>::fVec3_Zero, Vec3Constants<float>::fVec3_Zero)
	{
	}

	// Note: All positions a directions are in worldspace.
	Vec3 pos;                     // Position of the character.
	Vec3 entityDirection;
	Vec3 animationBodyDirection;
	Vec3 upDirection;             // Up direction of the character.
	Vec3 weaponPosition;          // Game logic position of the weapon of the character.
	Vec3 aimDirection;            // Direction from the weapon position to aim target, used for representation.
	Vec3 fireDirection;           // Direction from the weapon position to the fire target, used for emitting the bullets.
	Vec3 eyePosition;             // Game logic position of the eye of the character.
	Vec3 eyeDirection;            // Direction from the eye position to the lookat or aim-at target.
	float lean;                   // The amount the character is leaning -1 = left, 1 = right;
	float peekOver;               // The amount the character is peeking over 0 = none, 1 = max;
	AABB m_StanceSize;            // Game logic bounds of the character related to the 'pos'.
	AABB m_ColliderSize;          // The size of only the collider in this stance.
};

struct SMovementState : public SStanceState
{
	SMovementState() :
		SStanceState(),
		fireTarget(ZERO),
		stance(STANCE_NULL),
		animationEyeDirection(ZERO),
		movementDirection(ZERO),
		lastMovementDirection(ZERO),
		desiredSpeed(0.0f),
		minSpeed(0.2f),
		normalSpeed(1.0f),
		maxSpeed(2.0f),
		slopeAngle(0.0f),
		atMoveTarget(false),
		isAlive(true),
		isAiming(false),
		isFiring(false),
		isVisible(false)
	{
	}

	Vec3 fireTarget;						// Target position to fire at, note the direction from weapon to the fire target
															// can be different than aim direction. This value is un-smoothed target set by AI.
	EStance stance;							// Stance of the character.
	Vec3 animationEyeDirection; // Eye direction reported from Animation [used for cinematic look-ats]
	Vec3 movementDirection, lastMovementDirection;
	float desiredSpeed;
	float minSpeed;
	float normalSpeed;
	float maxSpeed;
	float slopeAngle; // Degrees of ground below character (positive when facing uphill, negative when facing downhill).
	bool atMoveTarget;
	bool isAlive;
	bool isAiming;
	bool isFiring;
	bool isVisible;
};

struct SStanceStateQuery
{
	SStanceStateQuery(const Vec3& pos, const Vec3& trg, EStance _stance, float _lean=0.0f, float _peekOver=0.0f, bool _defaultPose=false)
		: stance(_stance)
		, lean(_lean)
		, peekOver(_peekOver)
		, defaultPose(_defaultPose)
		, position(pos)
		, target(trg)
	{
	}

	SStanceStateQuery(EStance _stance, float _lean=0.0f, float _peekOver=0.0f, bool _defaultPose=false)
		: stance(_stance)
		, lean(_lean)
		, peekOver(_peekOver)
		, defaultPose(_defaultPose)
		, position(ZERO)
		, target(ZERO)
	{
	}

	SStanceStateQuery()
		: stance(STANCE_NULL)
		, lean(0.0f)
		, peekOver(0.0f)
		, defaultPose(true)
		, position(ZERO)
		, target(ZERO)
	{
	}

	EStance stance;
	float lean;
	float peekOver;
	bool defaultPose;
	Vec3 position;
	Vec3 target;
};

struct IMovementController
{
	virtual ~IMovementController(){}
	// Description:
	//    Request some movement; 
	//    If the request cannot be fulfilled, returns false, and request
	//    is updated to be a similar request that could be fulfilled
	//    (calling code is then free to inspect this, and call RequestMovement
	//    again to set a new movement)
	virtual bool RequestMovement( CMovementRequest& request ) = 0;
	// Description:
	//    Fetch the current movement state of the entity
	virtual void GetMovementState( SMovementState& state ) = 0;
	// Description:
	//    Returns the description of the stance as if the specified stance would be set right now.
	//		If the parameter 'defaultPose' is set to false, the current aim and look target info is used to
	//		calculate the stance info, else a default pose facing along positive Y-axis is returned.
	//		Returns false if the description cannot be queried.
	virtual bool GetStanceState( const SStanceStateQuery& query, SStanceState& state ) = 0;

	virtual inline Vec2 GetDesiredMoveDir() const { return Vec2(0,1); }
};

#endif
