#include "stdafx.h"

#include "CemonoGameRulesListener.h"
#include <IGameRulesSystem.h>

struct IGameRules;

CCemonoGameRulesListener::CCemonoGameRulesListener()
{
	gEnv->pEntitySystem->AddEntityEventListener(gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->GetCurrentGameRulesEntity()->GetId(), ENTITY_EVENT_START_GAME, this);
	//pEntitySystem->AddEntityEventListener(pGameRules->GetEntityId(), ENTITY_EVENT_START_GAME, this);
}

CCemonoGameRulesListener::~CCemonoGameRulesListener()
{
}

void CCemonoGameRulesListener::OnEntityEvent( IEntity *pEntity,SEntityEvent &event )
{

}