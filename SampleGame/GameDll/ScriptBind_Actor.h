/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Exposes actor functionality to LUA
  
 -------------------------------------------------------------------------
  History:
  - 7:10:2004   14:19 : Created by Márcio Martins

*************************************************************************/
#ifndef __SCRIPTBIND_ACTOR_H__
#define __SCRIPTBIND_ACTOR_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <IScriptSystem.h>
#include <ScriptHelpers.h>

#include <IMonoScriptBind.h>
#include <MonoCommon.h>

struct IGameFramework;
class CActor;

// <title Actor>
// Syntax: Actor
class CScriptBind_Actor 
	: public CScriptableBase
	, public IMonoScriptBind
{
public:
	CScriptBind_Actor(ISystem *pSystem);
	virtual ~CScriptBind_Actor();

	void AttachTo(CActor *pActor);

	//------------------------------------------------------------------------
	// <title DumpActorInfo>
	// Syntax: Actor.DumpActorInfo()
	// Description:
	//		Dumps actor info in the log file.
	virtual int DumpActorInfo(IFunctionHandler *pH);
	// <title SetViewAngleOffset>
	// Syntax: Actor.SetViewAngleOffset()
	// Description:
	//		Sets the offset angle for the view.
	virtual int SetViewAngleOffset(IFunctionHandler *pH);
	// <title GetViewAngleOffset>
	// Syntax: Actor.GetViewAngleOffset()
	// Description:
	//		Gets the offset angle for the view.
	virtual int GetViewAngleOffset(IFunctionHandler *pH);
	// <title Revive>
	// Syntax: Actor.Revive()
	// Description:
	//		Revives the actor.
	virtual int Revive(IFunctionHandler *pH);
	// <title Kill>
	// Syntax: Actor.Kill()
	// Description:
	//		Kills the actor.
	virtual int Kill(IFunctionHandler *pH);
	// <title RagDollize>
	// Syntax: Actor.RagDollize()
	// Description:
	//		Activates the ragdoll death animations for the actor.
	virtual int RagDollize(IFunctionHandler *pH);
	// <title SetStats>
	// Syntax: Actor.SetStats()
	// Description:
	//		Sets somes tats for the actor.
	virtual int SetStats(IFunctionHandler *pH);
	// <title SetParams>
	// Syntax: Actor.SetParams()
	// Description:
	//		Sets the actor parameters passing the parameter table to the actor.
	virtual int SetParams(IFunctionHandler *pH);
	// <title GetParams>
	// Syntax: Actor.GetParams()
	// Description:
	//		Gets the actor parameters.
	virtual int GetParams(IFunctionHandler *pH);
	// <title GetHeadDir>
	// Syntax: Actor.GetHeadDir()
	// Description:
	//		Gets the the actor's head direction.
	virtual int GetHeadDir(IFunctionHandler *pH);
	// <title GetHeadPos>
	// Syntax: Actor.GetHeadPos()
	// Description:
	//		Gets the actor's head position. 
	virtual int GetHeadPos(IFunctionHandler *pH);
	// <title PostPhysicalize>
	// Syntax: Actor.PostPhysicalize()
	// Description:
	//		Forces the physical proxy to be rebuilt.
	virtual int PostPhysicalize(IFunctionHandler *pH);
	// <title GetChannel>
	// Syntax: Actor.GetChannel()
	// Description:
	//		Gets network channel.
	virtual int GetChannel(IFunctionHandler *pH);
	// <title IsPlayer>
	// Syntax: Actor.IsPlayer()
	// Description:
	//		Checks if the actor is the player.
	virtual int IsPlayer(IFunctionHandler *pH);
	// <title IsLocalClient>
	// Syntax: Actor.IsLocalClient()
	// Description:
	//		Checks if the script is runnin on a local client.
	virtual int IsLocalClient(IFunctionHandler *pH);
	// <title GetLinkedVehicleId>
	// Syntax: Actor.GetLinkedVehicleId()
	// Description:
	//		Gets the id of the vehicle linked with the actor.
	virtual int GetLinkedVehicleId(IFunctionHandler *pH);
	// <title LinkToVehicle>
	// Syntax: Actor.LinkToVehicle()
	// Description:
	//		Links the actor to a vehicle.
	virtual int LinkToVehicle(IFunctionHandler *pH);
	// <title LinkToVehicleRemotely>
	// Syntax: Actor.LinkToVehicleRemotely()
	// Description:
	//		Remotely links the actor to a vehicle.
	virtual int LinkToVehicleRemotely(IFunctionHandler *pH);
	// <title LinkToEntity>
	// Syntax: Actor.LinkToEntity()
	// Description:
	//		Links the actor to an entity.
	virtual int LinkToEntity(IFunctionHandler *pH);
	// <title IsGhostPit>
	// Syntax: Actor.IsGhostPit()
	// Arguments:
	// Description:
	//		Checks if the actor in the vehicle is hidden.
	virtual int IsGhostPit(IFunctionHandler *pH);
	// <title IsFlying>
	// Syntax: Actor.IsFlying()
	// Description:
	//		Checks if the actor is flying.
	virtual int IsFlying(IFunctionHandler *pH);

	// <title SetAngles>
	// Syntax: Actor.SetAngles( Ang3 vAngles )
	// Arguments:
	//		vAngles - Rotation angle for the actor.
	// Description:
	//		Sets the rotation angle for the actor.
	virtual int SetAngles(IFunctionHandler *pH,Ang3 vAngles );
	// <title GetAngles>
	// Syntax: Actor.GetAngles()
	// Description:
	//		Gets the rotation angle of the actor.
	virtual int GetAngles(IFunctionHandler *pH);
	// <title AddAngularImpulse>
	// Syntax: Actor.AddAngularImpulse( Ang3 vAngular,float deceleration,float duration )
	// Arguments:
	//		vAngular		- Rotation angle value.
	//		deceleration	- Deceleration of the impulse.
	//		duration		- Duration of the impulse.
	// Description:
	//		Sets and angular impulse to the actor.
	virtual int AddAngularImpulse(IFunctionHandler *pH,Ang3 vAngular,float deceleration,float duration);
	// <title SetViewLimits>
	// Syntax: Actor.SetViewLimits( IFunctionHandler *pH,Vec3 dir,float rangeH,float rangeV )
	// Arguments:
	//		dir		- View direction.
	//		rangeH	- Height range.
	//		rangeV	- Vertical range.
	// Description:
	//		Sets the limit of the view.
	virtual int SetViewLimits(IFunctionHandler *pH,Vec3 dir,float rangeH,float rangeV);
	// <title PlayAction>
	// Syntax: Actor.PlayAction(IFunctionHandler *pH,const char *action,const char *extension)
	// Arguments:
	//		action		- Action name.
	//		extension	- Extension name.
	// Description:
	//		Plays a specific action.
	virtual int PlayAction(IFunctionHandler *pH,const char *action,const char *extension);
	// <title SimulateOnAction>
	// Syntax: Actor.SimulateOnAction( IFunctionHandler *pH,const char *action,int mode,float value )
	// Arguments:
	//		action	- Action name.
	//		mode	- Action activation mode.
	//		value	- Value of the action.
	// Description:
	virtual int SimulateOnAction(IFunctionHandler *pH,const char *action,int mode,float value);
	// <title SetMovementTarget>
	// Syntax: Actor.SetMovementTarget( Vec3 pos, Vec3 target, Vec3 up, float speed )
	// Arguments:
	//		pos		- Position.
	//		target	- Target position
	//		up		- Up vector.
	//		speed	- Speed of the movement.
	// Description:
	//		Sets the target movement.
	virtual int SetMovementTarget(IFunctionHandler *pH, Vec3 pos, Vec3 target, Vec3 up, float speed);
	// <title CameraShake>
	// Syntax: Actor.CameraShake( float amount,float duration,float frequency,Vec3 pos )
	// Arguments:
	//		amount		- Amount of the shake movement.
	//		duration	- Duration of the shake movement.
	//		frequency	- Frequency of the shake movement.
	//		pos			- Position of the shake movement.
	// Description:
	//		Applies a shake movement to the camera.
	virtual int CameraShake(IFunctionHandler *pH,float amount,float duration,float frequency,Vec3 pos);
	// <title SetViewShake>
	// Syntax: Actor.SetViewShake( Ang3 shakeAngle, Vec3 shakeShift, float duration, float frequency, float randomness )
	// Arguments:
	//		shakeAngle	- Angle of the shake movement.
	//		shakeShift	- Shift of the shake movement.
	//		duration	- Duration of the shake movement.
	//		frequency	- Frequency of the shake movement.
	//		randomness	- Randomness amount.
	// Description:
	//		Sets the shake movement for the view.
	virtual int SetViewShake(IFunctionHandler *pH, Ang3 shakeAngle, Vec3 shakeShift, float duration, float frequency, float randomness);
	// <title VectorToLocal>
	// Syntax: Actor.VectorToLocal()
	// Description:
	//		Converts a vector to the local coordinates system.
	virtual int VectorToLocal(IFunctionHandler *pH);
	// <title EnableAspect>
	// Syntax: Actor.EnableAspect( const char *aspect, bool enable )
	// Arguments:
	//		aspect - Aspect name.
	//		enable - True to enable the aspect, false otherwise.	
	// Description:
	//		Enables/Disables the specified aspect.
	virtual int EnableAspect(IFunctionHandler *pH, const char *aspect, bool enable);
	// <title SetExtensionActivation>
	// Syntax: Actor.SetExtensionActivation( const char *extension, bool activation )
	// Arguments:
	// Description:
	virtual int SetExtensionActivation(IFunctionHandler *pH, const char *extension, bool activation);
	// <title SetExtensionParams>
	// Syntax: Actor.SetExtensionParams( const char *extension, SmartScriptTable params )
	// Arguments:
	//		extension	- Extension name.
	//		params		- Parameters to be set for the extension.
	// Description:
	//		Sets some parameters to the specified extension.
	virtual int SetExtensionParams(IFunctionHandler* pH, const char *extension, SmartScriptTable params);
	// <title GetExtensionParams>
	// Syntax: Actor.GetExtensionParams( const char *extension, SmartScriptTable params )
	// Arguments:
	//		extension	- Extension name.
	//		params		- Parameters to be set for the extension.
	// Description:
	//		Gets the parameters for the specified extension.
	virtual int GetExtensionParams(IFunctionHandler* pH, const char *extension, SmartScriptTable params);



	// These functions are multiplayer safe
	// These should be called by the server to set ammo on the clients

	// <title SetInventoryAmmo>
	// Syntax: Actor.SetInventoryAmmo( const char *ammo, int amount )
	// Arguments:
	//		ammo	- Ammunition string name.
	//		amount	- Amount of the specified ammunition.
	// Description:
	//		Sets the amount of a specified ammunition in the inventory. 
	virtual int SetInventoryAmmo(IFunctionHandler *pH, const char *ammo, int amount);
	// <title AddInventoryAmmo>
	// Syntax: Actor.AddInventoryAmmo( const char *ammo, int amount )
	// Arguments:
	//		ammo	- Ammunition string name.
	//		amount	- Amount of the specified ammunition.
	// Description:
	//		Adds the amount of a specified ammunition in the inventory. 
	virtual int AddInventoryAmmo(IFunctionHandler *pH, const char *ammo, int amount);
	// <title GetInventoryAmmo>
	// Syntax: Actor.GetInventoryAmmo( const char *ammo )
	// Arguments:
	//		ammo	- Ammunition string name.
	// Description:
	//		Gets the amount of a specified ammunition in the inventory. 
	virtual int GetInventoryAmmo(IFunctionHandler *pH, const char *ammo);

	// <title SetHealth>
	// Syntax: Actor.SetHealth( float health )
	// Arguments:
	//		health - Health amount.
	// Description:
	//		Sets the amount of health.
	virtual int SetHealth(IFunctionHandler *pH, float health);
	// <title SetMaxHealth>
	// Syntax: Actor.SetMaxHealth( float health )
	// Arguments:
	//		health - Health amount.
	// Description:
	//		Sets the maximum of health.
	virtual int SetMaxHealth(IFunctionHandler *pH, float health);
	// <title GetHealth>
	// Syntax: Actor.GetHealth()
	// Description:
	//		Gets the amount of health.
	virtual int GetHealth(IFunctionHandler *pH);
	// <title GetMaxHealth>
	// Syntax: Actor.GetMaxHealth()
	// Description:
	//		Gets the maximum amount of health.
	virtual int GetMaxHealth(IFunctionHandler *pH);
	// <title GetArmor>
	// Syntax: Actor.GetArmor()
	// Description:
	//		Gets the amount of armor.
	virtual int GetArmor(IFunctionHandler *pH);
	// <title GetMaxArmor>
	// Syntax: Actor.GetMaxArmor()
	// Description:
	//		Gets the maximum amount of armor.
	virtual int GetMaxArmor(IFunctionHandler *pH);
	// <title GetFrozenAmount>
	// Syntax: Actor.GetFrozenAmount()
	// Description:
	//		Gets the frozen amount.
	virtual int GetFrozenAmount(IFunctionHandler *pH);
	// <title AddFrost>
	// Syntax: Actor.AddFrost( float frost )
	// Arguments:
	//		frost - Frost amount.
	// Description:
	//		Adds the specified frost amount.
	virtual int AddFrost(IFunctionHandler *pH, float frost);
	// <title DamageInfo>
	// Syntax: Actor.DamageInfo( ScriptHandle shooter, ScriptHandle target, ScriptHandle weapon, float damage, const char *damageType )
	// Arguments:
	//		shooter		- Shooter that caused the damage.
	//		target		- Target that suffered the damage.
	//		weapon		- Weapon that caused the damage.
	//		damage		- Amount of damage caused.
	//		damageType	- Type of damage caused.
	// Description:
	//		Gets damage info.
	virtual int DamageInfo(IFunctionHandler *pH, ScriptHandle shooter, ScriptHandle target, ScriptHandle weapon, float damage, const char *damageType);

	// <title SetPhysicalizationProfile>
	// Syntax: Actor.SetPhysicalizationProfile( const char *profile )
	// Arguments:
	//		profile - Profile for the physicalization.
	// Description:
	//		Sets the physicalization profile.
	virtual int SetPhysicalizationProfile(IFunctionHandler *pH, const char *profile);
	// <title GetPhysicalizationProfile>
	// Syntax: Actor.GetPhysicalizationProfile()
	// Description:
	//	Gets the physicalization profile.
	virtual int GetPhysicalizationProfile(IFunctionHandler *pH);

	// <title QueueAnimationState>
	// Syntax: Actor.QueueAnimationState( const char *animationState )
	// Arguments:
	//		animationState - Animation state.
	// Description:
	//		Pushes an animation state into the queue.
	virtual int QueueAnimationState(IFunctionHandler *pH, const char *animationState);
	// <title ChangeAnimGraph>
	// Syntax: Actor.ChangeAnimGraph( const char *graph, int layer )
	// Arguments:
	//		graph - Graph name.
	//		layer - .
	// Description:
	//		Changes the animation graph.
	virtual int ChangeAnimGraph(IFunctionHandler *pH, const char *graph, int layer);

	// <title CreateCodeEvent>
	// Syntax: Actor.CreateCodeEvent( SmartScriptTable params )
	// Arguments:
	//		params - Parameters for the event.
	// Description:
	//		Creates a code event.
	virtual int CreateCodeEvent(IFunctionHandler *pH,SmartScriptTable params);

	// <title GetCurrentAnimationState>
	// Syntax: Actor.GetCurrentAnimationState()
	// Description:
	//		Gets the state for the current animation.
	virtual int GetCurrentAnimationState(IFunctionHandler *pH);
	// <title SetAnimationInput>
	// Syntax: Actor.SetAnimationInput( const char * inputID, const char * value )
	// Arguments:
	//		inputID - Identifier for the input. 
	//		value	- Value for the input.
	// Description:
	//		Sets the animation input.
	virtual int SetAnimationInput( IFunctionHandler *pH, const char * inputID, const char * value );
	// <title TrackViewControlled>
	// Syntax: Actor.TrackViewControlled( int characterSlot )
	// Arguments:
	//		characterSlot - Slot of the character.
	// Description:
	//		Checks if the track view is controlled.
	virtual int TrackViewControlled( IFunctionHandler *pH, int characterSlot );

	// <title SetSpectatorMode>
	// Syntax: Actor.SetSpectatorMode( int mode, ScriptHandle targetId )
	// Arguments:
	//		mode		- Spectator mode.
	//		targetId	- Identifier of the spectator.
	// Description:
	//		Sets the specified spectator mode.
	virtual int SetSpectatorMode(IFunctionHandler *pH, int mode, ScriptHandle targetId);
	// <title GetSpectatorMode>
	// Syntax: Actor.GetSpectatorMode()
	// Description:
	//		Gets the current spectator mode.
	virtual int GetSpectatorMode(IFunctionHandler *pH);
	// <title GetSpectatorTarget>
	// Syntax: Actor.GetSpectatorTarget()
	// Description:
	//		Gets the current spectator target.
	virtual int GetSpectatorTarget(IFunctionHandler* pH);

	// <title Fall>
	// Syntax: Actor.Fall( Vec3 hitPos )
	// Arguments:
	//		hitPos - Hit position.
	// Description:
	//		Makes the actor falling down.
	virtual int Fall(IFunctionHandler *pH, Vec3 hitPos);
	// <title LooseHelmet>
	// Syntax: Actor.LooseHelmet( Vec3 hitPos )
	// Arguments:
	//		hitDir - Direction of the hit.
	//		hitPos - Position of the hit.
	// Description:
	//		Makes the actor loosing the helmet.
	virtual int LooseHelmet(IFunctionHandler *pH, Vec3 hitDir, Vec3 hitPos);
	// <title GoLimp>
	// Syntax: Actor.GoLimp()
	// Description:
	//		Makes the actor having a limp.
	virtual int GoLimp(IFunctionHandler *pH);
	// <title StandUp>
	// Syntax: Actor.StandUp()
	// Description:
	//		Makes the actor standing up.
	virtual int StandUp(IFunctionHandler *pH);


	//------------------------------------------------------------------------
	// ITEM STUFF
	//------------------------------------------------------------------------

	// <title CheckInventoryRestrictions>
	// Syntax: Actor.CheckInventoryRestrictions( const char *itemClassName )
	// Arguments:
	//		itemClassName - Name of the item class.
	// Description:
	//		Checks if there is any restriction in the inventory for the specific item class.
	virtual int CheckInventoryRestrictions(IFunctionHandler *pH, const char *itemClassName);
	// <title CheckVirtualInventoryRestrictions>
	// Syntax: Actor.CheckVirtualInventoryRestrictions( SmartScriptTable inventory, const char *itemClassName )
	// Arguments:
	//		inventory		- Inventory. 
	//		itemClassName	- Item class name.
	// Description:
	//		Checks if there is any restriction in the virtual inventory for the specific item class.
	virtual int CheckVirtualInventoryRestrictions(IFunctionHandler *pH, SmartScriptTable inventory, const char *itemClassName);
	// <title HolsterItem>
	// Syntax: Actor.HolsterItem( bool holster )
	// Arguments:
	//		holster - true if the weapon has to be put back in the holster, false otherwise.
	// Description:
	//		Puts back/extracts the current weapon in the holster.
	virtual int HolsterItem(IFunctionHandler *pH, bool holster);
	// <title DropItem>
	// Syntax: Actor.DropItem( ScriptHandle itemId )
	// Arguments:
	//		itemId - Identifier of the item to be dropped.
	// Description:
	//		Drops the specified item.
	virtual int DropItem(IFunctionHandler *pH, ScriptHandle itemId);
	// <title PickUpItem>
	// Syntax: Actor.PickUpItem( ScriptHandle itemId )
	// Arguments:
	//		itemId - Identifier for the item to be picked up.
	// Description:
	//		Picks up an item
	virtual int PickUpItem(IFunctionHandler *pH, ScriptHandle itemId);

	// <title SelectItemByName>
	// Syntax: Actor.SelectItemByName( const char *name )
	// Arguments:
	//		name - Item name.
	// Description:
	//		Selects an item using its name.
	virtual int SelectItemByName(IFunctionHandler *pH, const char *name);
	// <title SelectItem>
	// Syntax: Actor.SelectItem( ScriptHandle itemId )
	// Arguments:
	//		itemId - Identifier for the item.
	// Description:
	//		Selects the specified item.
	virtual int SelectItem(IFunctionHandler *pH, ScriptHandle itemId);
	// <title SelectLastItem>
	// Syntax: Actor.SelectLastItem()
	// Description:
	//		Selects the last item.
	virtual int SelectLastItem(IFunctionHandler *pH);

	// <title GetClosestAttachment>
	// Syntax: Actor.GetClosestAttachment( int characterSlot, Vec3 testPos, float maxDistance, const char* suffix )
	// Arguments:
	//		characterSlot	- Character slot.
	//		testPos			- Position to be tested.
	//		maxDistance		- Maximum distance to be tested.
	//		suffix			- Suffix string.
	// Description:
	//		Gets the closest attachment to the specified position within the specified distance.
	virtual int GetClosestAttachment(IFunctionHandler *pH, int characterSlot, Vec3 testPos, float maxDistance, const char* suffix);
	// <title AttachVulnerabilityEffect>
	// Syntax: Actor.AttachVulnerabilityEffect( int characterSlot, int partid, Vec3 hitPos, float radius, const char* effect, const char* attachmentIdentifier) 
	// Arguments:
	//		characterSlot			- Character slot.
	//		partid					- .
	//		hitPos					- Position of the hit.
	//		radius					- Radius of the hit.
	//		effect					- Effect string name.
	//		attachmentIndetifier	- Identifier for the attachment.
	// Description:
	//		Attaches vulnerability effect.
	virtual int AttachVulnerabilityEffect(IFunctionHandler *pH, int characterSlot, int partid, Vec3 hitPos, float radius, const char* effect, const char* attachmentIdentifier);
	// <title ResetVulnerabilityEffects>
	// Syntax: Actor.ResetVulnerabilityEffects( int characterSlot )
	// Arguments:
	//		characterSlot - Character slot.
	// Description:
	//		Resets the vulnerability effects.
	virtual int ResetVulnerabilityEffects(IFunctionHandler *pH, int characterSlot);
	// <title GetCloseColliderParts>
	// Syntax: Actor.GetCloseColliderParts( int characterSlot, Vec3 hitPos, float radius )
	// Arguments:
	//		hitPos - Position of the hit.
	//		radius - Radius of the hit effect.
	// Description:
	//		Gets the closer collider parts.
	virtual int GetCloseColliderParts(IFunctionHandler *pH, int characterSlot, Vec3 hitPos, float radius);
	// <title CreateIKLimb>
	// Syntax: Actor.CreateIKLimb( int slot, const char *limbName, const char *rootBone, const char *midBone, const char *endBone, int flags )
	// Arguments:
	//		slot		- Slot number.
	//		limbName	- Limb name string.
	//		rootBone	- Bone root.
	//		midBone		- Middle bone.
	//		endBone		- End bone.
	//		flags		- Flags.
	// Description:
	//		Creates an IK (Inverse Kinematics) limb.
	virtual int CreateIKLimb( IFunctionHandler *pH, int slot, const char *limbName, const char *rootBone, const char *midBone, const char *endBone, int flags);
	// <title ResetScores>
	// Syntax: Actor.ResetScores()
	// Description:
	//		Resets scores.
	virtual int ResetScores(IFunctionHandler *pH);
	// <title RenderScore>
	// Syntax: Actor.RenderScore( ScriptHandle player, int kills, int deaths, int ping )
	// Arguments:
	//		player	- Player identifier.
	//		kills	- Amount of kills.
	//		deaths	- Amount of deaths.
	//		ping	- .
	// Description:
	//		Renders the score to the HUD.
	virtual int RenderScore(IFunctionHandler *pH, ScriptHandle player, int kills, int deaths, int ping);

	// <title SetSearchBeam>
	// Syntax: Actor.SetSearchBeam( Vec3 dir )
	// Arguments:
	//		dir	- Beam direction.
	// Description:
	//		Sets the search beam.
	virtual int SetSearchBeam(IFunctionHandler *pH, Vec3 dir);

	//misc
	//virtual int MeleeEffect(IFunctionHandler *pH);

	// IMonoScriptBind
	virtual void Release() { delete this; }

	virtual const char* GetClassName() { return "ActorSystem"; }
	// ~IMonoScriptBind

	MonoMethod(void, RegisterPlayerClass, mono::string);

	MonoMethod(float, GetPlayerHealth, EntityId);
	MonoMethod(void, SetPlayerHealth, EntityId, float);
	MonoMethod(float, GetPlayerMaxHealth, EntityId);
	MonoMethod(void, SetPlayerMaxHealth, EntityId, float);

protected:
	// <title GetActor>
	// Syntax: Actor.GetActor()
	// Description:
	//		Gets the actor.
	CActor *GetActor(IFunctionHandler *pH);

	SmartScriptTable m_pParams;

	ISystem					*m_pSystem;
	IGameFramework	*m_pGameFW;
};

#endif //__SCRIPTBIND_ACTOR_H__
