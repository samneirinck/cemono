/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Simple Actor implementation
  
 -------------------------------------------------------------------------
  History:
  - 7:10:2004   14:46 : Created by Márcio Martins

*************************************************************************/
#ifndef __Actor_H__
#define __Actor_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IActorSystem.h>
#include <IActionMapManager.h>
#include <CoherentValue.h>
#include <IMusicSystem.h>
#include <CachedAIValues.h>
#include <ISound.h>

#include "Game.h" // for stance enum
#include <IAgent.h> // for stance enum
#include <IAIObject.h>
#include <IAIActorProxy.h>
#include "IAnimationGraph.h"
#include "IAnimatedCharacter.h"
#include "IMovementController.h"
#include "ScreenEffects.h"
#include "GrabHandler.h"
#include "WeaponAttachmentManager.h"
#include "AutoEnum.h"

struct SActorFrameMovementParams
{
	SActorFrameMovementParams() : 
		desiredVelocity(ZERO), 
		desiredLean(0.0f),
		deltaAngles(ZERO),
		lookTarget(ZERO),
		aimTarget(ZERO),
		lookIK(false), 
		aimIK(false), 
		jump(false),
		strengthJump(false),
		sprint(0.0f),
		stance(STANCE_NULL),
		allowStrafe(false)
	{
	}

	// desired velocity for this frame (meters/second) (in local space)
	Vec3 desiredVelocity;
	// desired lean
	float desiredLean;
	// desired turn
	Ang3 deltaAngles;
	Vec3 lookTarget;
	Vec3 aimTarget;
	// prediction
	SPredictedCharacterStates prediction;
	// look target
	bool lookIK;
	// aim target
	bool aimIK;
	bool jump;
	bool strengthJump;
	float sprint;
	// stance
	EStance stance;

	bool allowStrafe;
};

struct IActorMovementController : public IMovementController
{
	struct SStats
	{
		bool idle;
	};

	virtual void Reset() = 0;
	// return true if params was filled out, false if not
	virtual bool Update( float frameTime, SActorFrameMovementParams& params ) = 0;
	virtual bool GetStats(SStats& stats) = 0;
	virtual void PostUpdate( float frameTime ) = 0;
	virtual void Release() = 0;
	virtual void BindInputs( IAnimationGraphState * ) {}
	virtual void Serialize(TSerialize &ser) = 0;
};

//FIXME:not sure to put this here
#define ZEROG_AREA_ID PHYS_FOREIGN_ID_USER+1

// Collision ray piercability to ignore leaves and other things
#define COLLISION_RAY_PIERCABILITY 10

#define ActorBoneList(f) \
	f(BONE_BIP01)    \
	f(BONE_SPINE)    \
	f(BONE_SPINE2)   \
	f(BONE_SPINE3)   \
	f(BONE_HEAD)     \
	f(BONE_EYE_R)    \
	f(BONE_EYE_L)    \
	f(BONE_WEAPON)   \
	f(BONE_FOOT_R)   \
	f(BONE_FOOT_L)   \
	f(BONE_ARM_R)    \
	f(BONE_ARM_L)    \
	f(BONE_CALF_R)   \
	f(BONE_CALF_L)  
	
AUTOENUM_BUILDENUMWITHTYPE_WITHNUM(EBonesID, ActorBoneList, BONE_ID_NUM);
extern const char *s_BONE_ID_NAME[BONE_ID_NUM];

//For first person grabbing
enum EGrabbedCharacterType
{
	eGCT_UNKNOWN = 0,
	eGCT_HUMAN
};

//represent the key status of the actor
#define ACTION_JUMP						(1<<0)
#define ACTION_CROUCH					(1<<1)
#define ACTION_PRONE					(1<<2)
#define ACTION_SPRINT					(1<<3)
#define ACTION_LEANLEFT				(1<<4)
#define ACTION_LEANRIGHT			(1<<5)
#define ACTION_GYROSCOPE			(1<<6)
#define ACTION_ZEROGBACK			(1<<7)
#define ACTION_RELAXED				(1<<8)
#define ACTION_AIMING					(1<<9)
#define ACTION_STEALTH				(1<<10)
#define ACTION_MOVE						(1<<11)

struct IVehicle;
struct IInventory;
struct IInteractor;

struct SActorParams
{
	float maxGrabMass;
	float maxGrabVolume;
	bool nanoSuitActive;

	Vec3	viewPivot;
	float	viewDistance;
	float	viewHeightOffset;

	float	viewFoVScale;
	float	viewSensitivity;

	Vec3	vLimitDir;
	float	vLimitRangeH;
	float	vLimitRangeV;
	float vLimitRangeVUp;
	float vLimitRangeVDown;

	float	weaponBobbingMultiplier;
	float	weaponInertiaMultiplier;

	float speedMultiplier;

	float timeImpulseRecover;

	float lookFOVRadians;
	float lookInVehicleFOVRadians;
	float aimFOVRadians;

	bool canUseComplexLookIK;
	string lookAtSimpleHeadBoneName;
	uint32 aimIKLayer; // [0..15] default: 1
	uint32 lookIKLayer; // [0..15] default: 15

	string footstepEffectName;
	string remoteFootstepEffectName;
	string foleyEffectName;
	bool footstepGearEffect;
	string sFootstepIndGearAudioSignal_Walk;
	string sFootstepIndGearAudioSignal_Run;

	SActorParams()
		: lookFOVRadians(gf_PI)
		, lookInVehicleFOVRadians(gf_PI)
		, aimFOVRadians(gf_PI)
		, aimIKLayer(1)
		, lookIKLayer(15)
		, canUseComplexLookIK(true)
		, lookAtSimpleHeadBoneName("Bip01 Head")
	{
		viewPivot.Set(0,0,0);
		viewDistance = 0;
		viewHeightOffset = 0;

		vLimitDir.Set(0,0,0);
		vLimitRangeH = 0.0f;
		vLimitRangeV = 0.0f;
		vLimitRangeVUp = 0.0f;
		vLimitRangeVDown = 0.0f;

		viewFoVScale = 1.0f;
		viewSensitivity = 1.0f;

		weaponInertiaMultiplier = 1.0f;
		weaponBobbingMultiplier = 1.0f;

		speedMultiplier = 1.0f;

		timeImpulseRecover = 0.0f;
	}
};

struct SActorAnimationEvents
{
	SActorAnimationEvents()
		: m_initialized(false)
	{

	}

	void Init();

	uint32 m_soundId;
	uint32 m_footstepSignalId;
	uint32 m_foleySignalId;
	uint32 m_groundEffectId;
	uint32 m_swimmingStrokeId;
	uint32 m_footStepImpulseId;
	uint32 m_forceFeedbackId;
	uint32 m_grabObjectId;
	uint32 m_weaponLeftHandId;
	uint32 m_weaponRightHandId;
	uint32 m_deathReactionEndId;
	uint32 m_reactionOnCollision;
	uint32 m_forbidReactionsId;
	uint32 m_ragdollStartId;
	uint32 m_killId;
	uint32 m_startFire;
	uint32 m_stopFire;
	uint32 m_shootGrenade;
	uint32 m_meleeHitId;

private:
	bool   m_initialized;
};

struct SActorStats
{
	float inAir;//double purpose, tells if the actor is in air and for how long
	float onGround;//double purpose, tells if the actor is on ground and for how long

	float inWaterTimer;
	float relativeWaterLevel;
	float relativeBottomDepth;
	float worldWaterLevelDelta;
	bool swimJumping;

	float headUnderWaterTimer;
	float worldWaterLevel;

	float speed;
	float speedFlat;
	Vec3	angVelocity;

	Vec3 velocity;
	Vec3 velocityUnconstrained;

	Vec3 gravity;

	bool inZeroG;
	Vec3 zeroGUp;
	float nextZeroGCheck;

	int movementDir;
	float inFiring;
	float mass;
	Vec3 forceUpVector;
	Vec3 forceLookVector;
	float	bobCycle;

	float grabbedTimer;
	bool isGrabbed;
	bool isRagDoll;

	int8 inFreefallLast;
	CCoherentValue<int8> inFreefall;//1 == freefalling, 2 == parachute open
	CCoherentValue<bool> isHidden;
	CCoherentValue<bool> isShattered;
  CCoherentValue<bool> isFrozen;

	EntityId mountedWeaponID;

	int groundMaterialIdx;

	SActorStats()
	{
		memset( this,0,sizeof(SActorStats) );
	}

};

struct SStanceInfo
{
	//dimensions
	float heightCollider;
	float heightPivot;

	bool useCapsule;

	Vec3 size;

	//view
	Vec3 viewOffset;
	Vec3 leanLeftViewOffset;
	Vec3 leanRightViewOffset;
	float viewDownYMod;

	Vec3 peekOverViewOffset;
	Vec3 peekOverWeaponOffset;

	//weapon
	Vec3 weaponOffset;
	Vec3 leanLeftWeaponOffset;
	Vec3 leanRightWeaponOffset;

	//movement
	float normalSpeed;
	float maxSpeed;

	//movement optional
	float walkSpeed;
	float runSpeed;
	float sprintSpeed;
	float customStrafeMultiplier;
	float customBackwardsMultiplier;

	//misc
	char name[32];

	Vec3 modelOffset;

	inline Vec3	GetViewOffsetWithLean(float lean, float peekOver) const
	{
		float peek = clamp(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (peekOverViewOffset - viewOffset);

		if (lean < -0.01f)
		{
			float a = clamp(-lean, 0.0f, 1.0f);
			return viewOffset + a * (leanLeftViewOffset - viewOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp(lean, 0.0f, 1.0f);
			return viewOffset + a * (leanRightViewOffset - viewOffset) + peekOffset;
		}
		return viewOffset + peekOffset;
	}

	inline Vec3	GetWeaponOffsetWithLean(float lean, float peekOver) const
	{
		float peek = clamp(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (peekOverWeaponOffset - weaponOffset);

		if (lean < -0.01f)
		{
			float a = clamp(-lean, 0.0f, 1.0f);
			return weaponOffset + a * (leanLeftWeaponOffset - weaponOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp(lean, 0.0f, 1.0f);
			return weaponOffset + a * (leanRightWeaponOffset - weaponOffset) + peekOffset;
		}
		return weaponOffset + peekOffset;
	}

	static inline Vec3 GetOffsetWithLean(float lean, float peekOver, const Vec3& sOffset, const Vec3& sLeftLean, const Vec3& sRightLean, const Vec3& sPeekOffset)
	{
		float peek = clamp(peekOver, 0.0f, 1.0f);
		Vec3 peekOffset = peek * (sPeekOffset - sOffset);

		if (lean < -0.01f)
		{
			float a = clamp(-lean, 0.0f, 1.0f);
			return sOffset + a * (sLeftLean - sOffset) + peekOffset;
		}
		else if (lean > 0.01f)
		{
			float a = clamp(lean, 0.0f, 1.0f);
			return sOffset + a * (sRightLean - sOffset) + peekOffset;
		}
		return sOffset + peekOffset;
	}


	// Returns the size of the stance including the collider and the ground location.
	ILINE AABB	GetStanceBounds() const
	{
		AABB	aabb(-size, size);
		// Compensate for capsules.
		if(useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}
		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;
		// Include ground position
		aabb.Add(Vec3(0,0,0));

		// Make relative to the entity
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;
		return aabb;
	}

	// Returns the size of the collider of the stance.
	inline AABB	GetColliderBounds() const
	{
		AABB	aabb(-size, size);
		// Compensate for capsules.
		if(useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}
		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;
		// Make relative to the entity
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;
		return aabb;
	}

	SStanceInfo()
		: heightCollider(0.0f)
		, heightPivot(0.0f)
		, useCapsule(false)
		, size(0.5f,0.5f,0.5f)
		, viewOffset(ZERO)
		, leanLeftViewOffset(ZERO)
		, leanRightViewOffset(ZERO)
		, viewDownYMod(0.0f)
		, peekOverViewOffset(ZERO)
		, peekOverWeaponOffset(ZERO)
		, weaponOffset(ZERO)
		, leanLeftWeaponOffset(ZERO)
		, leanRightWeaponOffset(ZERO)
		, normalSpeed(0.0f)
		, maxSpeed(0.0f)
		, modelOffset(ZERO)
		, walkSpeed(0.0f)
		, runSpeed(0.0f)
		, sprintSpeed(0.0f)
		, customStrafeMultiplier(0.0f)
		, customBackwardsMultiplier(0.0f)
	{
		strcpy(name, "null");
	}
};

#define IKLIMB_RIGHTHAND (1<<0)
#define IKLIMB_LEFTHAND (1<<1)

struct SIKLimb
{
	int flags;

	int rootBoneID;
	int endBoneID;
	int middleBoneID;//optional for custom IK

	Vec3 goalWPos;
	Vec3 currentWPos;

	Vec3 goalNormal;

	//limb local position in the animation
	Vec3 lAnimPos;
	Vec3 lAnimPosLast;

	char name[64];

	float recoverTime;//the time to go back to the animation pose
	float recoverTimeMax;

	float blendTime;
	float blendTimeMax;

	int blendID;

	int characterSlot;

	bool keepGoalPos;

	SIKLimb()
	{
		memset(this,0,sizeof(SIKLimb));
	}

	void SetLimb(int slot,const char *limbName,int rootID,int midID,int endID,int iFlags)
	{
		rootBoneID = rootID;
		endBoneID = endID;
		middleBoneID = midID;

		strncpy(name,limbName,64);
		name[63] = 0;

		blendID = -1;

		flags = iFlags;

		characterSlot = slot;
	}

	void SetWPos(IEntity *pOwner,const Vec3 &pos,const Vec3 &normal,float blend,float recover,int requestID);
	void Update(IEntity *pOwner,float frameTime);
};

struct SLinkStats
{
	#define LINKED_FREELOOK (1<<0)
	#define LINKED_VEHICLE (1<<1)

	//which entity are we linked to?
	EntityId linkID;
	//
	uint32 flags;
	//when freelook is on, these are used to update the player rotation
	Quat	baseQuatLinked;
	Quat	viewQuatLinked;

	//
	SLinkStats() : linkID(0), flags(0), baseQuatLinked(IDENTITY), viewQuatLinked(IDENTITY)
	{
	}

	SLinkStats(EntityId eID,uint32 flgs = 0) : linkID(eID), flags(flgs), baseQuatLinked(IDENTITY), viewQuatLinked(IDENTITY)
	{
	}

	ILINE void UnLink()
	{
		linkID = 0;
		flags = 0;
	}

	ILINE bool CanRotate()
	{
		return (!linkID || flags & LINKED_FREELOOK);
	}

	ILINE bool CanMove()
	{
		return (!linkID);
	}

	ILINE bool CanDoIK()
	{
		return (!linkID);
	}

	ILINE bool CanMoveCharacter()
	{
		return (!linkID);
	}

	ILINE IEntity *GetLinked()
	{
		if (!linkID)
			return NULL;
		else
		{
			IEntity *pEnt = gEnv->pEntitySystem->GetEntity(linkID);
			//if the entity doesnt exist anymore forget about it.
			if (!pEnt)
				UnLink();

			return pEnt;
		}
	}

	IVehicle *GetLinkedVehicle();

	void Serialize( TSerialize ser );
};

class CItem;
class CWeapon;

class CActor :
	public CGameObjectExtensionHelper<CActor, IActor>,
	public IGameObjectView,
	public IGameObjectProfileManager,
	public ISoundEventListener
{

protected:

	enum EActorClass
	{
		eActorClass_Actor = 0,
		eActorClass_Player,
		eActorClass_Alien,
		eActorClass_Shark,
	};

public:
	struct ItemIdParam
	{
		ItemIdParam(): itemId(0) {};
		ItemIdParam(EntityId item): itemId(item) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemId", itemId, 'eid');
		}
		EntityId itemId;
	};

	struct DropItemParams
	{
		DropItemParams(): itemId(0), impulseScale(1.0f), selectNext(true), byDeath(false) {};
		DropItemParams(EntityId item, float scale, bool next=true, bool death=false): itemId(item), impulseScale(scale), selectNext(next), byDeath(death) {};
		
		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemId", itemId, 'eid');
			ser.Value("impulseScale", impulseScale, 'iScl');
			ser.Value("selectNext", selectNext, 'bool');
			ser.Value("byDeath", byDeath, 'bool');
		}

		float impulseScale;
		EntityId itemId;
		bool selectNext;
		bool byDeath;
	};

	struct ReviveParams
	{
		// PLAYERPREDICTION
		ReviveParams(): pos(0,0,0), rot(0,0,0,1.0f), teamId(0), physCounter(0) {};
		ReviveParams(const Vec3 &p, const Ang3 &a, int tId, uint8 counter): pos(p), rot(Quat::CreateRotationXYZ(a)), teamId(tId), physCounter(counter) {};
		ReviveParams(const Vec3 &p, const Quat &q, int tId, uint8 counter): pos(p), rot(q), teamId(tId), physCounter(counter) {};
		// ~PLAYERPREDICTION
		
		void SerializeWith(TSerialize ser)
		{
			// PLAYERPREDICTION
			ser.Value("physCounter", physCounter, 'ui4');
			// ~PLAYERPREDICTION
			
			ser.Value("teamId", teamId, 'team');
			ser.Value("pos", pos, 'wrld');
			ser.Value("rot", rot, 'ori1');
		};

		int	 teamId;
		// PLAYERPREDICTION
		uint8 physCounter;
		// ~PLAYERPREDICTION
		Vec3 pos;
		Quat rot;
	};

	struct ReviveInVehicleParams
	{
		ReviveInVehicleParams(): vehicleId(0), seatId(0), teamId(0) {};
		ReviveInVehicleParams(EntityId vId, int sId, int tId): vehicleId(vId), seatId(sId), teamId(tId) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("teamId", teamId, 'team');
			ser.Value("vehicleId", vehicleId, 'eid');
			ser.Value("seatId", seatId, 'seat');
		};

		int	 teamId;
		EntityId vehicleId;
		int seatId;
		// PLAYERPREDICTION
		uint8 physCounter;
		// ~PLAYERPREDICTION
	};

	struct KillParams
	{
		KillParams() {};
		KillParams(EntityId _shooterId, int _weaponClassId, float _damage, int _material, int _hit_type, const Vec3 &_impulse)
		: shooterId(_shooterId),
			weaponClassId(_weaponClassId),
			damage(_damage),
			material(_material),
			hit_type(_hit_type),
			impulse(_impulse)
		{};

		EntityId shooterId;
		int weaponClassId;
		float damage;
		int material;
		int hit_type;
		Vec3 impulse;

		void SerializeWith(TSerialize ser)
		{
			ser.Value("shooterId", shooterId, 'eid');
			ser.Value("weaponClassId", weaponClassId, 'ui16');
			ser.Value("damage", damage, 'dmg');
			ser.Value("material", material, 'mat');
			ser.Value("hit_type", hit_type, 'hTyp');
			ser.Value("impulse", impulse, 'kImp');
		};
	};
	struct MoveParams
	{
		MoveParams() {};
		MoveParams(const Vec3 &p, const Quat &q): pos(p), rot(q) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("pos", pos, 'wrld');
			ser.Value("rot", rot, 'ori1');
		}
		Vec3 pos;
		Quat rot;
	};
	struct AmmoParams
	{
		AmmoParams() {};
		AmmoParams(const char *name, int amount): ammo(name), count(amount) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("ammo", ammo);
			ser.Value("amount", count);
		}
		string ammo;
		int	count;
	};

	struct PickItemParams
	{
		PickItemParams(): itemId(0), select(false), sound(false) {};
		PickItemParams(EntityId item, bool slct, bool snd): itemId(item), select(slct), sound(snd) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemId", itemId, 'eid');
			ser.Value("select", select, 'bool');
			ser.Value("sound", sound, 'bool');
		}

		EntityId	itemId;
		bool			select;
		bool			sound;
	};

	struct SetSpectatorModeParams
	{
		SetSpectatorModeParams(): mode(0), targetId(0) {};
		SetSpectatorModeParams(uint8 _mode, EntityId _target): mode(_mode), targetId(_target) {};
		uint8 mode;
		EntityId targetId;
		void SerializeWith(TSerialize ser)
		{
			ser.Value("mode", mode, 'spec');
			ser.Value("target", targetId, 'eid');
		}
	};
	struct SetSpectatorHealthParams
	{
		SetSpectatorHealthParams(): health(0) {};
		SetSpectatorHealthParams(float _health): health(_health) {};
		float health;
		void SerializeWith(TSerialize ser)
		{
			ser.Value("health", health);
		}
	};

	struct NoParams
	{
		void SerializeWith(TSerialize ser) {};
	};

	enum EActorSpectatorMode
	{
		eASM_None = 0,												// normal, non-spectating

		eASM_FirstMPMode,
		eASM_Fixed = eASM_FirstMPMode,				// fixed position camera
		eASM_Free,														// free roaming, no collisions
		eASM_Follow,													// follows an entity in 3rd person
		eASM_LastMPMode = eASM_Follow,

		eASM_Cutscene,												// HUDInterfaceEffects.cpp sets this
	};

	DECLARE_SERVER_RMI_NOATTACH(SvRequestDropItem, DropItemParams, eNRT_ReliableOrdered);
	DECLARE_SERVER_RMI_NOATTACH(SvRequestPickUpItem, ItemIdParam, eNRT_ReliableOrdered);
	DECLARE_SERVER_RMI_NOATTACH(SvRequestUseItem, ItemIdParam, eNRT_ReliableOrdered);
	// cannot be _FAST - see comment on InvokeRMIWithDependentObject
	DECLARE_CLIENT_RMI_NOATTACH(ClPickUp, PickItemParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClClearInventory, NoParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_NOATTACH(ClDrop, DropItemParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClStartUse, ItemIdParam, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClStopUse, ItemIdParam, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_PREATTACH(ClSetSpectatorMode, SetSpectatorModeParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_PREATTACH(ClSetSpectatorHealth, SetSpectatorHealthParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_PREATTACH(ClRevive, ReviveParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_PREATTACH(ClReviveInVehicle, ReviveInVehicleParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_POSTATTACH(ClSimpleKill, NoParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClKill, KillParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClMoveTo, MoveParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_NOATTACH(ClSetAmmo, AmmoParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClAddAmmo, AmmoParams, eNRT_ReliableOrdered);

	CItem *GetItem(EntityId itemId) const;
	CItem *GetItemByClass(IEntityClass* pClass) const;
	CWeapon *GetWeapon(EntityId itemId) const;
	CWeapon *GetWeaponByClass(IEntityClass* pClass) const;

	virtual void SelectNextItem(int direction, bool keepHistory, const char *category=0);
	virtual void HolsterItem(bool holster, bool playSelect = true);
	virtual void SelectLastItem(bool keepHistory, bool forceNext = false);
	virtual void SelectItemByName(const char *name, bool keepHistory);
	virtual void SelectItem(EntityId itemId, bool keepHistory);

	virtual bool UseItem(EntityId itemId);
	virtual bool PickUpItem(EntityId itemId, bool sound, bool ignoreOffhand = false);
	virtual bool DropItem(EntityId itemId, float impulseScale=1.0f, bool selectNext=true, bool byDeath=false);
	virtual void DropAttachedItems();

	virtual void NetReviveAt(const Vec3 &pos, const Quat &rot, int teamId);
	virtual void NetReviveInVehicle(EntityId vehicleId, int seatId, int teamId);
	virtual void NetSimpleKill();
	virtual void NetKill(EntityId shooterId, uint16 weaponClassId, int damage, int material, int hit_type);

	virtual void SetSleepTimer(float timer);
	Vec3 GetWeaponOffsetWithLean(EStance stance, float lean, float peekOver, const Vec3& eyeOffset);

	virtual bool CanRagDollize() const;
		
public:
	CActor();
	virtual ~CActor();

	// IActor
	virtual void ProcessEvent(SEntityEvent& event);
	virtual void Release() { delete this; };
	virtual void ResetAnimGraph();
	virtual void NotifyAnimGraphTransition(const char *anim0){};
	virtual void NotifyAnimGraphInput(int id, const char *value) {};
	virtual void NotifyAnimGraphInput(int id, int value) {};
	virtual void FullSerialize( TSerialize ser );
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags );
	virtual void PostSerialize();
	virtual void SerializeSpawnInfo( TSerialize ser );
	virtual ISerializableInfoPtr GetSpawnInfo();
	virtual void SetChannelId(uint16 id);
	virtual void  SerializeLevelToLevel( TSerialize &ser );
	virtual IInventory *GetInventory() const;
	virtual void NotifyCurrentItemChanged(IItem* newItem) {};

	virtual bool IsPlayer() const;
	virtual bool IsClient() const;
	virtual bool IsMigrating() const;
	virtual void SetMigrating(bool ismigrating);
	virtual IMaterial *GetReplacementMaterial() { return m_pReplacementMaterial; };

	virtual bool Init( IGameObject * pGameObject );
	virtual void InitClient( int channelId );
	virtual void PostInit( IGameObject * pGameObject );
	virtual void PostInitClient(int channelId) {};
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params );
	virtual bool GetEntityPoolSignature( TSerialize signature );
	virtual void Update(SEntityUpdateContext& ctx, int updateSlot);
	virtual void UpdateView(SViewParams &viewParams) {};
	virtual void PostUpdateView(SViewParams &viewParams) {};

	virtual void InitLocalPlayer() {};

	virtual void InitiateCombat();
	virtual void ExtendCombat();

	virtual void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority);

	virtual void HandleEvent( const SGameObjectEvent& event );
	virtual void PostUpdate(float frameTime);
	virtual void PostRemoteSpawn() {};

	virtual bool IsThirdPerson() const { return true; };
	virtual void ToggleThirdPerson(){}

	virtual ICachedAIValues* GetCachedAIValues() { return &m_cachedAIValues; }

	virtual void RequestFacialExpression(const char* pExpressionName /* = NULL */, f32* sequenceLength /*= NULL*/);
	virtual void PrecacheFacialExpression(const char* pExpressionName);

	virtual void NotifyInventoryAmmoChange(IEntityClass* pAmmoClass, int amount);
	virtual EntityId	GetGrabbedEntityId() const { return 0; }

	virtual void HideAllAttachments(bool isHiding);

	virtual void OnAIProxyEnabled(bool enabled) {};
	virtual void OnReturnedToPool() {};
	virtual void OnPreparedFromPool() {};
	// ~IActor

	// IGameObjectProfileManager
	virtual bool SetAspectProfile( EEntityAspects aspect, uint8 profile );
	virtual uint8 GetDefaultProfile( EEntityAspects aspect ) { return aspect == eEA_Physics? eAP_NotPhysicalized : 0; }
	// ~IGameObjectProfileManager

	// IActionListener
	virtual void OnAction(const ActionId& actionId, int activationMode, float value);
	// ~IActionListener

	// ISoundEventListener
	virtual void OnSoundEvent(ESoundCallbackEvent event, ISound *pSound);
	// ~ISoundEventListener

	virtual void ProfileChanged( uint8 newProfile );

	//------------------------------------------------------------------------
	float GetAirControl() const { return m_airControl; };
	float GetAirResistance() const { return m_airResistance; };
	float GetTimeImpulseRecover() const { return m_timeImpulseRecover; };

	virtual void SetViewRotation( const Quat &rotation ) {};
	virtual Quat GetViewRotation() const { return GetEntity()->GetRotation(); };
	virtual void EnableTimeDemo( bool bTimeDemo ) {};

	// offset to add to the computed camera angles every frame
	virtual void SetViewAngleOffset(const Vec3 &offset) {};
	virtual Vec3 GetViewAngleOffset() { return Vec3(0, 0, 0); };

	//------------------------------------------------------------------------
	virtual void Revive( bool fromInit = false );
  virtual void Reset(bool toGame) {};
	//physicalization
	virtual void Physicalize(EStance stance=STANCE_NULL);
	virtual void PostPhysicalize();
	virtual void RagDollize( bool fallAndPlay );
	//
  virtual int IsGod(){ return 0; }

	virtual void SetSpectatorMode(uint8 mode, EntityId targetId) {};
	virtual uint8 GetSpectatorMode() const { return 0; };
	virtual void SetSpectatorTarget(EntityId targetId) {};
	virtual EntityId GetSpectatorTarget() const { return 0; };
	virtual void SetSpectatorHealth(float health) {};
	virtual float GetSpectatorHealth() const { return 0; };
	
	//get actor status
	virtual SActorStats *GetActorStats() { return 0; };
	virtual const SActorStats *GetActorStats() const { return 0; };
	virtual SActorParams *GetActorParams() { return 0; };

	virtual void SetStats(SmartScriptTable &rTable);
	virtual void UpdateScriptStats(SmartScriptTable &rTable);
	virtual ICharacterInstance *GetFPArms(int i) const { return GetEntity()->GetCharacter(3+i); };
	//set/get actor params
	virtual void SetParams(SmartScriptTable &rTable,bool resetFirst = false);
	virtual bool GetParams(SmartScriptTable &rTable) { return false; };
	//
	virtual void Freeze(bool freeze) {};
	virtual void Fall(Vec3 hitPos = Vec3(0,0,0), float time = 0.0f);
	//throw away the actors helmet (if available) [Jan Müller]
	virtual bool LooseHelmet(Vec3 hitDir = Vec3(0,0,0), Vec3 hitPos = Vec3(0,0,0));
	virtual void GoLimp();
	virtual void StandUp();
	virtual void NotifyLeaveFallAndPlay();
	virtual bool IsFallen() const;
	//
	virtual IEntity *LinkToVehicle(EntityId vehicleId);
	virtual IEntity *LinkToVehicleRemotely(EntityId vehicleId);
	virtual void LinkToMountedWeapon(EntityId weaponId) {};
	virtual IEntity *LinkToEntity(EntityId entityId, bool bKeepTransformOnDetach=true);
	
	virtual bool AllowLandingBob() { return true; }

	virtual IEntity *GetLinkedEntity() const
	{
		return m_linkStats.GetLinked();
	}

	virtual IVehicle *GetLinkedVehicle() const
	{
		return m_linkStats.GetLinkedVehicle();
	}

	float GetLookFOV(const SActorParams &actorParams) const
	{
		return GetLinkedVehicle() ? actorParams.lookInVehicleFOVRadians : actorParams.lookFOVRadians;
	}

	uint32 GetAimIKLayer(const SActorParams &actorParams) const
	{
		return actorParams.aimIKLayer;
	}

	uint32 GetLookIKLayer(const SActorParams &actorParams) const
	{
		return actorParams.lookIKLayer;
	}

	virtual void SetViewInVehicle(Quat viewRotation) {};

	virtual void SupressViewBlending() {};

	inline Vec3 GetLBodyCenter()
	{
		const SStanceInfo *pStance(GetStanceInfo(GetStance()));
		return Vec3(0,0,(pStance->viewOffset.z - pStance->heightPivot) * 0.5f);
	}

	inline Vec3 GetWBodyCenter()
	{
		return GetEntity()->GetWorldTM() * GetLBodyCenter();
	}

	//for animations
	virtual void PlayAction(const char *action,const char *extension, bool looping=false) {};
	//
	virtual void SetMovementTarget(const Vec3 &position,const Vec3 &looktarget,const Vec3 &up,float speed) {};
	//
	virtual void CreateScriptEvent(const char *event,float value,const char *str = NULL);
	virtual bool CreateCodeEvent(SmartScriptTable &rTable);
	virtual void AnimationEvent(ICharacterInstance *pCharacter, const AnimEventInstance &event);
	//
	virtual void CameraShake(float angle,float shift,float duration,float frequency,Vec3 pos,int ID,const char* source="") {};
	//
	virtual void VectorToLocal(Vec3 &v) {};
	//
	virtual void SetAngles(const Ang3 &angles) {};
	virtual Ang3 GetAngles() {return Ang3(0,0,0);};
	virtual void AddAngularImpulse(const Ang3 &angular,float deceleration=0.0f,float duration=0.0f){}
	//
	virtual void SetViewLimits(Vec3 dir,float rangeH,float rangeV) {};
	virtual void SetZoomSpeedMultiplier(float m);
	virtual float GetZoomSpeedMultiplier() const;
	virtual void SetHealth( float health );
	virtual void DamageInfo(EntityId shooterID, EntityId weaponID, float damage, const char *damageType);
	virtual IAnimatedCharacter * GetAnimatedCharacter() { return m_pAnimatedCharacter; }
	virtual void PlayExactPositioningAnimation( const char* sAnimationName, bool bSignal, const Vec3& vPosition, const Vec3& vDirection, float startWidth, float startArcAngle, float directionTolerance ) {}
	virtual void CancelExactPositioningAnimation() {}
	virtual void PlayAnimation( const char* sAnimationName, bool bSignal ) {}

	virtual bool PlayReadabilitySound(const Vec3& vSoundPosition, const SReadabilitySoundEntry& soundEntry, bool bPlaySoundAtActorTarget, bool bStopPreviousSound);

	void SetMaxHealth( float maxHealth );
	virtual int   GetHealthAsRoundedPercentage() const { return m_healthAsRoundedPercentage; }
	virtual float GetHealth() const { return m_health; }
	virtual float GetMaxHealth() const { return m_maxHealth; }
	virtual bool  IsDead() const { return m_healthAsRoundedPercentage == 0; }
	virtual int32 GetArmor() const { return 0; }
	virtual int32 GetMaxArmor() const { return 0; }
	virtual void Kill();

	ILINE const SActorAnimationEvents& GetAnimationEventsTable() const { return s_animationEventsTable; };

	void ResetHelmetAttachment();
	
  virtual float GetFrozenAmount(bool stages=false) const { return m_frozenAmount; }
  void SetFrozenAmount(float amount);  
  
  bool IsFrozen();
  virtual void AddFrost(float frost);  

	virtual void BindInputs( IAnimationGraphState * pAGState );
	//marcok: GC workaround
	virtual bool IsSwimming() {	return false; }
	virtual bool ShouldSwim(){ return false; };

	virtual bool IsSprinting() { return false;}
	virtual bool CanFire(){ return true; }
	virtual bool BecomeAggressiveToAgent(EntityId agentID) { return true; };

	//stances
	ILINE virtual EStance GetStance() const 
	{
/*
		if(!m_pAnimatedCharacter)
			return STANCE_NULL;
		int stance = m_pAnimatedCharacter?m_pAnimatedCharacter->GetCurrentStance():STANCE_NULL;

		if (stance < 0 || stance > STANCE_LAST)
			return STANCE_NULL;
		return (EStance)stance;
*/
		return m_stance;
	}
	ILINE const SStanceInfo *GetStanceInfo(EStance stance) const 
	{
		if (stance < 0 || stance > STANCE_LAST)
			return &m_defaultStance;
		return &m_stances[stance];
	}
	virtual void	SetupStance(EStance stance,SStanceInfo *info);
	//

	// forces the animation graph to select a state
	void QueueAnimationState( const char * state );
	void ChangeAnimGraph( const char *graph, int layer );
	virtual bool SetAnimationInput( const char * inputID, const char * value )
	{
		// Handle action and signal inputs via AIproxy, since the AI system and
		// the AI agent behavior depend on those inputs.
		if (IEntity* pEntity = GetEntity())
			if (IAIObject* pAI = pEntity->GetAI())
				if (IAIActorProxy* pProxy = pAI->GetProxy())
				{
					if(pProxy->IsEnabled())
					{
						bool	bSignal = strcmp(inputID, "Signal") == 0;
						bool	bAction = strcmp(inputID, "Action") == 0;
						if(bSignal)
						{
							return pProxy->SetAGInput( AIAG_SIGNAL, value );
						}
						else if(bAction)
						{
							// Dejan: actions should not go through the ai proxy anymore!
							/*
							if(_stricmp(value, "idle") == 0)
								return pProxy->ResetAGInput( AIAG_ACTION );
							else
							{
								return pProxy->SetAGInput( AIAG_ACTION, value );
							}
							*/
						}
					}
				}

		if (IAnimationGraphState * pState = GetAnimationGraphState())
		{
			pState->SetInput( pState->GetInputId(inputID), value );
			return true;
		}

		return false;
	}

	//
	virtual int GetBoneID(int ID,int slot = 0) const;
	virtual Vec3 GetLocalEyePos(int slot = 0) const;
	virtual Vec3 GetLocalEyePos2(int slot = 0) const;

	ILINE /* virtual */ bool InZeroG() const
	{
		if (const SActorStats* pActorStats = GetActorStats())
			return pActorStats->inZeroG;
		return false;
	}

	void UpdateZeroG(float frameTime);

	//
	virtual void ProcessBonesRotation(ICharacterInstance *pCharacter,float frameTime);

	virtual void OnPhysicsPreStep(float frameTime){};

	//grabbing
	virtual IGrabHandler *CreateGrabHanlder();
	virtual void UpdateGrab(float frameTime);
	
	virtual bool CheckInventoryRestrictions(const char *itemClassName);
	virtual bool CheckVirtualInventoryRestrictions(const std::vector<string> &inventory, const char *itemClassName);

	virtual bool CanPickUpObject(IEntity *obj, float& heavyness, float& volume);
	virtual bool CanPickUpObject(float mass, float volume);
	virtual float GetActorStrength() const;

	//
	virtual void ProcessIKLimbs(ICharacterInstance *pCharacter,float frameTime);

	//IK limbs
	int GetIKLimbIndex(const char *limbName);
	ILINE SIKLimb *GetIKLimb(int limbIndex)
	{
		return &m_IKLimbs[limbIndex];
	}
	void CreateIKLimb(int characterSlot, const char *limbName, const char *rootBone, const char *midBone, const char *endBone, int flags = 0);

	//
	virtual IMovementController * GetMovementController() const
	{
		return m_pMovementController;
	}

	//stances
	virtual void	SetStance(EStance stance);
	virtual void  StanceChanged(EStance last) {};
  virtual bool	TrySetStance(EStance stance); // Shared between humans and aliens.
	//
	
	IAnimationGraphState * GetAnimationGraphState();
	IAnimatedCharacter * GetAnimatedCharacter() const { return m_pAnimatedCharacter; };
	void SetFacialAlertnessLevel(int alertness);

	//weapons
	virtual IItem *GetCurrentItem(bool includeVehicle=false) const;
	EntityId GetCurrentItemId(bool includeVehicle=false) const;
	virtual IItem *GetHolsteredItem() const;

	IInteractor *GetInteractor() const;

	//Net
	EntityId NetGetCurrentItem() const;
	void NetSetCurrentItem(EntityId id);

	//AI
	Vec3 GetAIAttentionPos();

	CScreenEffects *GetScreenEffects() {return m_screenEffects;};
	CWeaponAttachmentManager* GetWeaponAttachmentManager() { return m_pWeaponAM; }
	void InitActorAttachments();

	virtual void SwitchDemoModeSpectator(bool activate) {};	//this is a player only function

	//misc
	virtual void ReplaceMaterial(const char *strMaterial);
	virtual void SendMusicLogicEvent(EMusicLogicEvents event){};
	
	virtual const char* GetActorClassName() const { return "CActor"; };

	static  ActorClass GetActorClassType() { return (ActorClass)eActorClass_Actor; }
	virtual ActorClass GetActorClass() const { return (ActorClass)eActorClass_Actor; };

	virtual const char* GetEntityClassName() const { return GetEntity()->GetClass()->GetName(); }

	//For sleeping bullets
	virtual bool  CanSleep() { return false; }

	//Grabbing
	virtual int	GetActorSpecies() { return eGCT_UNKNOWN; }

	virtual void SetAnimTentacleParams(pe_params_rope& rope, float animBlend) {};

  virtual bool IsCloaked() const { return false; }

  virtual void DumpActorInfo();

	bool IsFriendlyEntity( EntityId entityId ) const;

	virtual EntityId GetCurrentTargetEntityId() const { return 0; }

	// PLAYERPREDICTION
	ILINE uint8 GetNetPhysCounter() { return m_netPhysCounter; }

	bool AllowPhysicsUpdate(uint8 newCounter) const;
	static bool AllowPhysicsUpdate(uint8 newCounter, uint8 oldCounter);

	bool IsRemote() const;
	// ~PLAYERPREDICTION
	
	QuatT GetBoneTransform( int id ) const;

protected:
	virtual void SetMaterialRecursive(ICharacterInstance *charInst, bool undo, IMaterial *newMat = 0);

	void DisplayDebugInfo();

	virtual void UpdateAnimGraph( IAnimationGraphState * pState );

	//movement
	virtual IActorMovementController * CreateMovementController() = 0;
	//

	virtual Vec3 GetModelOffset() const { return GetStanceInfo(GetStance())->modelOffset; }

	void UpdateCachedAIValues();

private:
	mutable IInventory * m_pInventory;
	mutable IInteractor * m_pInteractor;
	void ClearExtensionCache();
	void CrapDollize();

protected:
	void RequestServerResync()
	{
		if (!IsClient())
			GetGameObject()->RequestRemoteUpdate(eEA_Physics | eEA_GameClientDynamic | eEA_GameServerDynamic | eEA_GameClientStatic | eEA_GameServerStatic);
	}

	void GetActorMemoryStatistics( ICrySizer * pSizer ) const;
	
	//
	typedef std::vector<SIKLimb> TIKLimbs;
	
	//
	virtual void SetActorModel();
	virtual bool UpdateStance();

	void UpdateReadabilitySound();
	void ClPlayReadabilitySound(const SReadabilitySoundParams& params);
  virtual void OnCloaked(bool cloaked){};

	mutable int16 m_boneIDs[BONE_ID_NUM];

	bool	m_isClient;
	float m_health;
	int   m_healthAsRoundedPercentage;
	float m_maxHealth;
	CCachedAIValues m_cachedAIValues;
	static SActorAnimationEvents s_animationEventsTable;
 
	EStance m_stance;
	EStance m_desiredStance;

	static SStanceInfo m_defaultStance;
	SStanceInfo m_stances[STANCE_LAST];

	float m_zoomSpeedMultiplier;

	SmartScriptTable m_actorStats;

	IAnimatedCharacter *m_pAnimatedCharacter;
	IActorMovementController * m_pMovementController;

	static IItemSystem			*m_pItemSystem;
	static IGameFramework		*m_pGameFramework;
	static IGameplayRecorder*m_pGameplayRecorder;

	//
	IAnimationGraph::InputID m_inputHealth;
	IAnimationGraph::InputID m_inputStance;
	IAnimationGraph::InputID m_inputFiring;
	IAnimationGraph::InputID m_inputWaterLevel;

	IGrabHandler *m_pGrabHandler;

	mutable SLinkStats m_linkStats;

	TIKLimbs m_IKLimbs;
  
  float m_frozenAmount; // internal amount. used to leave authority over frozen state at game

	// ScreenEffects-related variables
	CScreenEffects *m_screenEffects;

	// Weapon Attachment manager
	CWeaponAttachmentManager *m_pWeaponAM;

	uint8 m_currentPhysProfile;

	EBonesID m_currentFootID;

	float m_fallAndPlayTimer;

	float m_combatTimer;
	bool m_enterCombat;
	bool m_inCombat;
	bool m_rightFoot;
	bool m_bHasHUD;

	float m_airControl;
	float m_airResistance;
	float m_timeImpulseRecover;

	EntityId m_netLastSelectablePickedUp;

	//helmet serialization
	EntityId	m_lostHelmet, m_serializeLostHelmet;
	string		m_lostHelmetObj, m_serializelostHelmetObj;
	string		m_lostHelmetMaterial;
	string		m_lostHelmetPos;

	_smart_ptr<IMaterial> m_pReplacementMaterial;

	//
  std::map< ICharacterInstance*, _smart_ptr<IMaterial> > m_testOldMats;
  std::map< IAttachmentObject*, _smart_ptr<IMaterial> > m_attchObjMats;
  //std::map< EntityId, IMaterial* > m_wepAttchMats;

	float			m_sleepTimer,m_sleepTimerOrg;

	int				m_teamId;
	EntityId	m_lastItemId;
	
	// PLAYERPREDICTION
	uint8		m_netPhysCounter;				 //	Physics counter, to enable us to throw away old updates
	// ~PLAYERPREDICTION
};

#endif //__Actor_H__
