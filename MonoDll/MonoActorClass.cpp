/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  
 -------------------------------------------------------------------------
  History:
  - 7:10:2004   14:48 : Created by Márcio Martins
												taken over by Filippo De Luca

*************************************************************************/
#include "StdAfx.h"
#include "MonoActorClass.h"

#include <IMonoScriptSystem.h>

//------------------------------------------------------------------------
CMonoActorClass::CMonoActorClass()
	: m_health(0)
	, m_maxHealth(100)
{
}

//------------------------------------------------------------------------
CMonoActorClass::~CMonoActorClass()
{
	GetGameObject()->EnablePhysicsEvent( false, eEPE_OnPostStepImmediate );

	GetGameObject()->ReleaseView( this );
	GetGameObject()->ReleaseProfileManager( this );

	if(gEnv->pGameFramework && gEnv->pGameFramework->GetIActorSystem())
		gEnv->pGameFramework->GetIActorSystem()->RemoveActor( GetEntityId() );
}

//------------------------------------------------------------------------
bool CMonoActorClass::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	if (!pGameObject->CaptureView(this))
		return false;
	if (!pGameObject->CaptureProfileManager(this))
		return false;

	m_scriptId = gEnv->pMonoScriptSystem->InstantiateScript(EMonoScriptType_Actor, GetEntity()->GetClass()->GetName());

	gEnv->pGameFramework->GetIActorSystem()->AddActor( GetEntityId(), this );

	pGameObject->EnablePrePhysicsUpdate(  ePPU_Always );

	if (!pGameObject->BindToNetwork())
		return false;

	GetEntity()->SetFlags(GetEntity()->GetFlags()|(ENTITY_FLAG_ON_RADAR|ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO));

	return true;
}

//------------------------------------------------------------------------
void CMonoActorClass::PostInit(IGameObject *pGameObject)
{
	pGameObject->EnableUpdateSlot( this, 0 );	
	pGameObject->EnablePostUpdates( this );
}