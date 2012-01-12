////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001.
// -------------------------------------------------------------------------
//  File name:   flock.h
//  Version:     v1.00
//  Created:     5/4/2002 by Timur.
//  Compilers:   Visual C++ 7.0
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __flock_h__
#define __flock_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <IScriptSystem.h>
#include <IAISystem.h>

class CFlock;

//! Return random value in [-1,1] range.
inline float frand()
{
	return cry_frand()*2.0f - 1.0f;
}

enum EFlockType
{
	EFLOCK_BIRDS,
	EFLOCK_FISH,
	EFLOCK_BUGS,
	EFLOCK_CHICKENS,
	EFLOCK_FROGS,
	EFLOCK_TURTLES,
};

struct SBoidContext
{
	Vec3 playerPos;
	Vec3 flockPos;

	//! Some integer for various behaviors.
	int behavior;

	float fSpawnRadius;
	float fBoidRadius;
	float fBoidThickness; // For particle physics.

	float fBoidMass;
	float fGravity;

	float terrainZ;
	float waterLevel;

	// Flock constants.
	float MinHeight;
	float MaxHeight;

	// Attraction distances.
	float MaxAttractDistance;
	float MinAttractDistance;

	// Speed.
	float MaxSpeed;
	float MinSpeed;

	// AI factors.
	// Group behavior factors.
	float factorAlignment;
	float factorCohesion;
	float factorSeparation;
	// Other behavior factors.
	float factorAttractToOrigin;
	float factorKeepHeight;
	float factorAvoidLand;
	//! Cosine of boid field of view angle.
	//! Other boids which are not in field of view of this boid not considered as a neightboards.
	float cosFovAngle;
	float factorRandomAccel;

	//! Maximal Character animation speed.
	float MaxAnimationSpeed;

	// Settings.
	bool followPlayer;
	bool avoidObstacles;
	bool noLanding;
	bool bAvoidWater;
	bool bSpawnFromPoint;

	//! Max visible distance of flock from player.
	float maxVisibleDistance;

	//! Size of boid.
	float boidScale;
	float boidRandomScale;

	I3DEngine *engine;
	IPhysicalWorld *physics;
	IEntity* entity;

	Vec3 vEntitySlotOffset;

	float fSmoothFactor;

	// One time attraction point
	Vec3 attractionPoint;

	// Ratio that limits the speed at which boid can turn.
	float fMaxTurnRatio;

	Vec3 randomFlockCenter;
	
	Vec3 scarePoint;        // Point where some scaring event happened
	float scareRadius;      // Radius from scare point where boids get scared.
	float scareRatio;       // How scarry is scare point
	float scareThreatLevel; // Level of that scare threat, comparable with AI events

	std::vector<string> sounds;
	std::vector<string> animations;
};

/*!	This is flock creation context passed to flock Init method.
*/
struct SBoidsCreateContext
{
	int boidsCount;						//! Number of boids in flock.
	std::vector<string> models;	//! Geometry models (Static or character) to be used for flock.
	string characterModel;	//! Character model.
	string animation;	//! Looped character animation.
};

//////////////////////////////////////////////////////////////////////////
// BoidObject.
//////////////////////////////////////////////////////////////////////////
/*! Single Boid object.
 */
class CBoidObject
{
public:
	CBoidObject( SBoidContext &bc );
	virtual ~CBoidObject();

	virtual void Update( float dt,SBoidContext &bc ) {};
	virtual void Physicalize( SBoidContext &bc );

	//! Kill this boid object.
	//! @param force Force vector applyed on dying boid (shot vector).
	virtual void Kill( const Vec3 &hitPoint,const Vec3 &force ) {};

	virtual void OnFlockMove( SBoidContext &bc ) {};
	virtual void OnEntityEvent( SEntityEvent &event );

	virtual void Render( SRendParams &rp,CCamera &cam,SBoidContext &bc );
	void CalcFlockBehavior( SBoidContext &bc,Vec3 &vAlignment,Vec3 &vCohesion,Vec3 &vSeparation );
	void CalcMovement( float dt,SBoidContext &bc,bool banking );

	void CalcOrientation( Quat &qOrient );
	void CreateRigidBox( SBoidContext &bc,const Vec3 &size,float mass,float density );
	void CreateArticulatedCharacter( SBoidContext &bc,const Vec3 &size,float mass );

	void PlayAnimation( const char *animation,bool bLooped,float fBlendTime=0.3f );
	void PlayAnimationId( int nIndex,bool bLooped,float fBlendTime=0.3f );
	void PlaySound( int nIndex );

	int GetGeometrySurfaceType();

	virtual void OnPickup( bool bPickup,float fSpeed );
	virtual void OnCollision( SEntityEvent &event );

	void GetMemoryUsage(ICrySizer *pSizer )const
	{
		//pSizer->AddObject(this, sizeof(*this));
		//pSizer->AddObject(m_flock);
		//pSizer->AddObject(m_object);		
	}
public:
	//////////////////////////////////////////////////////////////////////////
	friend class CFlock;

	CFlock *m_flock;	//!< Flock of this boid.
	Vec3 m_pos;			//!< Boid position.
	Vec3 m_heading;	//!< Current heading direction.
	Vec3 m_accel;		//!< Desired acceleration vector.
	Vec3 m_currentAccel; // Current acceleration vector.
	float m_speed;		//!< Speed of bird at heading direction.
	float m_banking;	//!< Amount of banking to apply on boid.
	float m_bankingTrg;	//!< Amount of banking to apply on boid.
	float m_scale;
	ICharacterInstance *m_object;	//< Geometry of this boid.
	float m_alignHorizontally; // 0-1 to align bird horizontally when it lands.
	float m_scareRatio;  // When boid is scared it is between 0 and 1 to show how scared it is.
	EntityId m_entity;
	IPhysicalEntity	 *m_pPhysics;

	// Flags.
	unsigned m_dead : 1;			//! Boid is dead, do not update it.
	unsigned m_dying : 1;			//! Boid is dying.
	unsigned m_physicsControlled : 1;	//! Boid is controlled by physics.
	unsigned m_inwater : 1;		//! When boid falls in water.
	unsigned m_nodraw : 1;		//! Do not draw this boid.
	unsigned m_pickedUp : 1;	//! Boid was picked up by player.
	unsigned m_noentity : 1;  //! Entity for this boid suppose to be deleted.
};

//////////////////////////////////////////////////////////////////////////
class CBoidBird : public CBoidObject
{
public:
	CBoidBird( SBoidContext &bc );
	virtual ~CBoidBird();

	virtual void Update( float dt,SBoidContext &bc );
	virtual void UpdatePhysics( float dt,SBoidContext &bc );
	virtual void Kill( const Vec3 &hitPoint,const Vec3 &force );
	virtual void OnFlockMove( SBoidContext &bc );
	virtual void Physicalize( SBoidContext &bc );

	//void Render( CCamera &cam,SBoidContext &bc );
	virtual void Think( float dt,SBoidContext &bc );

	void TakeOff( SBoidContext &bc );
	void SetAttracted(bool bAttracted = true) { m_attractedToPt = bAttracted; m_fAttractionFactor = 0; }
	void SetSpawnFromPt(bool bSpawnFromPt = true) { m_spawnFromPt = bSpawnFromPt; }

	// Parameters for birds spawned from a point
	float m_fNoCenterAttract;		// Compensates for attraction to center point
	float m_fNoKeepHeight;			// Compensates for attraction to terrain
	float m_fAttractionFactor;

protected:
	float m_flightTime;	//!< Time flying after take off.
	float m_lastThinkTime; //! Time of last think operation.
	float m_maxFlightTime; // Time this bird can be in flight.
	float m_desiredHeigh; // Desired height this birds want to fly at.

	Vec3 m_birdOriginPos;
	Vec3 m_birdOriginPosTrg;

	// Flags.
	unsigned m_onGround : 1;			//! True if stand on ground.
	unsigned m_landing : 1;				//! True if bird wants to land.
	unsigned m_takingoff : 1;			//! True if bird is just take-off from land.
	unsigned m_attractedToPt : 1;	//! True if bird is attracted to a point
	unsigned m_spawnFromPt : 1;		//! True if bird is spawned from point
};

//////////////////////////////////////////////////////////////////////////
//! Boid object with fish behavior.
//////////////////////////////////////////////////////////////////////////
class CBoidFish : public CBoidObject
{
public:
	CBoidFish( SBoidContext &bc );
	~CBoidFish();

	virtual void Update( float dt,SBoidContext &bc );
	virtual void Kill( const Vec3 &hitPoint,const Vec3 &force );
	virtual void Physicalize( SBoidContext &bc );

protected:
	void SpawnParticleEffect( const Vec3 &pos,SBoidContext &bc,int nEffect );

	float m_dyingTime; // Deisred height this birds want to fly at.
	SmartScriptTable vec_Bubble;

	enum EScriptFunc {
		SPAWN_BUBBLE,
		SPAWN_SPLASH,
	};
	HSCRIPTFUNCTION m_pOnSpawnBubbleFunc;
	HSCRIPTFUNCTION m_pOnSpawnSplashFunc;
};

//! Structure passed to CFlock::RayTest method, filled with intersection parameters.
struct SFlockHit {
	//! Hit object.
	CBoidObject *object;
	//! Distance from ray origin to the hit distance.
	float dist;
};

//////////////////////////////////////////////////////////////////////////

/*!
 *	Define flock of boids, where every boid share common properties and recognize each other.
 */
class CFlock : public IAIEventListener
{
public:
	CFlock( IEntity *pEntity,EFlockType flockType );
	virtual ~CFlock();

	//! Initialize boids in flock.
	virtual void CreateBoids( SBoidsCreateContext &ctx );

	// Called when entering/leaving game mode.
	virtual void Reset();

	//! Create boids in flock.
	//! Must be overriden in derived specialized flocks.
	virtual bool CreateEntities();

	virtual CBoidObject* CreateBoid() { return 0; };

	void DeleteEntities( bool bForceDeleteAll );

	int GetId() const { return m_id; };
	EFlockType GetType() const { return m_type; };

	void SetPos( const Vec3& pos );
	Vec3 GetPos() const { return m_origin; };

	void AddBoid( CBoidObject *boid );
	int GetBoidsCount() { return m_boids.size(); }
	CBoidObject* GetBoid( int index ) { return m_boids[index]; }

	float GetMaxVisibilityDistance() const { return m_bc.maxVisibleDistance; };

	//! Retrieve general boids settings in this flock.
	void GetBoidSettings( SBoidContext &bc ) { bc = m_bc; };
	//! Set general boids settings in this flock.
	void SetBoidSettings( SBoidContext &bc );

	bool IsFollowPlayer() const { return m_bc.followPlayer; };

	void ClearBoids();

	//! Check ray to flock intersection.
	bool RayTest( Vec3 &raySrc,Vec3 &rayTrg,SFlockHit &hit );

	const char* GetModelName() const { return m_model; };

	//! Static function that initialize defaults of boids info.
	static void GetDefaultBoidsContext( SBoidContext &bc );

	//! Enable/Disable Flock to be updated and rendered.
	virtual void SetEnabled( bool bEnabled );
	//! True if this flock is enabled, and must be updated and rendered.
	bool IsEnabled() const { return m_bEnabled; }

	//! Set how much percent of flock is visible.
	//! value 0 - 100.
	void SetPercentEnabled( int percent );

	//! See if this flock must be active now.
	bool IsFlockActive();

	//! flock's container should not be saved
	bool IsSaveable() { return(false); }

	//! Get entity owning this flock.
	IEntity* GetEntity() const { return m_pEntity; }

	void UpdateBoidsViewDistRatio();

	//////////////////////////////////////////////////////////////////////////
	// IEntityContainer implementation.
	//////////////////////////////////////////////////////////////////////////
	void Update( CCamera *pCamera );
	void Render( const SRendParams &EntDrawParams );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// IAIEventListener implementation
	//////////////////////////////////////////////////////////////////////////
	virtual void OnAIEvent(EAIStimulusType type, const Vec3& pos, float radius, float threat, EntityId sender);
	//////////////////////////////////////////////////////////////////////////

	void OnBoidHit( EntityId nBoidId,SmartScriptTable &hit );
	void RegisterAIEventListener( bool bEnable );

	virtual void GetMemoryUsage(ICrySizer *pSizer )const;

public:
	static int m_e_flocks;
	static int m_e_flocks_hunt; // Hunting mode...
	bool m_bAnyKilled;

	//! All boid parameters.
	SBoidContext m_bc;

protected:
	typedef std::vector<CBoidObject*> Boids;
	Boids m_boids;
	Vec3 m_origin;

	// Bonding box.
	AABB m_bounds;

	//! Uniq id of this flock, assigned by flock manager at creation.
	int m_id;

	uint32 m_RequestedBoidsCount;
	//! Name of this flock.
	EFlockType m_type;
	
	string m_model;
	string m_boidEntityName;
	string m_boidDefaultAnimName;
	int m_nBoidEntityFlagsAdd;

	int m_updateFrameID;
	int m_percentEnabled;

	int m_nViewDistRatio;

	float m_fCenterFloatingTime;

	// Host entity.
	IEntity *m_pEntity;
	// precache
	_smart_ptr<ICharacterInstance> m_pPrecacheCharacter;
	bool m_bEnabled;
	bool m_bEntityCreated;
	bool m_bAIEventListenerRegistered;

	SBoidsCreateContext m_createContext;
};

//////////////////////////////////////////////////////////////////////////
// Specialized flocks for birds and fish.
//////////////////////////////////////////////////////////////////////////
class CBirdsFlock : public CFlock
{
public:
	CBirdsFlock( IEntity *pEntity ) : CFlock( pEntity,EFLOCK_BIRDS ), m_bAttractOutput(false)
		{ m_boidEntityName = "BirdBoid"; m_boidDefaultAnimName = "fly_loop"; };
	virtual void CreateBoids( SBoidsCreateContext &ctx );
	virtual void SetEnabled( bool bEnabled );

	void SetAttractionPoint(const Vec3 &point);
	bool GetAttractOutput() const { return m_bAttractOutput; }
	void SetAttractOutput(bool bAO) { m_bAttractOutput = bAO; }

private:
	bool m_bAttractOutput;	// Set to true when the AttractTo flownode output has been activated
};

//////////////////////////////////////////////////////////////////////////
class CFishFlock : public CFlock
{
public:
	CFishFlock( IEntity *pEntity ) : CFlock( pEntity,EFLOCK_FISH ) { m_boidEntityName = "FishBoid"; m_boidDefaultAnimName = "swim_loop"; };
	virtual void CreateBoids( SBoidsCreateContext &ctx );
};

#endif // __flock_h__
