////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2011.
// -------------------------------------------------------------------------
//  File name:   LivingEntitySample.cpp
//  Version:     v1.00
//  Created:     26/11/2010 by Pau Novau
//  Description: Simple example of a character that uses the living entity
//               physical representation to handle its navigation in the
//               game world.
//
//               Features minimal animation states and animation state
//               selection logic: Just enough to show a character animating
//               in an idle/walk pose depending on if the entity is moving
//               or not.
//
//               A parametric animation is used as a walking animation, so
//               this sample also modifies the motion parameters for the
//               animations to match the real movement of the entity as
//               closely as it can.
//
//               Additionally, this entity has minimal input control, but
//               it's not the preferred approach for this as it's registering
//               as an InputListener directly instead of using the ActionMap
//               system, which is a more versatile and configurable solution.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "LivingEntitySample.h"
#include <ICryAnimation.h>


//////////////////////////////////////////////////////////////////////////
#define DEFAULT_MODEL_NAME "Objects/Characters/neutral_male/sdk_character_male_v2.cdf" 

#define IDLE_ANIMATION_NAME "relaxed_idle_01"
#define WALK_ANIMATION_NAME "_RELAXED_WALK_NW"


//////////////////////////////////////////////////////////////////////////
CLivingEntitySample::CLivingEntitySample()
: m_moveLocalForward( false )
, m_moveLocalBackward( false )
, m_moveLocalLeft( false )
, m_moveLocalRight( false )
, m_animationState( None )
, m_previousEntityWorldPosition( ZERO )
, m_worldEntityVelocity( ZERO )
, m_localEntityVelocity( ZERO )
{
}


//////////////////////////////////////////////////////////////////////////
CLivingEntitySample::~CLivingEntitySample()
{
	IInput* pInput = gEnv->pInput;
	if ( pInput != NULL )
	{
		pInput->RemoveEventListener( this );
	}
}


//////////////////////////////////////////////////////////////////////////
bool CLivingEntitySample::Init( IGameObject* pGameObject )
{
	SetGameObject( pGameObject );

	IInput* pInput = gEnv->pInput;
	if ( pInput != NULL )
	{
		pInput->AddEventListener( this );
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::PostInit( IGameObject* pGameObject )
{
	Reset( true );
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::Reset( const bool enteringGameMode )
{
	ResetCharacterModel();
	ResetAnimationState();
	Physicalize();

	IGameObject* pGameObject = GetGameObject();
	if ( enteringGameMode )
	{
		pGameObject->EnablePostUpdates( this );
		pGameObject->EnablePrePhysicsUpdate( ePPU_Always );
	}
	else
	{
		pGameObject->DisablePostUpdates( this );
		pGameObject->EnablePrePhysicsUpdate( ePPU_Never );
	}
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::UnPhysicalize()
{
	IEntity* pEntity = GetEntity();

	const Ang3 oldRotation = pEntity->GetWorldAngles();
	const Quat newRotation = Quat::CreateRotationZ( oldRotation.z );
	pEntity->SetRotation( newRotation );

	SEntityPhysicalizeParams pp;
	pp.type = PE_NONE;
	pEntity->Physicalize( pp );
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::Physicalize()
{
	UnPhysicalize();

	// Parameters are hardcoded, as this is not the focus of this sample.
	SEntityPhysicalizeParams pp;

	pe_player_dimensions playerDim;
	pp.pPlayerDimensions = &playerDim;

	pe_player_dynamics playerDyn;
	pp.pPlayerDynamics = &playerDyn;

	pp.type = PE_LIVING;
	pp.nSlot = 0;
	pp.mass = 80;
	pp.nFlagsOR = 0;
	pp.fStiffnessScale = 70;

	playerDim.heightCollider = 1.2f;
	playerDim.sizeCollider = Vec3( 0.4f, 0.4f, 0.2f );
	playerDim.heightPivot = 0;
	playerDim.bUseCapsule = true;
	playerDim.headRadius = 0;
	playerDim.heightEye = 0;
	playerDim.maxUnproj = 0;

	playerDyn.gravity = Vec3( 0, 0, 9.81f );
	playerDyn.kAirControl = 0.9f;
	playerDyn.mass = 80;
	playerDyn.minSlideAngle = 45;
	playerDyn.maxClimbAngle = 50;
	playerDyn.minFallAngle = 50;
	playerDyn.maxVelGround = 16;

	const int surfaceTypeId = GetColliderSurfaceTypeId();
	const bool validSurfaceTypeId = ( surfaceTypeId != 0 );
	if ( validSurfaceTypeId )
	{
		playerDyn.surface_idx = surfaceTypeId;
	}

	IEntity* pEntity = GetEntity();
	pEntity->Physicalize( pp );
}


//////////////////////////////////////////////////////////////////////////
int CLivingEntitySample::GetColliderSurfaceTypeId() const
{
	I3DEngine* p3DEngine = gEnv->p3DEngine;
	assert( p3DEngine != NULL );

	IMaterialManager* pMaterialManager = p3DEngine->GetMaterialManager();
	assert( pMaterialManager != NULL );

	const int surfaceTypeId = pMaterialManager->GetSurfaceTypeIdByName( "mat_player_collider" );
	return surfaceTypeId;
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::ResetCharacterModel()
{
	IEntity* pEntity = GetEntity();

	const int slot = 0;
	const char* modelFilename = DEFAULT_MODEL_NAME;
	pEntity->LoadCharacter( slot, modelFilename );

	ICharacterInstance* pCharacterInstance = pEntity->GetCharacter( slot );
	if ( pCharacterInstance == NULL )
	{
		return;
	}

	ISkeletonAnim* pSkeletonAnim = pCharacterInstance->GetISkeletonAnim();
	if ( pSkeletonAnim == NULL )
	{
		return;
	}

	ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
	if ( pSkeletonPose == NULL )
	{
		return;
	}

 	pSkeletonPose->SetFootAnchoring( 1 );
 	pSkeletonAnim->SetAnimationDrivenMotion( 1 );
 	
	// We will want to set motion parameters directly ourselves for this sample:
	pSkeletonAnim->SetCharEditMode( 1 );
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::ResetAnimationState()
{
	m_animationState = None;
	StartAnimationForCurrentAnimationState();
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::OnPrePhysicsUpdate()
{
	IEntity* pEntity = GetEntity();
	IPhysicalEntity* pPhysEntity = pEntity->GetPhysics();
	if ( pPhysEntity == NULL )
	{
		return;
	}

	// The desired speed is chosen with a value within the motion capabilities 
	// of the walk animation used for this sample.
	const float desiredSpeed = 1.5f;

	const Vec3 desiredLocalDirection = CalculateDesiredLocalDirection();
	const Vec3 desiredLocalVelocity = desiredLocalDirection * desiredSpeed;

	const Quat worldOrientation = pEntity->GetWorldRotation();
	const Vec3 desiredWorldVelocity = worldOrientation * desiredLocalVelocity;

	pe_action_move pam;
	pam.dir = desiredWorldVelocity;
	
	pPhysEntity->Action( &pam );
}


//////////////////////////////////////////////////////////////////////////
Vec3 CLivingEntitySample::CalculateDesiredLocalDirection() const
{
	Vec3 desiredLocalDirection = Vec3( ZERO );
	desiredLocalDirection += ( m_moveLocalForward ) ? Vec3( 0, 1, 0 ) : Vec3( ZERO );
	desiredLocalDirection += ( m_moveLocalBackward ) ? Vec3( 0, -1, 0 ) : Vec3( ZERO );
	desiredLocalDirection += ( m_moveLocalRight ) ? Vec3( 1, 0, 0 ) : Vec3( ZERO );
	desiredLocalDirection += ( m_moveLocalLeft ) ? Vec3( -1, 0, 0 ) : Vec3( ZERO );
	desiredLocalDirection.NormalizeSafe( Vec3( ZERO ) );

	return desiredLocalDirection;
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::PostUpdate( float frameTime )
{
	UpdateEntityVelocities( frameTime );
	UpdateAnimationState( frameTime );
	UpdateAnimationParams( frameTime );
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::UpdateAnimationState( const float frameTime )
{
	const Vec2 localVelocity( m_localEntityVelocity.x, m_localEntityVelocity.y );
	const float speed = localVelocity.GetLength();

	// Overly simplified selection between animation states for sample purposes:
	AnimationState newAnimationState = ( speed < 0.1f ) ? Idle : Walk;
	
	if ( newAnimationState == m_animationState )
	{
		return;
	}

	m_animationState = newAnimationState;
	StartAnimationForCurrentAnimationState();
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::StartAnimationForCurrentAnimationState()
{
	IEntity* pEntity = GetEntity();

	const int slot = 0;
	ICharacterInstance* pCharacterInstance = pEntity->GetCharacter( slot );
	if ( pCharacterInstance == NULL )
	{
		return;
	}

	ISkeletonAnim* pSkeletonAnim = pCharacterInstance->GetISkeletonAnim();
	if ( pSkeletonAnim == NULL )
	{
		return;
	}

	CryCharAnimationParams animationParams;
	animationParams.m_nFlags |= CA_LOOP_ANIMATION;
	animationParams.m_nLayerID = 0;
	animationParams.m_fTransTime = 0.3f;

	const char* animationName = IDLE_ANIMATION_NAME;
	switch ( m_animationState )
	{
	case None:
	case Idle:
		animationName = IDLE_ANIMATION_NAME;
		break;

	case Walk:
		animationName = WALK_ANIMATION_NAME;
		break;
	}

	pSkeletonAnim->StartAnimation( animationName, animationParams );
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::UpdateAnimationParams( const float frameTime )
{
	IEntity* pEntity = GetEntity();

	const int slot = 0;
	ICharacterInstance* pCharacterInstance = pEntity->GetCharacter( slot );
	if ( pCharacterInstance == NULL )
	{
		return;
	}

	ISkeletonAnim* pSkeletonAnim = pCharacterInstance->GetISkeletonAnim();
	if ( pSkeletonAnim == NULL )
	{
		return;
	}

	ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
	if ( pSkeletonPose == NULL )
	{
		return;
	}

	const Vec2 localVelocity( m_localEntityVelocity.x, m_localEntityVelocity.y );
	const float speed = localVelocity.GetLength();
	const float angle = atan2f( -localVelocity.x, localVelocity.y );

	pSkeletonAnim->SetDesiredMotionParam( eMotionParamID_TravelSpeed, speed, frameTime );
	pSkeletonAnim->SetDesiredMotionParam( eMotionParamID_TravelAngle, angle, frameTime );
}


//////////////////////////////////////////////////////////////////////////
bool CLivingEntitySample::OnInputEvent( const SInputEvent& event )
{
	// A more robust approach would be to use ActionMaps for this instead of registering ourselves 
	// as input listeners.

	const bool eventStatePressedOrDown = ( event.state == eIS_Pressed || event.state == eIS_Down );

	switch ( event.keyId )
	{
	case eKI_W:
		m_moveLocalForward = eventStatePressedOrDown;
		break;

	case eKI_S:
		m_moveLocalBackward = eventStatePressedOrDown;
		break;

	case eKI_A:
		m_moveLocalLeft = eventStatePressedOrDown;
		break;

	case eKI_D:
		m_moveLocalRight = eventStatePressedOrDown;
		break;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::ProcessEvent( SEntityEvent& event )
{
	switch ( event.event )
	{
	case ENTITY_EVENT_RESET:
		{
			const bool enteringGameMode = ( event.nParam[ 0 ] == 1 );
			Reset( enteringGameMode );
		}
		break;

	case ENTITY_EVENT_PREPHYSICSUPDATE:
		OnPrePhysicsUpdate();
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
void CLivingEntitySample::UpdateEntityVelocities( const float frameTime )
{
	if ( frameTime == 0 )
	{
		return;
	}
	
	const IEntity* pEntity = GetEntity();
	const Vec3 currentEntityWorldPosition = pEntity->GetWorldPos();

	const Vec3 positionDelta = currentEntityWorldPosition - m_previousEntityWorldPosition;

	m_previousEntityWorldPosition = currentEntityWorldPosition;

	const float invElapsedFrameSeconds = 1.f / frameTime;
	m_worldEntityVelocity = positionDelta * invElapsedFrameSeconds;

	const Quat currentEntityWorldOrientation = pEntity->GetWorldRotation();
	const Quat invCurrentEntityWorldOrientation = currentEntityWorldOrientation.GetInverted();
	m_localEntityVelocity = invCurrentEntityWorldOrientation * m_worldEntityVelocity;
}