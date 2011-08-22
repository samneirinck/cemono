#include DEVIRTUALIZE_HEADER_FIX(IAnimationGraph.h)

#ifndef __IANIMATIONGRAPH_H__
#define __IANIMATIONGRAPH_H__

#if _MSC_VER > 1000
#	pragma once
#endif

#include "ICryAnimation.h"
#include "TimeValue.h"

class CCryName;
struct IGameObject;
struct IAnimatedCharacter;

const uint32 INVALID_ANIMATION_TOKEN = 0xffffffff;

enum EAnimationGraphInputType
{
	eAGIT_Integer,
	eAGIT_Float,
	eAGIT_String
};

AUTO_TYPE_INFO(EAnimationGraphInputType)

enum EAnimationGraphPauser
{
	eAGP_FlowGraph,
	eAGP_TrackView,
	eAGP_StartGame,
	eAGP_Freezing,
	eAGP_PlayAnimationNode,
	eAGP_ObjectInteraction,
	eAGP_HitDeathReactions,
};

enum EAnimationGraphTriggerUser
{
	eAGTU_AI = 0,
	eAGTU_VehicleSystem
};

enum EMovementControlMethod
{
	// !!! WARNING: Update g_szMCMString in AnimationGraph.cpp !!!

	eMCM_Undefined = 0,
	eMCM_Entity = 1,
	eMCM_Animation = 2,
	eMCM_DecoupledCatchUp = 3,
	eMCM_ClampedEntity = 4,
	eMCM_SmoothedEntity = 5,
	eMCM_AnimationHCollision = 6,

	// !!! WARNING: Update g_szMCMString in AnimationGraph.cpp !!!

	eMCM_COUNT,
	eMCM_FF = 0xFF
};
AUTO_TYPE_INFO(EMovementControlMethod)

extern const char* g_szMCMString[eMCM_COUNT];

enum EColliderMode
{
	// !!! WARNING: Update g_szColliderModeString in AnimationGraph.cpp !!!

	eColliderMode_Undefined = 0,
	eColliderMode_Disabled,
	eColliderMode_GroundedOnly,
	eColliderMode_Pushable,
	eColliderMode_NonPushable,
	eColliderMode_PushesPlayersOnly,
	eColliderMode_Spectator,

	// !!! WARNING: Update g_szColliderModeString in AnimationGraph.cpp !!!

	eColliderMode_COUNT,
	eColliderMode_FF = 0xFF
};
extern const char* g_szColliderModeString[eColliderMode_COUNT];

AUTO_TYPE_INFO(EColliderMode)

enum EColliderModeLayer
{
	// !!! WARNING: Update g_szColliderModeLayerString in AnimationGraph.cpp !!!

	eColliderModeLayer_AnimGraph = 0,
	eColliderModeLayer_Game,
	eColliderModeLayer_Script,
	eColliderModeLayer_FlowGraph,
	eColliderModeLayer_ForceSleep,

	eColliderModeLayer_Debug,

	// !!! WARNING: Update g_szColliderModeLayerString in AnimationGraph.cpp !!!

	eColliderModeLayer_COUNT,
	eColliderModeLayer_FF = 0xFF
};

extern const char* g_szColliderModeLayerString[eColliderModeLayer_COUNT];

enum EAnimationGraphUserData
{
	eAGUD_MovementControlMethodH = 0,
	eAGUD_MovementControlMethodV,
	eAGUD_ColliderMode,
	eAGUD_AnimationControlledView,
	eAGUD_AdditionalTurnMultiplier,
	eAGUD_NUM_BUILTINS
};

enum EBlendSpaceControl
{
  eBSP_SetBlendSpaceControl0 = 1 << 0,
  eBSP_SetBlendSpaceControl1 = 1 << 1,
  eBSP_SetIWeights           = 1 << 2,    
  
  eBSP_SetAny                = eBSP_SetBlendSpaceControl0|eBSP_SetBlendSpaceControl1|eBSP_SetIWeights,
};

typedef uint32 TimeAlignmentMask;

struct IAnimationGraphState;
struct IAnimationGraphStateListener;
class CAnimationRandomizer;

UNIQUE_IFACE struct IAnimationGraph
{
	typedef uint8 InputID;

	virtual ~IAnimationGraph(){}
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual IAnimationGraphState * CreateState() = 0;
	virtual IAnimationGraph::InputID LookupInputId( const char * name ) = 0;
	virtual int DeclareOutput( const char * name, const char * value ) = 0;
	virtual uint8 GetBlendValueID( const char * name ) = 0;
	virtual const char * GetName() = 0;
	virtual bool SerializeAsFile(const char* fileName, bool reading = false) = 0;
	virtual const char* RegisterVariationInputs( const char* animationName ) = 0;
	virtual TimeAlignmentMask CalculateTimeAlignMask(const string &name) = 0;
	virtual CAnimationRandomizer* GetRandomizerSetupInstance(const char* name) = 0;
};

typedef _smart_ptr<IAnimationGraph> IAnimationGraphPtr;

struct SAnimationOverrides
{
	SAnimationOverrides() : overrideStartAfter(false), overrideStartAtKeyFrame(false), startAtKeyFrame(0.5f) {}

	bool overrideStartAfter;
	bool overrideStartAtKeyFrame;
	float startAtKeyFrame;

	void ClearOverrides()
	{
		overrideStartAfter = overrideStartAtKeyFrame = false;
	}
	void StartAtKeyFrame( float frame )
	{
		startAtKeyFrame = frame;
		overrideStartAtKeyFrame = true;
	}
	bool operator<(const SAnimationOverrides& rhs) const
	{
		if (overrideStartAfter < rhs.overrideStartAfter)
			return true;
		else if (overrideStartAfter > rhs.overrideStartAfter)
			return false;
		else if (overrideStartAtKeyFrame < rhs.overrideStartAtKeyFrame)
			return true;
		else if (overrideStartAtKeyFrame > rhs.overrideStartAtKeyFrame)
			return false;
		else if (startAtKeyFrame < rhs.startAtKeyFrame)
			return true;
		else if (startAtKeyFrame > rhs.startAtKeyFrame)
			return false;
		else
			return false;
	}
	bool operator==( const SAnimationOverrides& rhs ) const
	{
		return overrideStartAfter == rhs.overrideStartAfter && overrideStartAtKeyFrame == rhs.overrideStartAtKeyFrame && startAtKeyFrame == rhs.startAtKeyFrame;
	}
	bool operator>( const SAnimationOverrides& rhs ) const
	{
		return !(*this < rhs) && !(*this == rhs);
	}
};



class CAnimationPlayerProxy;

struct SAnimationStateData
{
	static const int MAX_LAYERS = 9;

	SAnimationStateData() : pState(0), pEntity(0), pGameObject(0), pAnimatedCharacter(0), isPaused(false), queryChanged(false), MovementControlMethodH(eMCM_Entity), MovementControlMethodV(eMCM_Entity), ColliderMode(eColliderMode_Pushable), animationControlledView(false), additionalTurnMultiplier(3.0f), hurried(false), canMix(false), overrideTransitionTime(0)
	{
		for (int i=0; i<NUM_ANIMATION_USER_DATA_SLOTS; i++)
			userData[i] = 0.0f;

		animationProxy = NULL;
	}

	class CAnimationGraphState* pState;
	IGameObject* pGameObject;
	IEntity* pEntity;
	class CAnimatedCharacter* pAnimatedCharacter;
	CryCharAnimationParams params[MAX_LAYERS];
	SAnimationOverrides overrides[MAX_LAYERS];
	bool isPaused;
	bool queryChanged;
	uint8 MovementControlMethodH;
	uint8 MovementControlMethodV;
	uint8 ColliderMode;
	bool animationControlledView;
	bool hurried;
	bool canMix;
	float additionalTurnMultiplier;
	float userData[NUM_ANIMATION_USER_DATA_SLOTS];
	float overrideTransitionTime;
	CAnimationPlayerProxy *animationProxy;
};

class CAnimationPlayerProxy
{
public:
	virtual ~CAnimationPlayerProxy(){}

	virtual bool StartAnimation(IEntity *entity, const char* szAnimName0, const CryCharAnimationParams& Params, float speedMultiplier = 1.0f)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		if (pICharacter)
		{
			ISkeletonAnim* pISkeletonAnim = pICharacter->GetISkeletonAnim();

			if (pISkeletonAnim->StartAnimation(szAnimName0, Params))
			{
				pISkeletonAnim->SetLayerBlendMultiplier(Params.m_nLayerID, speedMultiplier);
				return true;
			}
		}
		return false;
	}

	virtual bool StartAnimationById(IEntity *entity, int animId, const CryCharAnimationParams& Params, float speedMultiplier = 1.0f)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		if (pICharacter)
		{
			ISkeletonAnim* pISkeletonAnim = pICharacter->GetISkeletonAnim();

			if (pISkeletonAnim->StartAnimationById(animId, Params))
			{
				pISkeletonAnim->SetLayerBlendMultiplier(Params.m_nLayerID, speedMultiplier);
				return true;
			}
		}
		return false;
	}

	virtual bool StopAnimationInLayer(IEntity *entity, int32 nLayer, f32 BlendOutTime)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		ISkeletonAnim* pISkeletonAnim = pICharacter ? pICharacter->GetISkeletonAnim() : NULL;

		return pISkeletonAnim ? pISkeletonAnim->StopAnimationInLayer(nLayer, BlendOutTime) : false;
	}

	virtual bool RemoveAnimationInLayer(IEntity *entity, int32 nLayer, uint32 token)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		if (pICharacter)
		{
			ISkeletonAnim* pISkeletonAnim = pICharacter->GetISkeletonAnim();

			int nAnimsInFIFO = pISkeletonAnim->GetNumAnimsInFIFO(nLayer);
			for (int i=0; i<nAnimsInFIFO; ++i)
			{
				const CAnimation& anim = pISkeletonAnim->GetAnimFromFIFO(nLayer, i);
				if (anim.m_AnimParams.m_nUserToken == token)
				{
					return pISkeletonAnim->RemoveAnimFromFIFO(nLayer, i);
				}
			}
		}

		return false;
	}

	virtual bool SetAnimTime(IEntity *entity, int32 layer, uint32 token, float fTime)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		if(!pICharacter)
			return false;

		ISkeletonAnim* pISkeletonAnim = pICharacter->GetISkeletonAnim();
		if(!pISkeletonAnim)
			return false;

		int nAnimsInFIFO = pISkeletonAnim->GetNumAnimsInFIFO(layer);
		if (nAnimsInFIFO == 0)
		{
			return false;
		}

		CAnimation* pAnim = NULL;
		if (token == INVALID_ANIMATION_TOKEN)
		{
			pAnim = &pISkeletonAnim->GetAnimFromFIFO(layer, 0);
		}
		else
		{
			pAnim = pISkeletonAnim->FindAnimInFIFO(token, layer, true);
		}
		
		if (pAnim)
			pAnim->m_fAnimTime = fTime;

		return (pAnim != NULL);
	}

	virtual const CAnimation *GetTopAnimation(IEntity *entity, int32 layer)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		if (pICharacter)
		{
			ISkeletonAnim* pISkeletonAnim = pICharacter->GetISkeletonAnim();

			int nAnimsInFIFO = pISkeletonAnim->GetNumAnimsInFIFOAndDeferredQueue(layer);
			if (nAnimsInFIFO > 0)
			{
				return &pISkeletonAnim->GetAnimFromFIFOAndDeferredQueue(layer, nAnimsInFIFO-1);
			}
		}

		return NULL;
	}

	virtual const CAnimation *GetAnimation(IEntity *entity, int32 layer, uint32 token)
	{
		ICharacterInstance* pICharacter = entity->GetCharacter(0);
		if(!pICharacter)
			return NULL;

		ISkeletonAnim* pISkeletonAnim = pICharacter->GetISkeletonAnim();
		if(!pISkeletonAnim)
			return NULL;

		if (token == INVALID_ANIMATION_TOKEN)
		{
			int nAnimsInFIFO = pISkeletonAnim->GetNumAnimsInFIFO(layer);
			if (nAnimsInFIFO == 0)
			{
				return NULL;
			}
			else
			{
				return &pISkeletonAnim->GetAnimFromFIFO(layer, 0);
			}
		}
		else
		{
			return pISkeletonAnim->FindAnimInFIFO(token, layer, true);
		}
	}

protected:
};

struct SAnimationTargetRequest
{
	SAnimationTargetRequest()
		: position(ZERO)
		, direction(FORWARD_DIRECTION)
		, startArcAngle(0.0f)
		, directionTolerance(gf_PI)
		, prepareRadius(3.0f)
		, startWidth(0.0f)
		, projectEnd(false)
		, navSO(false)
	{}
	Vec3 position;
	Vec3 direction;
	float startArcAngle;
	float directionTolerance;
	float prepareRadius;
	float startWidth;
	bool projectEnd;

	// we allow bigger errors in start position while passing through
	// a smart object to avoid too much slowing down in front of it
	bool navSO;

	bool operator==( const SAnimationTargetRequest& rhs ) const
	{
		static const float eps = 1e-3f;
		return position.GetSquaredDistance(rhs.position) < eps
			&& direction.Dot(rhs.direction) > (1.0f - eps)
			&& fabsf(startArcAngle - rhs.startArcAngle) < eps
			&& fabsf(directionTolerance - rhs.directionTolerance) < eps
			&& fabsf(prepareRadius - rhs.prepareRadius) < eps
			&& fabsf(startWidth - rhs.startWidth) < eps
			&& projectEnd == rhs.projectEnd
			&& navSO == rhs.navSO;
	}
	bool operator!=( const SAnimationTargetRequest& rhs ) const
	{
		return !this->operator==(rhs);
	}
};

struct SAnimationTarget
{
	SAnimationTarget()
		: preparing(false)
		, activated(false)
		, doingSomething(false)
		, allowActivation(false)
		, notAiControlledAnymore(false)
		, isNavigationalSO(false)
		, maxRadius(0)
		, position(ZERO)
		, startWidth(0.0f)
		, positionWidth(0.0f)
		, positionDepth(0.0f)
		, orientationTolerance(0.0f)
		, orientation(IDENTITY)
		, activationTimeRemaining(0)
		, errorVelocity(ZERO)
		, errorRotationalVelocity(IDENTITY)
		{}
	uint32 preparing : 1;
	uint32 activated : 1;
	uint32 doingSomething : 1;
	mutable uint32 allowActivation : 1;
	mutable uint32 notAiControlledAnymore : 1;
	uint32 isNavigationalSO : 1;
	float maxRadius;
	Vec3 position;
	float startWidth;
	float positionWidth;
	float positionDepth;
	float orientationTolerance;
	Quat orientation;
	float activationTimeRemaining;
	Vec3 errorVelocity;
	Quat errorRotationalVelocity;
};

//TODO: find a better place for this!!!
struct SPredictedCharacterState
{
	Vec3 position; // Position in world space coordinates.
	Quat orientation; // Orientation in world space coordinates.

	Vec3 velocity; // Linear velocity in world space coordinates.

	float deltatime; // Relative time of prediction.
};
struct SPredictedCharacterStates
{
#ifdef GAME_IS_CRYSIS2
	static const size_t maxStates = 2;
#else  //!GAME_IS_CRYSIS2
	static const size_t maxStates = 10;
#endif //!GAME_IS_CRYSIS2
	SPredictedCharacterState states[maxStates];
	int nStates;
	SPredictedCharacterStates() 
		: nStates(0)
	{}
	SPredictedCharacterStates(const SPredictedCharacterStates& src) 
	{ 
		Copy(src);
	}
	SPredictedCharacterStates& operator=(const SPredictedCharacterStates& src) 
	{ 
		Copy(src);

		return *this; 
	}

	ILINE void Copy(const SPredictedCharacterStates& src)
	{
		const int numStates = src.nStates;
		nStates = numStates;
		for (int i=0; i<numStates; ++i)
		{
			states[i].position		= src.states[i].position;
			states[i].orientation = src.states[i].orientation;
			states[i].velocity		= src.states[i].velocity;
			states[i].deltatime		= src.states[i].deltatime;
		}
	}

	bool IsValid() const
	{
		for (int i = 0; i < nStates; ++i)
		{
			if (!states[i].position.IsValid())
				return false;
			if (!states[i].orientation.IsValid())
				return false;
			if (!states[i].velocity.IsValid())
				return false;
			if (!NumberValid(states[i].deltatime))
				return false;
		}
		return true;
	}

	ILINE float GetMaxDeltaTime() const
	{
		CRY_ASSERT_TRACE(nStates > 0 && nStates <= maxStates, ("nStates=%d out of valid range (1 to %d)", nStates, maxStates));
		return states[nStates-1].deltatime;
	}

	ILINE SPredictedCharacterState GetFirstState() const
	{
		CRY_ASSERT(nStates > 0);
		return states[0];
	}

	ILINE SPredictedCharacterState GetInterpolatedState(float deltatime) const
	{
		CRY_ASSERT_TRACE(nStates > 0 && nStates <= maxStates, ("nStates=%d out of valid range (1 to %d)", nStates, maxStates));

		if (nStates == 1)
			return states[0];

		if (deltatime <= 0.0f)
			return states[0];

		if (deltatime >= GetMaxDeltaTime())
			return states[nStates-1];

		int i = 0;
		CRY_ASSERT((i + 1) < maxStates);
		while (deltatime > states[i+1].deltatime) 
		{
			i++;
			CRY_ASSERT((i + 1) < maxStates);
		}

		float fraction = (deltatime - states[i].deltatime) / (states[i+1].deltatime - states[i].deltatime);
		SPredictedCharacterState state;
		state.position = LERP(states[i].position, states[i+1].position, fraction);
		state.orientation.SetNlerp(states[i].orientation, states[i+1].orientation, fraction); /*state.orientation.Normalize();*/
		state.velocity = LERP(states[i].velocity, states[i+1].velocity, fraction);
		state.deltatime = deltatime;
		return state;
	}

};

typedef uint32 TAnimationGraphQueryID;

struct IAnimationGraphAuxillaryInputs
{
	typedef IAnimationGraph::InputID InputID;
	
	virtual ~IAnimationGraphAuxillaryInputs(){}
	virtual IAnimationGraphState * GetState() = 0;
	virtual void SetInput( InputID, float ) = 0;
	virtual void SetInput( InputID, int ) = 0;
	virtual void SetInput( InputID, const char * ) = 0;

	template <class T>
	inline void SetInput( const char * name, T value );
};

typedef IAnimationGraphAuxillaryInputs IAnimationSpacialTrigger;

UNIQUE_IFACE struct IAnimationGraphExistanceQuery : public IAnimationGraphAuxillaryInputs
{
	/// Execute the query.
	virtual bool Complete() = 0;
	/// Returns animation length after query is Complete() and successful. Otherwise returns CTimeValue(0).
	virtual CTimeValue GetAnimationLength() const = 0;
	virtual void Reset() = 0;
	virtual void Release() = 0;
};

UNIQUE_IFACE struct IAnimationGraphTargetPointVerifier
{
	virtual ~IAnimationGraphTargetPointVerifier(){}
	/// Returns true if the path can be modified to use request.targetPoint, and byproducts 
	/// of the test are cached in request.
	virtual ETriState CanTargetPointBeReached(class CTargetPointRequest &request) const = 0;
	/// Returns true if the request is still valid/can be used, false otherwise.
	virtual bool UseTargetPointRequest(const class CTargetPointRequest &request) = 0;
	virtual void NotifyFinishPoint( const Vec3& pt ) = 0;
	virtual void NotifyAllPointsNotReachable() = 0;
};

struct IAnimationGraphState
{
public:
	typedef IAnimationGraph::InputID InputID;
	
	virtual ~IAnimationGraphState() {}

	// (see lower level versions below)
	template <class T>
	inline bool SetInput( const char * name, T value, TAnimationGraphQueryID * pQueryID = 0 )
	{
		return SetInput( GetInputId(name), value, pQueryID );
	}
	// SetInputOptional is same as SetInput except that it will not set the default input value in case a non-existing value is passed
	inline bool SetInputOptional( const char * name, const char * value, TAnimationGraphQueryID * pQueryID = 0 )
	{
		return SetInputOptional( GetInputId(name), value, pQueryID );
	}
	inline void QueryChangeInput( const char * name, TAnimationGraphQueryID * pQueryID )
	{
		QueryChangeInput( GetInputId(name), pQueryID );
	}
	inline void LockInput( const char * name, bool locked )
	{
		LockInput( GetInputId(name), locked );
	}

	// recurse setting. query mechanism needs to be wrapped by wrapper.
	// Associated QueryID will be given to QueryComplete when ALL layers supporting the input have reached their matching states.
	// wrapper generates it's own query IDs which are associated to a bunch of sub IDs with rules for how to handle the sub IDs into wrapped IDs.
	virtual bool SetInput( InputID, float, TAnimationGraphQueryID * pQueryID = 0 ) = 0;
	virtual bool SetInput( InputID, int, TAnimationGraphQueryID * pQueryID = 0 ) = 0;
	virtual bool SetInput( InputID, const char *, TAnimationGraphQueryID * pQueryID = 0 ) = 0;
	// SetInputOptional is same as SetInput except that it will not set the default input value in case a non-existing value is passed
	virtual bool SetInputOptional( InputID, const char *, TAnimationGraphQueryID * pQueryID = 0 ) = 0;
	virtual void ClearInput( InputID ) = 0;
	virtual void LockInput( InputID, bool locked ) = 0;

	virtual bool SetVariationInput( const char* name, const char* value ) = 0;
	virtual bool SetVariationInput( InputID inputID, const char* value ) = 0;

	// assert all equal, use any (except if signalled, then return the one not equal to default, or return default of all default)
	virtual void GetInput( InputID, char * ) const = 0;

	// get input from specific layer if layer index is valid for this state
	virtual void GetInput( InputID, char *, int layerIndex ) const = 0;

	// AND all layers
	virtual bool IsDefaultInputValue( InputID ) const = 0;

	// returns NULL if InputID is out of range
	virtual const char* GetInputName( InputID ) const = 0;
	virtual const char* GetVariationInputName( InputID ) const = 0;

	// When QueryID of SetInput (reached queried state) is emitted this function is called by the outside, by convention(verify!).
	// Remember which layers supported the SetInput query and emit QueryLeaveState QueryComplete when all those layers have left those states.
	virtual void QueryLeaveState( TAnimationGraphQueryID * pQueryID ) = 0;

	// assert all equal, forward to all layers, complete when all have changed once (trivial, since all change at once via SetInput).
	// (except for signalled, forward only to layers which currently are not default, complete when all those have changed).
	virtual void QueryChangeInput( InputID, TAnimationGraphQueryID * ) = 0;

	// Just register and non-selectivly call QueryComplete on all listeners (regardless of what ID's they are actually interested in).
	virtual void AddListener( const char * name, IAnimationGraphStateListener * pListener ) = 0;
	virtual void RemoveListener( IAnimationGraphStateListener * pListener ) = 0;

	// Not used
	virtual bool DoesInputMatchState( InputID) const = 0;

	// TODO: This should be turned into registered callbacks or something instead (look at AnimationGraphStateListener).
	// Use to access the SelectLocomotionState() callback in CAnimatedCharacter.
	// Only set for fullbody, null for upperbody.
	virtual void SetAnimatedCharacter( class CAnimatedCharacter* animatedCharacter, int layerIndex, IAnimationGraphState* parentLayerState ) = 0;

	// simply recurse
	virtual bool Update() = 0;
	virtual void Release() = 0;
	virtual void ForceTeleportToQueriedState() = 0;

	// simply recurse (will be ignored by each layer individually if state not found)
	virtual void PushForcedState( const char * state, TAnimationGraphQueryID * pQueryID = 0 ) = 0;

	// simply recurse
	virtual void ClearForcedStates() = 0;

	// simply recurse
	virtual void SetBasicStateData( const SAnimationStateData& ) = 0;

	// same as GetInput above
	virtual float GetInputAsFloat( InputID inputId ) = 0;

	// wrapper generates it's own input IDs for the union of all inputs in all layers, and for each input it maps to the layer specific IDs.
	virtual InputID GetInputId( const char *input ) = 0;
	virtual InputID GetVariationInputId( const char* variationInputName ) const = 0;

	// simply recurse (preserve order), and don't forget to serialize the wrapper stuff, ID's or whatever.
	virtual void Serialize( TSerialize ser ) = 0;

	// simply recurse
	virtual void SetAnimationActivation( bool activated ) = 0;
	virtual bool GetAnimationActivation() = 0;

	// Concatenate all layers state names with '+'. Use only fullbody layer state name if upperbody layer is not allowed/mixed.
	virtual const char * GetCurrentStateName() = 0;

	// don't expose (should only be called on specific layer state directly, by AGAnimation)
	//virtual void ForceLeaveCurrentState() = 0;
	//virtual void InvalidateQueriedState() = 0;

	// simply recurse
	virtual void Pause( bool pause, EAnimationGraphPauser pauser, float fOverrideTransTime = -1.0f ) = 0;

	// is the same for all layers (equal assertion should not even be needed)
	virtual bool IsInDebugBreak() = 0;

	// don't expose (not used) (if used outside AGAnimation, specify layer)
	//virtual CTimeValue GetAnimationLength() = 0;

	// don't expose (only used by AGOutput)
	//virtual void SetOutput( int id ) = 0;
	//virtual void ClearOutput( int id ) = 0;

	// find highest layer that has output id, or null (this allows upperbody to override fullbody).
	// Use this logic when calling SetOutput on listeners.
	virtual const char * QueryOutput( const char * name ) = 0;

	// Don't expose (only used on specific layer in AGParams)
	//virtual void SetCurrentStructure( const CCryName& ) = 0;
	//virtual const CCryName& GetCurrentStructure() = 0;

	// don't expose (exact positioning uses fullbody layer only, and AGAnimation operates on specific layer already)
	//virtual uint32 GetCurrentToken() = 0;

	// Exact positioning: Forward to fullbody layer only (hardcoded)
	virtual IAnimationSpacialTrigger * SetTrigger( const SAnimationTargetRequest& req, EAnimationGraphTriggerUser user, TAnimationGraphQueryID * pQueryStart, TAnimationGraphQueryID * pQueryEnd ) = 0;
	virtual void ClearTrigger( EAnimationGraphTriggerUser user ) = 0;
	virtual const SAnimationTarget* GetAnimationTarget() = 0;
	virtual bool HasAnimationTarget() const = 0;
	virtual void SetTargetPointVerifier( IAnimationGraphTargetPointVerifier * ) = 0;
	virtual bool IsUpdateReallyNecessary() = 0;

	// Creates an object you can use to test whether a specific combination of inputs will select a state
	// (and to get a bit more information about that state)
	virtual IAnimationGraphExistanceQuery * CreateExistanceQuery() = 0;
	virtual IAnimationGraphExistanceQuery * CreateExistanceQuery(int layer) = 0;

	// simply recurse
	virtual void Reset() = 0;

	// we've been idle for a while, try to catch up and disrespect blending laws
	// simply recurse
	virtual void SetCatchupFlag() = 0;

	// (hardcoded forward to fullbody layer only) (used for exact positioning trigger and PMC::UpdateMovementState()).
	virtual Vec2 GetQueriedStateMinMaxSpeed() = 0;

	// simply recurse (hurry all layers, let them hurry independently where they can)
	virtual void Hurry() = 0;

	// simply recurse (first person skippable states are skipped independently by each layer)
	virtual void SetFirstPersonMode( bool on ) = 0;

	// Removed (remove this from original interface as well., including SetIWeights
	//virtual uint16 GetBlendSpaceWeightFlags() = 0;
	//virtual void  SetBlendSpaceWeightFlags(uint16 flags) = 0;

	// simply recurse (will add all layer's containers to the sizer)
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const =0;

	// the wrapper simply returns false
	virtual bool IsMixingAllowedForCurrentState() const = 0;

	// used by CAnimationGraphStates
	virtual bool IsSignalledInput( InputID intputId ) const = 0;
};


template <class T>
inline void IAnimationGraphAuxillaryInputs::SetInput( const char * name, T value )
{
	SetInput( GetState()->GetInputId(name), value );
}

struct IAnimationGraphStateListener
{
	virtual ~IAnimationGraphStateListener(){}
	virtual void SetOutput( const char * output, const char * value ) = 0;
	virtual void QueryComplete( TAnimationGraphQueryID queryID, bool succeeded ) = 0;
	virtual void DestroyedState(IAnimationGraphState* ) = 0;
};

template <class T> struct AnimGraph_GetTypeIdForType;
template <> struct AnimGraph_GetTypeIdForType<int>          { static const EAnimationGraphInputType value = eAGIT_Integer; };
template <> struct AnimGraph_GetTypeIdForType<float>        { static const EAnimationGraphInputType value = eAGIT_Float; };
template <> struct AnimGraph_GetTypeIdForType<const char *> { static const EAnimationGraphInputType value = eAGIT_String; };

#endif
