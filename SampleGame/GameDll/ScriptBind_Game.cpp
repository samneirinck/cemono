/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 14:08:2006   11:29 : Created by AlexL

*************************************************************************/
#include "StdAfx.h"
#include "ScriptBind_Game.h"
#include "Game.h"
#include "IAIActor.h"
#include "Lam.h"
#include "Audio/GameAudio.h"
#include "Audio/BattleStatus.h"
#include "IAIObject.h"


#ifdef WIN64
	#pragma warning(disable: 4244)
#endif


//------------------------------------------------------------------------
CScriptBind_Game::CScriptBind_Game(ISystem *pSystem, IGameFramework *pGameFramework)
: m_pSystem(pSystem),
	m_pSS(pSystem->GetIScriptSystem()),
	m_pGameFW(pGameFramework)
{
	Init(m_pSS, m_pSystem);
	SetGlobalName("Game");

	RegisterMethods();
	RegisterGlobals();
}

//------------------------------------------------------------------------
CScriptBind_Game::~CScriptBind_Game()
{
}

//------------------------------------------------------------------------
void CScriptBind_Game::RegisterGlobals()
{
}

//------------------------------------------------------------------------
void CScriptBind_Game::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_Game::

	SCRIPT_REG_TEMPLFUNC(QueryBattleStatus, "");
	SCRIPT_REG_TEMPLFUNC(GetNumLightsActivated,"");

	SCRIPT_REG_FUNC(IsMountedWeaponUsableWithTarget);

	SCRIPT_REG_TEMPLFUNC(IsPlayer, "entityId");

#undef SCRIPT_REG_CLASSNAME
}

//------------------------------------------------------------------------


//------------------------------------------------------------------------


//------------------------------------------------------------------------
int CScriptBind_Game::PauseGame( IFunctionHandler *pH, bool pause )
{
	bool forced = false;

	if (pH->GetParamCount() > 1)
	{
		pH->GetParam(2, forced);
	}
	m_pGameFW->PauseGame(pause, forced);

	return pH->EndFunction();
}

//------------------------------------------------------------------------


//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
int CScriptBind_Game::QueryBattleStatus(IFunctionHandler *pH)
{		
	float fStatus = SAFE_GAMEAUDIO_BATTLESTATUS_FUNC_RET(QueryBattleStatus());
	
	return pH->EndFunction(fStatus);
}

//////////////////////////////////////////////////////////////////////////

int CScriptBind_Game::IsPlayer(IFunctionHandler *pH, ScriptHandle entityId)
{
	EntityId eId = entityId.n;
	if (eId == LOCAL_PLAYER_ENTITY_ID)
		return pH->EndFunction(true);

	IActor *pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(eId);
	return pH->EndFunction(pActor && pActor->IsPlayer());
}

//////////////////////////////////////////////////////////////////////////
int CScriptBind_Game::GetNumLightsActivated(IFunctionHandler *pH)
{		
	return pH->EndFunction(CLam::GetNumLightsActivated());
}


#define GET_ENTITY(i) \
	ScriptHandle hdl;\
	pH->GetParam(i,hdl);\
	int nID = hdl.n;\
	IEntity* pEntity = gEnv->pEntitySystem->GetEntity(nID);


//////////////////////////////////////////////////////////////////////////
// IsMountedWeaponUsableWithTarget
// A piece of game-code moved from CryAction when scriptbind_AI moved to the AI system
//////////////////////////////////////////////////////////////////////////
int CScriptBind_Game::IsMountedWeaponUsableWithTarget(IFunctionHandler *pH)
{
	int paramCount = pH->GetParamCount();
	if(paramCount<2)
	{
		//gEnv->pAISystem->Warning("<CScriptBind> ", "IsMountedWeaponUsableWithTarget(): too few parameters.");
		GameWarning("%s: too few parameters.", __FUNCTION__);
		return pH->EndFunction();
	}

	GET_ENTITY(1);

	if(!pEntity)
	{
		//gEnv->pAISystem->Warning("<CScriptBind> ", "IsMountedWeaponUsableWithTarget(): wrong entity id in parameter 1.");
		GameWarning("%s: wrong entity id in parameter 1.", __FUNCTION__);
		return pH->EndFunction();
	}

	IAIObject* pAI = pEntity->GetAI();
	if (!pAI)
	{
		//gEnv->pAISystem->Warning("<CScriptBind> ", "IsMountedWeaponUsableWithTarget(): Entity '%s' does not have AI.", pEntity->GetName());
		GameWarning("%s: Entity '%s' does not have AI.",__FUNCTION__,  pEntity->GetName());
		return pH->EndFunction();
	}

	EntityId itemEntityId;
	ScriptHandle hdl2;

	if(!pH->GetParam(2,hdl2))
	{
		//gEnv->pAISystem->Warning("<CScriptBind> ", "IsMountedWeaponUsableWithTarget(): wrong parameter 2 format.");
		GameWarning("%s: wrong parameter 2 format.", __FUNCTION__);
		return pH->EndFunction();
	}

	itemEntityId = hdl2.n;

	if (!itemEntityId)
	{
		//gEnv->pAISystem->Warning("<CScriptBind> ", "IsMountedWeaponUsableWithTarget(): wrong entity id in parameter 2.");
		GameWarning("%s: wrong entity id in parameter 2.", __FUNCTION__);
		return pH->EndFunction();
	}
	
	IGameFramework *pGameFramework = gEnv->pGame->GetIGameFramework();
	IItem* pItem = pGameFramework->GetIItemSystem()->GetItem(itemEntityId);
	if (!pItem)
	{
		//gEnv->pAISystem->Warning("<CScriptBind> ", "entity in parameter 2 is not an item/weapon");
		GameWarning("%s: entity in parameter 2 is not an item/weapon.", __FUNCTION__);
		return pH->EndFunction();
	}

	float minDist = 7;
	bool bSkipTargetCheck = false;
	Vec3 targetPos(ZERO);

	if(paramCount > 2)
	{
		for(int i=3;i <= paramCount ; i++)
		{
			if(pH->GetParamType(i) == svtBool)
				pH->GetParam(i,bSkipTargetCheck);
			else if(pH->GetParamType(i) == svtNumber)
				pH->GetParam(i,minDist);
			else if(pH->GetParamType(i) == svtObject)
				pH->GetParam(i,targetPos);
		}
	}

	IAIActor* pAIActor = CastToIAIActorSafe(pAI);
	if (!pAIActor)
	{
		GameWarning("%s: entity '%s' in parameter 1 is not an AI actor.", __FUNCTION__, pEntity->GetName());
		return pH->EndFunction();
	}


	IEntity* pItemEntity = pItem->GetEntity();
	if(!pItemEntity)
		return pH->EndFunction();


	if(!pItem->GetOwnerId())
	{
		// weapon is not used, check if it is on a vehicle
		IEntity* pParentEntity = pItemEntity->GetParent();
		if(pParentEntity)
		{
			IAIObject* pParentAI = pParentEntity->GetAI();
			if(pParentAI && pParentAI->GetAIType()==AIOBJECT_VEHICLE)
			{
				// (MATT) Feature was cut and code was tricky, hence ignore weapons in vehicles  {2008/02/15:11:08:51}
				return pH->EndFunction();
			}
		}
	}
	else if( pItem->GetOwnerId()!= pEntity->GetId()) // item is used by someone else?
		return pH->EndFunction(false);

	// check target
	if(bSkipTargetCheck)
		return pH->EndFunction(true);

	IAIObject* pTarget = pAIActor->GetAttentionTarget();
	if(targetPos.IsZero())
	{
		if(!pTarget)
			return pH->EndFunction();
		targetPos = pTarget->GetPos();
	}

	Vec3 targetDir(targetPos - pItemEntity->GetWorldPos());
	Vec3 targetDirXY(targetDir.x, targetDir.y, 0);

	float length2D = targetDirXY.GetLength();
	if(length2D < minDist || length2D<=0)
		return pH->EndFunction();

	targetDirXY /= length2D;//normalize

	Vec3 mountedAngleLimits(pItem->GetMountedAngleLimits());

	float yawRange = DEG2RAD(mountedAngleLimits.z);
	if(yawRange > 0 && yawRange < gf_PI)
	{
		float deltaYaw = pItem->GetMountedDir().Dot(targetDirXY);
		if(deltaYaw < cosf(yawRange))
			return pH->EndFunction(false);
	}

	float minPitch = DEG2RAD(mountedAngleLimits.x);
	float maxPitch = DEG2RAD(mountedAngleLimits.y);

	//maxPitch = (maxPitch - minPitch)/2;
	//minPitch = -maxPitch;

	float pitch = atanf(targetDir.z / length2D);

	if ( pitch < minPitch || pitch > maxPitch )
		return pH->EndFunction(false);

	if(pTarget)
	{
		IEntity* pTargetEntity = pTarget->GetEntity();
		if(pTargetEntity)
		{
			// check target distance and where he's going
			IPhysicalEntity *phys = pTargetEntity->GetPhysics();
			if(phys)
			{
				pe_status_dynamics	dyn;
				phys->GetStatus(&dyn);
				Vec3 velocity ( dyn.v);
				velocity.z = 0;

				float speed = velocity.GetLength2D();
				if(speed>0)
				{
					//velocity /= speed;
					if(length2D< minDist * 0.75f && velocity.Dot(targetDirXY)<=0)
						return pH->EndFunction(false);
				}
			}
		}
	}
	return pH->EndFunction(true);

}
