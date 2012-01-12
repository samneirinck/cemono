////////////////////////////////////////////////////////////////////////////
//
//  Crytek Source File.
//  Copyright (C), Crytek Studios, 1999-2010.
// -------------------------------------------------------------------------
//  File Name        : AnimatedCharacterSample.h
//  Version          : v1.00
//  Created          : 9/11/2010 by Pau Novau
//  Description      : Sample entity implemented as a Game Object Extension
//                     that shows how to get started with Animated Character
//                     from a code point of view.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ANIMATED_CHARACTER_SAMPLE__H__
#define __ANIMATED_CHARACTER_SAMPLE__H__

#include <IGameObject.h>
#include "IAnimationGraph.h"


class CAnimatedCharacterSample
	: public CGameObjectExtensionHelper< CAnimatedCharacterSample, IGameObjectExtension >
{
public:
	CAnimatedCharacterSample();
	virtual ~CAnimatedCharacterSample();

	// IGameObjectExtension
	virtual bool Init( IGameObject* pGameObject );
	virtual void InitClient( int channelId ) {}
	virtual void PostInit( IGameObject* pGameObject );
	virtual void PostInitClient( int channelId ) {}
	virtual bool ReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) { return false; }
	virtual void PostReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) {}
	virtual bool GetEntityPoolSignature( TSerialize signature ) { return false; }
	virtual void Release() { delete this; }
	virtual void FullSerialize( TSerialize ser ) {}
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) { return true; }
	virtual void PostSerialize() {}
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() { return 0; }
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot ) {}
	virtual void PostUpdate( float frameTime ) {}
	virtual void PostRemoteSpawn() {}
	virtual void HandleEvent( const SGameObjectEvent& event ) {}
	virtual void ProcessEvent( SEntityEvent& event );
	virtual void SetChannelId( uint16 id ) {}
	virtual void SetAuthority( bool auth ) {}
	virtual void GetMemoryUsage( ICrySizer* s ) const { s->Add( *this ); }
	// ~IGameObjectExtension

protected:
	void Reset( const bool enteringGameMode );
	void ResetCharacterModel();
	void ResetAnimatedCharacter();
	void BindAnimationGraphInputs();

	void Physicalize();
	void UnPhysicalize();

	void OnPrePhysicsUpdate();
	
	void UpdateMovement();

	void UpdateAnimationGraphState();
	void UpdateAnimationGraphPseudoSpeed( IAnimationGraphState* pAnimationGraphState );
	void UpdateAnimationGraphHealth( IAnimationGraphState* pAnimationGraphState );

	const char* GetCharacterModelName() const;
	const char* GetCharacterModelNameFromScriptTable() const;
	int GetColliderSurfaceTypeId() const;

	void OnScriptEvent( const char* eventName );

private:
	IAnimatedCharacter* m_pAnimatedCharacter;
	IAnimationGraph::InputID m_healthInputId;
	IAnimationGraph::InputID m_pseudoSpeedInputId;
	IAnimationGraph::InputID m_actualMoveSpeedInputId;
};


#endif
