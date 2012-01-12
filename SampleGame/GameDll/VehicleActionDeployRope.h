/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a vehicle action to deploy a rope

-------------------------------------------------------------------------
History:
- 30:11:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEACTIONDEPLOYROPE_H__
#define __VEHICLEACTIONDEPLOYROPE_H__

struct IRopeRenderNode;
struct IVehicleSeat;

class CVehicleActionDeployRope
	: public IVehicleSeatAction
{
	IMPLEMENT_VEHICLEOBJECT;

public:

	CVehicleActionDeployRope();
	virtual ~CVehicleActionDeployRope();

	// IVehicleSeatAction
	virtual bool Init(IVehicle* pVehicle, IVehicleSeat* pSeat, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void StartUsing(EntityId passengerId) {}
	virtual void ForceUsage() {};

	virtual void StopUsing() {}
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void PostSerialize() {}
	virtual void Update(const float deltaTime);

	virtual void GetMemoryUsage(ICrySizer* pSizer) const;
	// ~IVehicleSeatAction

	bool DeployRope();
	void AttachOnRope(IEntity* pEntity);

protected:

	EntityId CreateRope(IPhysicalEntity* pLinkedEntity, const Vec3& highPos, const Vec3& lowPos);
	IRopeRenderNode* GetRopeRenderNode(EntityId ropeId);

	IVehicle* m_pVehicle;
	IVehicleSeat* m_pSeat;

	EntityId m_ropeUpperId;
	EntityId m_ropeLowerId;
	EntityId m_actorId;

	IVehicleHelper* m_pRopeHelper;

	IRopeRenderNode* m_pRopeRenderNode;

	IVehicleAnimation* m_pDeployAnim;
	TVehicleAnimStateId m_deployAnimOpenedId;
	TVehicleAnimStateId m_deployAnimClosedId;
};

#endif
