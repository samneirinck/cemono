////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   scriptobjectboids.cpp
//  Version:     v1.00
//  Created:     17/5/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <IEntitySystem.h>
#include "ScriptBind_Boids.h"
#include "BoidsProxy.h"
#include "Flock.h"
#include "BugsFlock.h"
#include "ChickenBoids.h"
#include "FrogBoids.h"

//////////////////////////////////////////////////////////////////////////
CScriptBind_Boids::CScriptBind_Boids( ISystem *pSystem )
{
	m_pSystem = pSystem;
	CScriptableBase::Init( pSystem->GetIScriptSystem(),pSystem );
	SetGlobalName( "Boids" );

	// For Xennon.
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_Boids::

	SCRIPT_REG_TEMPLFUNC(CreateFlock,"entity,nType,tParamsTable");
	SCRIPT_REG_TEMPLFUNC(SetFlockParams,"entity,tParamsTable");
	SCRIPT_REG_TEMPLFUNC(EnableFlock,"entity,bEnable" );
	SCRIPT_REG_TEMPLFUNC(SetFlockPercentEnabled,"entity,fPercentage" );
	SCRIPT_REG_TEMPLFUNC(OnBoidHit,"boidEntity,hit" );
	SCRIPT_REG_TEMPLFUNC(SetAttractionPoint, "entity,point");

	m_pMethodsTable->SetValue( "FLOCK_BIRDS",EFLOCK_BIRDS );
	m_pMethodsTable->SetValue( "FLOCK_FISH",EFLOCK_FISH );
	m_pMethodsTable->SetValue( "FLOCK_BUGS",EFLOCK_BUGS );
	m_pMethodsTable->SetValue( "FLOCK_CHICKENS",EFLOCK_CHICKENS );
	m_pMethodsTable->SetValue( "FLOCK_FROGS",EFLOCK_FROGS );
	m_pMethodsTable->SetValue( "FLOCK_TURTLES",EFLOCK_TURTLES );
}

//////////////////////////////////////////////////////////////////////////
CScriptBind_Boids::~CScriptBind_Boids(void)
{
}

//////////////////////////////////////////////////////////////////////////
IEntity* CScriptBind_Boids::GetEntity( IScriptTable *pEntityTable )
{
	if (!pEntityTable)
		return 0;
	
	ScriptHandle handle;
	if (pEntityTable->GetValue( "id",handle ))
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity((EntityId)handle.n);
		return pEntity;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
CFlock* CScriptBind_Boids::GetFlock( IScriptTable *pEntityTable )
{
	IEntity *pEntity = GetEntity(pEntityTable);
	if (!pEntity)
	{
		GameWarning( "Boids.SetFlockParams called with an invalid entity" );
		return 0;
	}
	CBoidsProxy *pProxy = (CBoidsProxy*)pEntity->GetProxy(ENTITY_PROXY_BOIDS);
	if (!pProxy)
	{
		GameWarning( "Boids.SetFlockParams specified entity is not a flock" );
		return 0;
	}
	return pProxy->GetFlock();
}

//////////////////////////////////////////////////////////////////////////
int CScriptBind_Boids::CreateFlock( IFunctionHandler *pH, SmartScriptTable entity,int nType,SmartScriptTable paramTable )
{
	IEntity *pEntity = GetEntity(entity);
	if (!pEntity)
	{
		GameWarning( "Boids.CreateFlock called with an invalid entity" );
		return pH->EndFunction();
	}

	pEntity->SetFlags( pEntity->GetFlags() | ENTITY_FLAG_CLIENT_ONLY );

	CFlock *pFlock = 0;
	switch (nType) {
	case EFLOCK_BIRDS:
		pFlock = new CBirdsFlock(pEntity);
		break;
	case EFLOCK_CHICKENS:
		pFlock = new CChickenFlock(pEntity);
		break;
	case EFLOCK_FISH:
		pFlock = new CFishFlock(pEntity);
		break;
	case EFLOCK_FROGS:
		pFlock = new CFrogFlock(pEntity);
		break;
	case EFLOCK_TURTLES:
		pFlock = new CTurtleFlock(pEntity);
		break;
	case EFLOCK_BUGS:
		pFlock = new CBugsFlock(pEntity);
		break;
	}
	if (!pFlock)
	{
		GameWarning( "Boids.CreateFlock wrong flock type %d specified",nType );
		return pH->EndFunction();
	}

	//////////////////////////////////////////////////////////////////////////
	// Creates a boids proxy for this entity, and attach flock to it.
	CBoidsProxy *pBoidsProxy = new CBoidsProxy(pEntity);
	pEntity->SetProxy( ENTITY_PROXY_BOIDS,pBoidsProxy );

	SBoidContext bc;
	pFlock->GetBoidSettings( bc );

	SBoidsCreateContext ctx;
	if (ReadParamsTable( paramTable,bc,ctx ))
	{
		bc.entity = pEntity;
		pFlock->SetBoidSettings( bc );
		pFlock->CreateBoids( ctx );
	}

	// Assign flock to boids proxy and update some settings.
	pBoidsProxy->SetFlock(pFlock);

	return pH->EndFunction();
}

//////////////////////////////////////////////////////////////////////////
int CScriptBind_Boids::SetFlockParams(IFunctionHandler *pH,SmartScriptTable entity,SmartScriptTable paramTable)
{
	CFlock *flock = GetFlock(entity);
	if (!flock)
		return pH->EndFunction();

	string currModel = flock->GetModelName();
	int currCount = flock->GetBoidsCount();
	SBoidContext bc;
	flock->GetBoidSettings(bc);

	SBoidsCreateContext ctx;
	if (ReadParamsTable( paramTable,bc,ctx ))
	{
		flock->SetBoidSettings( bc );
		string model = "";
		if (!ctx.models.empty())
			model  = ctx.models[0];
		if ((!model.empty()  && stricmp(model.c_str(),currModel.c_str()) == 0) ||
				(ctx.boidsCount > 0 && ctx.boidsCount != currCount))
		{
			flock->CreateBoids( ctx );
		}
	}

	IEntity *pEntity = GetEntity(entity);
	if (pEntity)
	{
		CBoidsProxy *pBoidsProxy = (CBoidsProxy*)pEntity->GetProxy(ENTITY_PROXY_BOIDS);
		if (pBoidsProxy)
		{
			// Update 
			pBoidsProxy->SetFlock(flock);
		}
	}

	return pH->EndFunction();
}

//////////////////////////////////////////////////////////////////////////
int CScriptBind_Boids::EnableFlock(IFunctionHandler *pH,SmartScriptTable entity,bool bEnable)
{
	CFlock *flock = GetFlock(entity);
	if (flock)
		flock->SetEnabled( bEnable );

	return pH->EndFunction();
}

int CScriptBind_Boids::SetFlockPercentEnabled(IFunctionHandler *pH,SmartScriptTable entity,int percent )
{
	CFlock *flock = GetFlock(entity);

	if (flock)
		flock->SetPercentEnabled( percent );

	return pH->EndFunction();
}

int CScriptBind_Boids::SetAttractionPoint(IFunctionHandler *pH,SmartScriptTable entity, Vec3 point )
{
	CBirdsFlock *pFlock = (CBirdsFlock *)GetFlock(entity);
	if (pFlock)
		pFlock->SetAttractionPoint(point);
	return pH->EndFunction();
}

//////////////////////////////////////////////////////////////////////////
bool CScriptBind_Boids::ReadParamsTable(IScriptTable *pTable, struct SBoidContext &bc,SBoidsCreateContext &ctx )
{
	pTable->BeginSetGetChain();
	float fval;
	const char *str;

	ctx.models.clear();
	ctx.boidsCount = 0;
	pTable->GetValueChain( "count",ctx.boidsCount );
	if (pTable->GetValueChain( "model",str ))
	{
		ctx.models.push_back(str);
	}
	if (pTable->GetValueChain( "model1",str ))
	{
		if (str[0])
			ctx.models.push_back(str);
	}
	if (pTable->GetValueChain( "model2",str ))
	{
		if (str[0])
			ctx.models.push_back(str);
	}
	if (pTable->GetValueChain( "model3",str ))
	{
		if (str[0])
			ctx.models.push_back(str);
	}
	if (pTable->GetValueChain( "model4",str ))
	{
		if (str[0])
			ctx.models.push_back(str);
	}
	if (pTable->GetValueChain( "character",str ))
	{
		ctx.characterModel = str;
	}
	if (pTable->GetValueChain( "animation",str ))
	{
		ctx.animation = str;
	}

	pTable->GetValueChain( "behavior",bc.behavior );

	pTable->GetValueChain( "boid_mass",bc.fBoidMass);

	pTable->GetValueChain( "boid_size",bc.boidScale );
	pTable->GetValueChain( "boid_size_random",bc.boidRandomScale );
	pTable->GetValueChain( "min_height",bc.MinHeight );
	pTable->GetValueChain( "max_height",bc.MaxHeight );
	pTable->GetValueChain( "min_attract_distance",bc.MinAttractDistance );
	pTable->GetValueChain( "max_attract_distance",bc.MaxAttractDistance );
	pTable->GetValueChain( "min_speed",bc.MinSpeed );
	pTable->GetValueChain( "max_speed",bc.MaxSpeed );

	pTable->GetValueChain( "factor_align",bc.factorAlignment );
	pTable->GetValueChain( "factor_cohesion",bc.factorCohesion );
	pTable->GetValueChain( "factor_separation",bc.factorSeparation );
	pTable->GetValueChain( "factor_origin",bc.factorAttractToOrigin );
	pTable->GetValueChain( "factor_keep_height",bc.factorKeepHeight );
	pTable->GetValueChain( "factor_avoid_land",bc.factorAvoidLand );
	pTable->GetValueChain( "factor_random_accel",bc.factorRandomAccel );

	pTable->GetValueChain( "max_anim_speed",bc.MaxAnimationSpeed );
	pTable->GetValueChain( "follow_player",bc.followPlayer );
	pTable->GetValueChain( "no_landing",bc.noLanding );
	pTable->GetValueChain( "avoid_water",bc.bAvoidWater );
	pTable->GetValueChain( "avoid_obstacles",bc.avoidObstacles );
	pTable->GetValueChain( "max_view_distance",bc.maxVisibleDistance );
	pTable->GetValueChain( "spawn_from_point", bc.bSpawnFromPoint );

	pTable->GetValueChain( "spawn_radius",bc.fSpawnRadius);
	//pTable->GetValueChain( "boid_radius",bc.fBoidRadius);
	pTable->GetValueChain( "gravity_at_death",bc.fGravity);
	pTable->GetValueChain( "boid_mass",bc.fBoidMass);

	if (pTable->GetValueChain( "fov_angle",fval ))
	{
		fval = fval/2.0f; // Half angle used for cos of fov.
		bc.cosFovAngle = cry_cosf(fval*gf_PI/180.0f);
	}

	SmartScriptTable sounds;
	if (pTable->GetValueChain("Sounds",sounds))
	{
		for (int i = 1; i < 100; i++)
		{
			str = "";
			if (sounds->GetAt(i,str))
			{
				bc.sounds.push_back(str);
			}
			else
				break;
		}
	}
	SmartScriptTable animations;
	if (pTable->GetValueChain("Animations",animations))
	{
		for (int i = 1; i < 100; i++)
		{
			str = "";
			if (animations->GetAt(i,str))
			{
				bc.animations.push_back(str);
			}
			else
				break;
		}
	}

	pTable->EndSetGetChain();

	return true;
}

//////////////////////////////////////////////////////////////////////////
int CScriptBind_Boids::OnBoidHit( IFunctionHandler *pH,SmartScriptTable flockEntity,SmartScriptTable boidEntity,SmartScriptTable hit )
{
	CFlock *flock = GetFlock(flockEntity);
	IEntity *pBoidEntity = GetEntity(boidEntity);
	if (flock && pBoidEntity)
	{
		flock->OnBoidHit( pBoidEntity->GetId(),hit );
	}
	return pH->EndFunction();
}
