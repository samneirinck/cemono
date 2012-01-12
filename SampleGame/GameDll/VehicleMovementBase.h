/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a base class for vehicle movements

-------------------------------------------------------------------------
History:
- 09:28:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEMOVEMENTBASE_H__
#define __VEHICLEMOVEMENTBASE_H__

#include <ParticleParams.h>
#include <IVehicleSystem.h>
#include "Actor.h"
#include "IMaterialEffects.h"
#include "IForceFeedbackSystem.h"

#define ENGINESOUND_MAX_DIST 150.f

struct IGroundEffect;

/** exhaust status structure
*/
struct SExhaustStatus
{
	IVehicleHelper* pHelper;
	int startStopSlot;
	int runSlot;
	int boostSlot;
	int enabled;

	SExhaustStatus()
	{    
		startStopSlot = -1;
		runSlot = -1;
		boostSlot = -1;
		enabled = 1;
	}
};

struct TEnvEmitter
{
	TEnvEmitter()
	{ 
		layer = -1;      
		slot = -1;
		matId = -1;
		group = -1;
		bContact = false;
		pGroundEffect = 0;    
		active = true;
	}

	QuatT quatT;  // local tm
	int layer; // layer idx the emitter belongs to    
	int slot;  // emitter slot
	int matId; // last surface idx    
	int group; // optional     
	IGroundEffect* pGroundEffect;
	bool bContact; // optional  
	bool active; 
};

struct SEnvParticleStatus
{
	bool initalized;

	typedef std::vector<TEnvEmitter> TEnvEmitters;
	TEnvEmitters emitters;  //maps emitter slots to user-defined index

	SEnvParticleStatus() : initalized(false)
	{}

	void Serialize(TSerialize ser, EEntityAspects aspects)
	{    
		ser.BeginGroup("EnvParticleStatus");   

		int readCount = emitters.size();
		ser.Value("NumEnvEmitters", readCount);
		const int count = min(readCount, static_cast<int>(emitters.size()));

		char name[16] = "slot_x";
		for (int i=0; i<count; ++i)
		{
			itoa(i, &name[5], 10);
			ser.ValueWithDefault(name, emitters[i].slot, -1);
		}

		ser.EndGroup();
	}
};


/** particle status structure
*/
struct SParticleStatus
{  
	std::vector<SExhaustStatus> exhaustStats; // can hold multiple exhausts
	SEnvParticleStatus envStats;

	SParticleStatus()
	{ 
	}

	void Serialize(TSerialize ser, EEntityAspects aspects)
	{
		ser.BeginGroup("ExhaustStatus");   

		size_t count = exhaustStats.size();
		ser.Value("NumExhausts", count);

		if (ser.IsWriting())
		{
			for(std::vector<SExhaustStatus>::iterator it = exhaustStats.begin(); it != exhaustStats.end(); ++it)      
			{
				ser.BeginGroup("ExhaustRunSlot");
				ser.Value("slot", it->runSlot);
				ser.EndGroup();
			}
		}
		else if (ser.IsReading())
		{
			//assert(count == exhaustStats.size());
			for (size_t i=0; i<count&&i<exhaustStats.size(); ++i)
			{
				ser.BeginGroup("ExhaustRunSlot");
				ser.Value("slot", exhaustStats[i].runSlot); 
				ser.EndGroup();
			}
		}   
		ser.EndGroup();

		envStats.Serialize(ser, aspects);
	}
};

struct SSurfaceSoundStatus
{
	int matId; 
	float surfaceParam;
	float slipRatio;
	float slipTimer;
	int scratching;

	SSurfaceSoundStatus()
	{
		Reset();
	}

	void Reset()
	{
		matId = 0;
		surfaceParam = 0.f;
		slipRatio = 0.f;
		slipTimer = 0.f;
		scratching = 0;
	}

	void Serialize(TSerialize ser, EEntityAspects aspects)
	{
		ser.BeginGroup("SurfaceSoundStats");
		ser.Value("surfaceParam", surfaceParam);		
		ser.EndGroup();
	}
};

// sounds
enum EVehicleMovementSound
{
	eSID_Start = 0,
	eSID_Run,
	eSID_Stop,
	eSID_Ambience,
	eSID_Bump,
	eSID_Splash,
	eSID_Gear,
	eSID_Slip,
	eSID_Acceleration,
	eSID_Boost,
	eSID_Damage,
	eSID_Max,
};

enum EVehicleMovementAnimation
{
	eVMA_Engine = 0,
	eVMA_Max,
};

struct SMovementSoundStatus
{
	SMovementSoundStatus(){ Reset(); }

	void Reset()
	{    
		for (int i=0; i<eSID_Max; ++i)    
		{      
			sounds[i] = INVALID_SOUNDID;
			lastPlayed[i].SetValue(0);
		}

		inout = 1.f;
	}

	tSoundID sounds[eSID_Max];  
	CTimeValue lastPlayed[eSID_Max];
	float inout;
};

#if ENABLE_VEHICLE_DEBUG
namespace 
{
	bool DebugParticles()
	{
		static ICVar* pVar = gEnv->pConsole->GetCVar("v_debugdraw");
		return pVar->GetIVal() == eVDB_Particles;
	}
}
#endif

// enum continuation of CryAction EVehicleMovementEvent for Game-Specific events
enum EVehicleMovementEventGame
{
	eVME_StartLargeObject = IVehicleMovement::eVME_Others,
};

struct SVehicleMovementEventLargeObject
{
	enum {k_KickFront, k_KickDiagonal, k_KickSide};

	Vec3 impulseDir;
	float speed;
	float rotationFactor;
	float swingRotationFactor;
	float suitBoost;
	Vec3 eyePos;
	Quat viewQuat;
	EntityId kicker;
	int kickType;
	float timer;
};

// Get information about any large object interaction
// that this vehicle is having
struct SVehicleMovementLargeObjectInfo
{
	Vec3 impulseDir;
	EntityId kicker;
	float countDownTimer;     // A timer counting down to zero. When zero is reached vehicle is no longer being 'interacted' with
	float startTime;         // The initial time to count down
	float invStartTime;       // Recip. of initial time
};


class CVehicleMovementBase : 
	public IVehicleMovement, 
	public IVehicleObject, 
	public ISoundEventListener
{
	IMPLEMENT_VEHICLEOBJECT
public:  
	CVehicleMovementBase();
	virtual ~CVehicleMovementBase();

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void PostInit();
	virtual void Release();
	virtual void Reset();  
	virtual void Physicalize();
	virtual void PostPhysicalize();

	virtual void ResetInput();

	virtual EVehicleMovementType GetMovementType() { return eVMT_Other; }

	virtual bool StartEngine(EntityId driverId);
	virtual void StopEngine();
	virtual bool IsPowered() { return m_isEnginePowered; }
	virtual void DisableEngine(bool disable);

	virtual float GetDamageRatio() { return m_damage; }

	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	virtual void OnEvent(EVehicleMovementEvent event, const SVehicleMovementEventParams& params);
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	virtual void ProcessMovement(const float deltaTime);
	virtual void ProcessActions(const float deltaTime) {}
	virtual void ProcessAI(const float deltaTime) {}
	virtual void Update(const float deltaTime);

	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
	virtual void SetChannelId(uint16 id) {};
	virtual void SetAuthority(bool auth){};
	virtual void PostSerialize();

	virtual void OnEngineCompletelyStopped();

	virtual void RequestActions(const SVehicleMovementAction& movementAction);
	virtual bool RequestMovement(CMovementRequest& movementRequest);
	virtual void GetMovementState(SMovementState& movementState);
	virtual bool GetStanceState(const SStanceStateQuery& query, SStanceState& state);

	virtual pe_type GetPhysicalizationType() const { return PE_RIGID; };
	virtual bool UseDrivingProxy() const { return false; };
	virtual int GetWheelContacts() const { return 0; }

	virtual void RegisterActionFilter(IVehicleMovementActionFilter* pActionFilter);
	virtual void UnregisterActionFilter(IVehicleMovementActionFilter* pActionFilter);

	virtual void OnSoundEvent(ESoundCallbackEvent event,ISound *pSound);

	virtual void EnableMovementProcessing(bool enable){ m_bMovementProcessingEnabled = enable; }
	virtual bool IsMovementProcessingEnabled(){ return m_bMovementProcessingEnabled; }

	virtual void ProcessEvent(SEntityEvent& event);
	virtual void SetSoundMasterVolume(float vol);

	virtual float GetEnginePedal(){ return m_movementAction.power; }

	const pe_status_dynamics& GetPhysicsDyn() { return m_PhysDyn; }
	const pe_status_pos& GetPhysicsPos() { return m_PhysPos; }

	void SetRemotePilot(bool on) { m_remotePilot = on; }
	SParticleStatus& GetParticleStats() { return m_paStats; }
	SParticleParams* GetParticleParams() { return m_pPaParams; }

	virtual const SVehicleMovementLargeObjectInfo* GetLargeObjectInfo() { return NULL; }

	virtual void GetMemoryUsage(ICrySizer * pSizer) const
	{
		pSizer->AddObject(this, sizeof(*this));
		GetMemoryUsageInternal(pSizer);
	}

	void GetMemoryUsageInternal(ICrySizer * pSizer) const
	{
		for( int i = 0 ; i < eSID_Max ; ++i )
			pSizer->AddObject(m_soundNames[i]);

		pSizer->AddObject(m_actionFilters);
		pSizer->AddObject(m_damageComponents);
		pSizer->AddObject(m_surfaceSoundInfo);		
	}

	static void CleanUp();

protected:

	ILINE IPhysicalEntity* GetPhysics() const { return m_pVehicle->GetEntity()->GetPhysics(); }
	bool IsProfilingMovement();

	// sound methods
	ISound* PlaySound(EVehicleMovementSound eSID, float pulse=0.f, const Vec3& offset=Vec3Constants<float>::fVec3_Zero, int soundFlags=0);
	ISound* GetOrPlaySound(EVehicleMovementSound eSID, float pulse=0.f, const Vec3& offset=Vec3Constants<float>::fVec3_Zero, int soundFlags=0);
	void StopSound(EVehicleMovementSound eSID);
	void StopSounds();
	ISound* GetSound(EVehicleMovementSound eSID);
	const string& GetSoundName(EVehicleMovementSound eSID);
	void SetSoundParam(EVehicleMovementSound eSID, const char* param, float value);
	void SetSoundParam(ISound* pSound, const char* param, float value);
	tSoundID GetSoundId(EVehicleMovementSound eSID);

#if ENABLE_VEHICLE_DEBUG
	void DebugDraw(const float deltaTime);
#endif

	// animation methiods
	void StartAnimation(EVehicleMovementAnimation eAnim);
	void StopAnimation(EVehicleMovementAnimation eAnim);
	void SetAnimationSpeed(EVehicleMovementAnimation eAnim, float speed);

	void SetDamage(float damage, bool fatal);
	virtual void UpdateDamage(const float deltaTime);

	virtual void UpdateRunSound(const float deltaTime);
	virtual void UpdateSpeedRatio(const float deltaTime);

	virtual void Boost(bool enable);
	virtual bool Boosting() { return m_boost; }
	virtual void UpdateBoost(const float deltaTime);
	virtual void ResetBoost();

	void EjectionTest(float deltaTime);
	void ApplyAirDamp(float angleMin, float angVelMin, float deltaTime, int threadSafe);
	void UpdateGravity(float gravity);

	// surface particle/sound methods
	virtual void InitExhaust();
	virtual void InitSurfaceEffects();
	virtual void ResetParticles();
	virtual void UpdateSurfaceEffects(const float deltaTime);  
	virtual void RemoveSurfaceEffects();
	virtual void GetParticleScale(const SEnvironmentLayer& layer, float speed, float power, float& countScale, float& sizeScale, float& speedScale);
	virtual void EnableEnvEmitter(TEnvEmitter& emitter, bool enable);
	virtual void UpdateExhaust(const float deltaTime);  
	void FreeEmitterSlot(int& slot);
	void FreeEmitterSlot(const int& slot);
	void StartExhaust(bool ignition=true, bool reload=true);
	void StopExhaust();  
	float GetWaterMod(SExhaustStatus& exStatus);
	float GetSoundDamage();

	SMFXResourceListPtr GetEffectNode(int matId);
	const char* GetEffectByIndex(int matId, const char* username);
	float GetSurfaceSoundParam(int matId);

	virtual bool GenerateWind() { return true; }
	void InitWind();
	void UpdateWind(const float deltaTime);
	void SetWind(const Vec3& wind);
	Vec3 GetWindPos(Vec3& posRad, Vec3& posLin);
	// ~surface particle/sound methods

	IVehicle* m_pVehicle;
	IEntity* m_pEntity;
	IEntitySoundProxy* m_pEntitySoundsProxy;	
	static IGameTokenSystem* m_pGameTokenSystem;
	static IVehicleSystem* m_pVehicleSystem;
	static IActorSystem* m_pActorSystem;

	SVehicleMovementAction m_movementAction;

	// Kept out of CryAction by not adding it to m_movementAction
	float m_lastPowerForward;
	float m_lastPowerBackward;

	bool m_isEngineDisabled;
	bool m_isEngineStarting;
	bool m_isEngineGoingOff;
	float m_engineStartup;
	float m_engineIgnitionTime;
	uint32 m_engineDisabledTimerId;
	uint32 m_engineDisabledFXId;


	bool m_bMovementProcessingEnabled;
	bool m_isEnginePowered;
	bool m_remotePilot;
	float m_damage;

	string m_soundNames[eSID_Max];

	Vec3 m_enginePos;
	float m_runSoundDelay;  
	float m_rpmScale, m_rpmScaleSgn;
	float m_rpmPitchSpeed;
	float m_maxSoundSlipSpeed;  
	float m_soundMasterVolume;

	bool m_boost;
	bool m_wasBoosting;
	float m_boostEndurance;
	float m_boostRegen;  
	float m_boostStrength;
	float m_boostCounter;

	IVehicleAnimation* m_animations[eVMA_Max];

	SParticleParams* m_pPaParams;
	SParticleStatus m_paStats;
	SSurfaceSoundStatus m_surfaceSoundStats;
	SMovementSoundStatus m_soundStats;

	EntityId m_actorId;
	pe_status_dynamics m_statusDyn; // gets updated once per update
	pe_status_pos m_statusPos; // gets updated once per update
	pe_status_dynamics m_PhysDyn; // gets updated once per phys update
	pe_status_pos	m_PhysPos; // gets updated once per phys update

	float m_speed;				// Gets updated once per update on the main thread
	Vec3 m_localSpeed;			// Gets updated once per update on the main thread

	unsigned int m_isProbablyDistant : 1;
	unsigned int m_isProbablyVisible : 1;

	float m_maxSpeed; 
	float m_speedRatio;
	float m_speedRatioUnit;

	float m_measureSpeedTimer;
	Vec3 m_lastMeasuredVel;

	// flight stabilization
	Vec3 m_dampAngle;
	Vec3 m_dampAngVel;  

	float m_ejectionDotProduct;
	float m_ejectionTimer;
	float m_ejectionTimer0;

	IPhysicalEntity* m_pWind[2];

	typedef std::list<IVehicleMovementActionFilter*> TVehicleMovementActionFilterList;
	TVehicleMovementActionFilterList m_actionFilters;

	typedef std::vector<IVehicleComponent*> TComponents;
	TComponents m_damageComponents;  

	struct SSurfaceSoundInfo
	{
		int paramIndex;

		SSurfaceSoundInfo() : paramIndex(0)
		{}
		SSurfaceSoundInfo(int index) : paramIndex(index)
		{}

		void GetMemoryUsage(ICrySizer * pSizer) const{}

	};
	typedef std::map<string, SSurfaceSoundInfo> TSurfaceSoundInfo;
	static TSurfaceSoundInfo m_surfaceSoundInfo;

	static float		m_sprintTime;

	static const float	ms_engineSoundIdleRatio, ms_engineSoundOverRevRatio;

	ForceFeedbackFxId	m_engineStartingForceFeedbackFxId;

	ForceFeedbackFxId	m_enginePoweredForceFeedbackFxId;

	ForceFeedbackFxId	m_collisionForceFeedbackFxId;

	CryCriticalSection	m_lock;
};

struct SPID
{
	ILINE SPID() : m_kP( 0 ),	m_kD( 0 ), m_kI( 0 ),	m_prevErr( 0 ),	m_intErr( 0 ){}
	ILINE void	Reset(){m_prevErr = 0; m_intErr = 0;}
	float	Update( float inputVal, float setPoint, float clampMin, float clampMax );
	void  Serialize(TSerialize ser);
	float	m_kP;
	float	m_kD;
	float	m_kI;
	float	m_prevErr;
	float	m_intErr;
};

#define MOVEMENT_VALUE_OPT(name, var, t) \
	t.getAttr(name, var);

#define MOVEMENT_VALUE_REQ(name, var, t) \
	if (!t.getAttr(name, var)) \
{ \
	CryLog("Movement Init (%s) - failed to init due to missing <%s> parameter", m_pVehicle->GetEntity()->GetClass()->GetName(), name); \
	return false; \
}

#define MOVEMENT_VALUE(name, var) \
	if (!table.getAttr(name, var)) \
{ \
	CryLog("Movement Init (%s) - failed to init due to missing <%s> parameter", m_pVehicle->GetEntity()->GetClass()->GetName(), name); \
	return false; \
}


#endif

