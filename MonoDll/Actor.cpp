#include "StdAfx.h"
#include "Actor.h"

#include <IGameRulesSystem.h>

CActor::CActor()
	: m_bClient(false)
{
}


CActor::~CActor()
{
	if(IActorSystem *pActorSystem = gEnv->pGameFramework->GetIActorSystem())
		pActorSystem->RemoveActor(GetEntityId());
}

bool CActor::Init( IGameObject * pGameObject ) 
{ 
	SetGameObject(pGameObject);

	gEnv->pGameFramework->GetIActorSystem()->AddActor(GetEntityId(), this);
	GetGameObject()->BindToNetwork();
	GetEntity()->SetFlags(GetEntity()->GetFlags()|(ENTITY_FLAG_ON_RADAR|ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO));

	return true; 
}

void CActor::HandleEvent(const SGameObjectEvent &event)
{
	if (event.event == 276 /* Ragdoll, defined in GameDll ._. */)
	{
		GetGameObject()->SetAspectProfile(eEA_Physics, eAP_Ragdoll);
	}
	else if (event.event == eGFE_BecomeLocalPlayer)
	{
		IEntity *pEntity = GetEntity();
		pEntity->SetFlags(GetEntity()->GetFlags() | ENTITY_FLAG_TRIGGER_AREAS);
		// Invalidate the matrix in order to force an update through the area manager
		pEntity->InvalidateTM(ENTITY_XFORM_POS);

		m_bClient = true;
		GetGameObject()->EnablePrePhysicsUpdate( ePPU_Always );
	}
}