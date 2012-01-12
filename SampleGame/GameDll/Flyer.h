#ifndef __FLYER_H__
#define __FLYER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Player.h"


class CFlyer : public CPlayer
{
public:

	struct SMovementRequestParams
	{
		Vec3 vLookTargetPos;
		Vec3 vMoveDir;
		float fDesiredSpeed;

		explicit SMovementRequestParams(CMovementRequest& movementRequest) :
			vLookTargetPos(movementRequest.HasLookTarget() ? movementRequest.GetLookTarget() : Vec3Constants<float>::fVec3_Zero),
			vMoveDir(ZERO),
			fDesiredSpeed(movementRequest.HasDesiredSpeed() ? movementRequest.GetDesiredSpeed() : 1.f)
		{	
		}
	};

	struct SBodyInfo
	{
		Vec3 vEyePos;		
		Vec3 velocity;
		EStance eStance;
		const SStanceInfo* pStanceInfo;

		SBodyInfo() :
			vEyePos(ZERO),
			velocity(ZERO),
			eStance(STANCE_NULL),
			pStanceInfo(NULL)
		{
		}
	};


	CFlyer();

	
	virtual void GetMemoryUsage(ICrySizer* pCrySizer) const { pCrySizer->Add(*this); }
	virtual void FullSerialize(TSerialize ser);
	virtual void PrePhysicsUpdate();
	virtual void Revive(bool bFromInit = false);

	void GetActorInfo(SBodyInfo& bodyInfo);
	void SetActorMovement(SMovementRequestParams& movementRequestParam);


protected:

	virtual IActorMovementController* CreateMovementController();


private:

	void ProcessMovement(float frameTime);	// Ad-hoc

	void SetDesiredVelocity(const Vec3& vDesiredVelocity);
	void SetDesiredDirection(const Vec3& vDesiredDir);


	Vec3 m_vDesiredVelocity;
	Quat m_qDesiredRotation;

	SCharacterMoveRequest m_moveRequest;
};

#endif	// #ifndef __FLYER_H__
