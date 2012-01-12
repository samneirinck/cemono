////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001.
// -------------------------------------------------------------------------
//  File name:   flock.cpp
//  Version:     v1.00
//  Created:     5/4/2002 by Timur.
//  Compilers:   Visual C++ 7.0
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Flock.h"
#include "BugsFlock.h"

#include <IEntitySystem.h>
#include "BoidsProxy.h"

#include <float.h>
#include <limits.h>
#include <ITimer.h>
#include <IScriptSystem.h>
#include <ICryAnimation.h>
#include <Cry_Camera.h>
#include <CryPath.h>
#include <ISound.h>

#define BIRDS_PHYSICS_DENSITY 200
#define BIRDS_PHYSICS_INWATER_DENSITY 900
#define FISH_PHYSICS_DENSITY 850

#define  PHYS_FOREIGN_ID_BOID PHYS_FOREIGN_ID_USER-1

#define MAX_BIRDS_DISTANCE 300

#define MAX_SPEED 15
#define MIN_SPEED 2.5f

#define MAX_ATTRACT_DISTANCE 20
#define MIN_ATTRACT_DISTANCE 5

#define MAX_FLIGHT_HEIGHT 40
#define MIN_FLIGHT_HEIGHT 5

#undef MAX_REST_TIME
#define MAX_REST_TIME 5
//#define LANDING_SPEED 1.0f
#define LANDING_FORCE 2.0f

#define MAX_FLIGHT_TIME 30
#define MIN_FLIGHT_TIME 10

#define SCARE_DISTANCE 10

#define ALIGNMENT_FACTOR 1.0f
#define COHESION_FACTOR 1.0f
#define SEPARATION_FACTOR 10.0f

#define ORIGIN_ATTRACT_FACTOR 0.1f
#define DESIRED_HEIGHT_FACTOR 0.4f
#define AVOID_LAND_FACTOR 10.0f

#define MAX_ANIMATION_SPEED 1.7f

int CFlock::m_e_flocks = 1;
int CFlock::m_e_flocks_hunt = 1;


// Fast normalize vector and return square distance.
inline float normalize_fast( Vec3 &v )
{
	f32 fLenSqr = v.x*v.x + v.y*v.y + v.z*v.z;
	//	assert(fLen>0.0001f);
  union { f32* pf; unsigned int* pi; } u0, u1; 
  u0.pf = &fLenSqr;
	unsigned int *n1 = u0.pi;
	unsigned int n = 0x5f3759df - (*n1 >> 1);
  u1.pi = &n; 
	f32 *n2 = u1.pf;
	float d = (1.5f - (fLenSqr * 0.5f) * *n2 * *n2) * *n2;
	v.x*=d; v.y*=d; v.z*=d;
	return fLenSqr;
}

//////////////////////////////////////////////////////////////////////////
CBoidObject::CBoidObject( SBoidContext &bc )
{
	m_flock = 0;
	m_entity = 0;

	m_heading.Set(1,0,0);
	m_accel.Set(0,0,0);
	m_currentAccel.Set(0,0,0);
	m_speed = 0;
	m_object = 0;
	m_banking = 0;
	m_bankingTrg = 0;
	m_alignHorizontally = 0;
	m_scale = 1.0f;
	// flags
	m_dead = false;
	m_dying = false;
	m_physicsControlled = false;
	m_inwater = false;
	m_nodraw = false;
	m_pPhysics = 0;
	m_pickedUp = false;
	m_scareRatio = 0;

	m_speed = bc.MinSpeed + (frand()+1)/2.0f*(bc.MaxSpeed - bc.MinSpeed);
	m_heading = Vec3(frand(),frand(),0).GetNormalized();

}

//////////////////////////////////////////////////////////////////////////
CBoidObject::~CBoidObject()
{
	m_pPhysics = 0;
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::PlayAnimation( const char *animation,bool bLooped,float fBlendTime )
{
	if (m_object)
	{
		CryCharAnimationParams animParams;
		if (bLooped)
			animParams.m_nFlags |= CA_LOOP_ANIMATION;

		animParams.m_nFlags |= CA_SUPPRESS_WARNINGS;
		animParams.m_nFlags |= CA_REMOVE_FROM_FIFO;

		animParams.m_fTransTime = fBlendTime;
		m_object->GetISkeletonAnim()->StartAnimation( animation, animParams );
		m_object->SetAnimationSpeed( 1.0f );
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::PlayAnimationId( int nIndex,bool bLooped,float fBlendTime )
{
	if (nIndex >= 0 && nIndex < (int)m_flock->m_bc.animations.size())
		PlayAnimation( m_flock->m_bc.animations[nIndex],bLooped,fBlendTime );
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::PlaySound( int nIndex )
{
	if (nIndex >= 0 && nIndex < (int)m_flock->m_bc.sounds.size())
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
		if (pEntity)
		{
			IEntitySoundProxy* pSndProxy = (IEntitySoundProxy*)pEntity->CreateProxy(ENTITY_PROXY_SOUND);
			Vec3 ofs(0,0,0),dir(FORWARD_DIRECTION);
			pSndProxy->PlaySound( m_flock->m_bc.sounds[nIndex],ofs,dir,FLAG_SOUND_DEFAULT_3D, eSoundSemantic_Living_Entity );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
int CBoidObject::GetGeometrySurfaceType()
{
	//////////////////////////////////////////////////////////////////////////
	if (m_object)
	{
		ISurfaceType *pSurfaceType = gEnv->pEntitySystem->GetBreakableManager()->GetFirstSurfaceType(m_object);
		if (pSurfaceType)
			return pSurfaceType->GetId();
	}
	else
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
		if (pEntity)
		{
			IStatObj *pStatObj = pEntity->GetStatObj(0|ENTITY_SLOT_ACTUAL);
			if (pStatObj)
			{
				ISurfaceType *pSurfaceType = gEnv->pEntitySystem->GetBreakableManager()->GetFirstSurfaceType(pStatObj);
				if (pSurfaceType)
					return pSurfaceType->GetId();
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::OnEntityEvent( SEntityEvent &event )
{
	switch (event.event)
	{
	case ENTITY_EVENT_DONE:
		m_entity = 0;
		m_object = 0;
		m_pPhysics = 0;
		break;
	case ENTITY_EVENT_COLLISION:
		OnCollision(event);
		break;
	case ENTITY_EVENT_PICKUP:
		{
			bool bPickUp = event.nParam[0] == 1;
			OnPickup( bPickUp,event.fParam[0] );
			if (m_pickedUp && m_noentity)
			{
				Kill( Vec3(0,0,0),Vec3(0,0,0) );
			}
			m_pickedUp = bPickUp;
			if (bPickUp)
			{
				if (m_pPhysics)
				{
					// Enabling reporting of physics collisions.
					pe_params_flags pf;
					pf.flagsOR = pef_log_collisions;
					m_pPhysics->SetParams(&pf);

					// Awake physical entity.
					pe_action_awake aa;
					aa.bAwake = 1;
					m_pPhysics->Action(&aa);
				}
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::OnCollision( SEntityEvent &event )
{
	EventPhysCollision *pCollision = (EventPhysCollision *)(event.nParam[0]);

	IPhysicalEntity *pPhysObject = 0;
	if (pCollision->pEntity[0] == m_pPhysics)
	{
		pPhysObject = pCollision->pEntity[1];
	}
	else if (pCollision->pEntity[1] == m_pPhysics)
	{
		pPhysObject = pCollision->pEntity[0];
	}

	// Get speed.
	if (pPhysObject)
	{
		pe_status_dynamics dynamics;
		pPhysObject->GetStatus(&dynamics);
		if (dynamics.v.GetLengthFast() > 1.0f)
		{
			m_physicsControlled = false;
			Kill( m_pos,Vec3(0,0,0) );
		}
	}

}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::OnPickup( bool bPickup,float fSpeed )
{
	if (bPickup)
	{
		m_physicsControlled = true;
	}
	else
	{
		m_physicsControlled = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::CalcOrientation( Quat &qOrient )
{
	if (m_physicsControlled && m_pPhysics)
	{
		pe_status_pos ppos;
		m_pPhysics->GetStatus(&ppos);
		qOrient = ppos.q;
		return;
	}

	if (m_heading.IsZero())
		m_heading = Vec3(1,0,0);

	Vec3 dir = m_heading;
	if (m_alignHorizontally != 0)
	{
		dir.z *= (1.0f-m_alignHorizontally);
	}
	dir.NormalizeFast();
	if (m_banking != 0)
		qOrient.SetRotationVDir( dir,-m_banking*0.5f );
	else
		qOrient.SetRotationVDir( dir );
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::Render( SRendParams &rp,CCamera &cam,SBoidContext &bc )
{
	/*
	if (m_nodraw)
		return;
	if (m_object)
	{
		// Cull boid.
		if (!cam.IsSphereVisibleFast( Sphere(m_pos,bc.fBoidRadius*bc.boidScale) ))
			return;

		Matrix44 mtx;
		CalcMatrix( mtx );

		Matrix44 ms;
		ms.SetIdentity();
		ms=Matrix33::CreateScale( Vec3(bc.boidScale,bc.boidScale,bc.boidScale) )*ms;

		mtx = ms * mtx;

		//m_object->Update();
		rp.pMatrix = &mtx;

		m_object->Update();
		m_object->Draw( rp, mtx.GetTranslationOLD() );
	}
	*/
}

//////////////////////////////////////////////////////////////////////////
inline float AngleBetweenVectors( const Vec3 &v1,const Vec3 &v2 )
{
	float a = acos_tpl(v1.Dot(v2));
	Vec3 r = v1.Cross(v2);
	if (r.z < 0)
		a = -a;
	return a;
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::CalcMovement( float dt,SBoidContext &bc,bool banking )
{
	// Calc movement with current velocity.
	Vec3 prevAccel(0,0,0);

	if (banking)
	{
		if (m_currentAccel.x != 0 && m_currentAccel.y != 0 && m_currentAccel.z != 0)
			prevAccel = m_currentAccel.GetNormalized();
		else
			banking = false;
	}

	m_currentAccel = m_currentAccel*bc.fSmoothFactor + m_accel*(1.0f - bc.fSmoothFactor);

	Vec3 velocity = m_heading*m_speed;
	m_pos = m_pos + velocity*dt;
	velocity = velocity + m_currentAccel*dt;
	m_speed = velocity.GetLength();

	if (fabs(m_speed) > 0.0001f)
	{
		Vec3 newHeading = velocity * (1.0f/m_speed); // Normalized velocity vector is our heading.

		if (bc.fMaxTurnRatio)
		{
			float fHeadingSmothFactor = bc.fMaxTurnRatio * bc.fSmoothFactor;
			if (fHeadingSmothFactor > 1.0f)
				fHeadingSmothFactor = 1.0f;
			m_heading = newHeading*fHeadingSmothFactor + m_heading*(1.0f - fHeadingSmothFactor);
		}
		else
		{
			m_heading = newHeading;
		}
	}

	if (m_speed > bc.MaxSpeed)
		m_speed = bc.MaxSpeed;
	if (m_speed < bc.MinSpeed)
		m_speed = bc.MinSpeed;

	if (banking)
	{
		Vec3 sideDir = m_heading.Cross(Vec3(0,0,1));
		if (sideDir.IsZero())
			sideDir = m_heading.Cross(Vec3(0,1,0));
//		Vec3 v = m_currentAccel.GetLength();
		m_bankingTrg = prevAccel.Dot(sideDir.GetNormalized());
	}
	else
		m_bankingTrg = 0;

	// Slowly go into the target banking.
	float bd = m_bankingTrg - m_banking;
	m_banking = m_banking + bd*dt*10.0f;

	if (m_object && fabs(m_speed) > 0.0001f)
	{
		float animSpeed = min(m_speed / (bc.MaxSpeed - bc.MinSpeed + 0.1f), bc.MaxAnimationSpeed);
		//float animSpeed = m_speed * bc.MaxAnimationSpeed;
		m_object->SetAnimationSpeed( animSpeed );
	}

	
	if (m_pPhysics)
	{
		//pe_params_pos ppos;
		//ppos.pos = Vec3(m_pos);
		//m_pPhysics->SetParams(&ppos);

		//pe_action_set_velocity asv;
		//asv.v = m_heading*m_speed;
		//m_pPhysics->Action(&asv);
	}
	
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::CalcFlockBehavior( SBoidContext &bc,Vec3 &vAlignment,Vec3 &vCohesion,Vec3 &vSeparation )
{
	// Vector of sight between boids.
	Vec3 sight;

	float MaxAttractDistance2 = bc.MaxAttractDistance*bc.MaxAttractDistance;
	float MinAttractDistance2 = bc.MinAttractDistance * bc.MinAttractDistance;

	vSeparation(0,0,0);
	vAlignment(0,0,0);
	vCohesion(0,0,0);

	// Average alignment and speed.
	Vec3 avgAlignment(0,0,0);
	Vec3 avgNeighborsCenter(0,0,0);
	int numMates = 0;

	int numBoids = m_flock->GetBoidsCount();
	for (int i = 0; i < numBoids; i++)
	{
		CBoidObject *boid = m_flock->GetBoid(i);
		if (boid == this) // skip myself.
			continue;

		sight = boid->m_pos - m_pos;

		float dist2 = normalize_fast(sight);

		// Check if this boid is in our range of sight.
		// And If this neighbor is in our field of view.
		if (dist2 < MaxAttractDistance2 && m_heading.Dot(sight) > bc.cosFovAngle)
		{
			// Separation from other boids.
			if (dist2 < MinAttractDistance2)
			{
				// Boid too close, distract from him.
				float w = (1.0f - dist2/MinAttractDistance2);
				vSeparation -= sight*(w)*bc.factorSeparation;
			}

			numMates++;

			// Alignment with boid direction.
			avgAlignment += boid->m_heading * boid->m_speed;

			// Calculate average center of all neighbor boids.
			avgNeighborsCenter += boid->m_pos;
		}
	}
	if (numMates > 0)
	{
		avgAlignment = avgAlignment * (1.0f/numMates);
		//float avgSpeed = avgAlignment.GetLength();
		vAlignment = avgAlignment;

		// Attraction to mates.
		avgNeighborsCenter = avgNeighborsCenter * (1.0f/numMates);
		Vec3 cohesionDir = avgNeighborsCenter - m_pos;

		float sqrDist = normalize_fast(cohesionDir);
		float w = (sqrDist - MinAttractDistance2)/(MaxAttractDistance2 - MinAttractDistance2);
		vCohesion = cohesionDir*w;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::Physicalize( SBoidContext &bc )
{
	pe_params_particle ppart;
	//ppart.gravity = Vec3(0,0,0);
	ppart.flags = particle_traceable | particle_no_roll | pef_never_affect_triggers | pef_log_collisions;
	ppart.mass = 0;
	ppart.size = max(bc.fBoidRadius,0.01f);
	ppart.thickness = max(bc.fBoidThickness,0.01f);
	ppart.gravity = Vec3(0,0,0);
	ppart.kAirResistance = 0.0f;
	ppart.surface_idx = GetGeometrySurfaceType();

	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
	if (pEntity)
	{
		SEntityPhysicalizeParams params;
		params.pParticle = &ppart;
		params.type = PE_PARTICLE;
		pEntity->Physicalize(params);
		m_pPhysics = pEntity->GetPhysics();
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::CreateRigidBox( SBoidContext &bc,const Vec3 &boxSize,float mass,float density )
{
	if (m_pPhysics)
	{
		m_pPhysics = 0;
	}

	Vec3 orgVelocity = m_speed*m_heading;
	if (m_pPhysics && m_pPhysics->GetType() == PE_PARTICLE)
	{
		pe_params_particle pparams;
		m_pPhysics->GetParams(&pparams);
		orgVelocity = pparams.velocity*pparams.heading;
	}

	Quat q;
	CalcOrientation( q);

	pe_params_pos bodypos;
	bodypos.pos = m_pos;
	bodypos.q = q;
	bodypos.scale = m_scale;

	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
	if (!pEntity)
		return;

	SEntityPhysicalizeParams entityPhysParams;
	entityPhysParams.type = PE_RIGID;
	pEntity->Physicalize( entityPhysParams );
	m_pPhysics =  pEntity->GetPhysics();
	if (!m_pPhysics)
		return;
	m_pPhysics->SetParams(&bodypos);

	pe_params_flags pf;
	pf.flagsOR = pef_never_affect_triggers|pef_never_break;
	m_pPhysics->SetParams(&pf);

	primitives::box geomBox;
	geomBox.Basis.SetIdentity();
	geomBox.center.Set(0,0,0);
	geomBox.size = boxSize;
	geomBox.bOriented = 0;
	IGeometry *pGeom = bc.physics->GetGeomManager()->CreatePrimitive( primitives::box::type,&geomBox );
	phys_geometry *physGeom = bc.physics->GetGeomManager()->RegisterGeometry( pGeom );
	pGeom->Release();

	pe_geomparams partpos;
	partpos.pos.Set(0,0,0);
	if (mass > 0)
		partpos.mass = mass; // some fish mass.
	if (density > 0)
		partpos.density = density;
	partpos.surface_idx = GetGeometrySurfaceType();

	m_pPhysics->AddGeometry( physGeom,&partpos,0 );
	bc.physics->GetGeomManager()->UnregisterGeometry( physGeom );

	pe_simulation_params symparams;
	symparams.damping = 0.3f;
	symparams.dampingFreefall = 0.2f;
	m_pPhysics->SetParams(&symparams);

	pe_params_buoyancy pb;
	pb.waterDensity = 1000.0f;
	pb.waterDamping = 1;
	pb.waterResistance = 1000;
	pb.waterPlane.n.Set(0,0,1);
	//pb.waterPlane.origin.set(0,0,gEnv->p3DEngine->GetWaterLevel(&m_center));
	pb.waterPlane.origin.Set( 0,0,bc.waterLevel );
	m_pPhysics->SetParams(&pb);

	// Set original velocity on physics.
	pe_action_set_velocity psetvel;
	psetvel.v = orgVelocity;
	m_pPhysics->Action( &psetvel );
}

//////////////////////////////////////////////////////////////////////////
void CBoidObject::CreateArticulatedCharacter( SBoidContext &bc,const Vec3 &size,float mass )
{
	Vec3 orgVelocity = m_speed*m_heading;
	if (m_pPhysics && m_pPhysics->GetType() == PE_PARTICLE)
	{
		pe_params_particle pparams;
		m_pPhysics->GetParams(&pparams);
		orgVelocity = pparams.velocity*pparams.heading;
	}

	if (m_pPhysics)
	{
		m_pPhysics = 0;
	}

	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
	if (!pEntity)
		return;

	Quat q;
	CalcOrientation(q);

	pe_params_pos bodypos;
	bodypos.pos = m_pos;
	bodypos.q = q;
	bodypos.scale = m_scale;
	bodypos.iSimClass = 2;

	SEntityPhysicalizeParams entityPhysParams;
	entityPhysParams.type = PE_ARTICULATED;
	entityPhysParams.nSlot = 0;
	entityPhysParams.mass = mass;
	entityPhysParams.nLod = 1;
	pEntity->Physicalize( entityPhysParams );

	// After physicalization reset entity slot matrix if present.
	if (!bc.vEntitySlotOffset.IsZero())
	{
		Matrix34 tmIdent;
		tmIdent.SetIdentity();
		pEntity->SetSlotLocalTM(0,tmIdent);
	}

	m_pPhysics =  pEntity->GetPhysics();
	if (!m_pPhysics)
		return;
	m_pPhysics->SetParams(&bodypos);

	//m_pPhysics =  m_object->RelinquishCharacterPhysics();

	pe_params_flags pf;
	pf.flagsOR = pef_never_affect_triggers|pef_never_break;
	m_pPhysics->SetParams(&pf);
	
	pe_params_articulated_body pab;
	pab.bGrounded = 0;
	pab.bCheckCollisions = 1;
	pab.bCollisionResp = 1;
	m_pPhysics->SetParams(&pab);

	pe_simulation_params symparams;
	symparams.damping = 0.3f;
	symparams.dampingFreefall = 0.2f;
	m_pPhysics->SetParams(&symparams);

	pe_params_buoyancy pb;
	pb.waterDensity = 1000.0f;
	pb.waterDamping = 1;
	pb.waterResistance = 1000;
	pb.waterPlane.n.Set(0,0,1);
	//pb.waterPlane.origin.set(0,0,gEnv->p3DEngine->GetWaterLevel(&m_center));
	pb.waterPlane.origin.Set( 0,0,bc.waterLevel );
	m_pPhysics->SetParams(&pb);

	// Set original velocity on ragdoll.
	pe_action_set_velocity psetvel;
	psetvel.v = orgVelocity;
	m_pPhysics->Action( &psetvel );

	pe_params_part pp;
	pp.flagsColliderOR=pp.flagsColliderAND = geom_colltype_debris;
	pp.flagsAND = ~(geom_colltype_vehicle|geom_colltype6);
	m_pPhysics->SetParams(&pp);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CBoidBird::CBoidBird( SBoidContext &bc )
: CBoidObject( bc )
{
	m_flightTime = 0;
	m_lastThinkTime = 0;
	m_maxFlightTime = 0;

	m_fNoCenterAttract	= 0.f;
	m_fNoKeepHeight			= 0.f;
	m_fAttractionFactor	= 0.f;

	m_landing = false;
	m_takingoff = true;
	m_onGround = false;
	m_attractedToPt = false;
	m_spawnFromPt = false;

	m_maxFlightTime = MIN_FLIGHT_TIME + (frand()+1)/2*(MAX_FLIGHT_TIME-MIN_FLIGHT_TIME);
	m_desiredHeigh = bc.MaxHeight + (frand()+1)/2*(bc.MaxHeight - bc.MinHeight);
	m_birdOriginPos = bc.flockPos;
	m_birdOriginPosTrg = m_birdOriginPos;
}

CBoidBird::~CBoidBird()
{
}

void CBoidBird::OnFlockMove( SBoidContext &bc )
{
	m_birdOriginPos = bc.flockPos;
	m_birdOriginPosTrg = bc.flockPos;
	bc.randomFlockCenter = bc.flockPos;
}

//////////////////////////////////////////////////////////////////////////
void CBoidBird::Physicalize( SBoidContext &bc )
{
	bc.fBoidThickness = bc.fBoidRadius*0.5f;
	CBoidObject::Physicalize( bc );
}

//////////////////////////////////////////////////////////////////////////
void CBoidBird::UpdatePhysics( float dt,SBoidContext &bc )
{
	if (m_pPhysics)
	{
		pe_status_pos ppos;
		m_pPhysics->GetStatus(&ppos);
		m_pos = ppos.pos;
		Vec3 pos = m_pos;
		// When hitting water surface, increase physics desnity.
		if (!m_inwater && m_pos.z+bc.fBoidRadius <= bc.engine->GetWaterLevel( &pos ))
		{
			m_inwater = true;
			pe_simulation_params sym;
			sym.density = BIRDS_PHYSICS_INWATER_DENSITY;
			m_pPhysics->SetParams( &sym );
		}
		pe_status_awake tmp;
		bool bAwake = m_pPhysics->GetStatus(&tmp) != 0;
		if (bAwake && m_pPhysics->GetType() == PE_ARTICULATED)
		{
 			m_object->GetISkeletonPose()->SynchronizeWithPhysicalEntity(m_pPhysics);
		}
		if (!m_inwater && !bAwake)
			//if (m_pPhysics->GetStatus(&pe_status_awake()))
		{
			// Resting. disable physics.
			//m_physicsControlled = false;
			//bc.physics->DestroyPhysicalEntity( m_pPhysics );
			//m_pPhysics = 0;
			//m_dead = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidBird::Update( float dt,SBoidContext &bc )
{
	if (m_physicsControlled)
	{
		UpdatePhysics(dt,bc);
		return;
	}
	if (m_dead)
		return;

	m_lastThinkTime += dt;

	if (bc.waterLevel > bc.terrainZ)
		bc.terrainZ = bc.waterLevel;

	//if (m_lastThinkTime) 
	{
	//	/*
		if (bc.followPlayer && !m_spawnFromPt)
		{
			if (m_pos.GetSquaredDistance(bc.playerPos) > MAX_BIRDS_DISTANCE*MAX_BIRDS_DISTANCE)
			{
				float z = bc.MinHeight + (frand()+1)/2.0f*(bc.MaxHeight - bc.MinHeight);
				m_pos = bc.playerPos + Vec3(frand()*MAX_BIRDS_DISTANCE,frand()*MAX_BIRDS_DISTANCE,z );
				m_speed = bc.MinSpeed + ((frand()+1)/2.0f) / (bc.MaxSpeed - bc.MinSpeed);
				m_heading = Vec3(frand(),frand(),0).GetNormalized();
			}
		}
//		*/


		if (!m_onGround)
		{
			Think(dt,bc);

			// Calc movement with current velocity.
			CalcMovement( dt,bc,true );
			m_accel.Set(0,0,0);
		}

		// Check if landed.
		if ((m_landing || m_dying) && !m_onGround)
		{
			float LandEpsilon = 0.5f;
			if (m_pos.z - bc.terrainZ < LandEpsilon)
			{
				m_onGround = true;
				m_landing = false;
				m_flightTime = 0;

				PlayAnimation( "landing",false );

				// Check if landed on water.
				if (m_pos.z-bc.waterLevel < LandEpsilon+0.1f && !m_dying)
				{
					//! From water immidiatly take off.
					//! Gives fishing effect. 
					TakeOff(bc);
				}
			}
		}

		if (m_takingoff)
		{
			if (m_pos.z - bc.terrainZ > bc.MinHeight)
			{
				m_takingoff = false;
			}
		}

		if (!bc.noLanding)
		{
			if (!m_onGround)
			{
				m_flightTime += dt;
				if (m_flightTime > m_maxFlightTime /*&& (m_pos.z - bc.terrainZ) < bc.MinHeight*2*/)
				{
					// Wants to land.
					if (!m_landing)
					{
						// Only now wants to land.
						//m_object->StartAnimation( "takeoff" );
					}
					m_landing = true;
				}
			}
			else
			{
				m_flightTime += dt;
				if (m_flightTime > MAX_REST_TIME || m_pos.GetDistance(bc.playerPos) < SCARE_DISTANCE)
				{
					// Take-off.
					TakeOff(bc);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidBird::TakeOff( SBoidContext &bc )
{
	// Take-off.
	m_flightTime = 0;
	m_landing = false;
	m_onGround = false;
	m_maxFlightTime = MIN_FLIGHT_TIME + (frand()+1)/2*(MAX_FLIGHT_TIME-MIN_FLIGHT_TIME);
	//m_desiredHeigh = bc.MinHeight + (frand()+1)/2*(MAX_FLIGHT_HEIGHT - bc.MinHeight);
	m_desiredHeigh = bc.flockPos.z;
	m_takingoff = true;
	m_heading.z = 0;
	
	if (m_object)
	{
		PlayAnimation( "fly_loop" ,true );
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidBird::Think( float dt,SBoidContext &bc )
{
	m_accel(0,0,0);
	if (m_spawnFromPt)
	{
		// Set the acceleration of the boid
		float fHalfDesired = m_desiredHeigh * .5f;
		if (m_pos.z < fHalfDesired)
		{
			m_accel.z = 3.f;
		}
		else
		{
			m_accel.z = .5f;
			// Set z-acceleration
			float fRange = 1.f - (m_pos.z-fHalfDesired)/fHalfDesired;	// 1..0 range

			if (m_heading.z > 0.2f)
				m_accel.z = .5f + fRange;	// 2..1

			m_accel.x += m_heading.x * .1f;
			m_accel.y += m_heading.y * .1f;
		}

		if (m_pos.z > m_desiredHeigh)
			SetSpawnFromPt(false);

		// Limits birds to above water and land.
		if (m_pos.z < bc.terrainZ-0.2f)
		{
			m_pos.z = bc.terrainZ-0.2f;
		}
		if (m_pos.z < bc.waterLevel-0.2f)
		{
			m_pos.z = bc.waterLevel-0.2f;
		}
		return;
	}

	Vec3 flockHeading(0,0,0);

	float height = m_pos.z - bc.terrainZ;
		
	// Free will.
	// Continue accelerating in same dir until target speed reached.
	// Try to maintain average speed of (maxspeed+minspeed)/2
	float targetSpeed = (bc.MaxSpeed + bc.MinSpeed)/2;
	m_accel -= m_heading*(m_speed-targetSpeed)*0.5f;

	// Desired height.
	float dh = m_desiredHeigh - m_pos.z;
	float dhexp = -(dh*dh)/(3*3);

	dhexp = clamp_tpl(dhexp,-70.0f,70.0f); // Max values for expf
	// Gaussian weight.
	float fKeepHeight = bc.factorKeepHeight - m_fNoKeepHeight;
	m_fNoKeepHeight = max(0.f, m_fNoKeepHeight - .01f*dt);
	m_accel.z = cry_expf(dhexp) * fKeepHeight;
	//m_accel.z = dh * DESIRED_HEIGHT_FACTOR;

		//exp(-(d.x*d.x*space_metric_r))
	//m_accel.z = (m_desiredHeigh+bc.terrainZ - m_pos.z) * DESIRED_HEIGHT_FACTOR;

	
	if (bc.factorAlignment != 0)
	{
		//CalcCohesion();
		Vec3 alignmentAccel;
		Vec3 cohesionAccel;
		Vec3 separationAccel;
		CalcFlockBehavior(bc,alignmentAccel,cohesionAccel,separationAccel);

		//! Adjust for allignment,
		//m_accel += alignmentAccel.Normalized()*ALIGNMENT_FACTOR;
		m_accel += alignmentAccel*bc.factorAlignment;
		m_accel += cohesionAccel*bc.factorCohesion;
		m_accel += separationAccel;
	}

	// Avoid land.
	if (height < bc.MinHeight && !m_landing)
	{
		float v = (1.0f - height/bc.MinHeight);
		m_accel += Vec3(0,0,v*v)*bc.factorAvoidLand;
	}
	else
	// Avoid max height.
	if (height > bc.MaxHeight)
	{
		float v = (height - bc.MaxHeight)*0.1f;
		m_accel += Vec3(0,0,-v);
	}
	else
	{
		// Always try to accelerate in direction opposite to current in Z axis.
		m_accel.z = -(m_heading.z*m_heading.z*m_heading.z * 100.0f);
	}

	// Attract to origin point.
	float fAttractToOrigin = bc.factorAttractToOrigin - m_fNoCenterAttract;
	m_fNoCenterAttract = max(0.f, m_fNoCenterAttract - .01f*dt);
	if (bc.followPlayer)
	{
		m_accel += (bc.playerPos - m_pos) * fAttractToOrigin;
	}
	else
	{
		//m_accel += (m_birdOriginPos - m_pos) * bc.factorAttractToOrigin;
		if ((rand()&31) == 1)
		{
			m_birdOriginPos = Vec3(	bc.flockPos.x+frand()*bc.fSpawnRadius,bc.flockPos.y+frand()*bc.fSpawnRadius,bc.flockPos.z+frand()*bc.fSpawnRadius );
			if (m_birdOriginPos.z - bc.terrainZ < bc.MinHeight)
			{
				m_birdOriginPos.z = bc.terrainZ + bc.MinHeight;
			}
		}

		/*
		if (m_pos.x < bc.flockPos.x-bc.fSpawnRadius || m_pos.x > bc.flockPos.x+bc.fSpawnRadius ||
				m_pos.y < bc.flockPos.y-bc.fSpawnRadius || m_pos.y > bc.flockPos.y+bc.fSpawnRadius ||
				m_pos.z < bc.flockPos.z-bc.fSpawnRadius || m_pos.z > bc.flockPos.z+bc.fSpawnRadius)
				*/
		{
			m_accel += (m_birdOriginPos - m_pos) * fAttractToOrigin;
		}
	}

	// Attract to bc.attractionPoint
	if (m_pos.GetSquaredDistance2D(bc.attractionPoint) < 5.f)
	{
		SetAttracted(false);

		CBirdsFlock *pFlock = (CBirdsFlock *)m_flock;
		if (!pFlock->GetAttractOutput())
		{	// Activate the AttractTo flownode output
			pFlock->SetAttractOutput(true);

			// Activate the flow node output
			IEntity *pFlockEntity = pFlock->GetEntity();
			if (pFlockEntity)
			{
				IScriptTable *scriptObject = pFlockEntity->GetScriptTable();
				if (scriptObject)
					Script::CallMethod(scriptObject, "OnAttractPointReached");
			}
		}
	}
	if (m_attractedToPt)
	{
		m_accel += (bc.attractionPoint - m_pos) * m_fAttractionFactor;
		if (m_fAttractionFactor < 300.f*bc.factorAttractToOrigin) m_fAttractionFactor += 3.f*dt;
	}

	// Avoid collision with Terrain and Static objects.
	float fCollisionAvoidanceWeight = 1.0f;
	float fCollisionDistance = 30;

	m_alignHorizontally = 0;
	// Wants to land.
	if (m_landing)
	{
		// Go down.
		m_accel.z = -LANDING_FORCE;
		fCollisionDistance = 4.0f;

		if (m_pos.z - bc.terrainZ < bc.MinHeight)
		{
			// Landing procedure.
			// Slow down bird, and align horizontally.
			float l = (bc.MinHeight - (m_pos.z - bc.terrainZ))/bc.MinHeight;
			//m_accel = m_accel.Normalized()*l;
			m_accel += -m_heading*m_speed*l;
			//m_speed = m_speed*(1.2f - l);
			m_alignHorizontally = l;
		}
	}

	if (bc.avoidObstacles)
	{
		// Avoid obstacles & terrain.
		IPhysicalWorld *physWorld = bc.physics;

		Vec3 vPos = m_pos;
		Vec3 vDir = m_heading*fCollisionDistance;
		int objTypes = ent_all|ent_no_ondemand_activation;
		int flags = rwi_stop_at_pierceable|rwi_ignore_terrain_holes;
		ray_hit hit;
		int col = physWorld->RayWorldIntersection( vPos,vDir,objTypes,flags,&hit,1 );
		if (col != 0 && hit.dist > 0)
		{
			//GetIEditor()->GetRenderer()->SetMaterialColor( 1,0,0,1 );
			//GetIEditor()->GetRenderer()->DrawLine( m_pos,m_pos+m_heading*hit.dist );

			if (m_landing)
			{
				// Decrease speed of bird.
				m_accel = -m_heading*(fCollisionDistance - hit.dist)/fCollisionDistance;
			}
			else
			{
				// Turn right/up.
				Vec3 normal = hit.n;
				//if (m_banking > 0) sideDir = -sideDir;
				//m_accel -= m_heading*(fCollisionDistance - hit.dist)*fCollisionAvoidanceWeight;
				//m_accel += (sideDir*2.0f+Vec3(0,0,1))*(fCollisionDistance - hit.dist)*fCollisionAvoidanceWeight;
				float w = (1.0f - hit.dist/fCollisionDistance);
				//Vec3 R = (m_heading - (2.0f*normal)).Normalized();
				m_accel += normal*w*w*bc.factorAvoidLand * fCollisionAvoidanceWeight;
			}
		}
	}

	if (m_landing)
	{
		//GetIEditor()->GetRenderer()->SetMaterialColor( 1,0,1,1 );
		//GetIEditor()->GetRenderer()->DrawLine( m_pos,m_pos+Vec3(0,0,-10) );
	}

	// Limits birds to above water and land.
	if (m_pos.z < bc.terrainZ-0.2f)
	{
		m_pos.z = bc.terrainZ-0.2f;
	}
	if (m_pos.z < bc.waterLevel-0.2f)
	{
		m_pos.z = bc.waterLevel-0.2f;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidBird::Kill( const Vec3 &hitPoint,const Vec3 &force )
{
	if (m_dead)
		return;

	m_flock->m_bAnyKilled = true;
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
	if (pEntity)
	{
		SpawnParams params;
		params.eAttachForm = GeomForm_Surface;
		params.eAttachType = GeomType_Render;
		gEnv->pEntitySystem->GetBreakableManager()->AttachSurfaceEffect( pEntity,0,SURFACE_BREAKAGE_TYPE("destroy"),params,ePEF_Independent );
	}

	if (CFlock::m_e_flocks_hunt == 0)
	{
		m_physicsControlled = false;
		m_dead = true;
		m_nodraw = true;

		if (pEntity)
		{
			pEntity->Hide(true);
		}

		if (!m_dead && m_pPhysics)
		{
			if (pEntity)
			{
				pEntity->UnphysicalizeSlot(0);
			}
			m_pPhysics = 0;
		}

		// No physics at all.
		return;
	}

	SBoidContext bc;
	m_flock->GetBoidSettings(bc);

	Vec3 impulse = force;
	if (impulse.GetLength() > 100.0f)
	{
		impulse.Normalize();
		impulse *= 100.0f;
	}

	//if (!m_physicsControlled)
	{
		if (!m_object)
			return;

		AABB aabb =	m_object->GetAABB( );
		Vec3 size = ((aabb.max - aabb.min)/2.2f)*m_scale;
		//CreateRigidBox( bc,size,bc.fBoidMass,-1 );
		CreateArticulatedCharacter( bc,size,bc.fBoidMass );
		//impulse += m_heading * (size.x*size.y*size.z)*BIRDS_PHYSICS_DENSITY;
		m_physicsControlled = true;

		// Small impulse.
		// Apply force on this body.
		pe_action_impulse theAction;
		Vec3 someforce;
		someforce = impulse;
		//someforce.Normalize();
		theAction.impulse = someforce;
		theAction.ipart = 0;
		theAction.iApplyTime = 0;
		if (m_pPhysics)
			m_pPhysics->Action(&theAction);
	}
	/*
	else
	//if (m_physicsControlled)
	{
		// Apply force on this body.
		pe_action_impulse theAction;
		theAction.impulse = impulse;
		theAction.point = hitPoint;
		theAction.iApplyTime = 0;
		if (m_pPhysics)
			m_pPhysics->Action(&theAction);
	}
	*/

	if (m_object && !m_dying && !m_dead)
	{
		m_object->GetISkeletonAnim()->StopAnimationsAllLayers();
		//m_object->StartAnimation( "death",CryCharAnimationParams() );
	}

	m_dead = true;
}

//////////////////////////////////////////////////////////////////////////
CBoidFish::CBoidFish( SBoidContext &bc )
: CBoidObject( bc )
{
	m_dead = 0;
	m_dying = 0;
	m_pOnSpawnBubbleFunc = NULL;
	m_pOnSpawnSplashFunc = NULL;
}

CBoidFish::~CBoidFish()
{
	if (m_pOnSpawnBubbleFunc)
		gEnv->pScriptSystem->ReleaseFunc( m_pOnSpawnBubbleFunc );
	if (m_pOnSpawnSplashFunc)
		gEnv->pScriptSystem->ReleaseFunc( m_pOnSpawnSplashFunc );
}

void CBoidFish::Update( float dt,SBoidContext &bc )
{
	if (m_dead)
		return;

	if (m_physicsControlled)
	{
		if (m_pPhysics)
		{
			// If fish is dead, get it position from physics.
			pe_status_pos ppos;
			m_pPhysics->GetStatus(&ppos);
			m_pos = ppos.pos;

			{
				m_dyingTime += frand()*0.2f;
				// Apply force on this body.
				pe_action_impulse theAction;
				theAction.impulse = Vec3(sinf(m_dyingTime*0.1f),cosf(m_dyingTime*0.13f),cosf(m_dyingTime*0.171f)*2.8f) * 0.01f;
				theAction.point = m_pos + Vec3(frand(),frand(),frand())*0.1f;
				theAction.iApplyTime = 0;
				theAction.ipart = 0;
				m_pPhysics->Action(&theAction);

				pe_simulation_params sym;
				sym.density = 950.0f + 200.0f*sinf(m_dyingTime);
				if (sym.density < FISH_PHYSICS_DENSITY)
					sym.density = FISH_PHYSICS_DENSITY;
				m_pPhysics->SetParams( &sym );
			}
		}
	}
	if (m_dying)
	{
		// If fish is dying it floats up to the water surface, and die there.
		//UpdateDying(dt,bc);
		m_dyingTime += dt;
		if (m_dyingTime > 60)
		{
			m_dead = true;
			m_dying = false;
			if (m_object)
				m_object->GetISkeletonAnim()->StopAnimationsAllLayers();
		}
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	if (bc.followPlayer)
	{
		if (m_pos.GetSquaredDistance(bc.playerPos) > MAX_BIRDS_DISTANCE*MAX_BIRDS_DISTANCE)
		{
			float z = bc.MinHeight + (frand()+1)/2.0f*(bc.MaxHeight - bc.MinHeight);
			m_pos = bc.playerPos + Vec3(frand()*MAX_BIRDS_DISTANCE,frand()*MAX_BIRDS_DISTANCE,z );
			m_speed = bc.MinSpeed + ((frand()+1)/2.0f) / (bc.MaxSpeed - bc.MinSpeed);
			m_heading = Vec3(frand(),frand(),0).GetNormalized();
		}
	}

	float height = m_pos.z - bc.terrainZ;

	m_accel.Set(0,0,0);
	m_accel = bc.factorRandomAccel*Vec3(frand(),frand(),frand());
	// Continue accelerating in same dir until target speed reached.
	// Try to maintain average speed of (maxspeed+minspeed)/2
	float targetSpeed = (bc.MaxSpeed + bc.MinSpeed)/2;
	m_accel -= m_heading*(m_speed-targetSpeed)*0.2f;


	
	if (bc.factorAlignment != 0)
	{
		Vec3 alignmentAccel;
		Vec3 cohesionAccel;
		Vec3 separationAccel;
		CalcFlockBehavior(bc,alignmentAccel,cohesionAccel,separationAccel);

		m_accel += alignmentAccel*bc.factorAlignment;
		m_accel += cohesionAccel*bc.factorCohesion;
		m_accel += separationAccel;
	}

	// Avoid water.
	if (m_pos.z > bc.waterLevel-1)
	{
		float h = bc.waterLevel - m_pos.z;
		float v = (1.0f - h);
		float vv = v*v;
		m_accel.z += (-vv)*bc.factorAvoidLand;

		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine( m_pos,ColorB(0,0,255,255),m_pos+m_accel,ColorB(0,0,255,255) );
	}
	// Avoid land.
	if (height < bc.MinHeight)
	{
		float v = (1.0f - height/(bc.MinHeight+0.01f));
		float vv = v*v;
		m_accel.z += vv*bc.factorAvoidLand;
		
		// Slow down fast.
		m_accel -= m_heading*(m_speed-0.1f)*vv*bc.factorAvoidLand;
		// Go to origin.
		m_accel += (bc.flockPos - m_pos) * vv * bc.factorAvoidLand;

		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine( m_pos,ColorB(255,0,0,255),Vec3(m_pos.x,m_pos.y,bc.terrainZ),ColorB(255,0,0,255) );
	}

	if (fabs(m_heading.z) > 0.5f)
	{
		// Always try to accelerate in direction opposite to the current in Z axis.
		m_accel.z += -m_heading.z * 0.8f;
	}

	// Attract to the origin point.
	if (bc.followPlayer)
	{
		m_accel += (bc.playerPos - m_pos) * bc.factorAttractToOrigin;
	}
	else
	{
		m_accel += (bc.randomFlockCenter - m_pos) * bc.factorAttractToOrigin;
	}

	bool bAvoidObstacles = bc.avoidObstacles;
	//////////////////////////////////////////////////////////////////////////
	// High Terrain avoidance.
	//////////////////////////////////////////////////////////////////////////
	Vec3 fwd_pos = m_pos + m_heading*1.0f; // Look ahead 1 meter.
	float fwd_z = bc.engine->GetTerrainElevation(fwd_pos.x,fwd_pos.y);
	if (fwd_z >= m_pos.z-bc.fBoidRadius)
	{
		// If terrain in front of the fish is high, enable obstacle avoidance.
		bAvoidObstacles = true;
	}

	//////////////////////////////////////////////////////////////////////////

	// Avoid collision with Terrain and Static objects.
	float fCollisionAvoidanceWeight = 10.0f;
	float fCollisionDistance = 2.0f;

	if (bAvoidObstacles)
	{
		// Avoid obstacles & terrain.
		IPhysicalWorld *physWorld = bc.physics;

		Vec3 vPos = m_pos;
		Vec3 vDir = m_heading*fCollisionDistance;
		// Add some random variation in probe ray.
		vDir.x += frand()*0.8f;
		vDir.y += frand()*0.8f;

		int objTypes = ent_all|ent_no_ondemand_activation;
		int flags = rwi_stop_at_pierceable|rwi_ignore_terrain_holes;
		ray_hit hit;

		//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine( vPos,ColorB(0,0,255,255),vPos+vDir,ColorB(0,0,255,255) );

		int col = physWorld->RayWorldIntersection( vPos,vDir,objTypes,flags,&hit,1 );
		if (col != 0 && hit.dist > 0)
		{
				// Turn from collided surface.
			Vec3 normal = hit.n;
			//normal.z = 0; // Only turn left/right.
			float w = (1.0f - hit.dist/fCollisionDistance);
			Vec3 R = m_heading - (2.0f*m_heading.Dot(normal))*normal;
			normalize_fast(R);
			R += normal;
			R.z = R.z*0.2f;
			m_accel += R*(w*w)*bc.factorAvoidLand * fCollisionAvoidanceWeight;

			//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine( m_pos,ColorB(0,0,255,255),hit.pt,ColorB(0,0,255,255) );

			//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine( hit.pt,ColorB(255,0,0,255),hit.pt+R*2,ColorB(255,0,0,255) );
			//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine( m_pos,ColorB(255,0,0,255),m_pos+R*2,ColorB(255,0,0,255) );
		}
	}

	/*
	if (rand()%80 == 1)
	{
		if (m_pos.GetSquaredDistance(bc.playerPos) < 10.0f*10.0f)
		{
			// Spawn bubble.
			SpawnParticleEffect( m_pos,bc,SPAWN_BUBBLE );
		}
	}
	*/

	//////////////////////////////////////////////////////////////////////////
	// Player must scare fishes off.
	//////////////////////////////////////////////////////////////////////////
	float sqrPlayerDist = m_pos.GetSquaredDistance(bc.playerPos);
	if (sqrPlayerDist < SCARE_DISTANCE*SCARE_DISTANCE)
	{
		Vec3 retreatDir = m_pos - bc.playerPos;
		normalize_fast(retreatDir);
		float scareFactor = (1.0f - sqrPlayerDist/(SCARE_DISTANCE*SCARE_DISTANCE));
		m_accel.x += retreatDir.x*scareFactor*bc.factorAvoidLand;
		m_accel.y += retreatDir.y*scareFactor*bc.factorAvoidLand;
	}

	//////////////////////////////////////////////////////////////////////////
	// Calc movement.
	CalcMovement( dt,bc,false );
	m_accel.Set(0,0,0);

	// Limits fishes to under water and above terrain.
	if (m_pos.z > bc.waterLevel-0.2f)
	{
		m_pos.z = bc.waterLevel-0.2f;
		if (rand()%40 == 1)
		{
			if (m_pos.GetSquaredDistance(bc.playerPos) < 10.0f*10.0f)
			{
				// Spawn splash.
				SpawnParticleEffect( m_pos,bc,SPAWN_SPLASH );
			}
		}
	}
	else if (m_pos.z < bc.terrainZ+0.2f && bc.terrainZ < bc.waterLevel)
	{
		m_pos.z = bc.terrainZ+0.2f;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidFish::Physicalize( SBoidContext &bc )
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
	if (pEntity)
	{
		AABB aabb;
		pEntity->GetLocalBounds(aabb);
		bc.fBoidRadius = max( (aabb.max-aabb.min).GetLength() * bc.boidScale,0.001f );
		bc.fBoidThickness = bc.fBoidRadius*0.5f;
	}

	CBoidObject::Physicalize( bc );
}

//////////////////////////////////////////////////////////////////////////
void CBoidFish::SpawnParticleEffect( const Vec3 &pos,SBoidContext &bc,int nEffect )
{
	if (!bc.entity)
		return;

	IScriptTable *pEntityTable = bc.entity->GetScriptTable();
	if (!pEntityTable)
		return;

	if (!m_pOnSpawnBubbleFunc)
	{
		pEntityTable->GetValue( "OnSpawnBubble",m_pOnSpawnBubbleFunc );
	}
	if (!m_pOnSpawnSplashFunc)
	{
		pEntityTable->GetValue( "OnSpawnSplash",m_pOnSpawnSplashFunc );
	}

	HSCRIPTFUNCTION pScriptFunc = NULL;
	switch (nEffect)
	{
	case SPAWN_BUBBLE:
		pScriptFunc = m_pOnSpawnBubbleFunc;
		break;
	case SPAWN_SPLASH:
		pScriptFunc = m_pOnSpawnSplashFunc;
		break;
	}

	if (pScriptFunc)
	{
		if (!vec_Bubble)
		{
			vec_Bubble = gEnv->pScriptSystem->CreateTable();
		}
		{
			CScriptSetGetChain bubbleChain(vec_Bubble);
			bubbleChain.SetValue( "x",pos.x );
			bubbleChain.SetValue( "y",pos.y );
			bubbleChain.SetValue( "z",pos.z );
		}

		Script::Call( gEnv->pScriptSystem,pScriptFunc,pEntityTable,pos );
	}
}

//////////////////////////////////////////////////////////////////////////
void CBoidFish::Kill( const Vec3 &hitPoint,const Vec3 &force )
{
	m_flock->m_bAnyKilled = true;
	SBoidContext bc;
	m_flock->GetBoidSettings(bc);

	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_entity);
	if (pEntity)
	{
		SpawnParams params;
		params.eAttachForm = GeomForm_Surface;
		params.eAttachType = GeomType_Render;
		gEnv->pEntitySystem->GetBreakableManager()->AttachSurfaceEffect( pEntity,0,SURFACE_BREAKAGE_TYPE("destroy"),params,ePEF_Independent );
	}

	/*
	// Can`t Kill fish in MP game.
	IGame *pGame = gEnv->pGame;
	if (pGame && pGame->GetModuleState( EGameMultiplayer ) )
		return;
		*/

	float boxSize = bc.fBoidRadius/2;
	float mass = ((boxSize/4)*boxSize*boxSize)*FISH_PHYSICS_DENSITY; // box volume * density
	Vec3 impulse = force * mass * 0.1f;

	if (!m_physicsControlled)
	{
		CreateRigidBox( bc,Vec3(boxSize/4,boxSize,boxSize),-1,950 );
		//impulse += m_heading*m_speed*mass;
		m_physicsControlled = true;
	}

	if (m_physicsControlled)
	{
		// Apply force on this body.
		pe_action_impulse theAction;
		theAction.impulse = impulse;
		theAction.point = hitPoint;
		theAction.iApplyTime = 0;
		m_pPhysics->Action(&theAction);
	}

	if (m_object && !m_dead && !m_dying)
	{
		//m_object->ResetAnimations();
		//m_object->StartAnimation( "death" );
		m_object->SetAnimationSpeed( 1 );
	}

	m_dying = true;
	m_dyingTime = 0;
}

//////////////////////////////////////////////////////////////////////////
CFlock::CFlock( IEntity *pEntity,EFlockType flockType )
{
	m_nViewDistRatio = 100;
	m_pEntity = pEntity;
	m_bEnabled = true;
	
	m_updateFrameID = 0;
	m_percentEnabled = 100;

	m_type = flockType;

	m_bounds.min = Vec3(-1,-1,-1);
	m_bounds.max = Vec3(1,1,1);

	m_origin = m_pEntity->GetWorldPos();
	m_fCenterFloatingTime = 0.0f;

	m_nBoidEntityFlagsAdd = 0;

	GetDefaultBoidsContext(m_bc);

	m_bc.engine = gEnv->p3DEngine;
	m_bc.physics = gEnv->pPhysicalWorld;
	m_bc.waterLevel = m_bc.engine->GetWaterLevel();
	m_bc.fBoidMass = 1;
	m_bc.fBoidRadius = 1;
	m_bc.fBoidThickness = 1;
	m_bc.flockPos = m_origin;
	m_bc.randomFlockCenter = m_origin;
	m_bc.factorRandomAccel = 0;
	m_bc.boidScale = 5;

	m_bc.scarePoint = Vec3(0,0,0);
	m_bc.scareRadius = 0;
	m_bc.scareRatio = 0;
	m_bc.scareThreatLevel = 0;

	m_bc.fSmoothFactor = 0;
	m_bc.fMaxTurnRatio = 0;

	m_bEntityCreated = false;
	m_bAnyKilled = false;
	m_pPrecacheCharacter = 0;
}

//////////////////////////////////////////////////////////////////////////
void CFlock::GetDefaultBoidsContext( SBoidContext &bc )
{
	ZeroStruct(bc);
	bc.MinHeight = MIN_FLIGHT_HEIGHT;
	bc.MaxHeight = MAX_FLIGHT_HEIGHT;

	bc.MaxAttractDistance = MAX_ATTRACT_DISTANCE;
	bc.MinAttractDistance = MIN_ATTRACT_DISTANCE;

	bc.MaxSpeed = MAX_SPEED;
	bc.MinSpeed = MIN_SPEED;

	bc.factorAlignment = ALIGNMENT_FACTOR;
	bc.factorCohesion = COHESION_FACTOR;
	bc.factorSeparation = SEPARATION_FACTOR;

	bc.factorAttractToOrigin = ORIGIN_ATTRACT_FACTOR;
	bc.factorKeepHeight = DESIRED_HEIGHT_FACTOR;
	bc.factorAvoidLand = AVOID_LAND_FACTOR;

	bc.MaxAnimationSpeed = MAX_ANIMATION_SPEED;

	bc.followPlayer = false;
	bc.avoidObstacles = true;
	bc.noLanding = false;
	bc.bAvoidWater = true;

	bc.cosFovAngle = cry_cosf(125.0f*gf_PI/180.0f);
	bc.maxVisibleDistance = 300;

	bc.boidScale = 5;

	bc.fSmoothFactor = 0;
	bc.fMaxTurnRatio = 0;
	bc.vEntitySlotOffset.Set(0,0,0);

	bc.attractionPoint.Set(0,0,0);
}

CFlock::~CFlock()
{
	ClearBoids();
	RegisterAIEventListener(false);
}

//////////////////////////////////////////////////////////////////////////
void CFlock::ClearBoids()
{
	I3DEngine *engine = gEnv->p3DEngine;

	m_pPrecacheCharacter = 0;

	DeleteEntities( true ); 
	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		CBoidObject* boid = *it;
		delete boid;
	}
	m_boids.clear();
}

void CFlock::DeleteEntities( bool bForceDeleteAll )
{
	if (m_pEntity)
	{
		IEntityRenderProxy *pRenderProxy = (IEntityRenderProxy*)m_pEntity->GetProxy(ENTITY_PROXY_RENDER);
		if (pRenderProxy)
			pRenderProxy->ClearSlots();
	}

	I3DEngine *engine = gEnv->p3DEngine;
	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		CBoidObject* boid = *it;

		if (boid->m_pickedUp && !bForceDeleteAll) // Do not delete picked up boids.
			continue;

		//		SAFE_RELEASE(boid->m_object);
		boid->m_object = 0;
		boid->m_noentity = true;
		if (boid->m_entity)
		{
			// Delete entity.

			IEntity *pBoidEntity = gEnv->pEntitySystem->GetEntity(boid->m_entity);
			if (pBoidEntity)
			{
				CBoidObjectProxy *pBoidObjectProxy = new CBoidObjectProxy(pBoidEntity);
				pBoidEntity->SetProxy(ENTITY_PROXY_BOID_OBJECT,pBoidObjectProxy);
				if (pBoidObjectProxy)
					pBoidObjectProxy->SetBoid(0);
			}

			gEnv->pEntitySystem->RemoveEntity(boid->m_entity);

			boid->m_entity = 0;
			boid->m_pPhysics = 0;
			boid->m_physicsControlled = false;
		}
	}
	m_bEntityCreated = false;
}
//////////////////////////////////////////////////////////////////////////
void CFlock::AddBoid( CBoidObject *boid )
{
	boid->m_flock = this;
	m_boids.push_back(boid);
}

//////////////////////////////////////////////////////////////////////////
bool CFlock::IsFlockActive()
{
	if (!m_bEnabled)
		return false;
	
	if (m_percentEnabled <= 0)
		return false;

	if (m_bc.followPlayer)
		return true;

	float d = m_bc.maxVisibleDistance;
	float dist = m_origin.GetSquaredDistance(GetISystem()->GetViewCamera().GetMatrix().GetTranslation());
	if (d*d < dist)
	{
		if (m_bEntityCreated)
			DeleteEntities(false);

		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void CFlock::SetEnabled( bool bEnabled )
{
	if (m_bEnabled != bEnabled)
	{
		m_bEnabled = bEnabled;
		if (!bEnabled && m_bEntityCreated)
			DeleteEntities(false);
	}
}

//////////////////////////////////////////////////////////////////////////
void CFlock::SetPercentEnabled( int percent )
{
	if (percent < 0)
		percent = 0;
	if (percent > 100)
		percent = 100;
	
	m_percentEnabled = percent;
}

//////////////////////////////////////////////////////////////////////////
void CFlock::UpdateBoidsViewDistRatio()
{
	IEntityRenderProxy *pRenderProxy = (IEntityRenderProxy*)m_pEntity->GetProxy(ENTITY_PROXY_RENDER);
	if (pRenderProxy)
	{
		m_nViewDistRatio = pRenderProxy->GetRenderNode()->GetViewDistRatio();
	}
	//////////////////////////////////////////////////////////////////////////

	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		CBoidObject* boid = *it;
		IEntity *pBoidEntity = gEnv->pEntitySystem->GetEntity(boid->m_entity);
		if (pBoidEntity)
		{
			pRenderProxy = (IEntityRenderProxy*)pBoidEntity->GetProxy(ENTITY_PROXY_RENDER);
			if (pRenderProxy)
			{
				pRenderProxy->GetRenderNode()->SetViewDistRatio(m_nViewDistRatio);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFlock::Update( CCamera *pCamera )
{
	FUNCTION_PROFILER( GetISystem(),PROFILE_ENTITY );

	if (!IsFlockActive())
		return;

	if (!m_e_flocks)
	{
		if (m_bEntityCreated)
			DeleteEntities( true );
		return;
	}

	if(GetISystem()->IsSerializingFile() == 1) //quickloading
		return;

	if (!m_bEntityCreated)
	{
		if (!CreateEntities())
			return;
	}

	float dt = gEnv->pTimer->GetFrameTime();
	// Make sure delta time is limited.
	if (dt > 1.0f)
		dt = 0.01f;
	if (dt > 0.1f)
		dt = 0.1f;

	m_bc.fSmoothFactor = 1.f - gEnv->pTimer->GetProfileFrameBlending();
	/*
	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		CBoidObject *boid = *it;
		boid->Think();
	}
	*/
	//m_bc.playerPos = m_flockMgr->GetPlayerPos();

	m_bc.playerPos = GetISystem()->GetViewCamera().GetMatrix().GetTranslation(); // Player position is position of camera.
	m_bc.flockPos = m_origin;
	m_bc.waterLevel = m_bc.engine->GetWaterLevel( &m_origin );

	m_bounds.min = Vec3(FLT_MAX,FLT_MAX,FLT_MAX);
	m_bounds.max = Vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	int numBoids = m_boids.size();
	if (m_percentEnabled < 100)
	{
		numBoids = (m_percentEnabled*numBoids)/100;
	}

	if (!m_pEntity->GetRotation().IsIdentity())
	{
		// Entity matrix must not have rotation.
		//Quat q;
		//q.SetIdentity();
		//m_pEntity->SetRotation(q);
	}

	//////////////////////////////////////////////////////////////////////////
	// Update flock random center.
	//////////////////////////////////////////////////////////////////////////
	m_fCenterFloatingTime += gEnv->pTimer->GetFrameTime();
	float tc = m_fCenterFloatingTime*0.2f;
	m_bc.randomFlockCenter = m_bc.flockPos + 
		//m_bc.fSpawnRadius*Vec3(sinf(0.9f*m_fCenterFloatingTime),cosf(1.1f*sin(0.9f*m_fCenterFloatingTime)),0.3f*sinf(1.2f*m_fCenterFloatingTime) );
		m_bc.fSpawnRadius*Vec3(sinf(tc*0.913f)*cosf(tc*1.12f),sinf(tc*0.931f)*cosf(tc*0.971f),0.4f*sinf(tc*1.045f)*cosf(tc*0.962f) );
	//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere( m_bc.randomFlockCenter,0.1f,ColorB(255,0,0,255) );

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	IEntityRenderProxy *pRenderProxy = (IEntityRenderProxy*)m_pEntity->GetProxy(ENTITY_PROXY_RENDER);
	if (pRenderProxy)
	{
		if (pRenderProxy->GetRenderNode()->GetViewDistRatio() != m_nViewDistRatio)
			UpdateBoidsViewDistRatio();
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Update scare factors.
	if (m_bc.scareThreatLevel > 0)
	{
		m_bc.scareThreatLevel *= 0.95f;
		m_bc.scareRatio *= 0.95f;
		if (m_bc.scareRatio < 0.01f)
		{
			m_bc.scareRatio = 0;
			m_bc.scareThreatLevel = 0;
		}
		if (m_e_flocks == 2)
		{
			int c = (int)(255*m_bc.scareRatio);
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere( m_bc.scarePoint,m_bc.scareRadius,ColorB(c,0,0,c),false );
		}
	}
	//////////////////////////////////////////////////////////////////////////


	Vec3 entityPos = m_pEntity->GetWorldPos();
	Matrix34 boidTM;
	int num = 0;
	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it,num++)
	{
		if (num > numBoids)
			break;

		CBoidObject* boid = *it;

		m_bc.terrainZ = m_bc.engine->GetTerrainElevation(boid->m_pos.x,boid->m_pos.y);
		boid->Update(dt,m_bc);

		if (!boid->m_physicsControlled && !boid->m_dead)
		{
			IEntity *pBoidEntity = gEnv->pEntitySystem->GetEntity(boid->m_entity);
			if (pBoidEntity)
			{
				Quat q;
				boid->CalcOrientation(q);
				//if (pBoidEntity->GetPos().z < m_bc.terrainZ)
				pBoidEntity->SetPosRotScale( boid->m_pos,q,Vec3(boid->m_scale,boid->m_scale,boid->m_scale) );
				/*
				else
				{
					Vec3 pos = pBoidEntity->GetPos();
					boid->m_pos = pos;
					pBoidEntity->SetPosRotScale( pos,q,Vec3(boid->m_scale,boid->m_scale,boid->m_scale) );
				}
				*/
			}
		}


		/*
		// Upxdate bounding box of flock.
		m_bounds.min.x = __min( m_bounds.min.x,boid->m_pos.x-m_bc.fBoidRadius );
		m_bounds.min.y = __min( m_bounds.min.y,boid->m_pos.y-m_bc.fBoidRadius );
		m_bounds.min.z = __min( m_bounds.min.z,boid->m_pos.z-m_bc.fBoidRadius );
		m_bounds.max.x = __max( m_bounds.max.x,boid->m_pos.x+m_bc.fBoidRadius );
		m_bounds.max.y = __max( m_bounds.max.y,boid->m_pos.y+m_bc.fBoidRadius );
		m_bounds.max.z = __max( m_bounds.max.z,boid->m_pos.z+m_bc.fBoidRadius );
		*/
	}
	/*
	if (m_pEntity && !m_boids.empty())
	{
		// Entity Bbox must be local.
		AABB box;
		box.min = m_bounds.min - m_origin;
		box.max = m_bounds.max - m_origin;
		//m_pEntity->SetBBox( box.min,box.max );
	}
	*/
	m_updateFrameID = gEnv->pRenderer->GetFrameID(false);	
	//gEnv->pLog->Log( "Birds Update" );
}

//////////////////////////////////////////////////////////////////////////
void CFlock::Render(const SRendParams & EntDrawParams)
{
	/*
	FUNCTION_PROFILER( GetISystem(),PROFILE_ENTITY );

	if (!m_e_flocks)
		return;

	// Only draw birds flock on the same frame id, as update call.
	int frameId = gEnv->pRenderer->GetFrameID(false);
	if (abs(frameId-m_updateFrameID) > 2)
		return;

	float d = GetMaxVisibilityDistance();
	if (d*d < GetSquaredDistance(m_playerPos,GetPos())
		return;

	SRendParams rp( EntDrawParams );
	CCamera &cam = GetISystem()->GetViewCamera();
	// Check if flock bounding box is visible.
	if (!cam.IsAABBVisibleFast( AABB(m_bounds.min,m_bounds.max) ))
		return;

	int numBoids = m_boids.size();
	if (m_percentEnabled < 100)
	{
		numBoids = (m_percentEnabled*numBoids)/100;
	}

	int num = 0;
	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		if (num++ > numBoids)
			break;

		CBoidObject *boid = *it;
		boid->Render(rp,cam,m_bc);
	}
	//gEnv->pLog->Log( "Birds Draw" );
*/
}

//////////////////////////////////////////////////////////////////////////
void CFlock::SetBoidSettings( SBoidContext &bc )
{
	m_bc = bc;
	if (m_bc.MinHeight == 0)
		m_bc.MinHeight = 0.01f;
	RegisterAIEventListener(true);
}

//////////////////////////////////////////////////////////////////////////
void CFlock::SetPos( const Vec3& pos )
{
	Vec3 ofs = pos - m_origin;
	m_origin = pos;
	m_bc.flockPos = m_origin;
	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		CBoidObject *boid = *it;
		boid->m_pos += ofs;
		boid->OnFlockMove( m_bc );
	}
	// Update bounding box of flock entity.
	if (m_pEntity)
	{
		//float s = m_bc.MaxAttractDistance;
//		float s = 1;
		//m_pEntity->SetBBox( pos-Vec3(s,s,s),pos+Vec3(s,s,s) );
		//m_pEntity->ForceRegisterInSectors();
	}
	RegisterAIEventListener(true);
}

//////////////////////////////////////////////////////////////////////////
void CFlock::RegisterAIEventListener( bool bEnable )
{
	if (!gEnv->pAISystem)
		return;

	if (bEnable)
	{
		m_bAIEventListenerRegistered = true;
		gEnv->pAISystem->RegisterAIEventListener(this,m_bc.flockPos,m_bc.maxVisibleDistance,(1<<AISTIM_EXPLOSION)|(1<<AISTIM_BULLET_HIT));
	}
	else if (m_bAIEventListenerRegistered)
	{
		m_bAIEventListenerRegistered = false;
		gEnv->pAISystem->UnregisterAIEventListener(this);
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFlock::RayTest( Vec3 &raySrc,Vec3 &rayTrg,SFlockHit &hit )
{
//	Vec3 v;
	Vec3 p1,p2;
	// Check all boids.
	for (unsigned int i = 0; i < m_boids.size(); i++)
	{
		CBoidObject *boid = m_boids[i];

		Lineseg lseg(raySrc,rayTrg);
		Sphere sphere(boid->m_pos,boid->m_scale);
		if ( Intersect::Lineseg_Sphere( lseg,sphere,  p1,p2 ) > 0)
		{
			hit.object = boid;
			hit.dist = (raySrc - p1).GetLength();
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void CFlock::OnBoidHit( EntityId nBoidId,SmartScriptTable &hit )
{
	int num = (int)m_boids.size();
	for (int i = 0; i < num; i++)
	{
		CBoidObject *boid = m_boids[i];
		if (boid->m_entity == nBoidId)
		{
			Vec3 pos = boid->m_pos;
			Vec3 force = Vec3(1,1,1);
			float damage = 1.0f;
			hit->GetValue("pos", pos);
			hit->GetValue("dir", force);
			hit->GetValue("damage", damage);

			boid->Kill( pos,force*damage );

			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFlock::CreateBoids(SBoidsCreateContext &ctx )
{
	m_createContext = ctx;

	ClearBoids();

	if (!m_e_flocks)
		return;

	if (!ctx.models.empty())
		m_model = ctx.models[0];

	static ICVar* e_obj_quality(gEnv->pConsole->GetCVar("e_ObjQuality"));
	int obj_quality = 0;
	if (e_obj_quality)
		obj_quality = e_obj_quality->GetIVal();

	//////////////////////////////////////////////////////////////////////////
	// Scale boids down depended on spec
	//////////////////////////////////////////////////////////////////////////
	if (obj_quality == CONFIG_LOW_SPEC)
	{
		if (ctx.boidsCount > 10)
			ctx.boidsCount /= 4;
		if (ctx.boidsCount > 20)
			ctx.boidsCount = 20;
	}
	if (obj_quality == CONFIG_MEDIUM_SPEC)
	{
		if (ctx.boidsCount > 10)
			ctx.boidsCount /= 2;
	}
	m_RequestedBoidsCount = ctx.boidsCount;
	if (ctx.boidsCount == 0)
		return;
	//////////////////////////////////////////////////////////////////////////

	// precache flock model
	m_pPrecacheCharacter = 0;
	if (IsCharacterFile(m_model) && gEnv && gEnv->pCharacterManager)
		m_pPrecacheCharacter = gEnv->pCharacterManager->CreateInstance(m_model);
	if (!m_pPrecacheCharacter)
		return;

	m_pPrecacheCharacter->GetICharacterModel()->AddRef();
	//strange workaround for Decide
	m_pPrecacheCharacter->GetICharacterModel()->AddRef();
}

//////////////////////////////////////////////////////////////////////////
bool CFlock::CreateEntities()
{
	if (!m_e_flocks)
		return false;

 	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Boid");
	if (!spawnParams.pClass)
		return false;
	
	/*
	int nViewDistRatio = 100;
	{
		IEntityRenderProxy *pRenderProxy = (IEntityRenderProxy*)m_pEntity->GetProxy(ENTITY_PROXY_RENDER);
		if (pRenderProxy)
		{
			nViewDistRatio = pRenderProxy->GetRenderNode()->GetViewDistRatio();
		}
	}
	*/

	spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY | ENTITY_FLAG_NO_SAVE | ENTITY_FLAG_NO_PROXIMITY | m_nBoidEntityFlagsAdd;
	spawnParams.sName = m_boidEntityName;

	//if (m_pEntity->GetFlags()&ENTITY_FLAG_CASTSHADOW)
	//spawnParams.nFlags |= ENTITY_FLAG_CASTSHADOW;

	bool bAnyCreated = false;

	// Different boids.
	//for (int i = 0; i < m_RequestedBoidsCount; i++)
	for (Boids::iterator it = m_boids.begin();  it != m_boids.end(); ++it)
	{
		CBoidObject *boid = *it;/*m_boids[i]*/;

		if (boid->m_dead || boid->m_dying)
			continue;

		spawnParams.vPosition = boid->m_pos;
		spawnParams.vScale = Vec3(boid->m_scale,boid->m_scale,boid->m_scale);
		spawnParams.id = 0;
		IEntity *pBoidEntity = gEnv->pEntitySystem->SpawnEntity( spawnParams );
		if (!pBoidEntity)
		{
			//delete boid;
			//it = m_boids.erase(it);
			continue;
		}
		boid->m_noentity = false;
		boid->m_entity = pBoidEntity->GetId();

		CBoidObjectProxy *pBoidObjectProxy = new CBoidObjectProxy(pBoidEntity);
		pBoidEntity->SetProxy(ENTITY_PROXY_BOID_OBJECT,pBoidObjectProxy);
		pBoidObjectProxy->SetBoid(boid);

		// check if character.
		if (IsCharacterFile(m_model))
		{
			pBoidEntity->LoadCharacter( 0,m_model );
			boid->m_object = pBoidEntity->GetCharacter(0);
			if (!boid->m_object)
			{
				gEnv->pEntitySystem->RemoveEntity(boid->m_entity,true);
				boid->m_entity = 0;
				//delete boid;
				//it = m_boids.erase(it);
				continue;
			}
		}
		else
		{
			pBoidEntity->LoadGeometry( 0,m_model );
		}

		bAnyCreated = true;

		//		boid->m_object->GetModel()->AddRef();

		AABB aabb;
		if (boid->m_object)
		{
			boid->m_object->SetFlags( CS_FLAG_DRAW_MODEL|CS_FLAG_UPDATE_ONRENDER );
			boid->PlayAnimation( m_boidDefaultAnimName,true );
			aabb = boid->m_object->GetAABB();
		}
		else
		{
			pBoidEntity->GetLocalBounds(aabb);
		}

		float fBBoxRadius = ((aabb.max-aabb.min).GetLength()/2.0f);
		m_bc.fBoidRadius = max( fBBoxRadius * m_bc.boidScale,0.001f );
		m_bc.fBoidThickness = m_bc.fBoidRadius;

		if (!m_bc.vEntitySlotOffset.IsZero())
		{
			pBoidEntity->SetSlotLocalTM(0,Matrix34::CreateTranslationMat(m_bc.vEntitySlotOffset*fBBoxRadius));
		}

		boid->Physicalize(m_bc);

		IEntityRenderProxy *pRenderProxy = (IEntityRenderProxy*)pBoidEntity->GetProxy(ENTITY_PROXY_RENDER);
		if (pRenderProxy && m_bc.fBoidRadius > 0)
		{
			float r = m_bc.fBoidRadius;
			AABB box;
			box.min = Vec3(-r,-r,-r);
			box.max = Vec3(r,r,r);
			pRenderProxy->SetLocalBounds( box,true );
		}
		IScriptTable *pScriptTable = pBoidEntity->GetScriptTable();
		if (pScriptTable)
		{
			pScriptTable->SetValue( "flock_entity",m_pEntity->GetScriptTable() );
		}
	}

	m_bEntityCreated = true;
	return bAnyCreated;
}

//////////////////////////////////////////////////////////////////////////
void CFlock::Reset()
{
	if (m_bAnyKilled)
	{
		CreateBoids( m_createContext );
	}
	m_bAnyKilled = false;
}

//////////////////////////////////////////////////////////////////////////
void CFlock::OnAIEvent(EAIStimulusType type, const Vec3& pos, float radius, float threat, EntityId sender)
{
	if (m_bc.scareThreatLevel*1.2f < threat)
	{
		m_bc.scareThreatLevel = threat;
		m_bc.scarePoint = pos;
		m_bc.scareRatio = 1.0f;
		m_bc.scareRadius = radius;
	}
}

void CFlock::GetMemoryUsage( ICrySizer *pSizer ) const
{
	pSizer->AddContainer(m_boids);		
	pSizer->AddObject(m_model);
	pSizer->AddObject(m_boidEntityName);
	pSizer->AddObject(m_boidDefaultAnimName);
	pSizer->AddObject(m_pPrecacheCharacter);
}
//////////////////////////////////////////////////////////////////////////
// Flock/Birds flocks.
//////////////////////////////////////////////////////////////////////////
void CBirdsFlock::CreateBoids( SBoidsCreateContext &ctx )
{
	CFlock::CreateBoids(ctx);

	for (uint32 i = 0; i < m_RequestedBoidsCount; i++)
	{
		CBoidObject *boid = new CBoidBird( m_bc );
		if (m_bc.bSpawnFromPoint)
		{
			CBoidBird *pBoid = (CBoidBird *)boid;
			float radius	= 0.5; // Spawn not far from origin
			float z				= /*gEnv->p3DEngine->GetTerrainElevation(m_origin.x,m_origin.y)*/ + 7.f + frand()*2.f;	// z = terrain height + [5-9]
			pBoid->m_pos	= m_origin + Vec3(radius*frand(),radius*frand(), radius*frand());
			z = .25f*frand() + .25f;	// z-heading = 0.0 - 0.5
			pBoid->m_heading		= ( Vec3(frand(),frand(),z) ).GetNormalized(); 
			pBoid->m_scale			= m_bc.boidScale + frand()*m_bc.boidRandomScale;
			boid->m_speed				= m_bc.MinSpeed + (frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
			pBoid->m_dead				= 0;
			pBoid->m_currentAccel(0,0,0);
			pBoid->SetSpawnFromPt(true);
			pBoid->m_fNoCenterAttract = m_bc.factorAttractToOrigin;
			pBoid->m_fNoKeepHeight		= m_bc.factorKeepHeight;
			pBoid->SetAttracted(false);
		}
		else
		{
			float radius = m_bc.fSpawnRadius;
			boid->m_pos = m_origin + Vec3(radius*frand(),radius*frand(),frand()*radius);
			boid->m_heading = ( Vec3(frand(),frand(),0) ).GetNormalized();
			boid->m_scale = m_bc.boidScale + frand()*m_bc.boidRandomScale;
			boid->m_speed = m_bc.MinSpeed + (frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
		}

		AddBoid(boid);
	}
}

void CBirdsFlock::SetAttractionPoint(const Vec3 &point)
{
	m_bc.attractionPoint	= point;
	SetAttractOutput(false);

	for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
	{
		((CBoidBird *)*it)->SetAttracted();
	}
}

void CBirdsFlock::SetEnabled( bool bEnabled )
{
	if (!m_bc.bSpawnFromPoint)
		CFlock::SetEnabled(bEnabled);
	else
	{
		if (m_bEnabled != bEnabled)
		{
			SetAttractOutput(false);
			m_bEnabled	= bEnabled;
			for (Boids::iterator it = m_boids.begin(); it != m_boids.end(); ++it)
			{
				CBoidBird *pBoid = (CBoidBird *)*it;
				float radius	= 0.5; // Spawn not far from origin
				float z				= /*gEnv->p3DEngine->GetTerrainElevation(m_origin.x,m_origin.y) + */7.f + frand()*2.f;	// z = terrain height + [5-9]
				pBoid->m_pos	= m_origin + Vec3(radius*frand(),radius*frand(), radius*frand());
				z = .25f*frand() + .25f;	// z-heading = 0.0 - 0.5
				pBoid->m_heading		= ( Vec3(frand(),frand(),z) ).GetNormalized(); 
				pBoid->m_scale			= m_bc.boidScale + frand()*m_bc.boidRandomScale;
				pBoid->m_speed				= m_bc.MinSpeed + (frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
				pBoid->m_dead				= 0;
				pBoid->m_currentAccel(0,0,0);
				pBoid->SetSpawnFromPt(true);
				pBoid->m_fNoCenterAttract = m_bc.factorAttractToOrigin;
				pBoid->m_fNoKeepHeight		= m_bc.factorKeepHeight;
				pBoid->SetAttracted(false);
			}

			if (!bEnabled && m_bEntityCreated)
				DeleteEntities(false);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFishFlock::CreateBoids( SBoidsCreateContext &ctx )
{
	CFlock::CreateBoids(ctx);

	for (uint32 i = 0; i < m_RequestedBoidsCount; i++)
	{
		CBoidObject *boid = new CBoidFish( m_bc );
		float radius = m_bc.fSpawnRadius;
		boid->m_pos = m_origin + Vec3(radius*frand(),radius*frand(),frand()*radius);

		float terrainZ = m_bc.engine->GetTerrainElevation(boid->m_pos.x,boid->m_pos.y);
		if (boid->m_pos.z <= terrainZ)
			boid->m_pos.z = terrainZ + 0.01f;
		if (boid->m_pos.z > m_bc.waterLevel)
			boid->m_pos.z = m_bc.waterLevel-1;

		boid->m_speed = m_bc.MinSpeed + (frand()+1)/2.0f*(m_bc.MaxSpeed - m_bc.MinSpeed);
		boid->m_heading = ( Vec3(frand(),frand(),0) ).GetNormalized();
		boid->m_scale = m_bc.boidScale + frand()*m_bc.boidRandomScale;

		AddBoid(boid);
	}
}


/*
//////////////////////////////////////////////////////////////////////////
CFlockManager::CFlockManager( ISystem *system )
{
	// Create one flock.
	m_system = system;
	m_lastFlockId = 1;
	//m_object = system->GetI3DEngine()->MakeCharacter( "Objects\\Other\\Seagull\\Seagull.cgf" );
	//m_object = system->GetI3DEngine()->LoadStatObj( "Objects\\Other\\Seagull\\Seagull.cgf" );
	REGISTER_CVAR2( "e_flocks",&m_e_flocks,1,VF_DUMPTODISK,"Enable Flocks (Birds/Fishes)" );
	REGISTER_CVAR2( "e_flocks_hunt",&m_e_flocks_hunt,0,0,"Birds will fall down..." );
}
*/
