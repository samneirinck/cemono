#ifndef IAIActor_h
#define IAIActor_h

#include "IAgent.h"
#include "IPathfinder.h"
#include "IAIRecorder.h"

struct IBlackBoard;
struct VisionID;

typedef enum EBehaviorEvent
{
	BehaviorStarted,
	BehaviorInterrupted,
	BehaviorFinished,
	BehaviorFailed,
};

struct IActorBehaviorListener
{
	virtual void BehaviorEvent(IAIObject* actor, EBehaviorEvent event) = 0;
	virtual void BehaviorChanged(IAIObject* actor, const char* current, const char* previous) = 0;
	virtual ~IActorBehaviorListener(){}
};

// SAIDetectionLevels
//	Used to give game code a snapshot of how this AI agent is currently being detected by the other agents around it
struct SAIDetectionLevels
{
	SAIDetectionLevels(float puppetExposure, float puppetThreat, float vehicleExposure, float vehicleThreat) :
	puppetExposure(puppetExposure),
	puppetThreat(puppetThreat),
	vehicleExposure(vehicleExposure),
	vehicleThreat(vehicleThreat),
	puppetTargetType(AITARGET_NONE),
	vehicleTargetType(AITARGET_NONE),
	puppetTargetThreat(AITHREAT_NONE),
	vehicleTargetThreat(AITHREAT_NONE)
	{
	}

	SAIDetectionLevels()
	{
		Reset();
	}

	inline void Append(const SAIDetectionLevels& levels)
	{
		puppetExposure = max(puppetExposure, levels.puppetExposure);
		puppetThreat = max(puppetThreat, levels.puppetThreat);
		vehicleExposure = max(vehicleExposure, levels.vehicleExposure);
		vehicleThreat = max(vehicleThreat, levels.vehicleThreat);

		puppetTargetType = max(puppetTargetType, levels.puppetTargetType);
		vehicleTargetType = max(vehicleTargetType, levels.vehicleTargetType);
		puppetTargetThreat = max(puppetTargetThreat, levels.puppetTargetThreat);
		vehicleTargetThreat = max(vehicleTargetThreat, levels.vehicleTargetThreat);
	}

	inline void Reset()
	{
		puppetExposure = 0;
		puppetThreat = 0;
		vehicleExposure = 0;
		vehicleThreat = 0;

		puppetTargetType = AITARGET_NONE;
		vehicleTargetType = AITARGET_NONE;
		puppetTargetThreat = AITHREAT_NONE;
		vehicleTargetThreat = AITHREAT_NONE;
	}

	float puppetExposure;
	float puppetThreat;
	float vehicleExposure;
	float vehicleThreat;

	EAITargetType puppetTargetType;
	EAITargetType vehicleTargetType;
	EAITargetThreat puppetTargetThreat;
	EAITargetThreat vehicleTargetThreat;
};

struct IAIActor : public IAIPathAgent
{
	virtual ~IAIActor() {}

	virtual SOBJECTSTATE & GetState() = 0;
	virtual const SOBJECTSTATE & GetState() const = 0;
	virtual IAIActorProxy* GetProxy() const = 0;
	virtual void SetSignal(int nSignalID, const char * szText, IEntity *pSender=0, IAISignalExtraData *pData=NULL, uint32 crcCode = 0) = 0;
	virtual void NotifySignalReceived(const char* szText, IAISignalExtraData* pData=NULL, uint32 crcCode = 0) = 0;
	virtual const AgentParameters& GetParameters() const = 0;
	virtual void SetParameters(const AgentParameters &pParams)=0;
	virtual const AgentMovementAbility& GetMovementAbility() const = 0;
	virtual void SetMovementAbility(AgentMovementAbility &pParams)=0;
	virtual bool CanAcquireTarget(IAIObject* pOther) const = 0;
	virtual void ResetPerception() = 0;
	virtual bool IsActive() const = 0;

	virtual bool CanDamageTarget(IAIObject* target = 0) const = 0;
	virtual bool CanDamageTargetWithMelee() const = 0;

	virtual bool IsObserver() const = 0;
	virtual bool CanSee(const VisionID& otherID) const = 0;

	// Returns the maximum visible range to the target
	virtual float GetMaxTargetVisibleRange(const IAIObject* pTarget, bool bCheckCloak = true) const = 0;

	virtual void EnablePerception(bool enable) = 0;
	virtual bool IsPerceptionEnabled() const = 0;

	virtual void RegisterBehaviorListener(IActorBehaviorListener* listener) = 0;
	virtual void UnregisterBehaviorListener(IActorBehaviorListener* listener) = 0;
	virtual void BehaviorEvent(EBehaviorEvent event) = 0;
	virtual void BehaviorChanged(const char* current, const char* previous) = 0;

	virtual IBlackBoard* GetBehaviorBlackBoard() { return NULL; }
	
	virtual IAIObject* GetAttentionTarget() const = 0;

	virtual EAITargetThreat GetAttentionTargetThreat() const = 0;
	virtual EAITargetType GetAttentionTargetType() const = 0;

	virtual EAITargetThreat GetPeakThreatLevel() const = 0;
	virtual EAITargetType GetPeakThreatType() const = 0;
	virtual tAIObjectID GetPeakTargetID() const = 0;

	virtual EAITargetThreat GetPreviousPeakThreatLevel() const = 0;
	virtual EAITargetType GetPreviousPeakThreatType() const = 0;
	virtual tAIObjectID GetPreviousPeakTargetID() const = 0;

	// Summary:
	//	 Returns a specified point projected on floor/ground.
	virtual Vec3 GetFloorPosition(const Vec3& pos) = 0;
	
	virtual bool IsDevalued(IAIObject* pAIObject) = 0;
	
	virtual void ResetLookAt() = 0;
	virtual bool SetLookAtPointPos(const Vec3& vPoint, bool bPriority = false) = 0;
	virtual bool SetLookAtDir(const Vec3& vDir, bool bPriority = false) = 0;

	virtual void ResetBodyTargetDir() = 0;
	virtual void SetBodyTargetDir(const Vec3& vDir) = 0;
	virtual const Vec3& GetBodyTargetDir() const = 0;

	virtual void SetMoveTarget(const Vec3& vMoveTarget) = 0;
	virtual void GoTo(const Vec3& vTargetPos) = 0;
	virtual void SetSpeed(float fSpeed) = 0;
	
	// Summary: 
	//	 Sets the shape that defines the AI Actor territory.
	virtual void SetTerritoryShapeName(const char* szName) = 0;
	virtual const char* GetTerritoryShapeName() const = 0;
	virtual const char* GetWaveName() const = 0;
	virtual bool IsPointInsideTerritoryShape(const Vec3& vPos, bool bCheckHeight) const = 0;
	virtual bool ConstrainInsideTerritoryShape(Vec3& vPos, bool bCheckHeight) const = 0;

	// Populates list of physics entities to skip for raycasting.
	virtual void GetPhysicalSkipEntities(PhysSkipList& skipList) const {}

	virtual bool HasAIRecorderUnit() const = 0;
	virtual void RecordAIDebugEvent(EAIDebugEvent eAIDebugEvent, const SAIDebugEventData* pAIDebugEventData = NULL) = 0;
	virtual IAIDebugStream* GetAIDebugStream(EAIDebugEvent streamTag) = 0;
};


#endif