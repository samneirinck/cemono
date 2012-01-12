#include "StdAfx.h"
#include "Flyer.h"
#include "FlyerMovementController.h"
#include "GameUtils.h"


CFlyer::CFlyer() : 
	m_vDesiredVelocity(ZERO),
	m_qDesiredRotation(IDENTITY)
{	
	memset(&m_moveRequest.prediction, 0, sizeof(m_moveRequest.prediction));
}


void CFlyer::FullSerialize(TSerialize ser)
{
	CPlayer::FullSerialize(ser);

	ser.BeginGroup("CFlyer");
	ser.Value("vDesiredVelocity", m_vDesiredVelocity);
	ser.Value("qDesiredRotation", m_qDesiredRotation);
	ser.EndGroup();
}


void CFlyer::PrePhysicsUpdate()
{
	if (m_stats.isRagDoll)
		return;

	if (GetHealth() <= 0.f)
		return;

	IEntity* pEntity = GetEntity();
	if (pEntity->IsHidden())
		return;

	if (IPhysicalEntity* pPhysicalEntity = pEntity->GetPhysics())
	{
		pe_player_dynamics player_dynamics;
		player_dynamics.gravity.zero();
		pPhysicalEntity->SetParams(&player_dynamics);
	}

	float frameTime = gEnv->pTimer->GetFrameTime();

	if (m_pMovementController)
	{
		SActorFrameMovementParams params;
		m_pMovementController->Update(frameTime, params);
	}

	if (m_linkStats.CanMove() && m_linkStats.CanRotate())
	{
		ProcessMovement(frameTime);

		if (m_pAnimatedCharacter)
		{
			m_pAnimatedCharacter->AddMovement(m_moveRequest);
		}
	}
}


void CFlyer::Revive(bool bFromInit)
{
	CPlayer::Revive(bFromInit);

	SetDesiredVelocity(Vec3Constants<float>::fVec3_Zero);
	SetDesiredDirection(GetEntity()->GetWorldTM().GetColumn1());
}


void CFlyer::GetActorInfo(SBodyInfo& bodyInfo)
{
	IEntity* pEntity = GetEntity();

	bodyInfo.vEyePos = pEntity->GetSlotWorldTM(0) * m_eyeOffset;
	bodyInfo.velocity = m_velocity;
	bodyInfo.eStance = m_stance;
	bodyInfo.pStanceInfo = GetStanceInfo(m_stance);
}


void CFlyer::SetActorMovement(SMovementRequestParams& movementRequestParams)
{
	SMovementState state;
	GetMovementController()->GetMovementState(state);

	if (movementRequestParams.vMoveDir.IsZero())
	{
		Vec3 vDesiredDirection = movementRequestParams.vLookTargetPos.IsZero()
			? GetEntity()->GetWorldRotation() * FORWARD_DIRECTION
			: (movementRequestParams.vLookTargetPos - state.eyePosition).GetNormalizedSafe();
		SetDesiredDirection(vDesiredDirection);
		SetDesiredVelocity(Vec3Constants<float>::fVec3_Zero);
	}
	else
	{
		Vec3 vDesiredDirection = movementRequestParams.vLookTargetPos.IsZero()
			? movementRequestParams.vMoveDir.GetNormalizedSafe()
			: (movementRequestParams.vLookTargetPos - state.eyePosition).GetNormalizedSafe();
		SetDesiredDirection(vDesiredDirection);
		SetDesiredVelocity(movementRequestParams.vMoveDir * movementRequestParams.fDesiredSpeed);
	}
}


IActorMovementController* CFlyer::CreateMovementController()
{
	return new CFlyerMovementController(this);
}


// Ad-hoc
void CFlyer::ProcessMovement(float frameTime)
{
	frameTime = min(1.f, frameTime);

	float desiredSpeed = m_vDesiredVelocity.GetLength();
	const float maxDeltaSpeed = 100.f;

	float deltaSpeed = min(maxDeltaSpeed, fabsf(desiredSpeed - m_stats.speed));

	// Advance "m_velocity" towards "m_vDesiredVelocity" at the speed proportional to (1 / square(deltaSpeed))
	Interpolate(
		m_velocity,
		m_vDesiredVelocity,
		2.5f * ((deltaSpeed > 0.f) ? min(frameTime, 2.f / square(deltaSpeed)) : frameTime),
		1.f);

	Quat desiredVelocityQuat = m_qDesiredRotation;

	// pitch/roll
	if (desiredSpeed > 0.f && m_stats.speed > 0.f)
	{
		const Vec3& vUp = Vec3Constants<float>::fVec3_OneZ;
		Vec3 vForward = m_velocity.GetNormalized();

		// If the direction is not too vertical
		if (fabs(vForward.dot(vUp)) < cosf(DEG2RAD(3.f)))
		{
			vForward.z = 0;
			vForward.NormalizeSafe();
			Vec3 vRight = vForward.Cross(vUp);
			vRight.NormalizeSafe();

			Vec3 vDesiredVelocityNormalized = m_vDesiredVelocity.GetNormalized();

			// Roll in an aircraft-like manner
			float cofRoll = 6.f * vRight.dot(vDesiredVelocityNormalized) * (m_stats.speed / maxDeltaSpeed);
			clamp(cofRoll, -1.f, 1.f);
			desiredVelocityQuat *= Quat::CreateRotationY(DEG2RAD(60.f) * cofRoll); 

			float cofPitch = vDesiredVelocityNormalized.dot(vForward) * (deltaSpeed / maxDeltaSpeed);
			clamp(cofPitch, -1.f, 1.f);
			desiredVelocityQuat *= Quat::CreateRotationX(DEG2RAD(-60.f) * cofPitch); 
		}
	}

	float cofRot = 2.5f * ((deltaSpeed > 0.f) ? min(frameTime, 1.f / square(deltaSpeed)) : frameTime);
	clamp(cofRot, 0.f, 1.f);
	const Quat& qRotation = GetEntity()->GetRotation();
	Quat newRot = Quat::CreateSlerp(qRotation, desiredVelocityQuat, cofRot);
	m_moveRequest.rotation = qRotation.GetInverted() * newRot;
	m_moveRequest.rotation.Normalize();

	m_moveRequest.velocity = m_velocity;

	m_moveRequest.type = eCMT_Fly;
}


void CFlyer::SetDesiredVelocity(const Vec3& vDesiredVelocity)
{
	m_vDesiredVelocity = vDesiredVelocity;
}


void CFlyer::SetDesiredDirection(const Vec3& vDesiredDirection)
{
	m_qDesiredRotation.SetRotationVDir(vDesiredDirection.GetNormalizedSafe());
}
