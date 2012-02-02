/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Actor dummy used to forward events to Mono scripts.
//////////////////////////////////////////////////////////////////////////
// 21/01/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ACTOR_CLASS__
#define __MONO_ACTOR_CLASS__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IActorSystem.h>

class CMonoActorClass :
	public CGameObjectExtensionHelper<CMonoActorClass, IActor>,
	public IGameObjectView,
	public IGameObjectProfileManager
{

public:
	CMonoActorClass();
	virtual ~CMonoActorClass();

	// IActor
	virtual void	SetHealth( float health ) { m_health = health; }
	virtual float	GetHealth() const { return m_health; }
	virtual int	GetHealthAsRoundedPercentage() const { return int_round(m_health * 100.0f / GetMaxHealth()); }
	virtual void	SetMaxHealth( float maxHealth ) { m_maxHealth = maxHealth; }
	virtual float	GetMaxHealth() const { return m_maxHealth; }
	virtual int	GetArmor() const { return 0; }
	virtual int	GetMaxArmor() const { return 0; }

	virtual bool	IsFallen() const { return false; }
	virtual bool	IsDead() const { return GetHealthAsRoundedPercentage() == 0; }
	virtual int	IsGod() { return false; }
	virtual void	Fall(Vec3 hitPos = Vec3(0,0,0), float time = 0.0f) {}
	virtual bool	AllowLandingBob() { return true; }

	virtual void	PlayAction(const char *action,const char *extension, bool looping=false) {}
	virtual IAnimationGraphState *GetAnimationGraphState() { return NULL; }
	virtual void	ResetAnimGraph() {}
	virtual void	NotifyAnimGraphTransition(const char *anim) {}
	virtual void	NotifyAnimGraphInput(int id, const char *value) {}
	virtual void	NotifyAnimGraphInput(int id, int value) {}

	virtual void CreateScriptEvent(const char *event,float value,const char *str = NULL) {}
	virtual bool BecomeAggressiveToAgent(EntityId entityID) { return true; }

	virtual void SetFacialAlertnessLevel(int alertness) {}
	virtual void RequestFacialExpression(const char* pExpressionName = NULL, f32* sequenceLength = NULL) {}
	virtual void PrecacheFacialExpression(const char* pExpressionName) {}

	virtual EntityId	GetGrabbedEntityId() const { return 0; }

	virtual void HideAllAttachments(bool isHiding) {}

	virtual void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority) {}

	virtual void SetViewInVehicle(Quat viewRotation) {}
	virtual void SetViewRotation( const Quat &rotation ) { GetEntity()->SetRotation(rotation); }
	virtual Quat GetViewRotation() const { return GetEntity()->GetRotation(); }

	//virtual Vec3 GetViewAngleOffset() {}
	virtual Vec3 GetLocalEyePos( int slot = 0 ) const { return Vec3(0,0,0); }

	virtual void	CameraShake(float angle,float shift,float duration,float frequency,Vec3 pos,int ID,const char* source="") {}

	virtual IItem* GetHolsteredItem() const { return NULL; }
	virtual void HolsterItem(bool holster, bool playSelect = true) {}
	//virtual IItem *GetCurrentItem() const {}
	virtual IItem* GetCurrentItem(bool includeVehicle=false) const { return NULL; }
	virtual bool DropItem(EntityId itemId, float impulseScale=1.0f, bool selectNext=true, bool byDeath=false) { return false; }
	virtual IInventory *GetInventory() const { return NULL; }
	virtual void NotifyCurrentItemChanged(IItem* newItem) {}

	virtual IMovementController * GetMovementController() const { return NULL; }

	// get currently linked vehicle, or NULL
	virtual IEntity *LinkToVehicle(EntityId vehicleId) { return NULL; }

	virtual IEntity* GetLinkedEntity() const { return NULL; }

	virtual uint8 GetSpectatorMode() const { return 0; }

	virtual void SetSleepTimer(float timer) {}

	virtual IMaterial *GetReplacementMaterial() { return NULL; }

	virtual bool	IsThirdPerson() const { return true; }
	virtual void ToggleThirdPerson() {}

	virtual void	Release() { delete this; }

	virtual bool IsPlayer() const { return false; }
	virtual bool IsClient() const { return false; }
	virtual bool IsMigrating() const { return false; }
	virtual void SetMigrating(bool isMigrating) {}

	virtual void InitLocalPlayer() {}

	virtual const char* GetActorClassName() const { return GetEntityClassName(); }
	virtual ActorClass GetActorClass() const { return (ActorClass)0; }

	virtual const char* GetEntityClassName() const { return GetEntity()->GetClass()->GetName(); }

	virtual void	SerializeXML( XmlNodeRef& node, bool bLoading ) {}
	virtual void  SerializeLevelToLevel( TSerialize &ser ) {}
	virtual void	ProcessEvent( SEntityEvent& event ) {}

	virtual IAnimatedCharacter * GetAnimatedCharacter() { return NULL; }
	virtual const IAnimatedCharacter * GetAnimatedCharacter() const { return NULL; }
	virtual void PlayExactPositioningAnimation( const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance ) {}
	virtual void CancelExactPositioningAnimation() {}
	virtual void PlayAnimation( const char* sAnimationName, bool bSignal ) {}

	virtual void EnableTimeDemo( bool bTimeDemo ) {}

	virtual void SwitchDemoModeSpectator(bool activate) {}

	// IVehicle
	virtual IVehicle *GetLinkedVehicle() const { return NULL; }

	virtual void OnAIProxyEnabled(bool enabled) {}
	virtual void OnReturnedToPool() {}
	virtual void OnPreparedFromPool() {}
	// ~IActor

	// IGameObjectExtension
	virtual bool Init( IGameObject* pGameObject );
	virtual void InitClient( int channelId ) {}
	virtual void PostInit( IGameObject* pGameObject );
	virtual void PostInitClient( int channelId ) {}
	virtual bool ReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) { return false; }
	virtual void PostReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) {}
	virtual bool GetEntityPoolSignature( TSerialize signature ) { return false; }
	virtual void FullSerialize( TSerialize ser ) {}
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) { return true; }
	virtual void PostSerialize() {}
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() { return NULL; }
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot ) {}
	virtual void PostUpdate( float frameTime ) {}
	virtual void PostRemoteSpawn() {}
	virtual void HandleEvent( const SGameObjectEvent& event ) {}
	virtual void SetChannelId( uint16 id ) {}
	virtual void SetAuthority( bool auth ) {}
	virtual void GetMemoryUsage( ICrySizer* s ) const { s->Add( *this ); }
	// ~IGameObjectExtension

	// IGameObjectView
	virtual void UpdateView( SViewParams& params ) {}
	virtual void PostUpdateView( SViewParams& params ) {}
	// ~IGameObjectView

	// IGameObjectProfileManager
	virtual bool SetAspectProfile( EEntityAspects aspect, uint8 profile ) { return true; }
	virtual uint8 GetDefaultProfile( EEntityAspects aspect ) { return aspect == eEA_Physics ? eAP_NotPhysicalized : 0; }
	// ~IGameObjectProfileManager

protected:
	int m_scriptId;

	float m_health;
	float m_maxHealth;
};

#endif // __MONO_ACTOR_CLASS__