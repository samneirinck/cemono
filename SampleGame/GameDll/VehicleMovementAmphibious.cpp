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
#include "StdAfx.h"
#include "Game.h"
#include "GameCVars.h"
#include <GameUtils.h>

#include "IVehicleSystem.h"
#include "VehicleMovementAmphibious.h"


#define THREAD_SAFE 1

//------------------------------------------------------------------------
template <class Wheeled>
CVehicleMovementAmphibiousT<Wheeled>::CVehicleMovementAmphibiousT()
{  
	m_boat.m_bNetSync = false; 
}

//------------------------------------------------------------------------
template <class Wheeled>
CVehicleMovementAmphibiousT<Wheeled>::~CVehicleMovementAmphibiousT()
{
}

template <class Wheeled>
bool CVehicleMovementAmphibiousT<Wheeled>::Submerged()
{
	return this->m_statusDyn.submergedFraction > 0.01f;
}
//------------------------------------------------------------------------
template <class Wheeled>
bool CVehicleMovementAmphibiousT<Wheeled>::InitWheeled(IVehicle* pVehicle, const CVehicleParams& table)
{
	assert(0);
	return false;
}

template <>	// Specialisation: CVehicleMovementStdWheeled
bool CVehicleMovementAmphibiousT<CVehicleMovementStdWheeled>::InitWheeled(IVehicle* pVehicle, const CVehicleParams& table)
{
	CVehicleParams wheeledTable = table.findChild("StdWheeled");
	if (wheeledTable)
	{
		return CVehicleMovementStdWheeled::Init(pVehicle, wheeledTable);
	}
	return false;
}

template <> // Specialisation: CVehicleMovementArcadeWheeled
bool CVehicleMovementAmphibiousT<CVehicleMovementArcadeWheeled>::InitWheeled(IVehicle* pVehicle, const CVehicleParams& table)
{
	CVehicleParams wheeledTable = table.findChild("ArcadeWheeled");
	if (wheeledTable)
	{
		return CVehicleMovementArcadeWheeled::Init(pVehicle, wheeledTable);
	}
	return false;
}

//------------------------------------------------------------------------
template <class Wheeled>
bool CVehicleMovementAmphibiousT<Wheeled>::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	if (InitWheeled(pVehicle, table))
	{
		CVehicleParams stdBoatTable = table.findChild("StdBoat");
		if (stdBoatTable )
		{
			return m_boat.Init(pVehicle, stdBoatTable);
		}
	}
	return false;
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::PostInit()
{
	Wheeled::PostInit();
	m_boat.PostInit();
}


//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::Reset()
{
	Wheeled::Reset();
	m_boat.Reset();
}


//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::PostPhysicalize()
{
	Wheeled::PostPhysicalize();
	m_boat.PostPhysicalize();
}

template <class Wheeled>
typename Wheeled::EVehicleMovementType CVehicleMovementAmphibiousT<Wheeled>::GetMovementType()
{
	return Wheeled::eVMT_Amphibious;
}


//------------------------------------------------------------------------
template <class Wheeled>
bool CVehicleMovementAmphibiousT<Wheeled>::StartEngine(EntityId driverId)
{
	if (!Wheeled::StartEngine(driverId))
		return false;

	m_boat.StartEngine(driverId);

	return true;
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::StopEngine()
{
	Wheeled::StopEngine();
	m_boat.StopEngine();
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::DisableEngine(bool disable)
{
	Wheeled::DisableEngine(disable);
	m_boat.DisableEngine(disable);
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
	Wheeled::OnAction(actionId, activationMode, value);  
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::OnEvent(EVehicleMovementEvent event, const SVehicleMovementEventParams& params)
{
	Wheeled::OnEvent(event, params);
	m_boat.OnEvent(event, params);  
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
	Wheeled::OnVehicleEvent(event, params);
	m_boat.OnVehicleEvent(event, params);
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::Update(const float deltaTime)
{
	Wheeled::Update(deltaTime);
	m_boat.Update(deltaTime);  
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::UpdateRunSound(const float deltaTime)
{
	Wheeled::UpdateRunSound(deltaTime);  

	if (Wheeled::m_pVehicle->IsProbablyDistant())
		return;

	SetSoundParam(eSID_Run, "swim", this->m_statusDyn.submergedFraction);

	if (this->Boosting())
		SetSoundParam(eSID_Boost, "swim", this->m_statusDyn.submergedFraction);
}


//////////////////////////////////////////////////////////////////////////
// NOTE: This function must be thread-safe. Before adding stuff contact MarcoC.
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::ProcessMovement(const float deltaTime)
{  
	Wheeled::ProcessMovement(deltaTime);    

	if (Submerged())
	{
		// assign movement action to boat (serialized by wheeled movement)
		m_boat.m_movementAction = this->m_movementAction;      
		m_boat.ProcessMovement(deltaTime);
	}  
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::Serialize(TSerialize ser, EEntityAspects aspects) 
{
	Wheeled::Serialize(ser, aspects);  

	if (ser.GetSerializationTarget() != eST_Network)
	{
		m_boat.Serialize(ser, aspects);
	}
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::PostSerialize()
{
	Wheeled::PostSerialize();
	m_boat.PostSerialize();
}

//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::ProcessEvent(SEntityEvent& event)
{  
	Wheeled::ProcessEvent(event);
	m_boat.ProcessEvent(event);
}


//------------------------------------------------------------------------
template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::Boost(bool enable)
{
	Wheeled::Boost(enable);  
	m_boat.Boost(enable);
}


template <class Wheeled>
void CVehicleMovementAmphibiousT<Wheeled>::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
}

// Explicit Instantiation
template class CVehicleMovementAmphibiousT<CVehicleMovementStdWheeled>;
template class CVehicleMovementAmphibiousT<CVehicleMovementArcadeWheeled>;

