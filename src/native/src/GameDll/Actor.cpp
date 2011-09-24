#include "StdAfx.h"
#include "Actor.h"
#include "Game.h"


CActor::CActor(void)
{
}


CActor::~CActor(void)
{
}

void CActor::SetHealth( float health ) { }
float CActor::GetHealth() const { return 1337; }
int	CActor::GetHealthAsRoundedPercentage() const { return 100; }
void CActor::SetMaxHealth( float maxHealth ) { }
float CActor::GetMaxHealth() const { return 1337; }
int	CActor::GetArmor() const { return 0; }
int	CActor::GetMaxArmor() const { return 0; }
bool CActor::IsFallen() const { return false; }
bool CActor::IsDead() const { return false; }
int	CActor::IsGod() { return false; }
void CActor::Fall(Vec3 hitPos, float time) { }
bool CActor::AllowLandingBob() { return false; }
void CActor::PlayAction(const char *action,const char *extension, bool looping) { }
IAnimationGraphState * CActor::GetAnimationGraphState() { return NULL; }
void CActor::ResetAnimGraph() { }
void CActor::NotifyAnimGraphTransition(const char *anim) { }
void CActor::NotifyAnimGraphInput(int id, const char *value) { }
void CActor::NotifyAnimGraphInput(int id, int value) { }
void CActor::CreateScriptEvent(const char *event,float value,const char *str) { }
bool CActor::BecomeAggressiveToAgent(EntityId entityID) { return false; }
void CActor::SetFacialAlertnessLevel(int alertness) { }
void CActor::RequestFacialExpression(const char* pExpressionName, f32* sequenceLength) { }
void CActor::PrecacheFacialExpression(const char* pExpressionName) { }
EntityId CActor::GetGrabbedEntityId() const { return 0; }
void CActor::HideAllAttachments(bool isHiding) { }
void CActor::SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority) { }
void CActor::SetViewInVehicle(Quat viewRotation) { }
void CActor::SetViewRotation( const Quat &rotation ) {  }
Quat CActor::GetViewRotation() const { return Quat(); }
Vec3 CActor::GetLocalEyePos( int slot) const { return Vec3(); }
void CActor::CameraShake(float angle,float shift,float duration,float frequency,Vec3 pos,int ID,const char* source) { }
IItem* CActor::GetHolsteredItem() const { return NULL; }
void CActor::HolsterItem(bool holster, bool playSelect) { }
IItem* CActor::GetCurrentItem(bool includeVehicle) const { return NULL; }
bool CActor::DropItem(EntityId itemId, float impulseScale, bool selectNext, bool byDeath) { return false; }
IInventory* CActor::GetInventory() const { return NULL; }
void CActor::NotifyCurrentItemChanged(IItem* newItem) { }
IMovementController* CActor::GetMovementController() const { return NULL; }
IEntity* CActor::LinkToVehicle(EntityId vehicleId) { return NULL; }
IEntity* CActor::GetLinkedEntity() const { return NULL; }
uint8 CActor::GetSpectatorMode() const { return 0; }
void CActor::SetSleepTimer(float timer) { }
IMaterial* CActor::GetReplacementMaterial() { return NULL; }
bool CActor::IsThirdPerson() const { return true; }
void CActor::ToggleThirdPerson() { }
void CActor::Release() { }
bool CActor::IsPlayer() const { return true; }
bool CActor::IsClient() const { return true; }
bool CActor::IsMigrating() const { return false; }
void CActor::SetMigrating(bool isMigrating) { }
void CActor::InitLocalPlayer() { }
const char* CActor::GetActorClassName() const { return "CActor"; }
ActorClass CActor::GetActorClass() const { return 0; }
const char* CActor::GetEntityClassName() const { return NULL; }
void CActor::SerializeXML( XmlNodeRef& node, bool bLoading ) { }
void CActor::SerializeLevelToLevel( TSerialize &ser ) { }
void CActor::ProcessEvent( SEntityEvent& event ) { }
IAnimatedCharacter* CActor::GetAnimatedCharacter() { return NULL; }
const IAnimatedCharacter* CActor::GetAnimatedCharacter() const { return NULL; }
void CActor::PlayExactPositioningAnimation( const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance ) { }
void CActor::CancelExactPositioningAnimation() { }
void CActor::PlayAnimation( const char* sAnimationName, bool bSignal ) { }
bool CActor::Respawn()  { return true; }
void CActor::ResetToSpawnLocation() { }
bool CActor::CanBreakGlass() const { return true; }
void CActor::EnableTimeDemo( bool bTimeDemo ) { }
void CActor::SwitchDemoModeSpectator(bool activate) { }
IVehicle* CActor::GetLinkedVehicle() const { return NULL; }
void CActor::OnAIProxyEnabled(bool enabled) { }
void CActor::OnReturnedToPool() { }
void CActor::OnPreparedFromPool() { }
void CActor::MountedGunControllerEnabled(bool val) { }
bool CActor::MountedGunControllerEnabled() const { return false; }
void CActor::GetMemoryUsage(ICrySizer *pSizer) const { }
bool CActor::Init( IGameObject * pGameObject ) 
{ 
	SetGameObject(pGameObject);

	GetGameObject()->BindToNetwork();

	g_pGame->GetIGameFramework()->GetIActorSystem()->AddActor(GetEntityId(), this);

	return true; 
}
void CActor::PostInit( IGameObject * pGameObject ) { }
void CActor::InitClient(int channelId) { }
void CActor::PostInitClient(int channelId) { }
bool CActor::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) { return true; }
void CActor::PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) { }
bool CActor::GetEntityPoolSignature( TSerialize signature ) { return false; }
void CActor::FullSerialize( TSerialize ser ) { }
bool CActor::NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) { return false; }
void CActor::PostSerialize() { }
void CActor::SerializeSpawnInfo( TSerialize ser ) { }
ISerializableInfoPtr CActor::GetSpawnInfo() { return NULL; }
void CActor::Update( SEntityUpdateContext& ctx, int updateSlot ) { }
void CActor::HandleEvent( const SGameObjectEvent& event ) { }
void CActor::SetChannelId(uint16 id) { }
void CActor::SetAuthority( bool auth ) { }
const void * CActor::GetRMIBase() const { return NULL; }
void CActor::PostUpdate( float frameTime ) { }
void CActor::PostRemoteSpawn() { }
