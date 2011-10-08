#include "StdAfx.h"
#include "GameRules.h"
#include "Game.h"
#include <IActorSystem.h>


CGameRules::CGameRules(void)
{
}


CGameRules::~CGameRules(void)
{
}

bool CGameRules::ShouldKeepClient(int channelId, EDisconnectionCause cause, const char *desc) const { return false; }
void CGameRules::PrecacheLevel() { }
void CGameRules::PrecacheLevelResource(const char* resourceName, EGameResourceType resourceType) { }
XmlNodeRef CGameRules::FindPrecachedXmlFile( const char *sFilename ) { return NULL; }
void CGameRules::OnConnect(struct INetChannel *pNetChannel) { }
void CGameRules::OnDisconnect(EDisconnectionCause cause, const char *desc) { } // notification to the client that he has been disconnected
bool CGameRules::OnClientConnect(int channelId, bool isReset) 
{ 
	IActor* pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->CreateActor(channelId, "CemonoMainActor", "Actor", Vec3(0.0f), Quat(IDENTITY), Vec3(1,1,1));
	return true; 
}
void CGameRules::OnClientDisconnect(int channelId, EDisconnectionCause cause, const char *desc, bool keepClient) { }
bool CGameRules::OnClientEnteredGame(int channelId, bool isReset)
{ 
	return true; 
}
void CGameRules::OnEntitySpawn(IEntity *pEntity) { }
void CGameRules::OnEntityRemoved(IEntity *pEntity) { }
void CGameRules::OnEntityReused(IEntity *pEntity, SEntitySpawnParams &params, EntityId prevId) { }
void CGameRules::SendTextMessage(ETextMessageType type, const char *msg, uint32 to, int channelId, const char *p0, const char *p1, const char *p2, const char *p3) { }
void CGameRules::SendChatMessage(EChatMessageType type, EntityId sourceId, EntityId targetId, const char *msg) { }
void CGameRules::ClientSimpleHit(const SimpleHitInfo &simpleHitInfo) { }
void CGameRules::ServerSimpleHit(const SimpleHitInfo &simpleHitInfo) { }
void CGameRules::ClientHit(const HitInfo &hitInfo) { }
void CGameRules::ServerHit(const HitInfo &hitInfo) { }
int CGameRules::GetHitTypeId(const char *type) const { return 0; }
const char* CGameRules::GetHitType(int id) const { return ""; }
void CGameRules::OnVehicleDestroyed(EntityId id) { }
void CGameRules::OnVehicleSubmerged(EntityId id, float ratio) { }
void CGameRules::CreateEntityRespawnData(EntityId entityId) { }
bool CGameRules::HasEntityRespawnData(EntityId entityId) const { return false; }
void CGameRules::ScheduleEntityRespawn(EntityId entityId, bool unique, float timer) { }
void CGameRules::AbortEntityRespawn(EntityId entityId, bool destroyData) { }
void CGameRules::ScheduleEntityRemoval(EntityId entityId, float timer, bool visibility) { }
void CGameRules::AbortEntityRemoval(EntityId entityId) { }
void CGameRules::AddHitListener(IHitListener* pHitListener) { }
void CGameRules::RemoveHitListener(IHitListener* pHitListener) { }
bool CGameRules::IsFrozen(EntityId entityId) const { return false; }
bool CGameRules::OnCollision(const SGameCollision& event) { return true; }
void CGameRules::ShowStatus() { }
bool CGameRules::IsTimeLimited() const { return false; }
float CGameRules::GetRemainingGameTime() const { return 1.0f; }
void CGameRules::SetRemainingGameTime(float seconds) { }
void CGameRules::ClearAllMigratingPlayers(void) { }
EntityId CGameRules::SetChannelForMigratingPlayer(const char* name, uint16 channelID) { return 0; }
void CGameRules::StoreMigratingPlayer(IActor* pActor) { }
void CGameRules::GetMemoryUsage(ICrySizer *pSizer) const { }
bool CGameRules::Init( IGameObject * pGameObject ) 
{ 
	SetGameObject(pGameObject);
	GetGameObject()->BindToNetwork();
	g_pGame->GetIGameFramework()->GetIGameRulesSystem()->SetCurrentGameRules(this);
	return true; 
}
void CGameRules::PostInit( IGameObject * pGameObject ) 
{
	GetGameObject()->EnableUpdateSlot(this,0);
}
void CGameRules::InitClient(int channelId) { }
void CGameRules::PostInitClient(int channelId) { }
bool CGameRules::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) { return true; }
void CGameRules::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) { }
bool CGameRules::GetEntityPoolSignature( TSerialize signature ) { return false; }
void CGameRules::FullSerialize( TSerialize ser ) { }
bool CGameRules::NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) { return false; }
void CGameRules::PostSerialize() { }
void CGameRules::SerializeSpawnInfo( TSerialize ser ) { }
ISerializableInfoPtr CGameRules::GetSpawnInfo() { return NULL; }
void CGameRules::Update( SEntityUpdateContext& ctx, int updateSlot ) 
{
	if (gEnv->bServer)
		GetGameObject()->ChangedNetworkState( eEA_GameServerDynamic );
}
void CGameRules::HandleEvent( const SGameObjectEvent& event ) { }
void CGameRules::SetChannelId(uint16 id) { }
void CGameRules::SetAuthority( bool auth ) { }
const void * CGameRules::GetRMIBase() const { return NULL; }
void CGameRules::PostUpdate( float frameTime ) { }
void CGameRules::PostRemoteSpawn() { }
void CGameRules::Release() { }
void CGameRules::ProcessEvent( SEntityEvent& event ) { }
