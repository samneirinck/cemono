#include "StdAfx.h"
#include "Actor.h"

#include <IGameRulesSystem.h>
#include <IViewSystem.h>

#include <IMonoAssembly.h>
#include <IMonoClass.h>

CActor::CActor()
	: m_bClient(false)
	, m_pScript(nullptr)
{
}


CActor::~CActor()
{
	if(IActorSystem *pActorSystem = gEnv->pGameFramework->GetIActorSystem())
		pActorSystem->RemoveActor(GetEntityId());
}

bool CActor::Init(IGameObject *pGameObject)
{ 
	SetGameObject(pGameObject);

	if(!GetGameObject()->CaptureView(this))
		return false;

	gEnv->pGameFramework->GetIActorSystem()->AddActor(GetEntityId(), this);
	GetGameObject()->BindToNetwork();
	GetEntity()->SetFlags(GetEntity()->GetFlags()|(ENTITY_FLAG_ON_RADAR|ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO));

	return true; 
}

void CActor::PostInit(IGameObject *pGameObject)
{
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

void CActor::UpdateView(SViewParams &viewParams)
{
	void *args[1];
	args[0] = &viewParams;

	m_pScript->GetClass()->Invoke(m_pScript, "UpdateView", args, 1);
}