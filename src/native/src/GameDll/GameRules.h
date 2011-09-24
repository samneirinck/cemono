#pragma once

#include <IGameRulesSystem.h>

class CGameRules : public CGameObjectExtensionHelper<CGameRules, IGameRules, 64>
{
public:
	CGameRules(void);
	virtual ~CGameRules(void);

	// IGameRules
	virtual bool ShouldKeepClient(int channelId, EDisconnectionCause cause, const char *desc) const override;
	virtual void PrecacheLevel() override;
	virtual void PrecacheLevelResource(const char* resourceName, EGameResourceType resourceType) override;
	virtual XmlNodeRef FindPrecachedXmlFile( const char *sFilename ) override;
	virtual void OnConnect(struct INetChannel *pNetChannel) override;
	virtual void OnDisconnect(EDisconnectionCause cause, const char *desc) override; // notification to the client that he has been disconnected
	virtual bool OnClientConnect(int channelId, bool isReset) override;
	virtual void OnClientDisconnect(int channelId, EDisconnectionCause cause, const char *desc, bool keepClient) override;
	virtual bool OnClientEnteredGame(int channelId, bool isReset) override;
	virtual void OnEntitySpawn(IEntity *pEntity) override;
	virtual void OnEntityRemoved(IEntity *pEntity) override;
	virtual void OnEntityReused(IEntity *pEntity, SEntitySpawnParams &params, EntityId prevId) override;
	virtual void SendTextMessage(ETextMessageType type, const char *msg, uint32 to=eRMI_ToAllClients, int channelId=-1, const char *p0=0, const char *p1=0, const char *p2=0, const char *p3=0) override;
	virtual void SendChatMessage(EChatMessageType type, EntityId sourceId, EntityId targetId, const char *msg) override;
	virtual void ClientSimpleHit(const SimpleHitInfo &simpleHitInfo) override;
	virtual void ServerSimpleHit(const SimpleHitInfo &simpleHitInfo) override;
	virtual void ClientHit(const HitInfo &hitInfo) override;
	virtual void ServerHit(const HitInfo &hitInfo) override;
	virtual int GetHitTypeId(const char *type) const override;
	virtual const char *GetHitType(int id) const override;
	virtual void OnVehicleDestroyed(EntityId id) override;
	virtual void OnVehicleSubmerged(EntityId id, float ratio) override;
	virtual void CreateEntityRespawnData(EntityId entityId) override;
	virtual bool HasEntityRespawnData(EntityId entityId) const override;
	virtual void ScheduleEntityRespawn(EntityId entityId, bool unique, float timer) override;
	virtual void AbortEntityRespawn(EntityId entityId, bool destroyData) override;
	virtual void ScheduleEntityRemoval(EntityId entityId, float timer, bool visibility) override;
	virtual void AbortEntityRemoval(EntityId entityId) override;
	virtual void AddHitListener(IHitListener* pHitListener) override;
	virtual void RemoveHitListener(IHitListener* pHitListener) override;
	virtual bool IsFrozen(EntityId entityId) const override;
	virtual bool OnCollision(const SGameCollision& event) override;
	virtual void ShowStatus() override;
	virtual bool IsTimeLimited() const override;
	virtual float GetRemainingGameTime() const override;
	virtual void SetRemainingGameTime(float seconds) override;
	virtual void ClearAllMigratingPlayers(void) override;
	virtual EntityId SetChannelForMigratingPlayer(const char* name, uint16 channelID) override;
	virtual void StoreMigratingPlayer(IActor* pActor) override;
	//-IGameRules

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
	virtual void Release() override;
	virtual void ProcessEvent( SEntityEvent& event ) override;	

};

