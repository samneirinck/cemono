/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements amphibious movement type

-------------------------------------------------------------------------
History:
- 13:06:2007: Created by MichaelR

*************************************************************************/
#ifndef __VEHICLEMOVEMENTAmphibious_H__
#define __VEHICLEMOVEMENTAmphibious_H__

#include "VehicleMovementStdWheeled.h"
#include "VehicleMovementArcadeWheeled.h"
#include "VehicleMovementStdBoat.h"


template <class Wheeled>
class CVehicleMovementAmphibiousT
	: public Wheeled
{
public:
	typedef typename Wheeled::EVehicleMovementType EVehicleMovementType;
	typedef typename Wheeled::EVehicleMovementEvent EVehicleMovementEvent;

public:

	CVehicleMovementAmphibiousT();
	virtual ~CVehicleMovementAmphibiousT();

	virtual bool InitWheeled(IVehicle* pVehicle, const CVehicleParams& table);
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void PostInit();
	virtual void Reset();  
	virtual void PostPhysicalize();

	virtual EVehicleMovementType GetMovementType();

	virtual bool StartEngine(EntityId driverId);
	virtual void StopEngine();
	virtual void DisableEngine(bool disable);

	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	virtual void OnEvent(EVehicleMovementEvent event, const SVehicleMovementEventParams& params);
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	virtual void Update(const float deltaTime);  
	virtual void ProcessMovement(const float deltaTime);

	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
	virtual void PostSerialize();

	virtual void ProcessEvent(SEntityEvent& event);

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

	bool Submerged();

	virtual void UpdateRunSound(const float deltaTime);

	virtual void Boost(bool enable);

	CVehicleMovementStdBoat m_boat;
};

typedef CVehicleMovementAmphibiousT<CVehicleMovementStdWheeled> CVehicleMovementAmphibious;
typedef CVehicleMovementAmphibiousT<CVehicleMovementArcadeWheeled> CVehicleMovementAmphibiousAracde;

#endif
