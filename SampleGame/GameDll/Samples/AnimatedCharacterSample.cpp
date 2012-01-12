////////////////////////////////////////////////////////////////////////////
//
//  Crytek Source File.
//  Copyright (C), Crytek Studios, 1999-2010.
// -------------------------------------------------------------------------
//  File Name        : AnimatedCharacterSample.cpp
//  Version          : v1.00
//  Created          : 9/11/2010 by Pau Novau
//  Description      : Sample entity implemented as a Game Object Extension
//                     that shows how to get started with Animated Character
//                     from a code point of view.
//
//                     It shows how to initialize the Animated Character
//                     extension, get and set values in the Animation Graph,
//                     and make the entity move in a simple way.
//
//                     The input values updated for the graph ( Health and
//                     PseudoSpeed ) have oversimplified calculations: They
//                     are only set so that the graph we're using shows the
//                     entity running or idle depending on how it is moving.
//
//                     See also the AnimatedCharacterSample.lua file and the 
//                     animation graph file specified there for a complete
//                     picture on this sample.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AnimatedCharacterSample.h"

#include "IAnimatedCharacter.h"


//////////////////////////////////////////////////////////////////////////
#define DEFAULT_MODEL_NAME "Objects/Characters/neutral_male/sdk_character_male_v2.cdf"
#define INVALID_ANIMATION_GRAPH_INPUT_ID -1


//////////////////////////////////////////////////////////////////////////
CAnimatedCharacterSample::CAnimatedCharacterSample()
: m_pAnimatedCharacter( NULL )
, m_healthInputId( IAnimationGraph::InputID( INVALID_ANIMATION_GRAPH_INPUT_ID ) )
, m_pseudoSpeedInputId( IAnimationGraph::InputID( INVALID_ANIMATION_GRAPH_INPUT_ID ) )
, m_actualMoveSpeedInputId( IAnimationGraph::InputID( INVALID_ANIMATION_GRAPH_INPUT_ID ) )
{
}


//////////////////////////////////////////////////////////////////////////
CAnimatedCharacterSample::~CAnimatedCharacterSample()
{
	if ( m_pAnimatedCharacter != NULL )
	{
		IGameObject* pGameObject = GetGameObject();
		pGameObject->ReleaseExtension( "AnimatedCharacter" );
	}
}


//////////////////////////////////////////////////////////////////////////
bool CAnimatedCharacterSample::Init( IGameObject* pGameObject )
{
	SetGameObject( pGameObject );

	m_pAnimatedCharacter = static_cast< IAnimatedCharacter* >( pGameObject->AcquireExtension( "AnimatedCharacter" ) );

	pGameObject->EnablePrePhysicsUpdate( ePPU_Always );
	pGameObject->EnablePhysicsEvent( true, eEPE_OnPostStepImmediate );

	return true;
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::PostInit( IGameObject* pGameObject )
{
	Reset( false );
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::UnPhysicalize()
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
void CAnimatedCharacterSample::Physicalize()
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
	pp.nFlagsOR = pef_monitor_poststep;
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
int CAnimatedCharacterSample::GetColliderSurfaceTypeId() const
{
	I3DEngine* p3DEngine = gEnv->p3DEngine;
	assert( p3DEngine != NULL );

	IMaterialManager* pMaterialManager = p3DEngine->GetMaterialManager();
	assert( pMaterialManager != NULL );

	const int surfaceTypeId = pMaterialManager->GetSurfaceTypeIdByName( "mat_player_collider" );
	return surfaceTypeId;
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::Reset( const bool enteringGameMode )
{
	ResetCharacterModel();
	ResetAnimatedCharacter();
	Physicalize();
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::ResetCharacterModel()
{
	IEntity* pEntity = GetEntity();

	const int slot = 0;
	const char* modelFilename = GetCharacterModelName();
	pEntity->LoadCharacter( slot, modelFilename );
}


//////////////////////////////////////////////////////////////////////////
const char* CAnimatedCharacterSample::GetCharacterModelName() const
{
	const char* characterModelName = GetCharacterModelNameFromScriptTable();
	if ( characterModelName != NULL )
	{
		return characterModelName;
	}

	return DEFAULT_MODEL_NAME;
}


//////////////////////////////////////////////////////////////////////////
const char* CAnimatedCharacterSample::GetCharacterModelNameFromScriptTable() const
{
	IEntity* pEntity = GetEntity();

	IScriptTable* pScriptTable = pEntity->GetScriptTable();
	if ( pScriptTable == NULL )
	{
		return NULL;
	}

	SmartScriptTable propertiesTable;
	const bool hasPropertiesTable = pScriptTable->GetValue( "Properties", propertiesTable );
	if ( ! hasPropertiesTable )
	{
		return NULL;
	}

	const char* modelName = NULL;
	const bool hasModelName = propertiesTable->GetValue( "objModel", modelName );
	if ( ! hasModelName )
	{
		return NULL;
	}

	return modelName;
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::ResetAnimatedCharacter()
{
	if ( m_pAnimatedCharacter == NULL )
	{
		return;
	}
	
	m_pAnimatedCharacter->ResetState();
	BindAnimationGraphInputs();
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::BindAnimationGraphInputs()
{
	assert( m_pAnimatedCharacter != NULL );
	IAnimationGraphState* pAnimationGraphState = m_pAnimatedCharacter->GetAnimationGraphState();

	m_healthInputId = pAnimationGraphState->GetInputId( "Health" );
	m_pseudoSpeedInputId = pAnimationGraphState->GetInputId( "PseudoSpeed" );
	m_actualMoveSpeedInputId = pAnimationGraphState->GetInputId( "ActualMoveSpeed" );
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::OnPrePhysicsUpdate()
{
	UpdateMovement();
	UpdateAnimationGraphState();
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::UpdateMovement()
{
	if ( m_pAnimatedCharacter == NULL )
	{
		return;
	}

	// Values here are hardcoded, just to show the minimal setup to make the
	// entity move.
	SCharacterMoveRequest cmr;
	cmr.type = eCMT_Normal;
	cmr.velocity = Vec3( 2, 2, 0 );

	m_pAnimatedCharacter->AddMovement( cmr );
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::UpdateAnimationGraphState()
{
	if ( m_pAnimatedCharacter == NULL )
	{
		return;
	}

	IAnimationGraphState* pAnimationGraphState  = m_pAnimatedCharacter->GetAnimationGraphState();

	UpdateAnimationGraphPseudoSpeed( pAnimationGraphState );
	UpdateAnimationGraphHealth( pAnimationGraphState );
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::UpdateAnimationGraphPseudoSpeed( IAnimationGraphState* pAnimationGraphState )
{
	const float actualMoveSpeed = pAnimationGraphState->GetInputAsFloat( m_actualMoveSpeedInputId );

	float pseudoSpeed = 0;
	if ( 0.01f < actualMoveSpeed )
	{
		pseudoSpeed = 1;
	}

	pAnimationGraphState->SetInput( m_pseudoSpeedInputId, pseudoSpeed );
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::UpdateAnimationGraphHealth( IAnimationGraphState* pAnimationGraphState )
{
	const int healthValue = 100;
	pAnimationGraphState->SetInput( m_healthInputId, healthValue );
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::OnScriptEvent( const char* eventName )
{
	assert( eventName != NULL );

	const bool isOnPropertyChangeEvent = ( strcmp( eventName, "OnPropertyChange" ) == 0 );
	if ( isOnPropertyChangeEvent )
	{
		Reset( false );
	}
}


//////////////////////////////////////////////////////////////////////////
void CAnimatedCharacterSample::ProcessEvent( SEntityEvent& event )
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

	case ENTITY_EVENT_SCRIPT_EVENT:
		{
			const char* eventName = reinterpret_cast< const char* >( event.nParam[ 0 ] );
			OnScriptEvent( eventName );
		}
		break;
	}
}
