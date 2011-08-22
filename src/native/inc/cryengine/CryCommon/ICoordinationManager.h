#ifndef	__ICoordinationManager_h__
#define __ICoordinationManager_h__

#pragma once

typedef uint32 CoordinationID;
typedef uint32 CoordinationSetupID;

typedef SmartScriptTable CoordinationInputs;


struct CoordinationStartParams
{
	CoordinationInputs inputs;
	EntityId* actors;
	uint32 actorCount;
};

struct ICoordinationManager
{
	virtual ~ICoordinationManager(){}
	virtual CoordinationID StartCoordination(const char* pCoordinationName, const CoordinationStartParams& params) = 0;
};

#endif //__ICoordinationManager_h__