/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a standard wheel based vehicle movements

-------------------------------------------------------------------------
History:
- Created : stan fichele, an attempt to provide different
handling from the engine default

*************************************************************************/

#include "StdAfx.h"
#include "Game.h"
#include "GameCVars.h"

#include "VehicleMovementArcadeWheeled.h"

#include "IVehicleSystem.h"
#include "Network/NetActionSync.h"
#include <IAgent.h>
#include "GameUtils.h"
#include "IGameTokens.h"
#include "Player.h"

#include "NetInputChainDebug.h"

DEFINE_SHARED_PARAMS_TYPE_INFO(CVehicleMovementArcadeWheeled::SSharedParams);

#define THREAD_SAFE 1
#define LOAD_RAMP_TIME 0.2f
#define LOAD_RELAX_TIME 0.25f


/*
============================================================================================================================
- MISC PHYSICS - 
============================================================================================================================
*/

#ifndef XYZ
#define XYZ(v) (v).x, (v).y, (v).z
#endif

static inline float approxExp(float x)
{
	return __fres(1.f+x);	// Approximation of exp(-x)
}

static inline float approxOneExp(float x)
{
	return x*__fres(1.f+x);	// Approximation of 1.f - exp(-x)
}

struct ClampedImpulse
{
	float min;
	float max;
	float applied;
};

static void clampedImpulseInit(ClampedImpulse* c, float mn, float mx)
{
	c->applied = 0.f;
	c->max = mx;	// max should be >= 0.f
	c->min = mn;	// min should be <= 0.f
}

static void clampedImpulseInit(ClampedImpulse* c, float mx)
{
	c->applied = 0.f;
	c->max = +fabsf(mx);
	c->min = -fabsf(mx);
}

static float clampedImpulseApply(ClampedImpulse* c, float impulse)
{
	float prev = c->applied;
	float total = c->applied + impulse;
	c->applied = clamp(total, c->min, c->max);
	return c->applied - prev;
}

static inline float computeDenominator(float invMass, float invInertia, const Vec3& offset, const Vec3& norm)
{
	// If you apply an impulse of 1.0f in the direction of 'norm'
	// at position specified by 'offset' then the point will change
	// velocity by the amount calculated here
	Vec3 cross = offset.cross(norm);
	cross = cross * invInertia;	// Assume sphere inertia for now
	cross = cross.cross(offset);
	return norm.dot(cross) + invMass;
}

static inline void addImpulseAtOffset(Vec3& vel, Vec3& angVel, float invMass, float invInertia, const Vec3& offset, const Vec3& impulse)
{
	vel = vel + impulse * invMass;
	angVel = angVel + (offset.cross(impulse) * invInertia);
}

struct Vehicle3Settings
{
	bool useNewSystem;
	Vehicle3Settings()
	{
		useNewSystem = false;
	}
};

/*
============================================================================================================================
- CVehicleMovementArcadeWheeled -
============================================================================================================================
*/
CVehicleMovementArcadeWheeled::CVehicleMovementArcadeWheeled()
{
	m_iWaterLevelUpdate = 0;
	m_wheelStatusLock = 0;

	m_passengerCount = 0;
	m_steerMax = 20.0f;
	m_speedSuspUpdated = -1.f;
	m_suspDamping = 0.f;
	m_stabi = 0.f;
	m_rpmTarget = 0.f;
	m_lastBump = 0.f;
	m_rpmScalePrev = 0.f;
	m_gearSoundPending = false;
	m_prevAngle = 0.0f;  
	m_lostContactTimer = 0.f;
	m_brakeTimer = 0.f;
	m_reverseTimer = 0.0f;
	m_tireBlownTimer = 0.f;
	m_boostEndurance = 7.5f;
	m_boostRegen = m_boostEndurance;
	m_boostStrength = 6.f;
	m_lastDebugFrame = 0;
	//m_netActionSync.PublishActions( CNetworkMovementArcadeWheeled(this) );  	  
	m_blownTires = 0;
	m_bForceSleep = false;
	m_forceSleepTimer = 0.0f;
	m_submergedRatioMax = 0.0f;
	m_initialHandbreak = true;

	m_stationaryHandbrake						= true;
	m_stationaryHandbrakeResetTimer	= 0.0f;

	m_handling.compressionScale = 1.f;

	m_handling.handBrakePowerSlideTimer = 0.f;
	m_handling.canPowerSlide = false;
	m_handling.powerSlideDir = 0.f;

	m_chassis.radius = 1.f;

	m_damageRPMScale = 1.f;

	m_frictionState = k_frictionNotSet;
	m_netLerp = false;

	// Gear defaults

	m_gears.curGear = SVehicleGears::kNeutral;

	m_handling.contactNormal.zero();
}

//------------------------------------------------------------------------
CVehicleMovementArcadeWheeled::~CVehicleMovementArcadeWheeled()
{ 
	m_wheelStatusLock = 0;
}

//------------------------------------------------------------------------

namespace
{
	CVehicleParams GetWheeledTable( const CVehicleParams& table )
	{
		CVehicleParams wheeledLegacyTable = table.findChild("WheeledLegacy");

		if (!wheeledLegacyTable)
			return table.findChild("Wheeled");

		return wheeledLegacyTable;
	}
}

bool CVehicleMovementArcadeWheeled::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	if (!CVehicleMovementBase::Init(pVehicle, table))
	{
		assert(0);
		return false;
	}

	CryFixedStringT<256>	sharedParamsName;

	sharedParamsName.Format("%s::%s::CVehicleMovementArcadeWheeled", pVehicle->GetEntity()->GetClass()->GetName(), pVehicle->GetModification());

	ISharedParamsManager	*pSharedParamsManager = gEnv->pGame->GetIGameFramework()->GetISharedParamsManager();

	CRY_ASSERT(pSharedParamsManager);

	m_pSharedParams = CastSharedParamsPtr<SSharedParams>(pSharedParamsManager->Get(sharedParamsName));

	if(!m_pSharedParams)
	{
		SSharedParams	sharedParams;

		sharedParams.isBreakingOnIdle			= false;
		sharedParams.steerSpeed						= 40.0f;
		sharedParams.steerSpeedMin				= 90.0f;
		sharedParams.kvSteerMax						= 10.0f;
		sharedParams.v0SteerMax						= 30.0f;
		sharedParams.steerSpeedScaleMin		= 1.0f;
		sharedParams.steerSpeedScale			= 0.8f;
		sharedParams.steerRelaxation			= 90.0f;
		sharedParams.vMaxSteerMax					= 20.f;
		sharedParams.pedalLimitMax				= 0.3f; 
		sharedParams.suspDampingMin				= 0.0f;
		sharedParams.suspDampingMax				= 0.0f;  
		sharedParams.suspDampingMaxSpeed	= 0.0f;
		sharedParams.stabiMin							= 0.0f;
		sharedParams.stabiMax							= 0.0f;
		sharedParams.rpmRelaxSpeed				= 4.0f;
		sharedParams.rpmInterpSpeed				= 4.0f;
		sharedParams.rpmGearShiftSpeed		= 4.0f;  
		sharedParams.bumpMinSusp					= 0.0f;
		sharedParams.bumpMinSpeed					= 0.0f;
		sharedParams.bumpIntensityMult		= 1.0f;
		sharedParams.airbrakeTime					= 0.0f;

		sharedParams.gears.ratios[0]					= -1.0f;	// Reverse gear.
		sharedParams.gears.invRatios[0]				= -1.0f;	// Reverse gear.
		sharedParams.gears.ratios[1]					= 0.0f;		// Neutral.
		sharedParams.gears.invRatios[1]				= 0.0f;		// Neutral.
		sharedParams.gears.ratios[2]					= 1.0f;		// First gear.
		sharedParams.gears.invRatios[2]				= 1.0f;		// First gear.
		sharedParams.gears.numGears						= 3;
		sharedParams.gears.minChangeUpTime		= 0.6f;
		sharedParams.gears.minChangeDownTime	= 0.3f;

		sharedParams.handling.acceleration										= 5.0f;
		sharedParams.handling.decceleration										= 5.0f;
		sharedParams.handling.topSpeed												= 10.0f;
		sharedParams.handling.reverseSpeed										= 5.0f;
		sharedParams.handling.reductionAmount									= 0.0f;
		sharedParams.handling.reductionRate										= 0.0f;
		sharedParams.handling.compressionBoost								= 0.0f;
		sharedParams.handling.compressionBoostHandBrake				= 0.0f;
		sharedParams.handling.backFriction										= 100.f;
		sharedParams.handling.frontFriction										= 100.f;
		sharedParams.handling.frictionOffset									= -0.05f;
		sharedParams.handling.grip1														= 0.8f;
		sharedParams.handling.grip2														= 1.0f;
		sharedParams.handling.gripK														= 1.0f;
		sharedParams.handling.accelMultiplier1								= 1.0f;
		sharedParams.handling.accelMultiplier2								= 1.0f;
		sharedParams.handling.handBrakeDecceleration					= 30.f;
		sharedParams.handling.handBrakeDeccelerationPowerLock	= 0.0f;
		sharedParams.handling.handBrakeLockFront							= true;
		sharedParams.handling.handBrakeLockBack								= true;
		sharedParams.handling.handBrakeFrontFrictionScale			= 1.0f;
		sharedParams.handling.handBrakeBackFrictionScale			= 1.0f;
		sharedParams.handling.handBrakeAngCorrectionScale			= 1.0f;
		sharedParams.handling.handBrakeLateralCorrectionScale	= 1.0f;
		sharedParams.handling.handBrakeRotationDeadTime				= 0.0f;

		sharedParams.correction.lateralSpring	= 0.0f;
		sharedParams.correction.angSpring			= 0.0f;

		table.getAttr("isBreakingOnIdle", sharedParams.isBreakingOnIdle);
		table.getAttr("steerSpeed", sharedParams.steerSpeed);
		table.getAttr("steerSpeedMin", sharedParams.steerSpeedMin);
		table.getAttr("kvSteerMax", sharedParams.kvSteerMax);
		table.getAttr("v0SteerMax", sharedParams.v0SteerMax);
		table.getAttr("steerSpeedScaleMin", sharedParams.steerSpeedScaleMin);
		table.getAttr("steerSpeedScale", sharedParams.steerSpeedScale);
		table.getAttr("steerRelaxation", sharedParams.steerRelaxation);
		table.getAttr("vMaxSteerMax", sharedParams.vMaxSteerMax);
		table.getAttr("pedalLimitMax", sharedParams.pedalLimitMax);
		table.getAttr("rpmRelaxSpeed", sharedParams.rpmRelaxSpeed);
		table.getAttr("rpmInterpSpeed", sharedParams.rpmInterpSpeed);
		table.getAttr("rpmGearShiftSpeed", sharedParams.rpmGearShiftSpeed);

		if(CVehicleParams wheeledTable = GetWheeledTable(table))
		{
			wheeledTable.getAttr("suspDampingMin", sharedParams.suspDampingMin);
			wheeledTable.getAttr("suspDampingMax", sharedParams.suspDampingMax);
			wheeledTable.getAttr("suspDampingMaxSpeed", sharedParams.suspDampingMaxSpeed);
			wheeledTable.getAttr("stabiMin", sharedParams.stabiMin);
			wheeledTable.getAttr("stabiMax", sharedParams.stabiMax);
		}

		if(CVehicleParams soundParams = table.findChild("SoundParams"))
		{
			soundParams.getAttr("roadBumpMinSusp", sharedParams.bumpMinSusp);
			soundParams.getAttr("roadBumpMinSpeed", sharedParams.bumpMinSpeed);
			soundParams.getAttr("roadBumpIntensity", sharedParams.bumpIntensityMult);
			soundParams.getAttr("airbrake", sharedParams.airbrakeTime);
		}

		if(CVehicleParams fakeGearsParams = table.findChild("FakeGears"))
		{
			fakeGearsParams.getAttr("minChangeUpTime", sharedParams.gears.minChangeUpTime);
			fakeGearsParams.getAttr("minChangeDownTime", sharedParams.gears.minChangeDownTime);

			if(CVehicleParams ratios = fakeGearsParams.findChild("Ratios"))
			{
				int	count = min(ratios.getChildCount(), SVehicleGears::kMaxGears - 3);

				if(count > 0)
				{
					sharedParams.gears.numGears = 2;

					for(int i = 0; i < count; ++ i)
					{	
						float	ratio = 0.0f;

						if(CVehicleParams gearRef = ratios.getChild(i))
						{
							gearRef.getAttr("value", ratio);

							if(ratio > 0.0f)
							{
								sharedParams.gears.ratios[sharedParams.gears.numGears ++] = ratio;
							}
						}
					}
				}

				for(int i = 0; i < sharedParams.gears.numGears; ++ i)
				{
					const float	ratio = sharedParams.gears.ratios[i];

					sharedParams.gears.invRatios[i] = (ratio != 0.0f) ? 1.0f / ratio : 0.f;
				}
			}
		}

		if(CVehicleParams	handlingParams = table.findChild("Handling"))
		{
			if(CVehicleParams powerParams = handlingParams.findChild("Power"))
			{
				powerParams.getAttr("acceleration", sharedParams.handling.acceleration);
				powerParams.getAttr("decceleration", sharedParams.handling.decceleration);
				powerParams.getAttr("topSpeed", sharedParams.handling.topSpeed);
				powerParams.getAttr("reverseSpeed", sharedParams.handling.reverseSpeed);
			}

			if(CVehicleParams reductionParams = handlingParams.findChild("SpeedReduction"))
			{
				reductionParams.getAttr("reductionAmount", sharedParams.handling.reductionAmount);
				reductionParams.getAttr("reductionRate", sharedParams.handling.reductionRate);
			}

			if(CVehicleParams compressionParams = handlingParams.findChild("Compression"))
			{
				compressionParams.getAttr("frictionBoost", sharedParams.handling.compressionBoost);
				compressionParams.getAttr("frictionBoostHandBrake", sharedParams.handling.compressionBoostHandBrake);
			}

			if(CVehicleParams frictionParams = handlingParams.findChild("Friction"))
			{
				frictionParams.getAttr("back", sharedParams.handling.backFriction);
				frictionParams.getAttr("front", sharedParams.handling.frontFriction);
				frictionParams.getAttr("offset", sharedParams.handling.frictionOffset);
			}

			if(CVehicleParams wheelSpin = handlingParams.findChild("WheelSpin"))
			{
				wheelSpin.getAttr("grip1", sharedParams.handling.grip1);
				wheelSpin.getAttr("grip2", sharedParams.handling.grip2);
				wheelSpin.getAttr("gripRecoverSpeed", sharedParams.handling.gripK);
				wheelSpin.getAttr("accelMultiplier1", sharedParams.handling.accelMultiplier1);
				wheelSpin.getAttr("accelMultiplier2", sharedParams.handling.accelMultiplier2);

				if(sharedParams.handling.gripK > 0.0f)
				{
					sharedParams.handling.gripK = 1.0f / sharedParams.handling.gripK;
				}
			}

			if(CVehicleParams handBrakeParams = handlingParams.findChild("HandBrake"))
			{
				handBrakeParams.getAttr("decceleration", sharedParams.handling.handBrakeDecceleration);
				handBrakeParams.getAttr("deccelerationPowerLock", sharedParams.handling.handBrakeDeccelerationPowerLock);
				handBrakeParams.getAttr("lockBack", sharedParams.handling.handBrakeLockBack);
				handBrakeParams.getAttr("lockFront", sharedParams.handling.handBrakeLockFront);
				handBrakeParams.getAttr("frontFrictionScale", sharedParams.handling.handBrakeFrontFrictionScale);
				handBrakeParams.getAttr("backFrictionScale", sharedParams.handling.handBrakeBackFrictionScale);
				handBrakeParams.getAttr("angCorrectionScale", sharedParams.handling.handBrakeAngCorrectionScale);
				handBrakeParams.getAttr("latCorrectionScale", sharedParams.handling.handBrakeLateralCorrectionScale);
			}

			if(CVehicleParams slideParams = handlingParams.findChild("PowerSlide"))
			{
				slideParams.getAttr("handBrakeDeadTime", sharedParams.handling.handBrakeRotationDeadTime);
			}

			if(CVehicleParams correctionParams = handlingParams.findChild("Correction"))
			{
				correctionParams.getAttr("lateralSpring", sharedParams.correction.lateralSpring);
				correctionParams.getAttr("angSpring", sharedParams.correction.angSpring);
			}
		}

		m_pSharedParams = CastSharedParamsPtr<SSharedParams>(pSharedParamsManager->Register(sharedParamsName, sharedParams));
	}

	CRY_ASSERT(m_pSharedParams.get());

	m_carParams.enginePower = 0.f;
	m_carParams.kStabilizer = 0.f;  
	m_carParams.engineIdleRPM = 0.f;
	m_carParams.engineMinRPM = m_carParams.engineMaxRPM = 0.f;     
	m_carParams.engineShiftDownRPM = m_carParams.engineShiftUpRPM = 0.f;
	m_carParams.steerTrackNeutralTurn = 0.f;  

	if (CVehicleParams handlingParams = table.findChild("Handling"))
	{
		/* Inertia */
		if (CVehicleParams inertiaParams = handlingParams.findChild("Inertia"))
		{
			inertiaParams.getAttr("radius", m_chassis.radius);
		}
	}

	m_action.steer = 0.0f;
	m_action.pedal = 0.0f;
	m_action.dsteer = 0.0f;

	// Initialise the steering history.
	m_prevAngle = 0.0f;

	m_rpmScale = 0.0f;
	m_currentGear = 0;
	m_compressionMax = 0.f;
	m_wheelContacts = 0;

	return InitPhysics(table);
}

//------------------------------------------------------------------------

#define ARCADE_WHEELED_MOVEMENT_VALUE_REQ(name, var, t)																																								\
	if(!t.getAttr(name, var))																																																							\
{																																																																			\
	CryLog("Movement Init (%s) - failed to init due to missing <%s> parameter", m_pVehicle->GetEntity()->GetClass()->GetName(), name);	\
	\
	return false;																																																												\
}

bool CVehicleMovementArcadeWheeled::InitPhysics(const CVehicleParams& table)
{
	CVehicleParams wheeledTable = GetWheeledTable( table );

	if (!wheeledTable)
		return false;

	m_carParams.maxTimeStep = 0.02f;

	ARCADE_WHEELED_MOVEMENT_VALUE_REQ("damping", m_carParams.damping, wheeledTable);
	ARCADE_WHEELED_MOVEMENT_VALUE_REQ("engineIdleRPM", m_carParams.engineIdleRPM, wheeledTable);
	ARCADE_WHEELED_MOVEMENT_VALUE_REQ("engineMaxRPM", m_carParams.engineMaxRPM, wheeledTable);
	ARCADE_WHEELED_MOVEMENT_VALUE_REQ("engineMinRPM", m_carParams.engineMinRPM, wheeledTable);
	ARCADE_WHEELED_MOVEMENT_VALUE_REQ("stabilizer", m_carParams.kStabilizer, wheeledTable);

	wheeledTable.getAttr("integrationType", m_carParams.iIntegrationType);
	wheeledTable.getAttr("maxTimeStep", m_carParams.maxTimeStep);
	wheeledTable.getAttr("minEnergy", m_carParams.minEnergy);

	m_stabi = m_carParams.kStabilizer;

	if (g_pGameCVars->pVehicleQuality->GetIVal()==1)
		m_carParams.maxTimeStep = max(m_carParams.maxTimeStep, 0.04f);

	{
		// Set up the Arcade Mass
		m_chassis.mass = 1.f;
		m_chassis.invMass = 1.f/m_chassis.mass;
		m_chassis.inertia = 0.4f*m_chassis.radius*m_chassis.radius*m_chassis.mass;
		m_chassis.invInertia = 1.f/m_chassis.inertia;

		// Kill some of the low level physics params
		m_carParams.enginePower=0.f;
		m_carParams.brakeTorque=0.f;
		m_carParams.clutchSpeed=0.f;
		//m_carParams.damping=0.f;
		m_carParams.minBrakingFriction=0.f;
		m_carParams.maxBrakingFriction=0.f;
		m_carParams.kDynFriction=0.f;
		m_carParams.axleFriction=0.f;
		m_carParams.pullTilt=0.f;

		// Better Collisions. Only make wheels part of chassis
		// when contact normal is over 85 degrees
		m_carParams.maxTilt = sinf(DEG2RAD(85.f));
		m_carParams.bKeepTractionWhenTilted = 1;

		m_maxSpeed = m_pSharedParams->handling.topSpeed;
	}

	return true;
}

#undef ARCADE_WHEELED_MOVEMENT_VALUE_REQ

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::PostInit()
{
	CVehicleMovementBase::PostInit();

	// This needs to be called from two places due to the init order on server and client
	//Reset();
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::Physicalize()
{
	CVehicleMovementBase::Physicalize();

	SEntityPhysicalizeParams physicsParams(m_pVehicle->GetPhysicsParams());	

	physicsParams.type = PE_WHEELEDVEHICLE;	  
	m_carParams.nWheels = m_pVehicle->GetWheelCount();
	m_numWheels = m_carParams.nWheels;
	m_wheels.resize(m_numWheels);
	m_wheelStatus.resize(m_numWheels);
	m_invNumWheels = 1.f/(float)(m_numWheels|iszero(m_numWheels));

	pe_params_car carParams(m_carParams);  
	physicsParams.pCar = &carParams;

	m_pVehicle->GetEntity()->Physicalize(physicsParams);

	IPhysicalEntity *pPhysEnt = GetPhysics();
	if (pPhysEnt)
	{
		if (g_pGameCVars->pVehicleQuality->GetIVal()==1 && m_carParams.steerTrackNeutralTurn)
		{
			pe_params_flags pf; pf.flagsOR = wwef_fake_inner_wheels;
			pe_params_foreign_data pfd; pfd.iForeignFlagsOR = PFF_UNIMPORTANT;
			pPhysEnt->SetParams(&pf);
			pPhysEnt->SetParams(&pfd);
		}
		GetCurrentWheelStatus(pPhysEnt);
	}
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::PostPhysicalize()
{
	CVehicleMovementBase::PostPhysicalize();

	// This needs to be called from two places due to the init order on server and client
	if(!gEnv->pSystem->IsSerializingFile()) //don't do this while loading a savegame, it will overwrite the engine
		Reset();
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::InitSurfaceEffects()
{ 
	IPhysicalEntity* pPhysics = GetPhysics();
	pe_status_nparts tmpStatus;
	int numParts = pPhysics->GetStatus(&tmpStatus);
	int numWheels = m_pVehicle->GetWheelCount();

	m_paStats.envStats.emitters.clear();

	// for each wheelgroup, add 1 particleemitter. the position is the wheels' 
	// center in xy-plane and minimum on z-axis
	// direction is upward
	SEnvironmentParticles* envParams = m_pPaParams->GetEnvironmentParticles();

	for (int iLayer=0; iLayer < (int)envParams->GetLayerCount(); ++iLayer)
	{
		const SEnvironmentLayer& layer = envParams->GetLayer(iLayer);

		m_paStats.envStats.emitters.reserve(m_paStats.envStats.emitters.size() + layer.GetGroupCount());

		for (int i=0; i < (int)layer.GetGroupCount(); ++i)
		{ 
			Matrix34 tm(IDENTITY);

			if (layer.GetHelperCount() == layer.GetGroupCount() && layer.GetHelper(i))
			{
				// use helper pos if specified
				if (IVehicleHelper* pHelper = layer.GetHelper(i))
					pHelper->GetVehicleTM(tm);
			}
			else
			{
				// else use wheels' center
				Vec3 pos(ZERO);

				for (int w=0; w < (int)layer.GetWheelCount(i); ++w)
				{       
					int ipart = numParts - numWheels + layer.GetWheelAt(i,w)-1; // wheels are last

					if (ipart < 0 || ipart >= numParts)
					{
						CryLog("%s invalid wheel index: %i, maybe asset/setup mismatch", m_pEntity->GetName(), ipart);
						continue;
					}

					pe_status_pos spos;
					spos.ipart = ipart;
					if (pPhysics->GetStatus(&spos))
					{
						spos.pos.z += spos.BBox[0].z;
						pos = (pos.IsZero()) ? spos.pos : 0.5f*(pos + spos.pos);        
					}
				}
				tm = Matrix34::CreateRotationX(DEG2RAD(90.f));      
				tm.SetTranslation( m_pEntity->GetWorldTM().GetInverted().TransformPoint(pos) );
			}     

			TEnvEmitter emitter;
			emitter.layer = iLayer;        
			emitter.slot = -1;
			emitter.group = i;
			emitter.active = layer.IsGroupActive(i);
			emitter.quatT = QuatT(tm);
			m_paStats.envStats.emitters.push_back(emitter);

#if ENABLE_VEHICLE_DEBUG
			if (DebugParticles())
			{
				const Vec3 loc = tm.GetTranslation();
				CryLog("WheelGroup %i local pos: %.1f %.1f %.1f", i, loc.x, loc.y, loc.z);        
			}      
#endif
		}
	}

	m_paStats.envStats.initalized = true;  
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::Reset()
{
	CVehicleMovementBase::Reset();

	m_prevAngle = 0.0f;
	m_action.pedal = 0.f;
	m_action.steer = 0.f;
	m_rpmScale = 0.0f;
	m_rpmScalePrev = 0.f;
	m_currentGear = 0;
	m_gearSoundPending = false;
	m_tireBlownTimer = 0.f; 
	m_wheelContacts = 0;

	if (m_blownTires)
		SetEngineRPMMult(1.0f);
	m_blownTires = 0;

	// needs to be after base reset (m_maxSpeed is overwritten by tweak groups)
	m_maxSpeed = m_pSharedParams->handling.topSpeed;

	IPhysicalEntity* pPhysics = GetPhysics();

	if (m_bForceSleep)
	{
		if (pPhysics)
		{
			pe_params_car params;
			params.minEnergy = m_carParams.minEnergy;
			pPhysics->SetParams(&params, 1);
		}
	}
	m_bForceSleep = false;
	m_forceSleepTimer = 0.0f;
	m_passengerCount = 0;
	m_initialHandbreak = true;

	m_damageRPMScale = 1.f;

	m_chassis.vel.zero();
	m_chassis.angVel.zero();
	m_chassis.collision1 = false;
	m_chassis.collision2 = false;

	m_gears.averageWheelRadius = 1.f;

	// Store numWheels once
	m_numWheels = m_pVehicle->GetWheelCount();
	m_wheels.resize(m_numWheels);
	m_wheelStatus.resize(m_numWheels);
	m_invNumWheels = 1.f/(float)(m_numWheels|iszero(m_numWheels));
	assert(m_numWheels<=maxWheels);

	int n = 0;
	const int nParts = m_pVehicle->GetPartCount();
	for (int i=0; i<nParts; ++i)
	{      
		IVehiclePart* pPart = m_pVehicle->GetPart(i);
		if (pPart->GetIWheel())
		{ 
			m_wheels[n++].wheelPart = pPart;
		}
	}
	assert(n == m_numWheels);

	if (pPhysics && m_numWheels>0)
	{
		pe_params_car car;
		pPhysics->GetParams(&car);
		if (car.nWheels != m_numWheels)
		{
			// Asset is probably missing, reset number of wheels
			m_numWheels = 0;
			m_wheels.clear();
			m_wheelStatus.clear();
			m_invNumWheels = 1.f;
			IEntity* pEntity = m_pVehicle->GetEntity();
			GameWarning("Vehicle '%s' has broken setup! Have %d physics wheels, but xml claims %d wheels", pEntity?pEntity->GetName():"", car.nWheels, m_numWheels);
		}

		pe_status_pos pos;
		pe_status_dynamics dyn;
		pPhysics->GetStatus(&pos);
		pPhysics->GetStatus(&dyn);
		Vec3 offset = (dyn.centerOfMass - pos.pos)*pos.q;		// Offset of centre of mass, in car space, from the physical-entity origin

		m_chassis.contactIdx0 = m_chassis.contactIdx1 = m_chassis.contactIdx2 = m_chassis.contactIdx3 = 0;
		float bestOffset[4] = {0.f, 0.f, 0.f, 0.f};

		float averageRadius = 0.f;

		GetCurrentWheelStatus(pPhysics);

		const int numWheels = m_numWheels;
		for (int i=0; i<numWheels; i++)
		{
			SVehicleWheel* w = &m_wheels[i];
			IVehiclePart* pPart = m_wheels[i].wheelPart;
			int physId = pPart->GetPhysId();
			pe_params_part paramsPart;
			paramsPart.partid = physId;
			if (pPhysics->GetParams(&paramsPart) != 0)
			{
				w->offset = paramsPart.pos - offset;	// This is relative to the centre of mass
			}
			else
			{
				w->offset.zero();
			}

			w->radius = m_wheelStatus[i].r;
			w->mass = m_chassis.mass*0.05f;
			w->invMass = 1.f/w->mass;
			w->inertia = 0.4f*w->radius*w->radius*w->mass;
			if (w->inertia != 0)
				w->invInertia = 1.f/w->inertia;
			else
				w->invInertia = 1.0f;
			w->w = 0.f;
			w->lastW = 0.f;
			w->suspLen = m_wheelStatus[i].suspLen;
			w->compression = 0.f;

			w->bottomOffset = w->offset.z - w->radius;
			w->offset.z = m_pSharedParams->handling.frictionOffset;
			w->bottomOffset -= w->offset.z;
			w->axleIndex = (w->offset.y > 0.f) ? 1 : 0;

			w->bCanLock = ((w->axleIndex == 0) && m_pSharedParams->handling.handBrakeLockBack) ? 1 : 0;
			w->bCanLock |= ((w->axleIndex == 1) && m_pSharedParams->handling.handBrakeLockFront) ? 1 : 0;

			// contactIdx0 = left-forward
			// contactIdx1 = left-back
			// contactIdx2 = right-forward
			// contactIdx3 = right-back
			// Manhatten distance used to find the best choice
			if ((w->offset.x < 0.f) && ((+w->offset.y - w->offset.x) >= bestOffset[0])) { bestOffset[0] = +w->offset.y - w->offset.x; m_chassis.contactIdx0 = i; }
			if ((w->offset.x < 0.f) && ((-w->offset.y - w->offset.x) >= bestOffset[1])) { bestOffset[1] = -w->offset.y - w->offset.x; m_chassis.contactIdx1 = i; }
			if ((w->offset.x >= 0.f) && ((+w->offset.y + w->offset.x) >= bestOffset[2])) { bestOffset[2] = +w->offset.y + w->offset.x; m_chassis.contactIdx2 = i; }
			if ((w->offset.x >= 0.f) && ((-w->offset.y + w->offset.x) >= bestOffset[3])) { bestOffset[3] = -w->offset.y + w->offset.x; m_chassis.contactIdx3 = i; }

			averageRadius += w->radius;
		}

		m_gears.averageWheelRadius = averageRadius*m_invNumWheels;
	}

	ResetWaterLevels();

	EnableLowLevelPhysics(k_frictionUseLowLevel, 0);
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::Release()
{
	CVehicleMovementBase::Release();

	delete this;
}

//------------------------------------------------------------------------
bool CVehicleMovementArcadeWheeled::StartEngine(EntityId driverId)
{
	if (!CVehicleMovementBase::StartEngine(driverId))
		return false;

	m_brakeTimer = 0.f;
	m_reverseTimer = 0.f;
	m_action.pedal = 0.f;
	m_action.steer = 0.f;
	m_initialHandbreak =false;

	// Gears
	m_gears.curGear = SVehicleGears::kNeutral;
	m_gears.curRpm = 0.f;
	m_gears.targetRpm = 0.f;
	m_gears.timer = 0.f;

	return true;
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::StopEngine()
{
	CVehicleMovementBase::StopEngine();
	m_movementAction.Clear(true);

	UpdateBrakes(0.f);
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::OnEvent(EVehicleMovementEvent event, const SVehicleMovementEventParams& params)
{
	CVehicleMovementBase::OnEvent(event, params);

	if (event == eVME_TireBlown || event == eVME_TireRestored)
	{		
		int wheelIndex = params.iValue;

		if (m_carParams.steerTrackNeutralTurn == 0.f)
		{
			int blownTiresPrev = m_blownTires;    
			m_blownTires = max(0, min(m_pVehicle->GetWheelCount(), event==eVME_TireBlown ? m_blownTires+1 : m_blownTires-1));

			// reduce speed based on number of tyres blown out        
			if (m_blownTires != blownTiresPrev)
			{	
				SetEngineRPMMult(GetWheelCondition());
			}
		}

		// handle particles (sparks etc.)
		SEnvironmentParticles* envParams = m_pPaParams->GetEnvironmentParticles();    

		SEnvParticleStatus::TEnvEmitters::iterator emitterIt = m_paStats.envStats.emitters.begin();
		SEnvParticleStatus::TEnvEmitters::iterator emitterItEnd = m_paStats.envStats.emitters.end();
		for (; emitterIt != emitterItEnd; ++emitterIt)
		{ 
			// disable this wheel in layer 0, enable in layer 1            
			if (emitterIt->group >= 0)
			{
				const SEnvironmentLayer& layer = envParams->GetLayer(emitterIt->layer);

				for (int i=0; i < (int)layer.GetWheelCount(emitterIt->group); ++i)
				{
					if (layer.GetWheelAt(emitterIt->group, i)-1 == wheelIndex)
					{
						bool bEnable = !strcmp(layer.GetName(), "rims");
						if (event == eVME_TireRestored)
							bEnable=!bEnable;
						EnableEnvEmitter(*emitterIt, bEnable);
						emitterIt->active = bEnable;
					}
				} 
			}
		}     
	} 

	if(event == eVME_EnableHandbrake)
	{
		// Set stationary handbrake and wake physics if necessary.

		m_stationaryHandbrake = params.bValue;

		if(!m_stationaryHandbrake)
		{
			m_stationaryHandbrakeResetTimer	= params.fValue;

			m_pVehicle->NeedsUpdate(IVehicle::eVUF_AwakePhysics, true);
		}
	}
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{

	CryAutoCriticalSection lk(m_lock);

	CVehicleMovementBase::OnAction(actionId, activationMode, value);

}

//------------------------------------------------------------------------
float CVehicleMovementArcadeWheeled::GetWheelCondition() const
{
	// for a 4-wheel vehicle, want to reduce speed by 20% for each wheel shot out. So I'm assuming that for an 8-wheel
	//	vehicle we'd want to reduce by 10% per wheel.
	return  1.0f - (float)m_blownTires*m_invNumWheels*0.8f;
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::SetEngineRPMMult(float mult, int threadSafe)
{
	m_damageRPMScale = mult;	
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
	CVehicleMovementBase::OnVehicleEvent(event,params);
}

//------------------------------------------------------------------------
float CVehicleMovementArcadeWheeled::GetMaxSteer(float speedRel)
{
	// reduce max steering angle with increasing speed
	if (m_handling.canPowerSlide==false)
	{
		m_steerMax = m_pSharedParams->v0SteerMax - (m_pSharedParams->kvSteerMax * speedRel);
	}
	else
	{
		m_steerMax = m_pSharedParams->v0SteerMax;
	}
	//  m_steerMax = 45.0f;
	return DEG2RAD(m_steerMax);
}


//------------------------------------------------------------------------
float CVehicleMovementArcadeWheeled::GetSteerSpeed(float speedRel)
{
	if (m_handling.canPowerSlide==false)
	{
		// reduce steer speed with increasing speed
		float steerDelta = m_pSharedParams->steerSpeed - m_pSharedParams->steerSpeedMin;
		float steerSpeed = m_pSharedParams->steerSpeedMin + steerDelta * speedRel;

		// additionally adjust sensitivity based on speed
		float steerScaleDelta = m_pSharedParams->steerSpeedScale - m_pSharedParams->steerSpeedScaleMin;
		float sensivity = m_pSharedParams->steerSpeedScaleMin + steerScaleDelta * speedRel;

		return steerSpeed * sensivity;
	}
	else
	{
		return 1000.f;
	}
}

//----------------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::ApplyBoost(float speed, float maxSpeed, float strength, float deltaTime)
{
	// This function need to be MT safe, called StdWeeled and Tank

	const float fullBoostMaxSpeed = 0.75f*m_maxSpeed;


	if (m_action.pedal > 0.01f && m_wheelContacts >= 0.5f*(float)m_numWheels && speed < maxSpeed)
	{     

		float fraction = 0.0f;
		if ( fabsf( maxSpeed-fullBoostMaxSpeed ) > 0.001f )
			fraction = max(0.f, 1.f - max(0.f, speed-fullBoostMaxSpeed)/(maxSpeed-fullBoostMaxSpeed));
		float amount = fraction * strength * m_action.pedal * m_PhysDyn.mass * deltaTime;

		float angle = DEG2RAD(m_carParams.steerTrackNeutralTurn == 0.f ? 30.f : 0.f);
		Vec3 dir(0, cosf(angle), -sinf(angle));

		AABB bounds;
		m_pVehicle->GetEntity()->GetLocalBounds(bounds);

		const Vec3 worldPos =  m_PhysPos.pos;
		const Matrix33 worldMat( m_PhysPos.q);

		pe_action_impulse imp;            
		imp.impulse = worldMat * dir * amount;
		imp.point = worldMat * Vec3(0, bounds.min.y, 0); // approx. at ground
		imp.point +=worldPos;
		imp.iApplyTime = 0;

		GetPhysics()->Action(&imp, THREAD_SAFE);

		//const static float color[] = {1,1,1,1};
		//gEnv->pRenderer->Draw2dLabel(400, 400, 1.4f, color, false, "fBoost: %.2f", fraction);
	}
}

#if ENABLE_VEHICLE_DEBUG
//----------------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::DebugDrawMovement(const float deltaTime)
{
	IPhysicalEntity* pPhysics = GetPhysics();
	IRenderer* pRenderer = gEnv->pRenderer;
	static float color[4] = {1,1,1,1};
	float green[4] = {0,1,0,1};
	float red[4] = {1,0,0,1};
	static ColorB colRed(255,0,0,255);
	static ColorB colWhite(255,255,255,255);
	ColorB colGreen(0,255,0,255);
	ColorB col1(255,255,0,255);
	float y = 50.f, step1 = 15.f, step2 = 20.f, size=1.3f, sizeL=1.5f;

#if defined(USER_stan)
	if (m_pVehicle->IsPlayerDriving())
	{
		pRenderer->Draw2dLabel(5.0f,   y, sizeL, color, false, "You are driving VehicleMovementStdWheeled 3"); y+=step2;
	}
#endif

	if (!IsProfilingMovement())
		return;

	if (g_pGameCVars->v_profileMovement==3 || g_pGameCVars->v_profileMovement==1 && m_lastDebugFrame == gEnv->pRenderer->GetFrameID())
		return;

	m_lastDebugFrame = gEnv->pRenderer->GetFrameID();

	const int width = pRenderer->GetWidth();
	const int height = pRenderer->GetHeight();

	Matrix33 bodyRot( m_PhysPos.q );
	Matrix34 bodyPose( bodyRot, m_PhysDyn.centerOfMass );
	const Vec3 xAxis = bodyPose.GetColumn0();
	const Vec3 yAxis = bodyPose.GetColumn1();
	const Vec3 zAxis = bodyPose.GetColumn2();
	const Vec3 chassisPos = bodyPose.GetColumn3();

	float speedMs = m_PhysDyn.v.dot(yAxis);
	float speed = m_vehicleStatus.vel.len();
	float speedRel = min(speed, m_pSharedParams->vMaxSteerMax) / m_pSharedParams->vMaxSteerMax;
	float steerMax = GetMaxSteer(speedRel);
	float steerSpeed = GetSteerSpeed(speedRel);  
	int percent = (int)(speed / m_maxSpeed * 100.f);
	Vec3 localVel = m_localSpeed;

	// Get the view name
	IActor* pActor = m_pVehicle->GetDriver();
	IVehicleSeat* pSeat = pActor ? m_pVehicle->GetSeatForPassenger(pActor->GetEntityId()) : NULL;
	IVehicleView* pView = pSeat ? pSeat->GetView(pSeat->GetCurrentView()) : NULL;
	const char* viewName = pView ? pView->GetName() : "";

	pe_params_car carparams;
	pPhysics->GetParams(&carparams);

	pRenderer->Draw2dLabel(5.0f,   y, sizeL, color, false, "Car movement");
	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "fake gear: %d", m_gears.curGear-1);

	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "invTurningRadius: %.1f", m_invTurningRadius);
	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "Speed: %.1f (%.1f km/h) (%i) (%f m/s)", speed, speed*3.6f, percent, speedMs);
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "localVel: %.1f %.1f %.1f", localVel.x, localVel.y, localVel.z);
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "Dampers:  %.2f", m_suspDamping);
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "Stabi:  %.2f", m_stabi);
	//pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "BrakeTime:  %.2f", m_brakeTimer);
	//pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "compressionScale:  %.2f", m_handling.compressionScale);

	//if (m_statusDyn.submergedFraction > 0.f)
	//	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "Submerged:  %.2f", m_statusDyn.submergedFraction);

	//if (m_damage > 0.f)
	//	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "Damage:  %.2f", m_damage);  

	//if (Boosting())
	//	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, green, false, "Boost:  %.2f", m_boostCounter);

	pRenderer->Draw2dLabel(5.0f,  y+=step2, sizeL, color, false, "using View: %s", viewName);

	pRenderer->Draw2dLabel(5.0f,  y+=step2, sizeL, color, false, "Driver input");
	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "power: %.2f", m_movementAction.power);
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "steer: %.2f", m_movementAction.rotateYaw); 
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "brake: %i", m_movementAction.brake);

	pRenderer->Draw2dLabel(5.0f,  y+=step2, sizeL, color, false, "Car action");
	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "pedal: %.2f", m_action.pedal);
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "steer: %.2f (max %.2f)", RAD2DEG(m_action.steer), RAD2DEG(steerMax)); 
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "steerFrac: %.2f", m_action.steer / DEG2RAD(m_steerMax));
	pRenderer->Draw2dLabel(5.0f,  y+=step1, size, color, false, "brake: %i", m_action.bHandBrake);

	pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "steerSpeed: %.2f", steerSpeed); 

	const Matrix34& worldTM = m_pVehicle->GetEntity()->GetWorldTM();

	IRenderAuxGeom* pAuxGeom = gEnv->pRenderer->GetIRenderAuxGeom();
	SAuxGeomRenderFlags flags = pAuxGeom->GetRenderFlags();
	SAuxGeomRenderFlags oldFlags = pAuxGeom->GetRenderFlags();
	flags.SetDepthWriteFlag(e_DepthWriteOff);
	flags.SetDepthTestFlag(e_DepthTestOff);
	pAuxGeom->SetRenderFlags(flags);

	pAuxGeom->DrawSphere(m_statusDyn.centerOfMass, 0.1f, colGreen);

	if (m_chassis.collision1)
	{
		pAuxGeom->DrawSphere(m_statusDyn.centerOfMass, 1.0f, colGreen);
	}
	if (m_chassis.collision2)
	{
		pAuxGeom->DrawSphere(m_statusDyn.centerOfMass - yAxis, 1.0f, colRed);
	}

	pAuxGeom->DrawLine(chassisPos, colGreen, chassisPos + 2.f*m_handling.contactNormal, col1);

	pe_status_wheel ws;
	pe_status_pos wp;
	pe_params_wheel wparams;

	pe_status_nparts tmpStatus;
	int numParts = pPhysics->GetStatus(&tmpStatus);

	int count = m_numWheels;
	float tScaleTotal = 0.f;


	// wheel-specific
	for (int i=0; i<count; ++i)
	{
		{
			pRenderer->Draw2dLabel(5.0f,  y+=step2, size, color, false, "slip speed: %.2f", m_wheels[i].slipSpeed);
		}

		if (0)
		{
			Vec3 pos;
			pos = m_PhysDyn.centerOfMass + m_PhysPos.q * m_wheels[i].offset;
			pAuxGeom->DrawSphere(pos, 0.05f, colGreen);
			pAuxGeom->DrawLine(pos, colGreen, pos + m_wheels[i].frictionDir[0], colGreen);
			pAuxGeom->DrawLine(pos, colGreen, pos + m_wheels[i].frictionDir[1], colGreen);
		}


		ws.iWheel = i;
		wp.ipart = numParts - count + i;
		wparams.iWheel = i;

		int ok = pPhysics->GetStatus(&ws);
		ok &= pPhysics->GetStatus(&wp);
		ok &= pPhysics->GetParams(&wparams);

		if (!ok)
			continue;

		// slip
		if (g_pGameCVars->v_draw_slip)
		{
			if (ws.bContact)
			{ 
				pAuxGeom->DrawSphere(ws.ptContact, 0.05f, colRed);

				float slip = ws.velSlip.len();        
				if (ws.bSlip>0)
				{ 
					pAuxGeom->DrawLine(wp.pos, colRed, wp.pos+ws.velSlip, colRed);
				}        
				pAuxGeom->DrawLine(wp.pos, colRed, wp.pos+ws.normContact, colRed);
			}
		}    

		// suspension    
		if (g_pGameCVars->v_draw_suspension)
		{
			ColorB col(255,0,0,255);

			Vec3 lower = m_wheels[i].wheelPart->GetLocalTM(false).GetTranslation();
			lower.x += fsgnf(lower.x) * 0.5f;

			Vec3 upper(lower);
			upper.z += ws.suspLen;

			lower = worldTM.TransformPoint(lower);
			pAuxGeom->DrawSphere(lower, 0.1f, col);              

			upper = worldTM.TransformPoint(upper);
			pAuxGeom->DrawSphere(upper, 0.1f, col);

			//pAuxGeom->DrawLine(lower, col, upper, col);
		}

		// Draw Wheel Markers (Helps for looking at wheel rotations)
		{
			const Matrix34 wheelMat = m_wheels[i].wheelPart->GetLocalTM(false);
			Vec3 pos1 = worldTM*(wheelMat*Vec3(0.f,0.f,0.f));
			Vec3 pos2 = worldTM*(wheelMat*Vec3(0.f,0.2f,0.f));
			pAuxGeom->DrawLine(pos1, colRed, pos2, colRed);
		}
	}

	if (tScaleTotal != 0.f)
	{
		gEnv->pRenderer->DrawLabel(worldTM.GetTranslation(),1.3f,"tscale: %.2f",tScaleTotal);
	}

	if (m_pWind[0])
	{
		pe_params_buoyancy buoy;
		pe_status_pos pos;
		if (m_pWind[0]->GetParams(&buoy) && m_pWind[0]->GetStatus(&pos))
		{
			gEnv->pRenderer->DrawLabel(pos.pos, 1.3f, "rad: %.1f", buoy.waterFlow.len());
		}
		if (m_pWind[1]->GetParams(&buoy) && m_pWind[1]->GetStatus(&pos))
		{
			gEnv->pRenderer->DrawLabel(pos.pos, 1.3f, "lin: %.1f", buoy.waterFlow.len());
		}
	}

	//========================
	// Draw Gears and RPM dial
	//========================
	{
		pRenderer->Set2DMode(true, width, height);

		float radius = 40.f;
		Vec3 centre (0.2f*(float)width, 0.8f*(float)height, 0.f);
		Vec3 centre1 = centre - Vec3(1.3f*radius, 0.f, 0.f);
		Vec3 centre2 = centre + Vec3(1.3f*radius, 0.f, 0.f);
		Vec3 circle0[32];
		Vec3 circle1[32];
		Vec3 circle2[32];
		float a = 0.f;
		float da = gf_PI2/32.f;
		for (int i=0; i<32; i++)
		{
			circle0[i].Set(radius*cosf(a), radius*sinf(a), 0.f);
			circle1[i] = circle0[i] + centre1; 
			circle2[i] = circle0[i] + centre2;
			a += da;
		}

		float angleOffset1 = gf_PI * 0.75f;
		float angleOffset2 = gf_PI * 2.25f;
		float angleDiff = angleOffset2 - angleOffset1;

		Vec3 minMarker(cosf(angleOffset1), sinf(angleOffset1), 0.f);
		Vec3 maxMarker(cosf(angleOffset2), sinf(angleOffset2), 0.f);

		float topSpeed = m_gears.curGear==SVehicleGears::kReverse ? -m_pSharedParams->handling.reverseSpeed : m_pSharedParams->handling.topSpeed;
		float speedNorm = angleOffset1 + clamp(speedMs/topSpeed, 0.f, 1.f) * angleDiff;
		float rpm = angleOffset1 + m_gears.curRpm * angleDiff;
		Vec3 speedDial(cosf(speedNorm), sinf(speedNorm), 0.f);
		Vec3 rpmDial(cosf(rpm), sinf(rpm), 0.f);

		pAuxGeom->DrawPolyline(circle1, 32, true, ColorB(255,255,255,255));
		pAuxGeom->DrawLine(centre1 + minMarker*0.9f*radius, colWhite, centre1 + minMarker*1.1f*radius, colWhite);
		pAuxGeom->DrawLine(centre1 + maxMarker*0.9f*radius, colWhite, centre1 + maxMarker*1.1f*radius, colWhite);
		pAuxGeom->DrawLine(centre1, colRed, centre1 + speedDial*radius*0.9f, colRed);

		pAuxGeom->DrawPolyline(circle2, 32, true, ColorB(255,255,255,255));
		pAuxGeom->DrawLine(centre2 + minMarker*0.9f*radius, colWhite, centre2 + minMarker*1.1f*radius, colWhite);
		pAuxGeom->DrawLine(centre2 + maxMarker*0.9f*radius, colWhite, centre2 + maxMarker*1.1f*radius, colWhite);
		pAuxGeom->DrawLine(centre2, colRed, centre2 + rpmDial*radius*0.9f, colRed);

		pRenderer->Draw2dLabel(centre1.x-radius*0.3f, centre1.y+radius*1.4f, 1.3f, color, false, "Speed");
		pRenderer->Draw2dLabel(centre2.x-radius*0.3f, centre2.y+radius*1.4f, 1.3f, color, false, "Rpm");

		pRenderer->Set2DMode(false, width, height);
	}

	pAuxGeom->SetRenderFlags(oldFlags);
}
#endif

void CVehicleMovementArcadeWheeled::GetCurrentWheelStatus(IPhysicalEntity* pPhysics)
{
	// Cache the current physics status once, rather than
	// getting it over-and-over again in every member function
	WriteLock lock(m_wheelStatusLock);
	const int numWheels = m_numWheels;
	for (int i=0; i<numWheels; i++)
	{
		m_wheelStatus[i].iWheel = i;
		if (pPhysics->GetStatus(&m_wheelStatus[i]) == 0)
		{
			new (&m_wheelStatus[i]) pe_status_wheel;
			m_wheelStatus[i].iWheel = i;
			m_wheelStatus[i].bContact = 0;
			m_wheelStatus[i].w = 0.f;
		}
	}
}


//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::Update(const float deltaTime)
{
	FUNCTION_PROFILER( GetISystem(), PROFILE_GAME );

	IEntity* pEntity = m_pVehicle->GetEntity();
	IPhysicalEntity* pPhysics = GetPhysics();
	if(!pPhysics)
	{
		assert(0 && "[CVehicleMovementArcadeWheeled::Update]: PhysicalEntity NULL!");
		return;
	}

	const SVehicleStatus& status = m_pVehicle->GetStatus();
	m_passengerCount = status.passengerCount;

	if (!pPhysics->GetStatus(&m_vehicleStatus))
		return;

	CVehicleMovementBase::Update(deltaTime);
	UpdateWaterLevels();
	UpdateSuspensionSound(deltaTime);
	UpdateBrakes(deltaTime);


	int notDistant = m_isProbablyDistant^1;

	if (gEnv->IsClient() && notDistant)
		UpdateSounds(deltaTime);

	if (notDistant && m_blownTires && m_carParams.steerTrackNeutralTurn == 0.f)
		m_tireBlownTimer += deltaTime;       

#if ENABLE_VEHICLE_DEBUG
	DebugDrawMovement(deltaTime);
#endif

	// update reversing
	if(notDistant && IsPowered() && m_actorId)
	{
		const float vel = m_localSpeed.y;

		if(vel < -0.1f && m_action.pedal < -0.1f)
		{
			if (m_reverseTimer == 0.f)
			{
				SVehicleEventParams params;
				params.bParam = true;
				m_pVehicle->BroadcastVehicleEvent(eVE_Reversing, params);
			}

			m_reverseTimer += deltaTime;
		}
		else
		{
			if(m_reverseTimer > 0.0f)
			{
				SVehicleEventParams params;
				params.bParam = false;
				m_pVehicle->BroadcastVehicleEvent(eVE_Reversing, params);
			}

			m_reverseTimer = 0.f;
		}
	}

	const SVehicleDamageParams& damageParams = m_pVehicle->GetDamageParams();
	m_submergedRatioMax = damageParams.submergedRatioMax;

	if (gEnv->bMultiplayer)
	{
		IActor* pActor = m_pVehicle->GetDriver();
		if (pActor)
		{
			m_netLerp = !pActor->IsClient();
		}
		else
		{
			m_netLerp = false;
		}
	}

	// Reset stationary handbrake?

	if(m_stationaryHandbrakeResetTimer > 0.0f)
	{
		m_stationaryHandbrakeResetTimer -= deltaTime;

		if(m_stationaryHandbrakeResetTimer <= 0.0f)
		{
			m_stationaryHandbrake = true;
		}
	}
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::UpdateSounds(const float deltaTime)
{ 
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_GAME );

	// update engine sound
	if (m_isEnginePowered && !m_isEngineGoingOff)
	{
		float rpmScale = clamp(m_gears.curRpm, m_carParams.engineMinRPM/m_carParams.engineMaxRPM, 1.f);


		if (m_vehicleStatus.bHandBrake)
		{
			m_rpmTarget = 0.f;
		}
		else if (m_rpmTarget)
		{
			Interpolate(rpmScale, m_rpmTarget, m_pSharedParams->rpmGearShiftSpeed, deltaTime);

			float diff = fabsf(rpmScale-m_rpmTarget);

			if (m_gearSoundPending && m_currentGear >= 3) // only from 1st gear upward
			{          
				if (diff < 0.5f*fabsf(m_rpmScalePrev-m_rpmTarget))
				{           
					GetOrPlaySound(eSID_Gear, 0.f, m_enginePos);          
					m_gearSoundPending = false;
				}
			}

			if (diff < 0.02)
			{
				m_rpmTarget = 0.f;        
			}
		}

		m_rpmScale = max(ms_engineSoundIdleRatio, rpmScale * (1.0f - ms_engineSoundOverRevRatio));

		m_rpmScale += (ms_engineSoundOverRevRatio / m_numWheels) * (m_numWheels - m_wheelContacts);

		SetSoundParam(eSID_Run, "rpm_scale", m_rpmScale);
		SetSoundParam(eSID_Ambience, "rpm_scale", m_rpmScale);

		if (m_currentGear != m_gears.curGear)
		{ 
			// when shifting up from 1st upward, set sound target to low rpm to simulate dropdown 
			// during clutch disengagement
			if (m_currentGear >= 2 && m_gears.curGear>m_currentGear)
			{
				m_rpmTarget = m_carParams.engineShiftDownRPM/m_carParams.engineMaxRPM;
				m_rpmScalePrev = rpmScale;

				if (DoGearSound())
				{
					m_gearSoundPending = true;        
				}
			}

			if (DoGearSound() && !m_rpmTarget && !(m_currentGear<=2 && m_gears.curGear<=2) && m_gears.curGear > m_currentGear)
			{
				// do gearshift sound only for gears higher than 1st forward
				// in case rpmTarget has been set, shift is played upon reaching it        
				GetOrPlaySound(eSID_Gear, 0.f, m_enginePos);        
			}

			m_currentGear = m_gears.curGear;
		}
	}

	//SetSoundParam(eSID_Run, "load", m_load);
	SetSoundParam(eSID_Run, "surface", m_surfaceSoundStats.surfaceParam);  
	//SetSoundParam(eSID_Run, "scratch", m_surfaceSoundStats.scratching);  // removed there is no "scratch" parameter in the run event [Tomas]


	float speed = m_vehicleStatus.vel.GetLength();
	float slipFraction = 0.0f;

	if(speed > 0.5f)
	{
		Vec3 velocityDir = m_vehicleStatus.vel.GetNormalized();
		Vec3 xDir = m_pEntity->GetWorldRotation().GetColumn0();

		float slipSpeedFraction = min(speed / m_maxSoundSlipSpeed, 1.0f);

		slipFraction = fabsf(xDir.dot(velocityDir) * slipSpeedFraction);
	}

	SetSoundParam(eSID_Run, "slip", slipFraction);

	// tire slip sound
	if (m_maxSoundSlipSpeed > 0.f)
	{
		ISound* pSound = GetSound(eSID_Slip);    

		if (m_surfaceSoundStats.slipRatio > 0.08f)
		{ 
			float slipTimerPrev = m_surfaceSoundStats.slipTimer;
			m_surfaceSoundStats.slipTimer += deltaTime;

			const static float slipSoundMinTime = 0.12f;
			if (!pSound && slipTimerPrev <= slipSoundMinTime && m_surfaceSoundStats.slipTimer > slipSoundMinTime)
			{
				pSound = PlaySound(eSID_Slip);
			}      
		}
		else if (m_surfaceSoundStats.slipRatio < 0.03f && m_surfaceSoundStats.slipTimer > 0.f)
		{
			m_surfaceSoundStats.slipTimer -= deltaTime;

			if (m_surfaceSoundStats.slipTimer <= 0.f)
			{
				StopSound(eSID_Slip);
				pSound = 0;
				m_surfaceSoundStats.slipTimer = 0.f;
			}      
		}

		if (pSound)
		{
			SetSoundParam(eSID_Slip, "slip_speed", m_surfaceSoundStats.slipRatio);
			SetSoundParam(eSID_Slip, "surface", m_surfaceSoundStats.surfaceParam);
			SetSoundParam(eSID_Slip, "scratch", (float)m_surfaceSoundStats.scratching);
			SetSoundParam(eSID_Slip, "in_out", m_soundStats.inout);
		}   
	}
}


//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::UpdateSuspension(const float deltaTime)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_GAME );

	float dt = max( deltaTime, 0.005f);

	IPhysicalEntity* pPhysics = GetPhysics();

	// update suspension and friction, if needed      
	bool bSuspUpdate = false;

	float diffSusp = m_pSharedParams->suspDampingMax - m_pSharedParams->suspDampingMin;    
	float diffStabi = m_pSharedParams->stabiMax - m_pSharedParams->stabiMin;

	if ((fabsf(diffSusp) + fabsf(diffStabi)) != 0.f) // if (diffSusp || diffStabi)
	{
		const float speed = m_PhysDyn.v.len();
		if (fabsf(m_speedSuspUpdated-speed) > 0.25f) // only update when speed changes
		{
			float maxSpeed = (float)__fsel(-m_pSharedParams->suspDampingMaxSpeed, 0.15f*m_maxSpeed, m_pSharedParams->suspDampingMaxSpeed);
			float speedNorm = min(1.f, speed/maxSpeed);

			if (diffSusp)
			{
				m_suspDamping = m_pSharedParams->suspDampingMin + (speedNorm * diffSusp);
				bSuspUpdate = true;
			}           

			if (diffStabi)
			{
				m_stabi = m_pSharedParams->stabiMin + (speedNorm * diffStabi);

				pe_params_car params;
				params.kStabilizer = m_stabi;
				pPhysics->SetParams(&params, 1);        
			}

			m_speedSuspUpdated = speed;    
		}
	}

	m_compressionMax = 0.f;
	int numRot = 0;  
	m_surfaceSoundStats.scratching = 0;

	ReadLock lock(m_wheelStatusLock);

	const int numWheels = m_numWheels;
	for (int i=0; i<numWheels; ++i)
	{ 
		pe_params_wheel wheelParams;
		bool bUpdate = bSuspUpdate;
		IVehicleWheel* pWheel = m_wheels[i].wheelPart->GetIWheel();

		const pe_status_wheel &ws = m_wheelStatus[i];

		numRot += ws.bContact;

		if (bSuspUpdate)
		{
			wheelParams.iWheel = i;      
			wheelParams.kDamping = m_suspDamping;
			pPhysics->SetParams(&wheelParams, THREAD_SAFE);
		}

		// check for hard bump
		int notDistantAndVisible = m_isProbablyVisible & (m_isProbablyDistant^1);
		if (notDistantAndVisible && (m_pSharedParams->bumpMinSusp + m_pSharedParams->bumpMinSpeed > 0.f) && m_lastBump > 1.f && ws.suspLen0 > 0.01f && ws.suspLen < ws.suspLen0)
		{ 
			// compression as fraction of relaxed length over time
			m_wheels[i].compression = ((m_wheels[i].suspLen-ws.suspLen)/ws.suspLen0) / dt;
			m_compressionMax = max(m_compressionMax, m_wheels[i].compression);
		}
		m_wheels[i].suspLen = ws.suspLen;
	}  

	m_wheelContacts = numRot;
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::UpdateBrakes(const float deltaTime)
{
	if (m_movementAction.brake || m_pVehicle->GetStatus().health <= 0.f)
		m_action.bHandBrake = 1;
	else
		m_action.bHandBrake = 0;

	if (m_pSharedParams->isBreakingOnIdle && m_movementAction.power == 0.0f)
	{
		m_action.bHandBrake = 1;
	}

	// brake lights should come on if... (+ engine is on, and actor is driving)
	//	- handbrake is activated 
	//	- pedal is pressed, and the vehicle is moving in the opposite direction
	if (IsPowered() && m_actorId )
	{
		const float forwardSpeed = m_localSpeed.y;

		if ((fabsf(m_action.pedal) > 0.1f && fabsf(forwardSpeed) > 0.1f && (forwardSpeed*m_action.pedal) < 0.f)
			|| m_action.bHandBrake)
		{
			if (m_brakeTimer == 0.f)
			{
				SVehicleEventParams params;
				params.bParam = true;
				m_pVehicle->BroadcastVehicleEvent(eVE_Brake, params);
			}

			m_brakeTimer += deltaTime;  
		}
		else
		{
			if (m_brakeTimer > 0.f)
			{
				SVehicleEventParams params;
				params.bParam = false;
				m_pVehicle->BroadcastVehicleEvent(eVE_Brake, params);

				// airbrake sound
				if (m_pSharedParams->airbrakeTime > 0.f)
				{ 
					if (m_brakeTimer > m_pSharedParams->airbrakeTime)
					{
						char name[256];
						_snprintf(name, sizeof(name), "sounds/vehicles:%s:airbrake", m_pVehicle->GetEntity()->GetClass()->GetName());
						name[sizeof(name)-1] = '\0';
						m_pEntitySoundsProxy->PlaySound(name, Vec3(0), FORWARD_DIRECTION, FLAG_SOUND_DEFAULT_3D, eSoundSemantic_Vehicle);                
					}          
				}  
			}

			m_brakeTimer = 0.f;  
		}
	}
}

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::UpdateSuspensionSound(const float deltaTime)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_GAME );

	if (m_pVehicle->GetStatus().health <= 0.f)
		return;

	if (m_isProbablyDistant | (m_isProbablyVisible^1))
		return;

	ReadLock lock(m_wheelStatusLock);

	const float speed = m_speed;
	const int numWheels = m_numWheels;

	int soundMatId = 0;

	m_lostContactTimer += deltaTime;
	for (int i=0; i<numWheels; ++i)
	{
		const pe_status_wheel* ws = &m_wheelStatus[i];

		if (ws->bContact)
		{ 
			// sound-related                   
			if (!m_surfaceSoundStats.scratching && soundMatId==0 && speed > 0.001f)
			{
				if(m_wheels[i].waterLevel > (ws->ptContact.z + 0.02f))
				{        
					soundMatId = gEnv->pPhysicalWorld->GetWaterMat();
					m_lostContactTimer = 0;
				}
				else if (ws->contactSurfaceIdx > 0 /*&& soundMatId != gEnv->pPhysicalWorld->GetWaterMat()*/)
				{   
					if (m_wheels[i].wheelPart->GetState() == IVehiclePart::eVGS_Damaged1)
						m_surfaceSoundStats.scratching = 1;

					soundMatId = ws->contactSurfaceIdx;
					m_lostContactTimer = 0;
				}
			}      
		}
	}

	m_lastBump += deltaTime;
	if (m_pVehicle->GetStatus().speed > m_pSharedParams->bumpMinSpeed && m_lastBump > 1.f)
	{ 
		if (m_compressionMax > m_pSharedParams->bumpMinSusp)
		{
			// do bump sound        
			if (ISound* pSound = PlaySound(eSID_Bump))
			{
				pSound->SetParam("speed", ms_engineSoundIdleRatio + (1.f-ms_engineSoundIdleRatio)*m_speedRatio, false);
				pSound->SetParam("intensity", min(1.f, m_pSharedParams->bumpIntensityMult*m_compressionMax/m_pSharedParams->bumpMinSusp), false);
				m_lastBump = 0;
			}      
		}            
	}   

	// set surface sound type
	if (soundMatId != m_surfaceSoundStats.matId)
	{ 
		if (m_lostContactTimer == 0.f || m_lostContactTimer > 3.f)
		{
			if (soundMatId > 0)
			{
				m_surfaceSoundStats.surfaceParam = GetSurfaceSoundParam(soundMatId);
			}    
			else
			{
				m_surfaceSoundStats.surfaceParam = 0.f;      
			}   
			m_surfaceSoundStats.matId = soundMatId;
		}
	} 

}


//////////////////////////////////////////////////////////////////////////
// NOTE: This function must be thread-safe. Before adding stuff contact MarcoC.
void CVehicleMovementArcadeWheeled::ProcessAI(const float deltaTime)
{
	FUNCTION_PROFILER( GetISystem(), PROFILE_GAME );

	float dt = max( deltaTime,0.005f);

	m_movementAction.brake = false;
	m_movementAction.rotateYaw = 0.0f;
	m_movementAction.power = 0.0f;

	float inputSpeed = 0.0f;
	{
		if (m_aiRequest.HasDesiredSpeed())
			inputSpeed = m_aiRequest.GetDesiredSpeed();
		Limit(inputSpeed, -m_maxSpeed, m_maxSpeed);
	}

	Vec3 vMove(ZERO);
	{
		if (m_aiRequest.HasMoveTarget())
			vMove = ( m_aiRequest.GetMoveTarget() - m_PhysPos.pos ).GetNormalizedSafe();
	}

	//start calculation
	if ( fabsf( inputSpeed ) < 0.0001f || m_tireBlownTimer > 1.5f )
	{
		m_movementAction.brake = true;
	}
	else
	{

		Matrix33 entRotMatInvert( m_PhysPos.q );
		entRotMatInvert.Invert();
		float currentAngleSpeed = RAD2DEG(-m_PhysDyn.w.z);

		const float maxSteer = RAD2DEG(gf_PI/4.f); // fix maxsteer, shouldn't change  
		Vec3 vVel = m_PhysDyn.v;
		Vec3 vVelR = entRotMatInvert * vVel;
		float currentSpeed =vVel.GetLength();
		vVelR.NormalizeSafe();
		if ( vVelR.Dot( FORWARD_DIRECTION ) < 0 )
			currentSpeed *= -1.0f;

		// calculate pedal
		const float accScale = 0.5f;
		m_movementAction.power = (inputSpeed - currentSpeed) * accScale;
		Limit( m_movementAction.power, -1.0f, 1.0f);

		// calculate angles
		Vec3 vMoveR = entRotMatInvert * vMove;
		Vec3 vFwd	= FORWARD_DIRECTION;

		vMoveR.z =0.0f;
		vMoveR.NormalizeSafe();

		float cosAngle = vFwd.Dot(vMoveR);
		float angle = RAD2DEG( acos_tpl(cosAngle));
		if ( vMoveR.Dot( Vec3( 1.0f,0.0f,0.0f ) )< 0 )
			angle = -angle;

		//		int step =0;
		m_movementAction.rotateYaw = angle * 1.75f/ maxSteer; 

		// implementation 1. if there is enough angle speed, we don't need to steer more
		if ( fabsf(currentAngleSpeed) > fabsf(angle) && angle*currentAngleSpeed > 0.0f )
		{
			m_movementAction.rotateYaw = m_action.steer*0.995f; 
			//			step =1;
		}

		// implementation 2. if we can guess we reach the distination angle soon, start counter steer.
		float predictDelta = inputSpeed < 0.0f ? 0.1f : 0.07f;
		float dict = angle + predictDelta * ( angle - m_prevAngle) / dt ;
		if ( dict*currentAngleSpeed<0.0f )
		{
			if ( fabsf( angle ) < 2.0f )
			{
				m_movementAction.rotateYaw = angle* 1.75f/ maxSteer;
				//				step =3;
			}
			else
			{
				m_movementAction.rotateYaw = currentAngleSpeed < 0.0f ? 1.0f : -1.0f; 
				//				step =2;
			}
		}

		if ( fabsf( angle ) > 20.0f && currentSpeed > 7.0f ) 
		{
			m_movementAction.power *= 0.0f ;
			//			step =4;
		}

		// for more tight condition to make curve.
		if ( fabsf( angle ) > 40.0f && currentSpeed > 3.0f ) 
		{
			m_movementAction.power *= 0.0f ;
			//			step =5;
		}

		//		m_prevAngle =  angle;
		//		char buf[1024];
		///		sprintf(buf, "steering	%4.2f	%4.2f %4.2f	%4.2f	%4.2f	%4.2f	%d\n", deltaTime,currentSpeed,angle,currentAngleSpeed, m_movementAction.rotateYaw,currentAngleSpeed-m_prevAngle,step);
		//		OutputDebugString( buf );
	}

}

//////////////////////////////////////////////////////////////////////////
// NOTE: This function must be thread-safe. Before adding stuff contact MarcoC.
void CVehicleMovementArcadeWheeled::ProcessMovement(const float deltaTime)
{
	FUNCTION_PROFILER( GetISystem(), PROFILE_GAME );

	// m_netActionSync.UpdateObject(this);

	CryAutoCriticalSection lk(m_lock);

	CVehicleMovementBase::ProcessMovement(deltaTime);

	IPhysicalEntity* pPhysics = GetPhysics();

	NETINPUT_TRACE(m_pVehicle->GetEntityId(), m_action.pedal);

	float speed = m_PhysDyn.v.len();
	bool isDestroyed = m_pVehicle->IsDestroyed();

	bool canApplyAction = !(gEnv->bMultiplayer && m_netLerp);	// Dont apply an Action unless this is a player controlled vehicle

	bool isDriverHidden = false;
	bool isAIDisabled = false;
	if (IEntity* pDriver = gEnv->pEntitySystem->GetEntity(m_actorId))
	{
		isDriverHidden = pDriver->IsHidden();
		if (m_movementAction.isAI)
		{
			IAIObject* pAI = pDriver->GetAI();
			isAIDisabled = pAI ? !pAI->IsEnabled() : true;
		}
	}

	if (isAIDisabled || isDriverHidden || !(m_actorId && m_isEnginePowered) || isDestroyed )
	{
		const float sleepTime = 3.0f;

		if ( m_passengerCount > 0 || ( isDestroyed && m_bForceSleep == false ))
		{
			UpdateSuspension(deltaTime);
		}

		if (m_frictionState!=k_frictionUseLowLevel)
			EnableLowLevelPhysics(k_frictionUseLowLevel, THREAD_SAFE);

		if (canApplyAction)
		{
			m_action.bHandBrake = m_stationaryHandbrake;
			m_action.pedal = 0;
			pPhysics->Action(&m_action, THREAD_SAFE);
		}

		bool maybeInTheAir = fabsf(m_PhysDyn.v.z) > 1.0f;
		if ( maybeInTheAir )
		{
			UpdateGravity(-9.81f * 1.4f);
			ApplyAirDamp(DEG2RAD(20.f), DEG2RAD(10.f), deltaTime, THREAD_SAFE);
		}

		if ( isDestroyed && m_bForceSleep == false )
		{
			int numContact= 0;
			const int numWheels = m_numWheels;
			for (int i=0; i<numWheels; ++i)
			{
				if (m_wheelStatus[i].bContact)
					numContact ++;
			}
			if ( numContact > numWheels/2 || speed<0.2f )
				m_forceSleepTimer += deltaTime;
			else
				m_forceSleepTimer = max(0.0f,m_forceSleepTimer-deltaTime);

			if ( m_forceSleepTimer > sleepTime )
			{
				pe_params_car params;
				params.minEnergy = 0.05f;
				pPhysics->SetParams(&params, 1);
				m_bForceSleep = true;
			}
		}
		return;

	}

	// Reset stationary handbrake.
	m_stationaryHandbrake = true;

	// moved to main thread
	UpdateSuspension(deltaTime);   	
	//UpdateBrakes(deltaTime);

	float damageMul = 1.0f - 0.7f*m_damage;  
	bool bInWater = m_PhysDyn.submergedFraction > 0.01f;
	if (canApplyAction)
	{
		pe_action_drive prevAction = m_action; 

		// speed ratio    
		float speedRel = min(speed, m_pSharedParams->vMaxSteerMax) / m_pSharedParams->vMaxSteerMax;  

		// reduce actual pedal with increasing steering and velocity
		float maxPedal = 1 - (speedRel * fabsf(m_movementAction.rotateYaw) * m_pSharedParams->pedalLimitMax);  
		float submergeMul = 1.0f;  
		float totalMul = 1.0f;  
		if ( GetMovementType()!=IVehicleMovement::eVMT_Amphibious && bInWater )
		{
			submergeMul = max( 0.0f, 0.04f - m_PhysDyn.submergedFraction ) * 10.0f;
			submergeMul *=submergeMul;
			submergeMul = max( 0.2f, submergeMul );
		}

		totalMul = max( 0.3f, damageMul *  submergeMul );
		m_action.pedal = clamp(m_movementAction.power, -maxPedal, maxPedal ) * totalMul;

		// make sure cars can't drive under water
		if(GetMovementType()!=IVehicleMovement::eVMT_Amphibious && m_PhysDyn.submergedFraction >= m_submergedRatioMax && m_damage >= 0.99f)
		{
			m_action.pedal = 0.0f;
		}

		m_action.steer = CalcSteering(m_action.steer, speedRel, m_movementAction.rotateYaw, deltaTime);

		if ((!is_unused(m_action.pedal) && !is_unused(prevAction.pedal) && abs(m_action.pedal-prevAction.pedal)>FLT_EPSILON) || 
			(!is_unused(m_action.steer) && !is_unused(prevAction.steer) && abs(m_action.steer-prevAction.steer)>FLT_EPSILON) || 
			(!is_unused(m_action.clutch) && !is_unused(prevAction.clutch) && abs(m_action.clutch-prevAction.clutch)>FLT_EPSILON) || 
			m_action.bHandBrake != prevAction.bHandBrake || 
			m_action.iGear != prevAction.iGear)
		{
			pPhysics->Action(&m_action, THREAD_SAFE);
		}
	}

	Vec3 vUp(m_PhysPos.q.GetColumn2());
	Vec3 vUnitUp(0.0f,0.0f,1.0f);

	float slopedot = vUp.Dot( vUnitUp );
	bool bSteep =  fabsf(slopedot) < 0.7f;
	{ //fix for 30911
		if ( bSteep && speed > 7.5f )
		{
			Vec3 vVelNorm = m_PhysDyn.v.GetNormalizedSafe();
			if ( vVelNorm.Dot(vUnitUp)> 0.0f )
			{
				pe_action_impulse imp;
				imp.impulse = -m_PhysDyn.v;
				imp.impulse *= deltaTime * m_PhysDyn.mass*5.0f;      
				imp.point = m_PhysDyn.centerOfMass;
				imp.iApplyTime = 0;
				GetPhysics()->Action(&imp, THREAD_SAFE);
			}
		}
	}

	if ( !bSteep && Boosting() )
		ApplyBoost(speed, 1.25f*m_maxSpeed*GetWheelCondition()*damageMul, m_boostStrength, deltaTime);  

	if (m_numWheels)
	{
		if (m_frictionState!=k_frictionUseHiLevel)
			EnableLowLevelPhysics(k_frictionUseHiLevel, THREAD_SAFE);
		InternalPhysicsTick(deltaTime);
	}

	if (m_wheelContacts <= 1 && speed > 5.f)
	{
		ApplyAirDamp(DEG2RAD(20.f), DEG2RAD(10.f), deltaTime, THREAD_SAFE);
		if ( !bInWater )
			UpdateGravity(-9.81f * 1.4f);  
	}

	EjectionTest(deltaTime);

	//if (m_netActionSync.PublishActions( CNetworkMovementArcadeWheeled(this) ))
	//	CHANGED_NETWORK_STATE(m_pVehicle,  eEA_GameClientDynamic );
}

void CVehicleMovementArcadeWheeled::EnableLowLevelPhysics(int state, int bThreadSafe)
{
	IPhysicalEntity* pPhysics = GetPhysics();
	assert(pPhysics);
	if(pPhysics)
	{
		pe_params_car carParams;
		pe_params_wheel wheelParams;
		const int numWheels = m_numWheels;

		float kLatFriction0 = 1.f;
		float kLatFriction = 0.f;

		if (state==k_frictionUseLowLevel)
		{
			const float friction = 1.f;
			kLatFriction = 1.f;
			carParams.minBrakingFriction = friction;
			carParams.maxBrakingFriction = friction;
			carParams.kDynFriction = friction;
			wheelParams.minFriction = friction;
			wheelParams.maxFriction = friction;
			wheelParams.bCanBrake = 1;
			wheelParams.bDriving = 0;
		}
		else // Assume Hi Level, game-side friction
		{
			carParams.minBrakingFriction = 0.f;
			carParams.maxBrakingFriction = 0.f;
			carParams.kDynFriction = 0.f;
			wheelParams.minFriction = 0.f;
			wheelParams.maxFriction = 0.f;
			wheelParams.bCanBrake = 0;
			wheelParams.bDriving = 0;
		}

		pPhysics->SetParams(&carParams, bThreadSafe);

		assert(state!=k_frictionNotSet);
		m_frictionState = state;
	}
}

static inline void SolveFriction(Vec3& dVel, Vec3& dAngVel, const Vec3& vel, const Vec3& angVel, SVehicleChassis* c, SVehicleWheel* w, ClampedImpulse* maxTractionImpulse, ClampedImpulse* maxLateralImpulse, float solverERP, float dt)
{
	Vec3 wheelVel = vel + angVel.cross(w->worldOffset);

	// Inline
	if (!w->locked)
	{
		float slipSpeed = -w->w * w->radius + wheelVel.dot(w->frictionDir[0]);
		float denom = w->invMass + w->radius * w->radius * w->invInertia;
		float impulse = clampedImpulseApply(maxTractionImpulse, solverERP * slipSpeed / denom);
		w->w += impulse * w->radius * w->invInertia;
		float velChange = - impulse * w->invMass;	// This is the vel change imparted on just the wheel
		// Bring velChange back to zero by applying impulse to chassis
		denom = w->invMass + computeDenominator(c->invMass, c->invInertia, w->worldOffset, w->frictionDir[0]);
		impulse = velChange / denom;
		addImpulseAtOffset(dVel, dAngVel, c->invMass, c->invInertia, w->worldOffset, impulse * w->frictionDir[0]);
	}
	else
	{
		float slipSpeed = wheelVel.dot(w->frictionDir[0]);
		float denom = computeDenominator(c->invMass, c->invInertia, w->worldOffset, w->frictionDir[0]);
		float impulse = clampedImpulseApply(maxTractionImpulse, -solverERP*slipSpeed / denom);
		addImpulseAtOffset(dVel, dAngVel, c->invMass, c->invInertia, w->worldOffset, impulse * w->frictionDir[0]);
	}

	// Lateral
	{
		float errorV = wheelVel.dot(w->frictionDir[1]);
		float denom = computeDenominator(c->invMass, c->invInertia, w->worldOffset, w->frictionDir[1]);
		float impulse0 = -solverERP * errorV / denom;
		float impulse = clampedImpulseApply(maxLateralImpulse, impulse0);
		Vec3 impulseV = impulse * w->frictionDir[1];
		addImpulseAtOffset(dVel, dAngVel, c->invMass, c->invInertia, w->worldOffset, impulseV);
	}
}

float CVehicleMovementArcadeWheeled::CalcSteering(float steer, float speedRel, float rotateYaw, float dt)
{
	float steerMax = GetMaxSteer(speedRel);
	float steeringFrac = rotateYaw;    
	float steerTarget = steeringFrac * steerMax;
	steerTarget = (float)__fsel(fabsf(steerTarget)-0.01f, steerTarget, 0.f);
	float steerError = steerTarget - steer;

	if ((steerError*steer) < 0.f)
	{
		// Decreasing, therefore use steerRelaxation speed
		// to relax towards central position, and calc any remaining dt
		float absSteer = fabsf(steer);
		float correction = dt * DEG2RAD(m_pSharedParams->steerRelaxation);
		absSteer = absSteer - correction;
		dt = (float)__fsel(absSteer, 0.f, absSteer/DEG2RAD(m_pSharedParams->steerRelaxation));	// Calculate any overshoot in dt
		steer = (float)__fsel(absSteer, (float)__fsel(steer, +absSteer, -absSteer), 0.f);
		steerError = steerTarget - steer;
	}

	if (dt>0.f)
	{
		float steerSpeed = DEG2RAD(GetSteerSpeed(speedRel));
		steer = steer + fsgnf(steerError) * steerSpeed * dt;  
		steer = clamp(steer, -steerMax, steerMax);
	}

	return steer;
}

void CVehicleMovementArcadeWheeled::InternalPhysicsTick(float dt)
{
	IPhysicalEntity* pPhysics = GetPhysics();

	GetCurrentWheelStatus(pPhysics);

	const int numWheels = m_numWheels;

	SVehicleChassis* c = &m_chassis;

	Matrix33 bodyRot( m_PhysPos.q );
	const Vec3 xAxis = bodyRot.GetColumn0();
	const Vec3 yAxis = bodyRot.GetColumn1();
	const Vec3 zAxis = bodyRot.GetColumn2();
	const Vec3 pos = m_PhysDyn.centerOfMass;

	Vec3 vel = m_PhysDyn.v;
	Vec3 angVel = m_PhysDyn.w;

#if ENABLE_VEHICLE_DEBUG
	DebugCheat(dt);
#endif

	//=============================================
	// Collision Mitigation 
	//=============================================
#define SQR(x) ((x)*(x))
	if(1)
	{
		static float t1 = 1.0f;
		static float t2 = 1.0f;
		float invDt2 = 1.f/(dt*dt);
		Vec3 change = vel - m_chassis.vel;
		float vs = change.GetLengthSquared();
		Vec3 change2 = angVel - m_chassis.angVel;
		float vs2 = change.GetLengthSquared();
		m_chassis.collision1 = vs>(SQR(t1));
		m_chassis.collision2 = vs2>(SQR(t2));

		if (m_chassis.collision1 || m_chassis.collision2)
		{
			angVel = (m_chassis.angVel*0.7f) + (angVel*0.3f);
		}
	}

	//=============================================
	// Get Wheel Status
	//=============================================

	int contactMap[2][2]={{0,0},{0,0}};
	int numContacts = 0;
	assert(numWheels <= maxWheels);
	const float invNumWheels = m_invNumWheels;
	Vec3 contacts[maxWheels];
	float suspensionExtension = 0.f;
	float suspensionVelocity = 0.f;

	for (int i=0; i<numWheels; ++i)
	{ 
		SVehicleWheel* w = &m_wheels[i];
		pe_status_wheel* ws = &m_wheelStatus[i];
		if (ws->bContact)
		{
			contacts[i] = ws->ptContact;
			w->contact = 1.f;
			contactMap[w->axleIndex][w->offset.x>0.f]=1;
		}
		else
		{
			// Pretend that the contact was at the bottom of the wheel
			Vec3 bottom = w->offset;
			bottom.z += w->bottomOffset + ws->suspLen0 - ws->suspLen;
			contacts[i] = pos + bodyRot * bottom;
			w->contact = 0.f;
		}
		if (ws->suspLenFull != 0)
		{
			float extension = (ws->suspLen - ws->suspLen0)/ws->suspLenFull;
			//if (extension > 0.f) extension = 0.f;
			extension = (float)__fsel(extension, 0.f, extension);
			suspensionExtension += extension;
		}
	}
	suspensionExtension *= invNumWheels;

	numContacts = contactMap[0][0] + contactMap[0][1] + contactMap[1][0] + contactMap[1][1];

	//==============================================
	// Scale friction and traction by the average 
	// suspension compression and velocity
	// BUT only for handbrake
	//==============================================
	if (m_action.bHandBrake)
	{
		m_handling.compressionScale = (1.f - m_pSharedParams->handling.compressionBoostHandBrake * suspensionExtension);
	}
	else
	{
		m_handling.compressionScale = (1.f - m_pSharedParams->handling.compressionBoost * suspensionExtension);
	}
	Limit(m_handling.compressionScale, 1.0f, 10.f);

	// Work out the base contact normal
	// This is somewhat faked, but works well enough
	if (numWheels>=4)
	{
		const int c0 = c->contactIdx0;
		const int c1 = c->contactIdx1;
		const int c2 = c->contactIdx2;
		const int c3 = c->contactIdx3;
		m_handling.contactNormal = (contacts[c3] - contacts[c0]).cross(contacts[c2] - contacts[c1]);
		m_handling.contactNormal.normalize();
	}
	else
	{
		m_handling.contactNormal = zAxis;
	}

	float speed = yAxis.dot(vel);
	float absSpeed = fabsf(speed);
	float angSpeed = m_handling.contactNormal.dot(angVel); 

	//=============================================
	// Calculate invR from the steering wheels
	//=============================================
	float steering = m_action.steer / DEG2RAD(m_steerMax);
	m_invTurningRadius = 0.f;
	for(int i=0; i<numWheels; i++)
	{
		float alpha = m_wheelStatus[i].steer;
		if ((fabsf(m_wheels[i].offset.y) > 0.01f) && (fabsf(alpha) > 0.01f))
		{
			// NB: this is a signed quantity
			m_invTurningRadius -= tanf(alpha) / m_wheels[i].offset.y;
		}
	}
	m_invTurningRadius *= invNumWheels;

	//=============================================
	// Speed Reduction from steering
	//=============================================
	float topSpeed = (float)__fsel(speed, m_pSharedParams->handling.topSpeed, m_pSharedParams->handling.reverseSpeed);
	float scale = m_action.bHandBrake ? 1.f : 1.f - (m_pSharedParams->handling.reductionAmount * fabsf(steering));// * approxExp(m_pSharedParams->handling.reductionRate*dt);
	topSpeed = topSpeed * scale;
	float throttle = m_isEnginePowered ? m_movementAction.power * scale : 0.f;

	topSpeed = topSpeed * m_damageRPMScale;
	throttle = throttle * (0.7f + 0.3f * m_damageRPMScale);

	//=============================================
	// PowerSlide
	//=============================================
	float chassisInvInertia = c->invInertia;


























	{
		m_handling.canPowerSlide = false;
		m_handling.powerSlideDir = 0.f;
	}

	//===============================================
	// Process Lateral and Traction Friction
	//===============================================
	{
		const float gravity = 9.8f;
		ClampedImpulse maxTractionImpulse[maxWheels];
		ClampedImpulse maxLateralImpulse[maxWheels];

		bool lockAllWheels = (absSpeed > 2.f) && ((speed * throttle) < 0.f);			// When throttle is opposite to current speed
		bool canDeccelerate = (absSpeed > (m_pSharedParams->handling.decceleration*dt));
		const float speedNorm = speed/topSpeed;
		const float accelerationMultiplier = m_pSharedParams->handling.accelMultiplier1 + (m_pSharedParams->handling.accelMultiplier2 - m_pSharedParams->handling.accelMultiplier1)*speedNorm;
		float forcePerWheel = m_pSharedParams->handling.acceleration * m_chassis.mass * invNumWheels;	                // Assume all wheels are powered
		float forcePerWheel2 = m_pSharedParams->handling.decceleration * m_chassis.mass * invNumWheels;	// Assume all wheels are powered
		float handBrakeForce;

		forcePerWheel *= m_damageRPMScale;
		forcePerWheel2 *= m_damageRPMScale;

		if (m_action.bHandBrake && fabsf(throttle)>0.1f)
		{
			handBrakeForce = m_pSharedParams->handling.handBrakeDeccelerationPowerLock * m_chassis.mass;
			handBrakeForce *= m_damageRPMScale;
		}
		else
		{
			handBrakeForce = m_pSharedParams->handling.handBrakeDecceleration * m_chassis.mass;
			handBrakeForce *= m_damageRPMScale;
		}

		int numWheelsThatCanHadnBrake = 0;

		// Prepare
		float contact = 0.f;
		float avWheelSpeed = 0.f;
		for (int i=0; i<numWheels; i++)
		{
			SVehicleWheel* w = &m_wheels[i];
			avWheelSpeed += w->w;
			//w->contact += (1.f - w->contact) * approxExp(dt);
			if (w->bCanLock || lockAllWheels) numWheelsThatCanHadnBrake++;
		}

		contact = (float)numContacts * 0.25f;
		avWheelSpeed *= invNumWheels;

		TickGears(dt, avWheelSpeed, throttle, speed);

		if (numWheelsThatCanHadnBrake) handBrakeForce /= (float)numWheelsThatCanHadnBrake;


		for (int i=0; i<numWheels; i++)
		{
			SVehicleWheel* w = &m_wheels[i];

			w->contactNormal = m_handling.contactNormal;
			if (m_wheelStatus[i].bContact) w->contactNormal = w->contactNormal + m_wheelStatus[i].normContact;
			//w->contactNormal = w->contactNormal - axis*(axis.dot(w->contactNormal));
			w->contactNormal.normalize();

			w->worldOffset = bodyRot * w->offset;
			Vec3 axis = (xAxis * cosf(m_wheelStatus[i].steer)) - (yAxis * sinf(m_wheelStatus[i].steer));

			// Calc inline and lateral direction
			w->frictionDir[0] = (w->contactNormal.cross(axis)).normalize();
			w->frictionDir[1] = (w->frictionDir[0].cross(w->contactNormal)).normalize(); //axis;

			Vec3 wheelVel = vel + angVel.cross(w->worldOffset);
			w->slipSpeed = fabsf(wheelVel.dot(w->frictionDir[0]) - w->w*w->radius);

			if (lockAllWheels || (m_action.bHandBrake & w->bCanLock))
			{
				float frictionImpulse = handBrakeForce * dt;
				clampedImpulseInit(&maxTractionImpulse[i], -frictionImpulse, frictionImpulse);
				w->locked = true;
				w->w = 0.f;
			}
			else
			{
				float minFrictionImpulse = m_chassis.mass * gravity * dt * invNumWheels;
				float frictionImpulse = forcePerWheel * dt;
				frictionImpulse = max(frictionImpulse, minFrictionImpulse);

				// Grip based on slip speed (workaround)
				const float grip = m_pSharedParams->handling.grip1 + (m_pSharedParams->handling.grip2 - m_pSharedParams->handling.grip1) * approxOneExp(w->slipSpeed * m_pSharedParams->handling.gripK);
				frictionImpulse *= grip;
				clampedImpulseInit(&maxTractionImpulse[i], -frictionImpulse, frictionImpulse);

				w->locked = false;
				if (fabsf(throttle) > 0.05f)
				{
					if ((throttle*w->w>0.f) && (throttle*w->w < throttle*w->lastW))
					{
						// Resist the terrain from decreasing the speed of the wheels
						w->w+=(w->w-w->lastW)*approxOneExp(dt);
					}
					float dw = contact*throttle * w->radius * accelerationMultiplier * forcePerWheel * dt * w->invInertia;
					w->lastW = w->w;
					w->w += dw;
				}
				else
				{
					if (canDeccelerate)
					{
						float dw = fsgnf(speed) * w->radius * forcePerWheel2 * dt * w->invInertia;
						w->w = (float)__fsel(fabsf(w->w)-fabsf(dw), w->w-dw, 0.f);
					}
					w->w *= 0.9f;
				}

				if ((w->w * w->radius) > m_pSharedParams->handling.topSpeed)
				{
					float target = m_pSharedParams->handling.topSpeed / w->radius;
					w->w = target;
				}
				else if ((w->w * w->radius) > topSpeed)
				{
					float target = topSpeed / w->radius;
					w->w += (target - w->w) * approxOneExp(m_pSharedParams->handling.reductionRate*dt);
				}

				else if ((w->w * w->radius) < -m_pSharedParams->handling.reverseSpeed)
				{
					float target = -m_pSharedParams->handling.reverseSpeed / w->radius;
					w->w = target;
				}
			}

			maxTractionImpulse[i].min *= m_handling.compressionScale * contact;
			maxTractionImpulse[i].max *= m_handling.compressionScale * contact;

			// Lateral Friction
			{
				float friction = w->axleIndex==0 ? m_pSharedParams->handling.backFriction : m_pSharedParams->handling.frontFriction;
				float frictionImpulse = friction * m_chassis.mass * gravity * invNumWheels * dt;
				if (m_action.bHandBrake & w->bCanLock)
				{
					if (w->axleIndex==0)
					{
						frictionImpulse *= m_pSharedParams->handling.handBrakeBackFrictionScale;
					}
					else
					{
						frictionImpulse *= m_pSharedParams->handling.handBrakeFrontFrictionScale;
					}
				}

				clampedImpulseInit(&maxLateralImpulse[i], -frictionImpulse, frictionImpulse);
			}

			maxLateralImpulse[i].min *= m_handling.compressionScale * contact;
			maxLateralImpulse[i].max *= m_handling.compressionScale * contact;
		}

		const int numIterations = 4;
		float solverERP = invNumWheels;
		float erpChange = (1.f - solverERP)/(float)(numIterations-1);

		// Keep track of lateral friction impulses
		Vec3 appliedImpulse[2]		= { Vec3Constants<float>::fVec3_Zero, Vec3Constants<float>::fVec3_Zero };
		Vec3 appliedAngImpulse[2] = { Vec3Constants<float>::fVec3_Zero, Vec3Constants<float>::fVec3_Zero };


		if (contact > 0.f)
		{
			// Iterate
			Vec3 dVel;
			Vec3 dAngVel;
			dVel.zero();
			dAngVel.zero();
			// First pass is explicit, velocity is only added to the chassis after solving all wheels
			for (int i=0; i<numWheels; i++)
			{
				SVehicleWheel* w = &m_wheels[i];
				SolveFriction(dVel, dAngVel, vel, angVel, &m_chassis, w, &maxTractionImpulse[i], &maxLateralImpulse[i], solverERP, dt);
			}
			vel = vel + dVel;
			angVel = angVel + dAngVel;
			solverERP = solverERP + erpChange;

			for (int repeat=1; repeat<numIterations; repeat++)
			{
				for (int i=0; i<numWheels; i++)
				{
					SVehicleWheel* w = &m_wheels[i];
					dVel.zero();
					dAngVel.zero();
					SolveFriction(dVel, dAngVel, vel, angVel, &m_chassis, w, &maxTractionImpulse[i], &maxLateralImpulse[i], solverERP, dt);
					vel = vel + dVel;
					angVel = angVel + dAngVel;
				}
				solverERP = solverERP + erpChange;
			}
		}

		//===============================
		// Set the low level wheel speeds
		//===============================
		for (int i=0; i<numWheels; i++)
		{
			if (abs(m_wheels[i].w)<FLT_EPSILON)
				continue; 
			pe_params_wheel wp;
			wp.iWheel = i;
			wp.w = m_wheels[i].w;
			pPhysics->SetParams(&wp, 1);
		}
	}

	//===============================
	// InvR Correction
	//===============================
	if ((numContacts>=3) && (m_handling.canPowerSlide==false))
	{
		float target = 1.f;
		float angSpring0 = m_pSharedParams->correction.angSpring;
		float lateralSpring0 = m_pSharedParams->correction.lateralSpring;
		if (m_action.bHandBrake)
		{
			angSpring0 *= m_pSharedParams->handling.handBrakeAngCorrectionScale;
			lateralSpring0 *= m_pSharedParams->handling.handBrakeLateralCorrectionScale;
		}
		float angularCorrection = speed*target*m_invTurningRadius - angSpeed;
		float angSpring = approxOneExp(dt*angSpring0);
		angVel = angVel + m_handling.contactNormal*(angularCorrection*angSpring);
		vel = vel - xAxis * (xAxis.dot(vel) * approxOneExp(dt * lateralSpring0));	// Lateral damp
	}

	//==============================================
	// Commit the velocity back to the physics engine
	//==============================================
	if (vel.GetLengthSquared()>0.001f || angVel.GetLengthSquared()>0.001f) 
	{ 
		pe_action_set_velocity setVelocity;
		setVelocity.v = vel;
		setVelocity.w = angVel;
		pPhysics->Action(&setVelocity, THREAD_SAFE);
	}

	m_chassis.vel = vel;
	m_chassis.angVel = angVel;
	c->invInertia = chassisInvInertia;
}

#if ENABLE_VEHICLE_DEBUG
void CVehicleMovementArcadeWheeled::DebugCheat(float dt)
{
	if (g_pGameCVars->v_profileMovement == 0) return;

	IPhysicalEntity* pPhysics = GetPhysics();
	Matrix33 bodyRot( m_PhysPos.q );
	const Vec3 xAxis = bodyRot.GetColumn0();
	const Vec3 yAxis = bodyRot.GetColumn1();
	const Vec3 zAxis = bodyRot.GetColumn2();
	const Vec3 pos = m_PhysDyn.centerOfMass;

	if (g_pGameCVars->v_debugMovementMoveVertically!=0.f || g_pGameCVars->v_debugMovementX!=0.f || g_pGameCVars->v_debugMovementY!=0.f || g_pGameCVars->v_debugMovementZ!=0.f)
	{
		const float angleX = 0.5f * g_pGameCVars->v_debugMovementX;
		const float angleY = 0.5f * g_pGameCVars->v_debugMovementY;
		const float angleZ = 0.5f * g_pGameCVars->v_debugMovementZ;

		Quat qx, qy, qz;
		pe_params_pos physPos;

		qx.SetRotationAA(cosf(DEG2RAD(angleX)), sinf(DEG2RAD(angleX)), xAxis);
		qy.SetRotationAA(cosf(DEG2RAD(angleY)), sinf(DEG2RAD(angleY)), yAxis);
		qz.SetRotationAA(cosf(DEG2RAD(angleZ)), sinf(DEG2RAD(angleZ)), zAxis);

		m_PhysPos.pos.z += g_pGameCVars->v_debugMovementMoveVertically;
		physPos.pos = m_PhysPos.pos;
		physPos.q = qx * qy * qz * m_PhysPos.q;

		pPhysics->SetParams(&physPos, 1);
		g_pGameCVars->v_debugMovementMoveVertically=0.f;
		g_pGameCVars->v_debugMovementX=0.f;
		g_pGameCVars->v_debugMovementY=0.f;
		g_pGameCVars->v_debugMovementZ=0.f;
	}

	if (g_pGameCVars->v_debugMovement)
	{
		const float angleX = 0.5f * dt * m_movementAction.power * g_pGameCVars->v_debugMovementSensitivity;
		const float angleY = 0.5f * dt * m_movementAction.rotateYaw * g_pGameCVars->v_debugMovementSensitivity;

		Quat qx, qy;
		pe_params_pos physPos;
		qx.SetRotationAA(cosf(DEG2RAD(angleX)), sinf(DEG2RAD(angleX)), xAxis);
		qy.SetRotationAA(cosf(DEG2RAD(angleY)), sinf(DEG2RAD(angleY)), yAxis);
		physPos.q = qx * qy * m_PhysPos.q;
		pPhysics->SetParams(&physPos, 1);

		// Kill gravity
		pe_simulation_params paramsSet;
		paramsSet.gravityFreefall.Set(0.f, 0.f, 0.f);
		pPhysics->SetParams(&paramsSet, 1);

		// Kill velocity
		pe_action_set_velocity setVelocity;
		setVelocity.v.zero();
		setVelocity.w.zero();
		pPhysics->Action(&setVelocity, THREAD_SAFE);
		return;
	}
}
#endif

void CVehicleMovementArcadeWheeled::UpdateWaterLevels()
{
	size_t	numberOfWheels = m_wheels.size();

	if(numberOfWheels > 0)
	{
		if(m_iWaterLevelUpdate >= numberOfWheels)
		{
			m_iWaterLevelUpdate = 0;
		}

		pe_status_wheel	wheelStatus;

		wheelStatus.iWheel = m_iWaterLevelUpdate;

		if(GetPhysics()->GetStatus(&wheelStatus))
		{
			I3DEngine	*p3DEngine = gEnv->p3DEngine;

			m_wheels[m_iWaterLevelUpdate].waterLevel = gEnv->p3DEngine->GetWaterLevel(&wheelStatus.ptContact);
		}
		else
		{
			m_wheels[m_iWaterLevelUpdate].waterLevel = WATER_LEVEL_UNKNOWN;
		}

		++ m_iWaterLevelUpdate;
	}
}

void CVehicleMovementArcadeWheeled::ResetWaterLevels()
{
	for(TWheelArray::iterator iWheel = m_wheels.begin(), end = m_wheels.end(); iWheel != end; ++ iWheel)
	{
		iWheel->waterLevel = WATER_LEVEL_UNKNOWN;
	}

	m_iWaterLevelUpdate = 0;
}

// A voltile is used to remove a load-hit-store by
// forcing an early store.




#define F2I(x) (int)(x)
#define TMPINT const int


void CVehicleMovementArcadeWheeled::TickGears(float dt, float averageWheelSpeed, float throttle, float forwardSpeed)
{
	assert(throttle>=-1.f && throttle<=1.f);

	// Break up the conversion to avoid load hit stores
	TMPINT ivThrottle = F2I(throttle*20.f);
	TMPINT ivSpeed = F2I(forwardSpeed*10.f);

	if (m_isEnginePowered)
	{
		// Update the target rpm, smoothly
		const float topSpeed = (float)__fsel(forwardSpeed, m_pSharedParams->handling.topSpeed, m_pSharedParams->handling.reverseSpeed);
		const float wheelRpm = (averageWheelSpeed * m_gears.averageWheelRadius) / topSpeed;	// Normalised between 0 and 1
		const float ratio = m_pSharedParams->gears.ratios[m_gears.curGear];
		const float invRatio = m_pSharedParams->gears.invRatios[m_gears.curGear];
		float lastTargetRpm = m_gears.targetRpm;
		m_gears.targetRpm += (clamp(wheelRpm * invRatio, 0.f, 1.f) - m_gears.targetRpm)*approxOneExp(20.f*dt);

		// Change curRpm smoothly
		const float rpmChange = m_gears.targetRpm - m_gears.curRpm;
		const float interpSpeed = (float)__fsel(fabsf(throttle)-0.05f, m_pSharedParams->rpmInterpSpeed, m_pSharedParams->rpmRelaxSpeed);
		m_gears.curRpm += rpmChange * approxOneExp(dt*interpSpeed*3.f);

		// Determine throttle direction
		// iThrottle=-1,0,+1, if (throttle>0.05f) iThrottle = 1; if (throttle<-0.05f) iThrottle = -1;
		const int32 iThrottle = sgn((int32)ivThrottle);

		switch (m_gears.curGear)
		{
		case SVehicleGears::kReverse:
			{
				// Change to first if throttle>eps && speed>eps
				const int32 iSpeedGtz = 1-isneg((int32)ivSpeed);        
				const int32 iThrottleGtz = (iThrottle+1);        
				m_gears.curGear += (iSpeedGtz*iThrottleGtz);
				m_gears.timer = 0.f;
				break;
			}
		case SVehicleGears::kNeutral:
			{
				// Change to up or down based on throttle
				m_gears.curGear += iThrottle;
				m_gears.timer = 0.f;
				break;
			}
		default:
			{
				if (isneg(iThrottle) & isneg(ivSpeed))
				{
					m_gears.curGear = SVehicleGears::kReverse;
					m_gears.timer = 0.f;
				}
				else
				{
					float accelerating = (float)__fsel((m_gears.targetRpm-lastTargetRpm) - 0.1f*dt, 1.f, 0.f);
					float filteredRpm = 0.2f*m_gears.curRpm + 0.8f*m_gears.targetRpm;

					if ((accelerating*filteredRpm) > 0.6f)
					{
						if (m_gears.timer>m_pSharedParams->gears.minChangeUpTime)
						{
							if (m_gears.curGear<(m_pSharedParams->gears.numGears-1))
							{
								m_gears.curGear++;
								m_gears.timer = 0.f;
								m_gears.targetRpm = 0.2f;
							}
						}
					}
					else if (accelerating==0.f)
					{
						assert(m_gears.curGear > 0 && m_gears.curGear <= SVehicleGears::kMaxGears);

						// Change down if the rpm has fallen below a certain fraction of the lower gear, curGear-1
						if ((filteredRpm*ratio) < m_pSharedParams->gears.ratios[m_gears.curGear-1] * 0.8f)
						{
							int noHandBrake = 1-m_action.bHandBrake;
							int throttleAndNoHandBrake = noHandBrake & (((iThrottle+1)>>1)^m_action.bHandBrake);
							if (m_gears.timer>m_pSharedParams->gears.minChangeDownTime || throttleAndNoHandBrake)
							{
								m_gears.curGear--;
								m_gears.timer = 0.f;
								m_gears.targetRpm = 0.2f;
							}
						}
					}
				}
				m_gears.timer += dt;
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::Boost(bool enable)
{  
	if (enable)
	{
		if (m_action.bHandBrake)
			return;
	}

	CVehicleMovementBase::Boost(enable);
}


//------------------------------------------------------------------------
bool CVehicleMovementArcadeWheeled::RequestMovement(CMovementRequest& movementRequest)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_GAME );

	m_movementAction.isAI = true;
	if (!m_isEnginePowered)
		return false;

	CryAutoCriticalSection lk(m_lock);

	if (movementRequest.HasLookTarget())
		m_aiRequest.SetLookTarget(movementRequest.GetLookTarget());
	else
		m_aiRequest.ClearLookTarget();

	if (movementRequest.HasMoveTarget())
	{
		Vec3 entityPos = m_pEntity->GetWorldPos();
		Vec3 start(entityPos);
		Vec3 end( movementRequest.GetMoveTarget() );
		Vec3 pos = ( end - start ) * 100.0f;
		pos +=start;
		m_aiRequest.SetMoveTarget( pos );
	}
	else
		m_aiRequest.ClearMoveTarget();

	float fDesiredSpeed = 0.0f;

	if (movementRequest.HasDesiredSpeed())
		fDesiredSpeed = movementRequest.GetDesiredSpeed();
	else
		m_aiRequest.ClearDesiredSpeed();

	if (movementRequest.HasForcedNavigation())
	{
		const Vec3 forcedNavigation = movementRequest.GetForcedNavigation();
		const Vec3 entityPos = m_pEntity->GetWorldPos();
		m_aiRequest.SetForcedNavigation(forcedNavigation);
		m_aiRequest.SetMoveTarget(entityPos+forcedNavigation.GetNormalizedSafe()*100.0f);

		if (fabsf(fDesiredSpeed) <= FLT_EPSILON)
			fDesiredSpeed = forcedNavigation.GetLength();
	}
	else
		m_aiRequest.ClearForcedNavigation();

	m_aiRequest.SetDesiredSpeed(fDesiredSpeed);

	if(fabs(fDesiredSpeed) > FLT_EPSILON)
	{
		m_pVehicle->NeedsUpdate(IVehicle::eVUF_AwakePhysics, true);
	}

	return true;

}

void CVehicleMovementArcadeWheeled::GetMovementState(SMovementState& movementState)
{
	IPhysicalEntity* pPhysics = GetPhysics();
	if (!pPhysics)
		return;

	movementState.minSpeed = 0.0f;
	movementState.maxSpeed = m_pSharedParams->handling.topSpeed;
	movementState.normalSpeed = movementState.maxSpeed;
}


//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::Serialize(TSerialize ser, EEntityAspects aspects) 
{
	MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "Vehicle movement arcade wheeled serialization");

	CVehicleMovementBase::Serialize(ser, aspects);

	if (ser.GetSerializationTarget() == eST_Network)
	{
		//if (aspects&CNetworkMovementArcadeWheeled::CONTROLLED_ASPECT)
		//	m_netActionSync.Serialize(ser, aspects);
	}
	else 
	{	
		ser.Value("brakeTimer", m_brakeTimer);
		ser.Value("brake", m_movementAction.brake);
		ser.Value("tireBlownTimer", m_tireBlownTimer);
		ser.Value("initialHandbreak", m_initialHandbreak);

		int blownTires = m_blownTires;
		ser.Value("blownTires", m_blownTires);
		ser.Value("bForceSleep", m_bForceSleep);

		if (ser.IsReading() && blownTires != m_blownTires)
			SetEngineRPMMult(GetWheelCondition());

		ser.Value("m_prevAngle", m_prevAngle);

		m_frictionState = k_frictionNotSet;
	}

	if(ser.IsReading())
	{
		ResetWaterLevels();
	}
};

//------------------------------------------------------------------------
void CVehicleMovementArcadeWheeled::UpdateSurfaceEffects(const float deltaTime)
{ 
	FUNCTION_PROFILER( GetISystem(), PROFILE_GAME );

	if (0 == g_pGameCVars->v_pa_surface)
	{
		ResetParticles();
		return;
	}

	const SVehicleStatus& status = m_pVehicle->GetStatus();
	if (status.speed < 0.01f)
		return;

	float distSq = m_pVehicle->GetEntity()->GetWorldPos().GetSquaredDistance(gEnv->pRenderer->GetCamera().GetPosition());
	if (distSq > sqr(300.f) || (distSq > sqr(50.f) && !m_isProbablyVisible ))
		return;

	IPhysicalEntity* pPhysics = GetPhysics();

	// don't render particles for drivers in 1st person (E3 request)
	bool hideForFP = false;
	if (GetMovementType() == eVMT_Land && m_carParams.steerTrackNeutralTurn == 0.f)
	{
		IActor* pActor = m_pVehicle->GetDriver();
		IVehicleSeat* pSeat = (pActor != NULL) ? m_pVehicle->GetSeatForPassenger(pActor->GetEntityId()) : NULL;
		IVehicleView* pView = (pSeat != NULL) ? pSeat->GetView(pSeat->GetCurrentView()) : NULL;
		if (pActor && pActor->IsClient() && (pView != NULL) && !pView->IsThirdPerson())
			hideForFP = true;
	}

	float soundSlip = 0;

#if ENABLE_VEHICLE_DEBUG
	if (DebugParticles())
	{
		float color[] = {1,1,1,1};
		gEnv->pRenderer->Draw2dLabel(100, 280, 1.3f, color, false, "%s:", m_pVehicle->GetEntity()->GetName());
	}
#endif

	SEnvironmentParticles* envParams = m_pPaParams->GetEnvironmentParticles();
	SEnvParticleStatus::TEnvEmitters::iterator emitterIt = m_paStats.envStats.emitters.begin();
	SEnvParticleStatus::TEnvEmitters::iterator emitterItEnd = m_paStats.envStats.emitters.end();

	for (; emitterIt != emitterItEnd; ++emitterIt)
	{ 
		if (emitterIt->layer < 0)
		{
			assert(0);
			continue;
		}

		if (!emitterIt->active)
			continue;

		const SEnvironmentLayer& layer = envParams->GetLayer(emitterIt->layer);

		//if (!layer.active || !layer.IsGroupActive(emitterIt->group))

		// scaling for each wheelgroup is based on vehicle speed + avg. slipspeed
		float slipAvg = 0; 
		int cnt = 0;
		bool bContact = false;
		int matId = 0;

		// Calculate average water level of wheels in group.

		float		fWaterLevel = 0.0f;

		size_t	wheelCount = layer.GetWheelCount(emitterIt->group);

		if(wheelCount && !m_wheels.empty())
		{
			for(size_t w = 0; w < wheelCount; ++ w)
			{
				CRY_ASSERT((layer.GetWheelAt(emitterIt->group, w) - 1) < (int)m_wheels.size());

				float	wheelWaterLevel = m_wheels[layer.GetWheelAt(emitterIt->group, w) - 1].waterLevel;

				if(wheelWaterLevel != WATER_LEVEL_UNKNOWN)
				{
					fWaterLevel += wheelWaterLevel;
				}
			}

			fWaterLevel /= float(wheelCount);
		}
		else
		{
			fWaterLevel = -FLT_MAX;
		}

		for (size_t w=0; w<wheelCount; ++w)
		{
			// all wheels in group
			++cnt;
			pe_status_wheel wheelStats;
			wheelStats.iWheel = layer.GetWheelAt(emitterIt->group, static_cast<int>(w)) - 1;

			if (!pPhysics->GetStatus(&wheelStats))
				continue;

			if (wheelStats.bContact)
			{
				bContact = true;

				// take care of water
				if (fWaterLevel > wheelStats.ptContact.z+0.02f)
				{
					if ( fWaterLevel > wheelStats.ptContact.z+2.0f)
					{
						slipAvg =0.0f;
						bContact = false;
					}
					matId = gEnv->pPhysicalWorld->GetWaterMat();
				}
				else if (wheelStats.contactSurfaceIdx > matId)
					matId = wheelStats.contactSurfaceIdx;

				if (wheelStats.bSlip)
					slipAvg += wheelStats.velSlip.len();
			}
		}

		if (!bContact && !emitterIt->bContact)
			continue;

		emitterIt->bContact = bContact;    
		slipAvg /= cnt;

		bool isSlip = !strcmp(layer.GetName(), "slip");    
		float vel = isSlip ? 0.f : m_speed;
		vel += 1.f*slipAvg;

		soundSlip = max(soundSlip, slipAvg);       

		float countScale = 1;
		float sizeScale = 1;
		float speedScale = 1;

		if (hideForFP || !bContact || matId == 0)    
			countScale = 0;          
		else
			GetParticleScale(layer, vel, 0.f, countScale, sizeScale, speedScale);

		IEntity* pEntity = m_pVehicle->GetEntity();
		SEntitySlotInfo info;
		info.pParticleEmitter = 0;
		pEntity->GetSlotInfo(emitterIt->slot, info);

		if (matId != emitterIt->matId)
		{
			// change effect                        
			const char* effect = GetEffectByIndex(matId, layer.GetName());
			IParticleEffect* pEff = 0;   

			if (effect && (pEff = gEnv->pParticleManager->FindEffect(effect)))
			{     
#if ENABLE_VEHICLE_DEBUG
				if (DebugParticles())          
					CryLog("<%s> changes sfx to %s (slot %i)", pEntity->GetName(), effect, emitterIt->slot);
#endif

				if (info.pParticleEmitter)
				{   
					// free old emitter and load new one, for old effect to die gracefully           
					info.pParticleEmitter->Activate(false);            
					pEntity->FreeSlot(emitterIt->slot);
				}         

				emitterIt->slot = pEntity->LoadParticleEmitter(emitterIt->slot, pEff);

				if (emitterIt->slot != -1)
					pEntity->SetSlotLocalTM(emitterIt->slot, Matrix34(emitterIt->quatT));

				info.pParticleEmitter = 0;
				pEntity->GetSlotInfo(emitterIt->slot, info);

				emitterIt->matId = matId;
			}
			else 
			{
#if ENABLE_VEHICLE_DEBUG
				if (DebugParticles())
					CryLog("<%s> found no effect for %i", pEntity->GetName(), matId);
#endif

				// effect not available, disable
				//info.pParticleEmitter->Activate(false);
				countScale = 0.f; 
				emitterIt->matId = 0;
			}        
		}

		if (emitterIt->matId == 0)      
			countScale = 0.f;

		if (info.pParticleEmitter)
		{
			SpawnParams sp;
			sp.fSizeScale = sizeScale;
			sp.fCountScale = countScale;
			sp.fSpeedScale = speedScale;
			info.pParticleEmitter->SetSpawnParams(sp);
		}

#if ENABLE_VEHICLE_DEBUG
		if (DebugParticles())
		{
			float color[] = {1,1,1,1};
			gEnv->pRenderer->Draw2dLabel((float)(100+330*emitterIt->layer), (float)(300+25*emitterIt->group), 1.2f, color, false, "group %i, matId %i: sizeScale %.2f, countScale %.2f, speedScale %.2f (emit: %i)", emitterIt->group, emitterIt->matId, sizeScale, countScale, speedScale, info.pParticleEmitter?1:0);
			gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_pVehicle->GetEntity()->GetSlotWorldTM(emitterIt->slot).GetTranslation(), 0.2f, ColorB(0,0,255,200));
		}
#endif
	}

	// Increase slip if handbrake is engaged.

	if(m_action.bHandBrake)
	{
		const float	handbrakeSlipScale = 10.0f;	// TODO : Expose this parameter to designers (after Crysis 2 ships?).

		soundSlip *= handbrakeSlipScale;
	}

	if (m_maxSoundSlipSpeed > 0.f)
		m_surfaceSoundStats.slipRatio = min(soundSlip/m_maxSoundSlipSpeed, 1.f);
}

//------------------------------------------------------------------------
bool CVehicleMovementArcadeWheeled::DoGearSound()
{
	return true;
}

void CVehicleMovementArcadeWheeled::GetMemoryUsage(ICrySizer * pSizer) const
{
	pSizer->Add(*this);
	pSizer->AddContainer(m_wheels);
	pSizer->AddContainer(m_wheelStatus);
	CVehicleMovementBase::GetMemoryUsageInternal(pSizer);
}
















































