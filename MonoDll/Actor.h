/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IActor implementation, selectively used by Actor scripts if the
// developer choses to.
//////////////////////////////////////////////////////////////////////////
// 15/05/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <IActorSystem.h>

class CActor : public CGameObjectExtensionHelper<CActor, IActor>
{
public:
	CActor();
	~CActor();

	// IActor
	virtual void	SetHealth( float health ) override {}
	virtual float	GetHealth() const override { return 1.0f; }
	virtual int	GetHealthAsRoundedPercentage() const override { return 1; }
	virtual void	SetMaxHealth( float maxHealth ) override {}
	virtual float	GetMaxHealth() const override { return 1.0f; }
	virtual int	GetArmor() const override { return 0; }
	virtual int	GetMaxArmor() const override { return 0; }

	virtual bool	IsFallen() const override { return false; }
	virtual bool	IsDead() const override { return false; }
	virtual int	IsGod() override { return false; }
	virtual void	Fall(Vec3 hitPos = Vec3(0,0,0), float time = 0.0f) override {}
	virtual bool	AllowLandingBob() override { return false; }

	virtual void	PlayAction(const char *action,const char *extension, bool looping=false) override {}
	virtual IAnimationGraphState * GetAnimationGraphState() override { return NULL; }
	virtual void	ResetAnimGraph() override {}
	virtual void	NotifyAnimGraphTransition(const char *anim) override {}
	virtual void	NotifyAnimGraphInput(int id, const char *value) override {}
	virtual void	NotifyAnimGraphInput(int id, int value) override {}

	virtual void CreateScriptEvent(const char *event,float value,const char *str = NULL) override {}
	virtual bool BecomeAggressiveToAgent(EntityId entityID) override { return false; }

	virtual void SetFacialAlertnessLevel(int alertness) override {}
	virtual void RequestFacialExpression(const char* pExpressionName = NULL, f32* sequenceLength = NULL) override {}
	virtual void PrecacheFacialExpression(const char* pExpressionName) override {}

	virtual EntityId	GetGrabbedEntityId() const override { return 0; }

	virtual void HideAllAttachments(bool isHiding) override {}

	virtual void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority) override {}

	virtual void SetViewInVehicle(Quat viewRotation) override {}
	virtual void SetViewRotation( const Quat &rotation ) override {}
	virtual Quat GetViewRotation() const override { return GetEntity()->GetRotation();}

	//virtual Vec3 GetViewAngleOffset();
	virtual Vec3 GetLocalEyePos( int slot = 0 ) const override { return Vec3(ZERO); }

	virtual void	CameraShake(float angle,float shift,float duration,float frequency,Vec3 pos,int ID,const char* source="") override {}

	virtual IItem *GetHolsteredItem() const override { return NULL; }
	virtual void HolsterItem(bool holster, bool playSelect = true) override {}
	//virtual IItem *GetCurrentItem() const;
	virtual IItem *GetCurrentItem(bool includeVehicle=false) const override { return NULL; }
	virtual bool DropItem(EntityId itemId, float impulseScale=1.0f, bool selectNext=true, bool byDeath=false) override { return false; }
	virtual IInventory *GetInventory() const override { return NULL; }
	virtual void NotifyCurrentItemChanged(IItem* newItem) override {}

	virtual IMovementController *GetMovementController() const override { return NULL; }

	// get currently linked vehicle, or NULL
	virtual IEntity *LinkToVehicle(EntityId vehicleId) override { return NULL; }

	virtual IEntity* GetLinkedEntity() const override { return NULL; }

	virtual uint8 GetSpectatorMode() const override { return 0; }

	virtual void SetSleepTimer(float timer) override {}

	virtual IMaterial *GetReplacementMaterial() override { return NULL; }

	virtual bool	IsThirdPerson() const override { return true; }
	virtual void ToggleThirdPerson() override {}

	//virtual void SendRevive(const Vec3& position, const Quat& orientation, int team, bool clearInventory);

	virtual void	Release() override { delete this; }

	virtual bool IsPlayer() const override { return GetChannelId() != 0; }
	virtual bool IsClient() const override { return m_bClient; }
	virtual bool IsMigrating() const override { return false; }
	virtual void SetMigrating(bool isMigrating) override {}

	virtual void InitLocalPlayer() override {}

	virtual const char *GetActorClassName() const override { return "CActor"; }
	virtual ActorClass GetActorClass() const override { return (ActorClass)0; }

	virtual const char *GetEntityClassName() const override { return GetEntity()->GetClass()->GetName(); }

	virtual void	SerializeXML( XmlNodeRef& node, bool bLoading ) override {}
	virtual void  SerializeLevelToLevel( TSerialize &ser ) override {}
	virtual void	ProcessEvent( SEntityEvent& event ) override {}

	virtual IAnimatedCharacter * GetAnimatedCharacter() override { return NULL; }
	virtual const IAnimatedCharacter * GetAnimatedCharacter() const override { return NULL; }
	virtual void PlayExactPositioningAnimation( const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance ) override {}
	virtual void CancelExactPositioningAnimation() override {}
	virtual void PlayAnimation( const char* sAnimationName, bool bSignal ) override {}

	virtual bool Respawn()  override { return true; }
	virtual void ResetToSpawnLocation() override {}
	virtual bool CanBreakGlass() const override { return true; }
	virtual void EnableTimeDemo( bool bTimeDemo ) override {}

	virtual void SwitchDemoModeSpectator(bool activate) override {}

	virtual IVehicle *GetLinkedVehicle() const override { return NULL; }

	virtual void OnAIProxyEnabled(bool enabled) override {}
	virtual void OnReturnedToPool() override {}
	virtual void OnPreparedFromPool() override {}

	virtual void MountedGunControllerEnabled(bool val) override {}
	virtual bool MountedGunControllerEnabled() const override { return false; }
	// ~IActor

	// IGameObjectExtension
  	virtual void GetMemoryUsage(ICrySizer *pSizer) const override {}
	virtual bool Init( IGameObject * pGameObject ) override;
	virtual void PostInit( IGameObject * pGameObject ) override {}
	virtual void InitClient(int channelId) override {}
	virtual void PostInitClient(int channelId) override {}
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) override { return true; }
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) override {}
	virtual bool GetEntityPoolSignature( TSerialize signature ) override { signature.BeginGroup("Actor"); signature.EndGroup(); return true;}
	virtual void FullSerialize( TSerialize ser ) override {}
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) override { return true; }
	virtual void PostSerialize() override {}
	virtual void SerializeSpawnInfo( TSerialize ser ) override {}
	virtual ISerializableInfoPtr GetSpawnInfo() override { return NULL; }
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot ) override {}
	virtual void HandleEvent( const SGameObjectEvent& event ) override;
	virtual void SetChannelId(uint16 id) override {}
	virtual void SetAuthority( bool auth ) override {}
	virtual const void *GetRMIBase() const override { return NULL; }
	virtual void PostUpdate( float frameTime ) override {}
	virtual void PostRemoteSpawn() override {}
	// ~IGameObjectExtension

protected:

	bool m_bClient;
};

#endif //__ACTOR_H__