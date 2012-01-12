/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 18:10:2005   14:14 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "Game.h"
#include "GameCVars.h"
#include "Projectile.h"
#include "Bullet.h"
#include "WeaponSystem.h"
#include "ISerialize.h"
#include "IGameObject.h"

#include <IEntitySystem.h>
#include <ISound.h>
#include <IItemSystem.h>
#include <IAgent.h>
#include "ItemParamReader.h"
#include "GameRules.h"
#include "Environment/BattleDust.h"
#include <IVehicleSystem.h>
#include "IAIActor.h"
//------------------------------------------------------------------------
CProjectile::CProjectile()
: m_whizSoundId(INVALID_SOUNDID),
	m_trailSoundId(INVALID_SOUNDID),
	m_trailEffectId(-1),
	m_trailUnderWaterId(-1),
	m_pPhysicalEntity(0),
	m_pAmmoParams(0),
	m_destroying(false),
	m_remote(false),
	m_totalLifetime(0.0f),
	m_scaledEffectval(0.0f),
	m_obstructObject(0),
	m_damageDropPerMeter(0.0f),
	m_damageDropMinDisSqr(0.0f),
	m_firstDropApplied(false),
  m_hitTypeId(0),
	m_scaledEffectSignaled(false),
	m_hitListener(false),
	m_hitPoints(-1),
	m_noBulletHits(false),
	m_initial_pos(ZERO),
	m_initial_dir(ZERO),
	m_initial_vel(ZERO)
{
}

//------------------------------------------------------------------------
CProjectile::~CProjectile()
{
	if (g_pGame)
		g_pGame->GetWeaponSystem()->RemoveProjectile(this);
}

//------------------------------------------------------------------------
bool CProjectile::SetAspectProfile( EEntityAspects aspect, uint8 profile )
{
	//if (m_pAmmoParams->physicalizationType == ePT_None)
		//return true;

	if (aspect == eEA_Physics)
	{
		Vec3 spin(m_pAmmoParams->spin);
		Vec3 spinRandom(BiRandom(m_pAmmoParams->spinRandom.x), BiRandom(m_pAmmoParams->spinRandom.y), BiRandom(m_pAmmoParams->spinRandom.z));
		spin += spinRandom;
		spin = DEG2RAD(spin);

		switch (profile)
		{
		case ePT_Particle:
			{
				if (m_pAmmoParams->pParticleParams)
				{
					m_pAmmoParams->pParticleParams->wspin = spin;
					if (!m_initial_dir.IsZero() && !gEnv->bServer)
						m_pAmmoParams->pParticleParams->heading=m_initial_dir;
				}

				SEntityPhysicalizeParams params;
				params.type = PE_PARTICLE;
				params.mass = m_pAmmoParams->mass;
				if (m_pAmmoParams->pParticleParams)
					params.pParticle = m_pAmmoParams->pParticleParams;

				GetEntity()->Physicalize(params);
			}
			break;
		case ePT_Rigid:
			{
				SEntityPhysicalizeParams params;
				params.type = PE_RIGID;
				params.mass = m_pAmmoParams->mass;
				params.nSlot = 0;

				GetEntity()->Physicalize(params);

				pe_action_set_velocity velocity;
				m_pPhysicalEntity = GetEntity()->GetPhysics();
				velocity.w = spin;
				m_pPhysicalEntity->Action(&velocity);

				if (m_pAmmoParams->pSurfaceType)
				{
					int sfid = m_pAmmoParams->pSurfaceType->GetId();

					pe_params_part part;
					part.ipart = 0;

					GetEntity()->GetPhysics()->GetParams(&part);
					if (!is_unused(part.pMatMapping))
					{
						for (int i=0; i<part.nMats; i++)
							part.pMatMapping[i] = sfid;
					}
				}
			}
			break;

		case ePT_Static:
			{
				SEntityPhysicalizeParams params;
				params.type = PE_STATIC;
				params.nSlot = 0;

				GetEntity()->Physicalize(params);

				if (m_pAmmoParams->pSurfaceType)
				{
					int sfid = m_pAmmoParams->pSurfaceType->GetId();

					pe_params_part part;
					part.ipart = 0;

					if (GetEntity()->GetPhysics()->GetParams(&part))
						if (!is_unused(part.pMatMapping))
							for (int i=0; i<part.nMats; i++)
								part.pMatMapping[i] = sfid;
				}
			}
			break;
		case ePT_None:
			{
				SEntityPhysicalizeParams params;
				params.type = PE_NONE;
				params.nSlot = 0;

				GetEntity()->Physicalize(params);
			}
			break;
		}

		m_pPhysicalEntity = GetEntity()->GetPhysics();

		if (m_pPhysicalEntity)
		{
			pe_simulation_params simulation;
			simulation.maxLoggedCollisions = m_pAmmoParams->maxLoggedCollisions;

			pe_params_flags flags;
			flags.flagsOR = pef_log_collisions|(m_pAmmoParams->traceable?pef_traceable:0);

			pe_params_part colltype;
			colltype.flagsAND=~geom_colltype_explosion;

			m_pPhysicalEntity->SetParams(&simulation);
			m_pPhysicalEntity->SetParams(&flags);
			m_pPhysicalEntity->SetParams(&colltype);
		}
	}

	return true;
}

//------------------------------------------------------------------------
bool CProjectile::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags)
{
	if (aspect == eEA_Physics)
	{
		pe_type type = PE_NONE;
		switch (profile)
		{
		case ePT_Rigid:
			type = PE_RIGID;
			break;
		case ePT_Particle:
			type = PE_PARTICLE;
			break;
		case ePT_None:
			return true;
		case ePT_Static:
			{
				Vec3 pos = GetEntity()->GetWorldPos();
				Quat ori = GetEntity()->GetWorldRotation();
				ser.Value("pos", pos, 'wrld');
				ser.Value("ori", ori, 'ori1');
				if (ser.IsReading())
					GetEntity()->SetWorldTM( Matrix34::Create( Vec3(1,1,1), ori, pos ) );
			}
			return true;
		default:
			return false;
		}

		IEntityPhysicalProxy * pEPP = (IEntityPhysicalProxy *) GetEntity()->GetProxy(ENTITY_PROXY_PHYSICS);
		if (ser.IsWriting())
		{
			if (!pEPP || !pEPP->GetPhysicalEntity() || pEPP->GetPhysicalEntity()->GetType() != type)
			{
				gEnv->pPhysicalWorld->SerializeGarbageTypedSnapshot( ser, type, 0 );
				return true;
			}
		}
		else if (!pEPP)
		{
			return false;
		}

		pEPP->SerializeTyped( ser, type, pflags );
	}
	return true;
}

//------------------------------------------------------------------------
bool CProjectile::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	g_pGame->GetWeaponSystem()->AddProjectile(GetEntity(), this);

	if (!GetGameObject()->CaptureProfileManager(this))
		return false;

	m_pAmmoParams = g_pGame->GetWeaponSystem()->GetAmmoParams(GetEntity()->GetClass());

	if (0 == (GetEntity()->GetFlags() & (ENTITY_FLAG_CLIENT_ONLY | ENTITY_FLAG_SERVER_ONLY)))
		if (!m_pAmmoParams->predictSpawn)
			if (!GetGameObject()->BindToNetwork())
				return false;

	LoadGeometry();
	Physicalize();

	IEntityRenderProxy *pProxy = static_cast<IEntityRenderProxy *>(GetEntity()->GetProxy(ENTITY_PROXY_RENDER));
	if (pProxy && pProxy->GetRenderNode())
	{
		pProxy->GetRenderNode()->SetViewDistRatio(255);
		pProxy->GetRenderNode()->SetLodRatio(255);
	}

	float lifetime = m_pAmmoParams->lifetime;
	if (lifetime > 0.0f)
		GetEntity()->SetTimer(ePTIMER_LIFETIME, (int)(lifetime*1000.0f));

	float showtime = m_pAmmoParams->showtime;
	if (showtime > 0.0f)
	{
		GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)&(~ENTITY_SLOT_RENDER));
		GetEntity()->SetTimer(ePTIMER_SHOWTIME, (int)(showtime*1000.0f));
	}
	else
		GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)|ENTITY_SLOT_RENDER);

	return true;
}

//---------------------------------------------------------------------
////If the projectile is in a pool, this function will be called when this projectile is about to be "re-spawn"
void CProjectile::ReInitFromPool()
{
	assert(m_pAmmoParams);

	float lifetime = m_pAmmoParams->lifetime;
	if (lifetime > 0.0f)
		GetEntity()->SetTimer(ePTIMER_LIFETIME, (int)(lifetime*1000.0f));

	float showtime = m_pAmmoParams->showtime;
	if (showtime > 0.0f)
	{
		GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)&(~ENTITY_SLOT_RENDER));
		GetEntity()->SetTimer(ePTIMER_SHOWTIME, (int)(showtime*1000.0f));
	}
	else
		GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)|ENTITY_SLOT_RENDER);

	//Reset some members
	m_remote=false;
	m_totalLifetime=0.0f;
	m_scaledEffectval=0.0f;
	m_obstructObject=0;
	m_scaledEffectSignaled=false;
	m_hitListener=false;
	m_hitPoints=-1;
	m_noBulletHits=false;
}

//------------------------------------------------------------------------
void CProjectile::PostInit(IGameObject *pGameObject)
{
	GetGameObject()->EnableUpdateSlot(this, 0);
}

//------------------------------------------------------------------------
void CProjectile::Release()
{
	delete this;
}

//------------------------------------------------------------------------
void CProjectile::FullSerialize(TSerialize ser)
{
	assert(ser.GetSerializationTarget() != eST_Network);

	ser.Value("Remote", m_remote);
	// m_tracerpath should be serialized but the template-template stuff doesn't work under VS2005
	ser.Value("Owner", m_ownerId, 'eid');
	ser.Value("Weapon", m_weaponId, 'eid');
	ser.Value("TrailEffect", m_trailEffectId);
	ser.Value("TrailSound", m_trailSoundId);
	ser.Value("WhizSound", m_whizSoundId);
	ser.Value("Damage", m_damage);
	ser.Value("Destroying", m_destroying);
	ser.Value("LastPos", m_last);
	ser.Value("InitialPos", m_initial_pos);
	ser.Value("DamageDrop", m_damageDropPerMeter);
	ser.Value("DamageDropMinR", m_damageDropMinDisSqr);
	ser.Value("FirstDropApplied", m_firstDropApplied);
	ser.Value("ScaledEffectSignaled", m_scaledEffectSignaled);
	ser.Value("HitListener", m_hitListener);
	ser.Value("HitPoints", m_hitPoints);

	bool wasVisible = false;
	if(ser.IsWriting())
		wasVisible = (GetEntity()->GetSlotFlags(0)&(ENTITY_SLOT_RENDER))?true:false;
	ser.Value("Visibility", wasVisible);
	if(ser.IsReading())
	{
		if(wasVisible)
			GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)|ENTITY_SLOT_RENDER);
		else
			GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)&(~ENTITY_SLOT_RENDER));
		InitWithAI();
	}
}

//------------------------------------------------------------------------
void CProjectile::Update(SEntityUpdateContext &ctx, int updateSlot)
{
	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);

	if (updateSlot!=0)
		return;

	float color[4] = {1,1,1,1};
	bool bDebug = g_pGameCVars->i_debug_projectiles > 0;

	if(bDebug)
		gEnv->pRenderer->Draw2dLabel(50,15,2.0f,color,false,"Projectile: %s",GetEntity()->GetClass()->GetName());

	Vec3 pos = GetEntity()->GetWorldPos();

	ScaledEffect(m_pAmmoParams->pScaledEffect);

	// update whiz
	if(m_pAmmoParams->pWhiz)
	{
		if (m_whizSoundId == INVALID_SOUNDID)
		{
			IActor *pActor = g_pGame->GetIGameFramework()->GetClientActor();
			if (pActor && (m_ownerId != pActor->GetEntityId()))
			{
				float probability = 0.85f;

				if (Random()<=probability)
				{
					Lineseg line(m_last, pos);
					Vec3 player = pActor->GetEntity()->GetWorldPos();

					float t;
					float distanceSq=Distance::Point_LinesegSq(player, line, t);

					if (distanceSq < 4.7f*4.7f && (t>=0.0f && t<=1.0f))
					{
						if (distanceSq >= 0.65*0.65)
						{
							Sphere s;
							s.center = player;
							s.radius = 4.7f;

							Vec3 entry,exit;
							int intersect=Intersect::Lineseg_Sphere(line, s, entry,exit);
							if (intersect==0x1 || intersect==0x3) // one entry or one entry and one exit
								WhizSound(true, entry, (pos-m_last).GetNormalized());
						}
					}
				}
			}
		}
	}

	if (m_trailSoundId==INVALID_SOUNDID)
		TrailSound(true);

	m_totalLifetime += ctx.fFrameTime;
	m_last = pos;
}

//------------------------------------------------------------------------
void CProjectile::HandleEvent(const SGameObjectEvent &event)
{
	if (m_destroying)
		return;

	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);

	if (event.event == eGFE_OnCollision)
	{
		EventPhysCollision *pCollision = (EventPhysCollision *)event.ptr;

		if(pCollision == NULL)
			return;

		const SCollisionParams* pCollisionParams = m_pAmmoParams->pCollision;
		if (pCollisionParams)
		{
			if (pCollisionParams->pParticleEffect)
				pCollisionParams->pParticleEffect->Spawn(true, IParticleEffect::ParticleLoc(pCollision->pt, pCollision->n, pCollisionParams->scale));

			if (pCollisionParams->sound)
			{
				_smart_ptr<ISound> pSound = gEnv->pSoundSystem->CreateSound(pCollisionParams->sound, FLAG_SOUND_DEFAULT_3D);
		
				if(pSound)
				{
					pSound->SetSemantic(eSoundSemantic_Projectile);
					pSound->SetPosition(pCollision->pt);
					pSound->Play();
				}	
			}
		}

		// add battledust for bulletimpact
		if(gEnv->bServer && g_pGame->GetGameRules())
		{
			if(CBattleDust* pBD = g_pGame->GetGameRules()->GetBattleDust())
			{
				pBD->RecordEvent(eBDET_ShotImpact, pCollision->pt, GetEntity()->GetClass());
			}
		}

		Ricochet(pCollision);

		//Update damage
		if((m_damageDropPerMeter>0.0001f)&&
			(((pCollision->pt-m_initial_pos).len2()>m_damageDropMinDisSqr)||m_firstDropApplied))
		{
			
			if(!m_firstDropApplied)
			{
				m_firstDropApplied = true;
				m_initial_pos = m_initial_pos + (m_initial_dir*(sqrt_fast_tpl(m_damageDropMinDisSqr)));
			}

			Vec3 vDiff = pCollision->pt - m_initial_pos;
			float dis  = vDiff.len();

			m_damage -= (int)(floor_tpl(m_damageDropPerMeter * dis));
					
			//Check m_damage is positive
			if(m_damage<MIN_DAMAGE)
					m_damage=MIN_DAMAGE;

			//Also modify initial position (the projectile could not be destroyed, cause of pirceability)
			m_initial_pos = pCollision->pt;
		}

		// Notify AI system about grenades.
		if (gEnv->pAISystem)
		{
			IAIObject* pAI = 0;
			if ((pAI = GetEntity()->GetAI()) != NULL && pAI->GetAIType() == AIOBJECT_GRENADE)
			{
				// Associate event with vehicle if the shooter is in a vehicle (tank cannon shot, etc)
				EntityId ownerId = m_ownerId;
				IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(ownerId);
				if (pActor && pActor->GetLinkedVehicle() && pActor->GetLinkedVehicle()->GetEntityId())
					ownerId = pActor->GetLinkedVehicle()->GetEntityId();

				SAIStimulus stim(AISTIM_GRENADE, AIGRENADE_COLLISION, ownerId, GetEntityId(),
					GetEntity()->GetWorldPos(), ZERO, 12.0f);
				gEnv->pAISystem->RegisterStimulus(stim);
			}
		}
  }
}

//------------------------------------------------------------------------
void CProjectile::ProcessEvent(SEntityEvent &event)
{
	switch(event.event)
	{
	case ENTITY_EVENT_TIMER:
		{
			switch(event.nParam[0])
			{
			case ePTIMER_SHOWTIME:
				GetEntity()->SetSlotFlags(0, GetEntity()->GetSlotFlags(0)|ENTITY_SLOT_RENDER);
				break;
			case ePTIMER_LIFETIME:
				if(m_pAmmoParams->quietRemoval)	// claymores don't explode when they timeout
					Destroy();
				else
					Explode(true);
				break;
			}
		}
		break;
	}
}

//------------------------------------------------------------------------
void CProjectile::SetAuthority(bool auth)
{
}

//------------------------------------------------------------------------
void CProjectile::LoadGeometry()
{
	if (m_pAmmoParams && !m_pAmmoParams->fpGeometryName.empty())
	{
		//m_pAmmoParams->CacheGeometry(); //Ammo geometry is cached on load (the weapon that uses this ammo will take care)
		GetEntity()->LoadGeometry(0,m_pAmmoParams->fpGeometryName.c_str());
		GetEntity()->SetSlotLocalTM(0, m_pAmmoParams->fpLocalTM);
	}
}

//------------------------------------------------------------------------
void CProjectile::Physicalize()
{
	if (!m_pAmmoParams || m_pAmmoParams->physicalizationType == ePT_None)
		return;

	GetGameObject()->SetAspectProfile(eEA_Physics, m_pAmmoParams->physicalizationType);
}

//------------------------------------------------------------------------
void CProjectile::SetVelocity(const Vec3 &pos, const Vec3 &dir, const Vec3 &velocity, float speedScale)
{
	if (!m_pPhysicalEntity)
		return;

	Vec3 totalVelocity = (dir * m_pAmmoParams->speed * speedScale) + velocity;

	if (m_pPhysicalEntity->GetType()==PE_PARTICLE)
	{
		pe_params_particle particle;
		particle.heading = totalVelocity.GetNormalized();
		particle.velocity = totalVelocity.GetLength();

		m_pPhysicalEntity->SetParams(&particle);
	}
	else if (m_pPhysicalEntity->GetType()==PE_RIGID)
	{
		pe_action_set_velocity vel;
		vel.v = totalVelocity;

		m_pPhysicalEntity->Action(&vel);
	}
}

//------------------------------------------------------------------------
void CProjectile::SetParams(EntityId ownerId, EntityId hostId, EntityId weaponId, int damage, int hitTypeId, float damageDrop /*= 0.0f*/, float damageDropMinR /*=0.0f*/)
{
	m_ownerId = ownerId;
	m_weaponId = weaponId;
	m_hostId = hostId;
	m_damage = damage;
  m_hitTypeId = hitTypeId;
	m_damageDropPerMeter = damageDrop;
	m_damageDropMinDisSqr = damageDropMinR*damageDropMinR;

	if (m_hostId || m_ownerId)
	{
		IEntity* pSelfEntity = GetEntity();
		if (pSelfEntity)
			pSelfEntity->AddEntityLink("Shooter", m_ownerId);

		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_hostId?m_hostId:m_ownerId);
		if (pEntity)
		{
			if (pSelfEntity)
			{
				//need to set AI species to the shooter - not to be scared of it's own rockets 
				IAIObject* projectileAI = pSelfEntity->GetAI();
				IAIObject* shooterAI = pEntity->GetAI();

				if (projectileAI && shooterAI)
					projectileAI->SetFactionID(shooterAI->GetFactionID());
			}
			if (m_pPhysicalEntity && m_pPhysicalEntity->GetType()==PE_PARTICLE)
			{
				pe_params_particle pparams;
				pparams.pColliderToIgnore = pEntity->GetPhysics();

				m_pPhysicalEntity->SetParams(&pparams);
			}
		}
	}
}

//------------------------------------------------------------------------
void CProjectile::Launch(const Vec3 &pos, const Vec3 &dir, const Vec3 &velocity, float speedScale)
{
	m_destroying = false;

	GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionLogged);

	// Only for bullets
	m_hitPoints = m_pAmmoParams->hitPoints;
	m_hitListener = false;
	if(m_hitPoints>0)
	{
		//Only projectiles with hit points are hit listeners
		g_pGame->GetGameRules()->AddHitListener(this);
		m_hitListener = true;
		m_noBulletHits = m_pAmmoParams->noBulletHits;
	}

	Matrix34 worldTM=Matrix34(Matrix33::CreateRotationVDir(dir.GetNormalizedSafe()));
	worldTM.SetTranslation(pos);
	GetEntity()->SetWorldTM(worldTM);

	//Must set velocity after position, if not velocity could be reseted for PE_RIGID
	SetVelocity(pos, dir, velocity, speedScale);

	m_initial_pos = pos;
	m_initial_dir = dir;
	m_initial_vel = velocity;

	m_last = pos;

	// Attach effect when fired (not first update)
	if (m_trailEffectId<0)
		TrailEffect(true);

	IAIObject* pAI = 0;
	if ((pAI = GetEntity()->GetAI()) != NULL && pAI->GetAIType() == AIOBJECT_GRENADE)
	{
		IEntity *pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerId);
		pe_status_dynamics dyn;
		pe_status_dynamics dynProj;
		if (pOwnerEntity->GetPhysics() 
			&& pOwnerEntity->GetPhysics()->GetStatus(&dyn) 
			&& GetEntity()->GetPhysics()->GetStatus(&dynProj))
		{
			
//			Vec3 ownerVel(dyn.v);
			Vec3 grenadeDir(dynProj.v.GetNormalizedSafe());

			// Trigger the signal at the predicted landing position.
			Vec3 predictedPos = pos;
			float dummySpeed;
			if (GetWeapon())
				GetWeapon()->PredictProjectileHit(pOwnerEntity->GetPhysics(), pos, dir, velocity, speedScale * m_pAmmoParams->speed, predictedPos, dummySpeed);

			// Associate event with vehicle if the shooter is in a vehicle (tank cannon shot, etc)
			EntityId ownerId = pOwnerEntity->GetId();
			IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(ownerId);
			if (pActor && pActor->GetLinkedVehicle() && pActor->GetLinkedVehicle()->GetEntityId())
				ownerId = pActor->GetLinkedVehicle()->GetEntityId();

			SAIStimulus stim(AISTIM_GRENADE, AIGRENADE_THROWN, ownerId, GetEntityId(),
				predictedPos, ZERO, 20.0f);
			gEnv->pAISystem->RegisterStimulus(stim);
		}
	}

}

//------------------------------------------------------------------------
void CProjectile::Destroy()
{
	if (m_destroying)
		return;

	m_destroying=true;

	EndScaledEffect(m_pAmmoParams->pScaledEffect);

	GetGameObject()->ReleaseProfileManager(this);
	GetGameObject()->EnablePhysicsEvent(false, eEPE_OnCollisionLogged);

	if (m_obstructObject)
		gEnv->pPhysicalWorld->DestroyPhysicalEntity(m_obstructObject);

	if(m_hitListener)
		if (CGameRules * pGameRules = g_pGame->GetGameRules())
			pGameRules->RemoveHitListener(this);

	WhizSound(false, ZERO, ZERO);

	bool returnToPoolOK = true;
	if (m_pAmmoParams->reusable)
	{
		returnToPoolOK = g_pGame->GetWeaponSystem()->ReturnToPool(this);
	}
	
	if (!m_pAmmoParams->reusable || !returnToPoolOK)
	{
		if ((GetEntity()->GetFlags()&ENTITY_FLAG_CLIENT_ONLY) || gEnv->bServer)
			gEnv->pEntitySystem->RemoveEntity(GetEntity()->GetId());
	}
}

//------------------------------------------------------------------------
bool CProjectile::IsRemote() const
{
	return m_remote;
}

//------------------------------------------------------------------------
void CProjectile::SetRemote(bool remote)
{
	m_remote = remote;
}

//------------------------------------------------------------------------
void CProjectile::Explode(bool destroy, bool impact, const Vec3 &pos, const Vec3 &normal, const Vec3 &vel, EntityId targetId)
{
	const SExplosionParams* pExplosionParams = m_pAmmoParams->pExplosion;
	if (pExplosionParams)
	{
		Vec3 dir(0,0,1);
		if (impact && vel.len2()>0)
			dir = vel.normalized();
		else if (normal.len2()>0)
			dir = -normal;

		m_hitPoints = 0;

		// marcok: using collision pos sometimes causes explosions to have no effect. Anton advised to use entity pos
		Vec3 epos = pos.len2()>0 ? (pos - dir * 0.2f) : GetEntity()->GetWorldPos();

		CGameRules *pGameRules = g_pGame->GetGameRules();
		float minRadius = pExplosionParams->minRadius;
		float maxRadius = pExplosionParams->maxRadius;
		if (m_pAmmoParams->pFlashbang)
		{
			minRadius = m_pAmmoParams->pFlashbang->maxRadius;
			maxRadius = m_pAmmoParams->pFlashbang->maxRadius;
		}

		ExplosionInfo explosionInfo(m_ownerId, GetEntityId(), 0, (float)m_damage, epos, dir, minRadius, maxRadius, pExplosionParams->minPhysRadius, pExplosionParams->maxPhysRadius, 0.0f, pExplosionParams->pressure, pExplosionParams->holeSize, pGameRules->GetHitTypeId(pExplosionParams->type.c_str()));
		if(m_pAmmoParams->pFlashbang)
			explosionInfo.SetEffect(pExplosionParams->effectName, pExplosionParams->effectScale, pExplosionParams->maxblurdist, m_pAmmoParams->pFlashbang->blindAmount, m_pAmmoParams->pFlashbang->flashbangBaseTime);
		else
			explosionInfo.SetEffect(pExplosionParams->effectName, pExplosionParams->effectScale, pExplosionParams->maxblurdist);
		explosionInfo.SetEffectClass(m_pAmmoParams->pEntityClass->GetName());

		if (impact)
			explosionInfo.SetImpact(normal, vel, targetId);

		if (gEnv->bServer)
		{
			pGameRules->ServerExplosion(explosionInfo);

			// add battle dust as well
			CBattleDust* pBD = pGameRules->GetBattleDust();
			if(pBD)
				pBD->RecordEvent(eBDET_Explosion, pos, GetEntity()->GetClass());
		}
	}

	if(!gEnv->bMultiplayer)
	{
		//Single player (AI related code)is processed here, CGameRules::ClientExplosion process the effect
		if (m_pAmmoParams->pFlashbang)
			FlashbangEffect(m_pAmmoParams->pFlashbang);
	}

	if (destroy)
		Destroy();
}

//------------------------------------------------------------------------
void CProjectile::TrailSound(bool enable, const Vec3 &dir)
{
	if (enable)
	{
		if (!m_pAmmoParams->pTrail || !m_pAmmoParams->pTrail->sound)
			return;

		m_trailSoundId = GetSoundProxy()->PlaySound(m_pAmmoParams->pTrail->sound, Vec3(0,0,0), FORWARD_DIRECTION, FLAG_SOUND_DEFAULT_3D, eSoundSemantic_Projectile, 0, 0);
		if (m_trailSoundId != INVALID_SOUNDID)
		{
			ISound *pSound=GetSoundProxy()->GetSound(m_trailSoundId);
			if (pSound)
				pSound->GetInterfaceDeprecated()->SetLoopMode(true);
		}
	}
	else if (m_trailSoundId!=INVALID_SOUNDID)
	{
		GetSoundProxy()->StopSound(m_trailSoundId);
		m_trailSoundId=INVALID_SOUNDID;
	}
}

//------------------------------------------------------------------------
void CProjectile::WhizSound(bool enable, const Vec3 &pos, const Vec3 &dir)
{
	if (enable)
	{
		if (!m_pAmmoParams->pWhiz)
			return;

		ISound *pSound=gEnv->pSoundSystem->CreateSound(m_pAmmoParams->pWhiz->sound, FLAG_SOUND_DEFAULT_3D|FLAG_SOUND_SELFMOVING);
		if (pSound)
		{
			m_whizSoundId = pSound->GetId();

			pSound->SetSemantic(eSoundSemantic_Projectile);
			pSound->SetPosition(pos);
			pSound->SetDirection(dir*m_pAmmoParams->pWhiz->speed);
			pSound->Play();
		}
	}
	else if (m_whizSoundId!=INVALID_SOUNDID)
	{
		ISound *pSound = gEnv->pSoundSystem->GetSound(m_whizSoundId);
		// only stop looping sounds and oneshots does not get cut when hitting a surface
		if (pSound && pSound->GetFlags() & FLAG_SOUND_LOOP) 
			pSound->Stop();
		
			m_whizSoundId=INVALID_SOUNDID;
	}
}

//------------------------------------------------------------------------
void CProjectile::RicochetSound(const Vec3 &pos, const Vec3 &dir)
{
	if (!m_pAmmoParams->pRicochet)
		return;

	ISound *pSound = gEnv->pSoundSystem->CreateSound(m_pAmmoParams->pRicochet->sound, FLAG_SOUND_DEFAULT_3D|FLAG_SOUND_SELFMOVING);
	if (pSound)
	{
		pSound->GetId();
		pSound->SetSemantic(eSoundSemantic_Projectile);
		pSound->SetPosition(pos);
		pSound->SetDirection(dir*m_pAmmoParams->pRicochet->speed);
		pSound->Play();
	}
}

//------------------------------------------------------------------------
void CProjectile::TrailEffect(bool enable, bool underWater /*=false*/)
{
	if (enable)
	{
		const STrailParams* pTrail = NULL;
		if(!underWater)
			pTrail = m_pAmmoParams->pTrail;
		else
			pTrail = m_pAmmoParams->pTrailUnderWater;

		if (!pTrail)
			return;

		bool fpOwner = false;
		if(CWeapon *pWep = GetWeapon())
			if(pWep->GetStats().fp)
				fpOwner = true;

		if(fpOwner && pTrail->effect_fp)
		{
			m_trailEffectId = AttachEffect(true, 0, pTrail->effect_fp, Vec3(0,0,0), Vec3(0,1,0), pTrail->scale, pTrail->prime);
		}
		else if (pTrail->effect)
		{
			if(!underWater)
				m_trailEffectId = AttachEffect(true, 0, pTrail->effect, Vec3(0,0,0), Vec3(0,1,0), pTrail->scale, pTrail->prime);
			else
				m_trailUnderWaterId = AttachEffect(true, 0, pTrail->effect, Vec3(0,0,0), Vec3(0,1,0), pTrail->scale, pTrail->prime);
		}

	}
	else if (m_trailEffectId>=0 && !underWater)
	{
		AttachEffect(false, m_trailEffectId);
		m_trailEffectId=-1;
	}
	else if(m_trailUnderWaterId>=0 && underWater)
	{
		AttachEffect(false, m_trailUnderWaterId);
		m_trailUnderWaterId=-1;
	}
}

//------------------------------------------------------------------------
int CProjectile::AttachEffect(bool attach, int id, const char *name, const Vec3 &offset, const Vec3 &dir, float scale, bool bParticlePrime)
{
	// m_trailEffectId is -1 for invalid, otherwise it's the slot number where the particle effect was loaded
	if (!attach)
	{
		if (id>=0)
			GetEntity()->FreeSlot(id);
	}
	else
	{
		IParticleEffect *pParticleEffect = gEnv->pParticleManager->FindEffect(name);
		if (!pParticleEffect)
			return -1;

		// find a free slot
		SEntitySlotInfo dummy;
		int i=0;
		while (GetEntity()->GetSlotInfo(i, dummy))
			i++;

		GetEntity()->LoadParticleEmitter(i, pParticleEffect, 0, bParticlePrime, true);
		Matrix34 tm = IParticleEffect::ParticleLoc(offset, dir, scale);
		GetEntity()->SetSlotLocalTM(i, tm);

		return i;
	}

	return -1;
}

//------------------------------------------------------------------------
IEntitySoundProxy *CProjectile::GetSoundProxy()
{
	IEntitySoundProxy *pSoundProxy=static_cast<IEntitySoundProxy *>(GetEntity()->GetProxy(ENTITY_PROXY_SOUND));
	if (!pSoundProxy)
		pSoundProxy=static_cast<IEntitySoundProxy *>(GetEntity()->CreateProxy(ENTITY_PROXY_SOUND));

	assert(pSoundProxy);

	return pSoundProxy;
}

void CProjectile::FlashbangEffect(const SFlashbangParams* flashbang)
{
	if (!flashbang)
		return;
	const float radius = flashbang->maxRadius;

	if (!gEnv->pAISystem)
		return;

	// Associate event with vehicle if the shooter is in a vehicle (tank cannon shot, etc)
	EntityId ownerId = m_ownerId;
	IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(ownerId);
	if (pActor && pActor->GetLinkedVehicle() && pActor->GetLinkedVehicle()->GetEntityId())
		ownerId = pActor->GetLinkedVehicle()->GetEntityId();

	SAIStimulus stim(AISTIM_GRENADE, AIGRENADE_FLASH_BANG, ownerId, GetEntityId(),
		GetEntity()->GetWorldPos(), ZERO, radius);
	gEnv->pAISystem->RegisterStimulus(stim);

	SAIStimulus stimSound(AISTIM_SOUND, AISOUND_WEAPON, ownerId, 0,
		GetEntity()->GetWorldPos(), ZERO, radius * 3.0f);
	gEnv->pAISystem->RegisterStimulus(stimSound);
}
//------------------------------------------------------------------------
void CProjectile::ScaledEffect(const SScaledEffectParams* pScaledEffect)
{
	if (!pScaledEffect)
		return;

	float lifetime = m_pAmmoParams->lifetime;
	
	IActor *local = gEnv->pGame->GetIGameFramework()->GetClientActor();
	if (local)
	{
		float dist = (GetEntity()->GetWorldPos() - local->GetEntity()->GetWorldPos()).len();
		if (m_totalLifetime < pScaledEffect->delay || pScaledEffect->radius == 0.0f)
			return;

		float fadeInAmt = 1.0f;
		float fadeOutAmt = 1.0f;
		if (pScaledEffect->fadeInTime > 0.0f)
		{
			fadeInAmt = (m_totalLifetime - pScaledEffect->delay) / pScaledEffect->fadeInTime;
			fadeInAmt = min(fadeInAmt, 1.0f);
			fadeOutAmt = 1.0f - (m_totalLifetime - (lifetime - pScaledEffect->fadeOutTime)) / pScaledEffect->fadeOutTime;
			fadeOutAmt = max(fadeOutAmt, 0.0f);
		}

		if (!m_obstructObject && pScaledEffect->aiObstructionRadius != 0.0f)
		{
			pe_params_pos pos;
			pos.scale = 0.1f;
			pos.pos = GetEntity()->GetWorldPos() + Vec3(0,0,pScaledEffect->aiObstructionRadius/4 * pos.scale);
			m_obstructObject = gEnv->pPhysicalWorld->CreatePhysicalEntity(PE_STATIC, &pos);
			if (m_obstructObject)
			{
				primitives::sphere sphere;
				sphere.center = Vec3(0,0,0);
				sphere.r = pScaledEffect->aiObstructionRadius;
				int obstructID = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeIdByName("mat_obstruct");
				IGeometry *pGeom = gEnv->pPhysicalWorld->GetGeomManager()->CreatePrimitive(primitives::sphere::type, &sphere);
				phys_geometry *geometry = gEnv->pPhysicalWorld->GetGeomManager()->RegisterGeometry(pGeom, obstructID);
				pe_geomparams params;
				params.flags = geom_colltype14;
				geometry->nRefCount = 0; // automatically delete geometry
				m_obstructObject->AddGeometry(geometry, &params);
			}
		}
		else
		{
			pe_params_pos pos;
			pos.scale = 0.1f + min(fadeInAmt, fadeOutAmt) * 0.9f;
			pos.pos = GetEntity()->GetWorldPos() + Vec3(0,0, pScaledEffect->aiObstructionRadius/4.0f * pos.scale);
			m_obstructObject->SetParams(&pos);

			// Signal the AI
			if (gEnv->pAISystem && !m_scaledEffectSignaled &&  m_totalLifetime > (pScaledEffect->delay + pScaledEffect->fadeInTime))
			{
				m_scaledEffectSignaled = true;

				// Associate event with vehicle if the shooter is in a vehicle (tank cannon shot, etc)
				EntityId ownerId = m_ownerId;
				IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(ownerId);
				if (pActor && pActor->GetLinkedVehicle() && pActor->GetLinkedVehicle()->GetEntityId())
					ownerId = pActor->GetLinkedVehicle()->GetEntityId();

				SAIStimulus stim(AISTIM_GRENADE, AIGRENADE_SMOKE, ownerId, GetEntityId(),
					pos.pos, ZERO, pScaledEffect->aiObstructionRadius*1.5f);
				gEnv->pAISystem->RegisterStimulus(stim);
			}
		}

		if (dist > pScaledEffect->radius)
		{
			gEnv->p3DEngine->SetPostEffectParam(pScaledEffect->ppname, 0.0f);
			return;
		}

		float effectAmt = 1.0f - (dist / pScaledEffect->radius);
		effectAmt = max(effectAmt, 0.0f);
		float effectVal = effectAmt * pScaledEffect->maxValue;
		effectVal *= fadeInAmt;
		m_scaledEffectval = effectVal;

		gEnv->p3DEngine->SetPostEffectParam(pScaledEffect->ppname, effectVal);
	}
}
//------------------------------------------------------------------------
void CProjectile::EndScaledEffect(const SScaledEffectParams* pScaledEffect)
{
	if (!pScaledEffect || m_scaledEffectval == 0.0f)
		return;

	IActor *local = gEnv->pGame->GetIGameFramework()->GetClientActor();
	if (local)
	{
		if (pScaledEffect->fadeOutTime > 0.0f)
		{
			CActor *act = (CActor *)local;
			CScreenEffects* pSE = act->GetScreenEffects();
			if (pSE)
			{
				IBlendedEffect *blur = CBlendedEffect<CPostProcessEffect>::Create(CPostProcessEffect(local->GetEntityId(),pScaledEffect->ppname, 0.0f));
				IBlendType *linear = CBlendType<CLinearBlend>::Create(CLinearBlend(1.0f));
				pSE->StartBlend(blur, linear, 1.0f/pScaledEffect->fadeOutTime, CScreenEffects::eSFX_GID_SmokeBlur);
			}
		}
		else
		{
			gEnv->p3DEngine->SetPostEffectParam(pScaledEffect->ppname, 0.0f);
		}
	}
}

//------------------------------------------------------------------------
void CProjectile::SetTrackedByHUD()
{
}

//------------------------------------------------------------------------
void CProjectile::Ricochet(EventPhysCollision *pCollision)
{
	IActor *pActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pActor)
		return;

	Vec3 dir=pCollision->vloc[0];
	dir.NormalizeSafe();

	float dot=pCollision->n.Dot(dir);

	if (dot>=0.0f) // backface
		return;

	float b=0,f=0;
	uint32 matPierceability=0;
	if (!gEnv->pPhysicalWorld->GetSurfaceParameters(pCollision->idmat[1], b, f, matPierceability))
		return;

	matPierceability&=sf_pierceable_mask;
	float probability=0.25f+0.25f*((float)MAX(0,7-matPierceability)/7.0f);
	if ((matPierceability && matPierceability>=8) || Random()>probability)
		return;

	f32 cosine = dir.Dot(-pCollision->n);
	if (cosine>1.0f)cosine=1.0f;
	if (cosine<-1.0f)	cosine=-1.0f;
	float angle=RAD2DEG( cry_fabsf(cry_acosf(cosine)) );
	if (angle<10.0f)
		return;

	Vec3 ricochetDir = -2.0f*dot*pCollision->n+dir;
	ricochetDir.NormalizeSafe();

	Ang3 angles=Ang3::GetAnglesXYZ(Matrix33::CreateRotationVDir(ricochetDir));

	float rx=Random()-0.5f;
	float rz=Random()-0.5f;

	angles.x+=rx*DEG2RAD(10.0f);
	angles.z+=rz*DEG2RAD(10.0f);

	ricochetDir=Matrix33::CreateRotationXYZ(angles).GetColumn(1).normalized();

	Lineseg line(pCollision->pt, pCollision->pt+ricochetDir*20.0f);
	Vec3 player = pActor->GetEntity()->GetWorldPos();

	float t;
	float distanceSq=Distance::Point_LinesegSq(player, line, t);

	if (distanceSq < 7.5*7.5 && (t>=0.0f && t<=1.0f))
	{
		if (distanceSq >= 0.25*0.25)
		{
			Sphere s;
			s.center = player;
			s.radius = 6.0f;

			Vec3 entry,exit;
			int intersect=Intersect::Lineseg_Sphere(line, s, entry,exit);
			if (intersect) // one entry or one entry and one exit
			{
				if (intersect==0x2)
					entry=pCollision->pt;
				RicochetSound(entry, ricochetDir);

				//gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(entry, ColorB(255, 255, 255, 255), entry+ricochetDir, ColorB(255, 255, 255, 255), 2);
			}
		}
	}
}


CWeapon *CProjectile::GetWeapon()
{
	if (m_weaponId)
	{
		IItem *pItem=g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem(m_weaponId);
		if (pItem)
			return static_cast<CWeapon *>(pItem->GetIWeapon());
	}
	return 0;
}

EntityId CProjectile::GetOwnerId()const
{
    return m_ownerId;
}

float CProjectile::GetSpeed() const
{ 
	return m_pAmmoParams->speed;
}

//==================================================================
void CProjectile::OnHit(const HitInfo& hit)
{
	//C4, special case
	if(m_noBulletHits)
		return;

	//Reduce hit points if hit, and explode (only for C4, AVMine and ClayMore)
	if(hit.targetId==GetEntityId() && m_hitPoints>0 && !m_destroying)
	{
		m_hitPoints -= (int)hit.damage;

		if(m_hitPoints<=0)
			Explode(true);
	}
}
//==================================================================
void CProjectile::OnExplosion(const ExplosionInfo& explosion)
{	

}
//==================================================================
void CProjectile::OnServerExplosion(const ExplosionInfo& explosion)
{
	//In case this was the same projectile that created the explosion, hitPoints should be already 0
	if(m_hitPoints<=0 || m_destroying)
		return;

	//Not explode if frozen
	if (CGameRules * pGameRules = g_pGame->GetGameRules())
		if(pGameRules->IsFrozen(GetEntityId()))
			return;

	//One check more, just in case...
	//if(CWeapon* pWep = GetWeapon())
		//if(pWep->GetEntityId()==explosion.weaponId)
			//return;

	//Stolen from SinglePlayer.lua ;p
	IPhysicalEntity *pPE = GetEntity()->GetPhysics();
	if(pPE)
	{
		float obstruction = 1.0f-gEnv->pSystem->GetIPhysicalWorld()->IsAffectedByExplosion(pPE);

	  float distance	= (GetEntity()->GetWorldPos()-explosion.pos).len();
    distance = max(0.0f, min(distance,explosion.radius));
		
		float		 effect = (explosion.radius-distance)/explosion.radius;
		effect =  max(min(1.0f, effect*effect), 0.0f);
		effect =  effect*(1.0f-obstruction*0.7f); 
		
		m_hitPoints -= (int)(effect*explosion.damage);

		if(m_hitPoints<=0)
			Explode(true);
	}

}

//---------------------------------------------------------------------------------
void CProjectile::SetDefaultParticleParams(pe_params_particle *pParams)
{
 //Use ammo params if they exist
 if(m_pAmmoParams && m_pAmmoParams->pParticleParams)
 {
	 pParams->mass = m_pAmmoParams->pParticleParams->mass; 
	 pParams->size = m_pAmmoParams->pParticleParams->size;
	 pParams->thickness = m_pAmmoParams->pParticleParams->thickness;
	 pParams->heading.Set(0.0f,0.0f,0.0f);
	 pParams->velocity = 0.0f;
	 pParams->wspin = m_pAmmoParams->pParticleParams->wspin;
	 pParams->gravity = m_pAmmoParams->pParticleParams->gravity;
	 pParams->normal.Set(0.0f,0.0f,0.0f);
	 pParams->kAirResistance = m_pAmmoParams->pParticleParams->kAirResistance;
	 pParams->accThrust = m_pAmmoParams->pParticleParams->accThrust;
	 pParams->accLift = m_pAmmoParams->pParticleParams->accLift;
	 pParams->q0.SetIdentity(); 
	 pParams->surface_idx = m_pAmmoParams->pParticleParams->surface_idx;
	 pParams->flags = m_pAmmoParams->pParticleParams->flags;
	 pParams->pColliderToIgnore = NULL;
	 pParams->iPierceability = m_pAmmoParams->pParticleParams->iPierceability;
 }
 else
 {
	 int type = pParams->type;
	 memset(pParams,0,sizeof(pe_params_particle));
	 pParams->type = type;
	 pParams->velocity = 0.0f;
	 pParams->iPierceability = 7;	  
 }
}

void CProjectile::GetMemoryUsage(ICrySizer *s) const
{
	s->Add(*this);
}

void CProjectile::PostRemoteSpawn()
{
	Launch(m_initial_pos, m_initial_dir, m_initial_vel);
}

//------------------------------------------------------------------------
void CProjectile::SerializeSpawnInfo( TSerialize ser )
{
	ser.Value("hostId", m_hostId, 'eid');
	ser.Value("ownerId", m_ownerId, 'eid');
	ser.Value("weaponId", m_weaponId, 'eid');
	ser.Value("pos", m_initial_pos, 'wrld');
	ser.Value("dir", m_initial_dir, 'dir0');
	ser.Value("vel", m_initial_vel, 'vel0');

	if (ser.IsReading())
		SetParams(m_ownerId, m_hostId, m_weaponId, m_damage, m_hitTypeId, m_damageDropPerMeter, m_damageDropMinDisSqr);
}

//------------------------------------------------------------------------
namespace ProjectileDetail
{ 
	struct SInfo : public ISerializableInfo
	{
		EntityId hostId;
		EntityId ownerId;
		EntityId weaponId;
		Vec3 pos;
		Vec3 dir;
		Vec3 vel;
		void SerializeWith( TSerialize ser )
		{
			ser.Value("hostId", hostId, 'eid');
			ser.Value("ownerId", ownerId, 'eid');
			ser.Value("weaponId", weaponId, 'eid');
			ser.Value("pos", pos, 'wrld');
			ser.Value("dir", dir, 'dir0');
			ser.Value("vel", vel, 'vel0');
		}
	};
}
ISerializableInfoPtr CProjectile::GetSpawnInfo()
{

  ProjectileDetail::SInfo *p = new ProjectileDetail::SInfo();
	p->hostId=m_hostId;
	p->ownerId=m_ownerId;
	p->weaponId=m_weaponId;
	p->pos=m_initial_pos;
	p->dir=m_initial_dir;
	p->vel=m_initial_vel;
	
	return p;
}

uint8 CProjectile::GetDefaultProfile( EEntityAspects aspect )
{
	if (aspect == eEA_Physics)
		return m_pAmmoParams->physicalizationType;
	else
		return 0;
}


//------------------------------------------------------------------------
void CProjectile::PostSerialize()
{
//	InitWithAI();
}

//------------------------------------------------------------------------
void CProjectile::InitWithAI()
{
	// register with ai if needed
	//FIXME
	//make AI ignore grenades thrown by AI; needs proper/readable grenade reaction
	if (m_pAmmoParams->aiType!=AIOBJECT_NONE)
	{
		bool	isFriendlyGrenade(true);
		IEntity *pOwnerEntity = gEnv->pEntitySystem->GetEntity(m_ownerId);

		if (pOwnerEntity && pOwnerEntity->GetAI())
			isFriendlyGrenade = (pOwnerEntity->GetAI()->GetAIType() == AIOBJECT_ACTOR);

		if (!isFriendlyGrenade)
		{
			GetEntity()->RegisterInAISystem(AIObjectParams(m_pAmmoParams->aiType));
		}
	}
	GetGameObject()->SetAIActivation(eGOAIAM_Always);
}

