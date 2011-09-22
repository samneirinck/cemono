#pragma once

#include <IActorSystem.h>

class CActor : 	public CGameObjectExtensionHelper<CActor, IActor>
{
public:
	CActor(void);
	virtual ~CActor(void);

	// IActor
		virtual void	SetHealth( float health ) override;
		virtual float	GetHealth() const override;
		virtual int		GetHealthAsRoundedPercentage() const override;
		virtual void	SetMaxHealth( float maxHealth ) override;
		virtual float	GetMaxHealth() const override;
		virtual int		GetArmor() const override;
		virtual int		GetMaxArmor() const override;

		virtual bool	IsFallen() const override;
		virtual bool	IsDead() const override;
		virtual int		IsGod() override;
		virtual void	Fall(Vec3 hitPos = Vec3(0,0,0), float time = 0.0f) override;
		virtual bool	AllowLandingBob() override;

		virtual void	PlayAction(const char *action,const char *extension, bool looping=false) override;
		virtual IAnimationGraphState * GetAnimationGraphState() override;
		virtual void	ResetAnimGraph() override;
		virtual void	NotifyAnimGraphTransition(const char *anim) override;
		virtual void	NotifyAnimGraphInput(int id, const char *value) override;
		virtual void	NotifyAnimGraphInput(int id, int value) override;

		virtual void CreateScriptEvent(const char *event,float value,const char *str = NULL) override;
		virtual bool BecomeAggressiveToAgent(EntityId entityID) override;

		virtual void SetFacialAlertnessLevel(int alertness) override;
		virtual void RequestFacialExpression(const char* pExpressionName = NULL, f32* sequenceLength = NULL) override;
		virtual void PrecacheFacialExpression(const char* pExpressionName) override;

		virtual EntityId	GetGrabbedEntityId() const override;

		virtual void HideAllAttachments(bool isHiding) override;

		virtual void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority) override;

		virtual void SetViewInVehicle(Quat viewRotation) override;
		virtual void SetViewRotation( const Quat &rotation ) override;
		virtual Quat GetViewRotation() const override;

		//virtual Vec3 GetViewAngleOffset();
		virtual Vec3 GetLocalEyePos( int slot = 0 ) const override;

		virtual void	CameraShake(float angle,float shift,float duration,float frequency,Vec3 pos,int ID,const char* source="") override;

		virtual IItem* GetHolsteredItem() const override;
		virtual void HolsterItem(bool holster, bool playSelect = true) override;
		//virtual IItem *GetCurrentItem() const;
		virtual IItem* GetCurrentItem(bool includeVehicle=false) const override;
		virtual bool DropItem(EntityId itemId, float impulseScale=1.0f, bool selectNext=true, bool byDeath=false) override;
		virtual IInventory *GetInventory() const override;
		virtual void NotifyCurrentItemChanged(IItem* newItem) override;

		virtual IMovementController * GetMovementController() const override;

		// get currently linked vehicle, or NULL
		virtual IEntity *LinkToVehicle(EntityId vehicleId) override;

		virtual IEntity* GetLinkedEntity() const override;

		virtual uint8 GetSpectatorMode() const override;

		virtual void SetSleepTimer(float timer) override;

		virtual IMaterial *GetReplacementMaterial() override;

		virtual bool	IsThirdPerson() const override;
		virtual void ToggleThirdPerson() override;

	//virtual void SendRevive(const Vec3& position, const Quat& orientation, int team, bool clearInventory);

	virtual void	Release() override;

	virtual bool IsPlayer() const override;
	virtual bool IsClient() const override;
	virtual bool IsMigrating() const override;
	virtual void SetMigrating(bool isMigrating) override;

	virtual void InitLocalPlayer() override;

	virtual const char* GetActorClassName() const override;
	virtual ActorClass GetActorClass() const override;

	virtual const char* GetEntityClassName() const override;

	virtual void	SerializeXML( XmlNodeRef& node, bool bLoading ) override;
	virtual void  SerializeLevelToLevel( TSerialize &ser ) override;
	virtual void	ProcessEvent( SEntityEvent& event ) override;

	virtual IAnimatedCharacter * GetAnimatedCharacter() override;
	virtual const IAnimatedCharacter * GetAnimatedCharacter() const override;
	virtual void PlayExactPositioningAnimation( const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance ) override;
	virtual void CancelExactPositioningAnimation() override;
	virtual void PlayAnimation( const char* sAnimationName, bool bSignal ) override;

	virtual bool Respawn()  override;
	virtual void ResetToSpawnLocation() override;
	virtual bool CanBreakGlass() const override;
	virtual void EnableTimeDemo( bool bTimeDemo ) override;

	virtual void SwitchDemoModeSpectator(bool activate) override;

	// IVehicle
	virtual IVehicle *GetLinkedVehicle() const override;

	virtual void OnAIProxyEnabled(bool enabled) override;
	virtual void OnReturnedToPool() override;
	virtual void OnPreparedFromPool() override;

  virtual void MountedGunControllerEnabled(bool val) override;
  virtual bool MountedGunControllerEnabled() const override;

	//-IActor

  // IGameObjectExtension
  	virtual void GetMemoryUsage(ICrySizer *pSizer) const override;
	virtual bool Init( IGameObject * pGameObject ) override;
	virtual void PostInit( IGameObject * pGameObject ) override;
	virtual void InitClient(int channelId) override;
	virtual void PostInitClient(int channelId) override;
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) override;
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) override;
	virtual bool GetEntityPoolSignature( TSerialize signature ) override;
	virtual void FullSerialize( TSerialize ser ) override;
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) override;
	virtual void PostSerialize() override;
	virtual void SerializeSpawnInfo( TSerialize ser ) override;
	virtual ISerializableInfoPtr GetSpawnInfo() override;
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot ) override;
	virtual void HandleEvent( const SGameObjectEvent& event ) override;
	virtual void SetChannelId(uint16 id) override;
	virtual void SetAuthority( bool auth ) override;
	virtual const void * GetRMIBase() const override;
	virtual void PostUpdate( float frameTime ) override;
	virtual void PostRemoteSpawn() override;

};

