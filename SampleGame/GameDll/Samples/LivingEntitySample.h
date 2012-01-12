////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2011.
// -------------------------------------------------------------------------
//  File name:   LivingEntitySample.h
//  Version:     v1.00
//  Created:     26/11/2010 by Pau Novau
//  Description: Simple example of a character that uses the living entity
//               physical representation to handle its navigation in the
//               game world.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __LIVING_ENTITY_SAMPLE__H__
#define __LIVING_ENTITY_SAMPLE__H__

#include <IGameObject.h>

class CLivingEntitySample
	: public CGameObjectExtensionHelper< CLivingEntitySample, IGameObjectExtension >
	, IInputEventListener
{
public:
	CLivingEntitySample();
	virtual ~CLivingEntitySample();

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
	virtual void PostUpdate( float frameTime );
	virtual void PostRemoteSpawn() {}
	virtual void HandleEvent( const SGameObjectEvent& event ) {}
	virtual void ProcessEvent( SEntityEvent& event );
	virtual void SetChannelId( uint16 id ) {}
	virtual void SetAuthority( bool auth ) {}
	virtual void GetMemoryUsage( ICrySizer* s ) const { s->Add( *this ); }
	// ~IGameObjectExtension

	// IInputEventListener
	virtual bool OnInputEvent( const SInputEvent& event );
	// ~IInputEventListener

protected:
	void Reset( const bool enteringGameMode );
	void ResetCharacterModel();
	void ResetAnimationState();

	void OnPrePhysicsUpdate();

	void Physicalize();
	void UnPhysicalize();

	int GetColliderSurfaceTypeId() const;

	void UpdateAnimationState( const float frameTime );
	void UpdateAnimationParams( const float frameTime );

	void UpdateEntityVelocities( const float frameTime );

	Vec3 CalculateDesiredLocalDirection() const;

	void StartAnimationForCurrentAnimationState();

private:
	bool m_moveLocalForward;
	bool m_moveLocalBackward;
	bool m_moveLocalLeft;
	bool m_moveLocalRight;

	typedef enum
	{
		None,
		Idle,
		Walk
	} AnimationState;
	AnimationState m_animationState;

	Vec3 m_previousEntityWorldPosition;
	Vec3 m_worldEntityVelocity;
	Vec3 m_localEntityVelocity;
};

#endif