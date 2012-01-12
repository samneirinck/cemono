#include "StdAfx.h"
#include "NetPlayerInput.h"
#include "Player.h"
#include "Game.h"
#include "GameCVars.h"
// PLAYERPREDICTION
#include "Weapon.h"
// ~PLAYERPREDICTION
#include "IAIActor.h"

// PLAYERPREDICTION
#include "Utility/CryWatch.h"
#include "Cry_GeoDistance.h"
// ~PLAYERPREDICTION

/*
moveto (P1);                            // move pen to startpoint
for (int t=0; t < steps; t++)
{
float s = (float)t / (float)steps;    // scale s to go from 0 to 1
float h1 =  2s^3 - 3s^2 + 1;          // calculate basis function 1
float h2 = -2s^3 + 3s^2;              // calculate basis function 2
float h3 =   s^3 - 2*s^2 + s;         // calculate basis function 3
float h4 =   s^3 -  s^2;              // calculate basis function 4
vector p = h1*P1 +                    // multiply and sum all funtions
h2*P2 +                    // together to build the interpolated
h3*T1 +                    // point along the curve.
h4*T2;
lineto (p)                            // draw to calculated point on the curve
}*/

static Vec3 HermiteInterpolate( float s, const Vec3& p1, const Vec3& t1, const Vec3& p2, const Vec3& t2 )
{
	float s2 = s*s;
	float s3 = s2*s;
	float h1 = 2*s3 - 3*s2 + 1.0f;
	float h2 = -2*s3 + 3*s2;
	float h3 = s3 - 2*s2 + s;
	float h4 = s3 - s2;
	return h1*p1 + h2*p2 + h3*t1 + h4*t2;
}


// PLAYERPREDICTION
CNetPlayerInput::CNetPlayerInput( CPlayer * pPlayer ) 
: 
	m_pPlayer(pPlayer),
	m_netDesiredSpeed(0.0f),
	m_netLerpSpeed(0.0f),
	m_passedNetPos(true),
	m_passedPredictionPos(true),
	m_newInterpolation(false),
	m_blockedTime(0.0f)
{
	m_breadCrumb.Set(0.0f, 0.0f, 0.0f);
	m_predictedPosition.Set(0.0f, 0.0f, 0.0f);
	m_lookDir(0.0f, 1.0f, 0.0f);
// ~PLAYERPREDICTION
}
// PLAYERPREDICTION
static const float k_timePerBC											= 0.25f;	// time per BC drop
static const float k_BCMovingRatio									= 0.1f;		// min dist towards target expected within a BC update
static const float k_reachedTargetAllowedDeviation	= 1.0f;
static const float k_maxBlockTime										= 2.0f;		// max possible time to be blocked for
static const float k_minInterpolateSpeed						= 0.15f;		// min speed that any interpolation can take place at (get reduced later to account for dt)
static const float k_maxInterpolateSpeed						= 12.0f;		// max speed that any interpolation can take place at (get reduced later to account for dt)
static const float k_maxPredictTime									= 1.0f;   // max time over which any prediction can take place
static const float k_maxInterpolateDist							= 4.0f;		// beyond this snap the actor
static const float k_allowedHeightDiffInAir					= 5.0f;
static const float k_allowedHeightDiff							= 2.0f;
static const float k_onGroundTime										= 0.5f;
static volatile float k_lerpTargetTime							= 0.2f;
static volatile float k_staticSpeed									= 0.1f;

static const float k_minDistStatic = 0.1f;
static const float k_minDistMoving = 0.001f;

void CNetPlayerInput::InitialiseInterpolation(f32 netPosDist, const Vec3 &desPosOffset, const Vec3 &desiredVelocity, const CTimeValue	&curTime)
{
	m_newInterpolation = false;

	//--- Don't trigger a fresh interpolation for minor adjustments if the player is on the ground & stopping or 
	//--- would be moving against the local player's velocity
	bool isStatic = (m_pPlayer->GetActorStats()->onGround > 0.1f) && ((m_netDesiredSpeed < k_staticSpeed) || (desPosOffset.Dot(desiredVelocity) < 0.0f));
	const float minDist = isStatic ? k_minDistStatic : k_minDistMoving;

	if (g_pGameCVars->pl_debugInterpolation)
	{
		CryWatch("NewInterp NetPosDist: %f MinDist: %f)", netPosDist, minDist);
	}

	m_netLastUpdate = curTime;

	if (netPosDist > minDist)
	{
		if (m_netDesiredSpeed > k_staticSpeed)
		{
			m_initialDir = desiredVelocity / m_netDesiredSpeed;
		}
		else
		{
			m_initialDir = desPosOffset / netPosDist;
		}

		// Always set position when an update is first received.
		m_passedNetPos = false;
		m_passedPredictionPos = false;
	}
}

void CNetPlayerInput::UpdateErrorSnap(const Vec3 &entPos, const Vec3 &desiredPos, f32 netPosDist, const Vec3 &desPosOffset, const CTimeValue &curTime)
{
	if (g_pGameCVars->pl_velocityInterpAlwaysSnap)
	{
		//--- Snap to target
		m_pPlayer->GetEntity()->SetPos(desiredPos);
		m_passedNetPos = true;
		m_passedPredictionPos = true;
	}
	else if (curTime > m_nextBCTime && !(m_pPlayer->m_stats.inFreefall.Value() > 0))
	{
		//--- Breadcrumbs dropped for simulated annealing, whilst the player is blocked we reduce the maximum
		//--- allowed offset until the character is snapped to the last net position
		Vec3 prevOffset = entPos - m_breadCrumb;
		bool blocked = false;
		if (!m_passedNetPos)
		{
			float moveDist = prevOffset.Dot(desPosOffset) / netPosDist;
			float moveSpeed  = moveDist / k_timePerBC;
			blocked = ((moveSpeed / m_netLerpSpeed) <= k_BCMovingRatio);
		}
		if (blocked)
		{
			m_blockedTime += k_timePerBC;
		}
		else
		{
			m_blockedTime = 0.0f;
		}

		m_breadCrumb = entPos;
		m_nextBCTime = curTime+k_timePerBC;

		Vec3 v0(desiredPos.x, desiredPos.y, 0.0f);
		Vec3 v1(m_predictedPosition.x, m_predictedPosition.y, 0.0f);
		Vec3 pt(entPos.x, entPos.y, 0.0f);
		Lineseg validPath(v0, v1);
		float t;
		float distSqrXY = Distance::Point_LinesegSq(pt, validPath, t);

		float heightDiff = m_predictedPosition.z - entPos.z;
		float blockedFactor = (1.0f - (m_blockedTime / k_maxBlockTime));
		float maxDistXY = !m_passedNetPos ? blockedFactor * k_maxInterpolateDist : k_reachedTargetAllowedDeviation;
		float timeOnGround = m_pPlayer->GetActorStats()->onGround;
		float allowedHeightDiff = (float)__fsel(timeOnGround - k_onGroundTime, k_allowedHeightDiff, k_allowedHeightDiffInAir);
		if ((distSqrXY > (maxDistXY * maxDistXY)) || (cry_fabsf(heightDiff) > allowedHeightDiff))
		{
			//--- Snap to target
			m_pPlayer->GetEntity()->SetPos(desiredPos);
			m_passedNetPos = true;
			const bool isfallen = m_pPlayer->m_stats.inFreefall.Value()==1;
		}
	}
}

void CNetPlayerInput::UpdateInterpolation()
{
	Vec3 desiredPosition = m_curInput.position;

	Vec3 entPos = m_pPlayer->GetEntity()->GetPos();
	Vec3 displacement = desiredPosition - entPos;
	displacement.z = 0.0f;
	float dist = displacement.len();

	CTimeValue	curTime=gEnv->pTimer->GetFrameStartTime();
	Vec3 desiredVelocity = m_curInput.deltaMovement * g_pGameCVars->pl_netSerialiseMaxSpeed;
	m_netDesiredSpeed = desiredVelocity.GetLength2D();

	if (m_newInterpolation)
	{
		InitialiseInterpolation(dist, displacement, desiredVelocity, curTime);
	}

	float dt = curTime.GetDifferenceInSeconds(m_netLastUpdate) + k_lerpTargetTime;
	dt = min(dt, k_maxPredictTime);
	m_predictedPosition = desiredPosition + (desiredVelocity * dt);

	Vec3 predOffset = m_predictedPosition - entPos;
	float predDist = predOffset.GetLength2D();

	float lerpSpeed = (predDist/k_lerpTargetTime);
	lerpSpeed=clamp(lerpSpeed, k_minInterpolateSpeed, k_maxInterpolateSpeed);

	m_netLerpSpeed = lerpSpeed;

	UpdateErrorSnap(entPos, desiredPosition, dist, displacement, curTime);

	if (!m_passedNetPos && (m_initialDir.Dot(displacement) < 0.0f))
	{
		m_passedNetPos = true;
	}
	Vec3 maxPrediction = desiredPosition + (desiredVelocity * k_maxPredictTime);
	if (m_passedNetPos && !m_passedPredictionPos && (m_initialDir.Dot(maxPrediction - entPos) < 0.0f))
	{
		m_passedPredictionPos = true;
	}

#if !defined(_RELEASE)

	if (g_pGameCVars->pl_debugInterpolation)
	{
		CryWatch("Cur: (%f, %f, %f) Des: (%f, %f, %f) Pred: (%f, %f, %f) ", entPos.x, entPos.y, entPos.z, desiredPosition.x, desiredPosition.y, desiredPosition.z, m_predictedPosition.x, m_predictedPosition.y, m_predictedPosition.z);
		CryWatch("BlockTime: (%f) PredictTime (%f) LastNetTime (%f) CurTime (%f)", m_blockedTime, dt, m_netLastUpdate.GetSeconds(), curTime.GetSeconds());
		CryWatch("Lerp Speed: (%f) Passed pred pos (%d) Passed net pos (%d)", m_netLerpSpeed, m_passedPredictionPos, m_passedNetPos);
		CryWatch("InputSpeed: (%f, %f, %f) ", desiredVelocity.x, desiredVelocity.y, desiredVelocity.z);

		IRenderAuxGeom* pRender = gEnv->pRenderer->GetIRenderAuxGeom();

		SAuxGeomRenderFlags flags = pRender->GetRenderFlags();
		SAuxGeomRenderFlags oldFlags = pRender->GetRenderFlags();
		flags.SetDepthWriteFlag(e_DepthWriteOff);
		flags.SetDepthTestFlag(e_DepthTestOff);
		pRender->SetRenderFlags(flags);
		pRender->DrawSphere(desiredPosition + Vec3(0.0f, 0.0f, 0.035f), 0.07f, ColorB(255,0,0,255));
		pRender->DrawSphere(m_predictedPosition + Vec3(0.0f, 0.0f, 0.025f), 0.05f, ColorB(255,255,255,255));
		pRender->SetRenderFlags(oldFlags);

		ColorF ballCol = m_passedPredictionPos ? ColorF(1.0f,1.0f,0.0f,0.75f) : (m_passedNetPos ? ColorF(1.0f,1.0f,1.0f,0.75f) : ColorF(0.0f,1.0f,0.0f,0.75f));

		g_pGame->GetIGameFramework()->GetIPersistantDebug()->Begin("INTERPOLATION TRAIL", false);
		g_pGame->GetIGameFramework()->GetIPersistantDebug()->AddSphere(desiredPosition + Vec3(0.0f, 0.0f, 0.1f),  0.04f, ColorF(1.0f,0.0f,0.0f,0.75f), 30.f);
		g_pGame->GetIGameFramework()->GetIPersistantDebug()->AddSphere(m_predictedPosition + Vec3(0.0f, 0.0f, 0.1f),  0.03f, ColorF(0.0f,0.0f,1.0f,0.8f), 30.f);

		ballCol.a = 1.0f;
		g_pGame->GetIGameFramework()->GetIPersistantDebug()->AddSphere(entPos + Vec3(0.0f, 0.0f, 0.1f),  0.02f, ballCol, 30.f);
		g_pGame->GetIGameFramework()->GetIPersistantDebug()->AddLine(entPos + Vec3(0.0f, 0.0f, 0.1f), m_predictedPosition + Vec3(0.0f, 0.0f, 0.1f), ballCol, 30.f);
	}

#endif //!_RELEASE
}

void CNetPlayerInput::UpdateMoveRequest()
{
	CMovementRequest moveRequest;
	SMovementState moveState;
	m_pPlayer->GetMovementController()->GetMovementState(moveState);
	Quat worldRot = m_pPlayer->GetBaseQuat(); // m_pPlayer->GetEntity()->GetWorldRotation();
	Vec3 deltaMovement = worldRot.GetInverted().GetNormalized() * m_curInput.deltaMovement;
	// absolutely ensure length is correct
	deltaMovement = deltaMovement.GetNormalizedSafe(ZERO) * m_curInput.deltaMovement.GetLength();
	moveRequest.AddDeltaMovement( deltaMovement );
	if( IsDemoPlayback() )
	{
		Vec3 localVDir(m_pPlayer->GetViewQuatFinal().GetInverted() * m_curInput.lookDirection);
		Ang3 deltaAngles(asinf(localVDir.z),0,cry_atan2f(-localVDir.x,localVDir.y));
		moveRequest.AddDeltaRotation(deltaAngles*gEnv->pTimer->GetFrameTime());
	}
	//else
	{
		//--- Vector slerp actually produces QNans if the vectors are exactly opposite, in that case snap to the target
		if (m_lookDir.Dot(m_curInput.lookDirection) < (float)-0.99f)
		{
			m_lookDir = m_curInput.lookDirection;
		}
		else
		{
			m_lookDir.SetSlerp(m_lookDir, m_curInput.lookDirection, g_pGameCVars->pl_netAimLerpFactor);
		}

		Vec3 distantTarget = moveState.eyePosition + 1000.0f * m_lookDir;
		Vec3 lookTarget = distantTarget;

		if (m_curInput.usinglookik)
			moveRequest.SetLookTarget( lookTarget );
		else
			moveRequest.ClearLookTarget();

		if (m_curInput.aiming)
			moveRequest.SetAimTarget( lookTarget );
		else
			moveRequest.ClearAimTarget();

		if (m_curInput.deltaMovement.GetLengthSquared() > sqr(0.02f)) // 0.2f is almost stopped
			moveRequest.SetBodyTarget( distantTarget );
		else
			moveRequest.ClearBodyTarget();
	}

	moveRequest.SetAllowStrafing(m_curInput.allowStrafing);

	if(m_pPlayer->IsPlayer())
		moveRequest.SetPseudoSpeed(CalculatePseudoSpeed());
	else
		moveRequest.SetPseudoSpeed(m_curInput.pseudoSpeed);

	float lean=0.0f;
	if (m_curInput.leanl)
		lean-=1.0f;
	if (m_curInput.leanr)
		lean+=1.0f;

	if (fabsf(lean)>0.01f)
		moveRequest.SetLean(lean);
	else
		moveRequest.ClearLean();

	moveRequest.SetStance( (EStance)m_curInput.stance );

	m_pPlayer->GetMovementController()->RequestMovement(moveRequest);

	if (m_curInput.sprint)
		m_pPlayer->m_actions |= ACTION_SPRINT;
	else
		m_pPlayer->m_actions &= ~ACTION_SPRINT;

	if (m_curInput.leanl)
		m_pPlayer->m_actions |= ACTION_LEANLEFT;
	else
		m_pPlayer->m_actions &= ~ACTION_LEANLEFT;

	if (m_curInput.leanr)
		m_pPlayer->m_actions |= ACTION_LEANRIGHT;
	else
		m_pPlayer->m_actions &= ~ACTION_LEANRIGHT;

#if !defined(_RELEASE)
	// debug..
	if (g_pGameCVars->g_debugNetPlayerInput & 2)
	{
		IPersistantDebug * pPD = gEnv->pGame->GetIGameFramework()->GetIPersistantDebug();
		pPD->Begin( string("update_player_input_") + m_pPlayer->GetEntity()->GetName(), true );
		Vec3 wp = m_pPlayer->GetEntity()->GetWorldPos();
		wp.z += 2.0f;
		pPD->AddSphere( moveRequest.GetLookTarget(), 0.5f, ColorF(1,0,1,0.3f), 1.0f );
		//		pPD->AddSphere( moveRequest.GetMoveTarget(), 0.5f, ColorF(1,1,0,0.3f), 1.0f );
		pPD->AddDirection( m_pPlayer->GetEntity()->GetWorldPos() + Vec3(0,0,2), 1, m_curInput.deltaMovement, ColorF(1,0,0,0.3f), 1.0f );
	}
#endif

	//m_curInput.deltaMovement.zero();
}


void CNetPlayerInput::PreUpdate()
{
	IPhysicalEntity * pPhysEnt = m_pPlayer->GetEntity()->GetPhysics();

	if (pPhysEnt && !m_pPlayer->IsDead())
	{
		if (m_pPlayer->AllowPhysicsUpdate(m_curInput.physCounter) && m_pPlayer->IsRemote() && HasReceivedUpdate())
		{
			UpdateInterpolation();
		}
		else
		{
			m_newInterpolation = false;
			m_passedNetPos = true;
			m_passedPredictionPos = true;
		}

		UpdateMoveRequest();
	}
}
// ~PLAYERPREDICTION

void CNetPlayerInput::Update()
{
	if (gEnv->bServer && (g_pGameCVars->sv_input_timeout>0) && ((gEnv->pTimer->GetFrameStartTime()-m_lastUpdate).GetMilliSeconds()>=g_pGameCVars->sv_input_timeout))
	{
		m_curInput.deltaMovement.zero();
		// PLAYERPREDICTION
		m_curInput.sprint=false;
		m_curInput.stance=(uint8)STANCE_NULL;

		CHANGED_NETWORK_STATE(m_pPlayer,  IPlayerInput::INPUT_ASPECT );
		// ~PLAYERPREDICTION
	}

	// Disallow animation movement control for remotely controlled
	// characters.  We'll rely on the interpolation to server-controlled
	// location & rotation.
	if( m_pPlayer && m_pPlayer->GetAnimatedCharacter() )
	{
		IAnimatedCharacter* pAnimChar = m_pPlayer->GetAnimatedCharacter();
		pAnimChar->SetMovementControlMethods( eMCM_Entity, eMCM_Entity );
		pAnimChar->UseAnimationMovementForEntity( false, false, false );
	}
}

void CNetPlayerInput::PostUpdate()
{
}

void CNetPlayerInput::SetState( const SSerializedPlayerInput& input )
{
	DoSetState(input);

	m_lastUpdate = gEnv->pTimer->GetCurrTime();
}

void CNetPlayerInput::GetState( SSerializedPlayerInput& input )
{
	input = m_curInput;
}

void CNetPlayerInput::Reset()
{
	SSerializedPlayerInput i(m_curInput);
	i.leanl=i.leanr=i.sprint=false;
	i.deltaMovement.zero();

	DoSetState(i);

	m_pPlayer->GetGameObject()->ChangedNetworkState(IPlayerInput::INPUT_ASPECT);
}

void CNetPlayerInput::DisableXI(bool disabled)
{
}

// PLAYERPREDICTION
float CNetPlayerInput::CalculatePseudoSpeed() const
{
	float pseudoSpeed = m_pPlayer->CalculatePseudoSpeed(m_curInput.sprint);
	return pseudoSpeed;
}
// ~PLAYERPREDICTION

void CNetPlayerInput::DoSetState(const SSerializedPlayerInput& input )
{
	// PLAYERPREDICTION
	m_newInterpolation |= (input.position != m_curInput.position) || (input.deltaMovement != m_curInput.deltaMovement);

	m_curInput = input;
	CHANGED_NETWORK_STATE(m_pPlayer,  IPlayerInput::INPUT_ASPECT );

	// not having these set seems to stop a remote avatars rotation being reflected
	m_curInput.aiming = true;
	m_curInput.allowStrafing = true;
	m_curInput.usinglookik = true;
	// ~PLAYERPREDICTION

	IAIActor* pAIActor = CastToIAIActorSafe(m_pPlayer->GetEntity()->GetAI());
	if (pAIActor)
		pAIActor->GetState().bodystate=input.bodystate;

	CMovementRequest moveRequest;
	moveRequest.SetStance( (EStance)m_curInput.stance );

	if(IsDemoPlayback())
	{
		Vec3 localVDir(m_pPlayer->GetViewQuatFinal().GetInverted() * m_curInput.lookDirection);
		Ang3 deltaAngles(asinf(localVDir.z),0,cry_atan2f(-localVDir.x,localVDir.y));
		moveRequest.AddDeltaRotation(deltaAngles*gEnv->pTimer->GetFrameTime());
	}
	{
		if (m_curInput.usinglookik)
			moveRequest.SetLookTarget( m_pPlayer->GetEntity()->GetWorldPos() + 10.0f * m_curInput.lookDirection );
		else
			moveRequest.ClearLookTarget();
		if (m_curInput.aiming)
			moveRequest.SetAimTarget(moveRequest.GetLookTarget());
		else
			moveRequest.ClearAimTarget();
	}
/*
	float pseudoSpeed = 0.0f; 
	if (m_curInput.deltaMovement.len2() > 0.0f)
	{
		pseudoSpeed = m_pPlayer->CalculatePseudoSpeed(m_curInput.sprint);
	}
	*/
	// PLAYERPREDICTION
	moveRequest.SetPseudoSpeed(CalculatePseudoSpeed());
	// ~PLAYERPREDICTION
	moveRequest.SetAllowStrafing(input.allowStrafing);

	float lean=0.0f;
	if (m_curInput.leanl)
		lean-=1.0f;
	if (m_curInput.leanr)
		lean+=1.0f;
	moveRequest.SetLean(lean);

	m_pPlayer->GetMovementController()->RequestMovement(moveRequest);

	IAnimationGraphState *pState=0;
	if (m_pPlayer->GetAnimatedCharacter())
		pState=m_pPlayer->GetAnimatedCharacter()->GetAnimationGraphState();
		
// PLAYERPREDICTION
	if (pState)
	{
		pState->SetInput(m_pPlayer->m_inputAiming, m_curInput.aiming);
		pState->SetInput(m_pPlayer->m_inputUsingLookIK, m_curInput.usinglookik);
	}

#if !defined(_RELEASE)
	// debug..
	if (g_pGameCVars->g_debugNetPlayerInput & 1)
	{
		IPersistantDebug * pPD = gEnv->pGame->GetIGameFramework()->GetIPersistantDebug();
		pPD->Begin( string("net_player_input_") + m_pPlayer->GetEntity()->GetName(), true );
		pPD->AddSphere( moveRequest.GetLookTarget(), 0.5f, ColorF(1,0,1,1), 1.0f );
		//			pPD->AddSphere( moveRequest.GetMoveTarget(), 0.5f, ColorF(1,1,0,1), 1.0f );

		Vec3 wp(m_pPlayer->GetEntity()->GetWorldPos() + Vec3(0,0,2));
		pPD->AddDirection( wp, 1.5f, m_curInput.deltaMovement, ColorF(1,0,0,1), 1.0f );
		pPD->AddDirection( wp, 1.5f, m_curInput.lookDirection, ColorF(0,1,0,1), 1.0f );
	}
#endif
// ~PLAYERPREDICTION
}

// PLAYERPREDICTION
void CNetPlayerInput::GetDesiredVel(const Vec3 &pos, Vec3 &vel) const
{
	bool doInterp = !m_passedPredictionPos;
	vel.Set(0.0f, 0.0f, 0.0f);
	if (doInterp)
	{
		Vec3 offset = m_predictedPosition - pos;
		offset.z = 0.0f;
		float dist  = offset.GetLength2D();
		if (dist > 0.0f)
		{
			vel = offset * (m_netLerpSpeed / dist);

			float parallelSpeed = m_initialDir.Dot(vel);
			Vec3	parallelVel		= m_initialDir * parallelSpeed;
			Vec3  pathCorrection = vel - parallelVel;
			vel = parallelVel + (g_pGameCVars->pl_velocityInterpPathCorrection * pathCorrection);

			if (g_pGameCVars->pl_debugInterpolation)
			{
				CryWatch("Offset: (%f, %f, %f) InitDir: (%f, %f, %f) DesiredVel: (%f, %f, %f)", offset.x, offset.y, offset.z, m_initialDir.x, m_initialDir.y, m_initialDir.z, vel.x, vel.y, vel.z);
			}
		}
	}
}
// ~PLAYERPREDICTION
