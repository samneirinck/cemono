#ifndef __FLYER_MOVEMENT_CONTROLLER_H__
#define __FLYER_MOVEMENT_CONTROLLER_H__

#pragma once

#include "IMovementController.h"
#include "Actor.h"

class CFlyer;

class CFlyerMovementController : public IActorMovementController
{
public:

	CFlyerMovementController(CFlyer* pFlyer) : m_pFlyer(pFlyer) {}

	virtual bool RequestMovement(CMovementRequest& request);
	virtual void GetMovementState(SMovementState& movementState) { movementState = m_movementState; }
	virtual bool GetStanceState(const SStanceStateQuery& query, SStanceState& state) { return false; }


	virtual void Reset() {}
	virtual bool Update(float frameTime, SActorFrameMovementParams& actorFrameMovementParams);
	virtual bool GetStats(SStats& stats) { return false; }
	virtual void PostUpdate(float frameTime) {}
	virtual void Release() { delete this; }
	virtual void Serialize(TSerialize& ser) {}


private:

	CFlyer* m_pFlyer;

	CMovementRequest m_movementRequest;
	SMovementState m_movementState;
};

#endif	// #ifndef __FLYER_MOVEMENT_CONTROLLER_H__
