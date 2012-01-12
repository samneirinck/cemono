#include "StdAfx.h"
#include "FlyerMovementController.h"
#include "Flyer.h"


bool CFlyerMovementController::RequestMovement(CMovementRequest& movementRequest)
{
	CFlyer::SMovementRequestParams movementRequestParams(movementRequest);

	if (movementRequest.HasForcedNavigation())
	{
		movementRequestParams.vMoveDir = movementRequest.GetForcedNavigation();
		movementRequestParams.fDesiredSpeed = movementRequestParams.vMoveDir.GetLength();
		movementRequestParams.vMoveDir.NormalizeSafe();
	}

	m_pFlyer->SetActorMovement(movementRequestParams);

	return true;
}

bool CFlyerMovementController::Update(float frameTime, SActorFrameMovementParams&)
{
	IEntity* pEntity = m_pFlyer->GetEntity();

	CFlyer::SBodyInfo bodyInfo;
	m_pFlyer->GetActorInfo(bodyInfo);

	SMovementState& state = m_movementState;
	state.pos = pEntity->GetWorldPos();
	state.entityDirection = pEntity->GetWorldRotation() * Vec3Constants<float>::fVec3_OneY;
	state.animationBodyDirection = state.entityDirection;
	state.upDirection = Vec3Constants<float>::fVec3_OneZ;
	state.weaponPosition.zero();
	state.aimDirection = state.entityDirection;
	state.fireDirection = state.entityDirection;
	state.eyePosition = bodyInfo.vEyePos;
	state.eyeDirection = state.entityDirection;
	state.lean = 0.f;
	state.peekOver = 0.f;
	state.m_StanceSize = bodyInfo.pStanceInfo->GetStanceBounds();
	state.m_ColliderSize = bodyInfo.pStanceInfo->GetColliderBounds();
	state.fireTarget.zero();
	state.stance = bodyInfo.eStance;
	state.animationEyeDirection = state.entityDirection;
	state.movementDirection = state.entityDirection;
	state.lastMovementDirection = state.entityDirection;
	state.desiredSpeed = bodyInfo.velocity.GetLength();
	state.minSpeed = 0.f;
	state.normalSpeed = bodyInfo.pStanceInfo->normalSpeed;
	state.maxSpeed = bodyInfo.pStanceInfo->maxSpeed;
	state.slopeAngle = 0.f;
	state.atMoveTarget = false;
	state.isAlive = (m_pFlyer->GetHealth() > 0.f);
	state.isAiming = false;
	state.isFiring = false;
	state.isVisible = !pEntity->IsInvisible();

	return true;
}
