#include "stdafx.h"
#include "GameSystemBinding.h"
#include "CemonoGameRulesListener.h"

#include <IGameFramework.h>
#include <IGameRulesSystem.h>

CGameSystemBinding::CGameSystemBinding()
{
	REGISTER_METHOD(_RegisterGameClass);
}

CGameSystemBinding::~CGameSystemBinding()
{

}

void CGameSystemBinding::_RegisterGameClass(MonoObject* game)
{
	CCemonoGameRulesListener* listener = new CCemonoGameRulesListener();
	//gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->GetCurrentGameRulesEntity()->GetId();
}