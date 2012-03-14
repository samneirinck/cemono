#include "StdAfx.h"
#include "ItemSystem.h"

#include "MonoCommon.h"
#include <IGameFramework.h>
#include <IItemSystem.h>

CScriptbind_ItemSystem::CScriptbind_ItemSystem()
{
	REGISTER_EXPOSED_METHOD(CacheItemGeometry);
	REGISTER_EXPOSED_METHOD(CacheItemSound);

	REGISTER_METHOD(GiveItem);
	REGISTER_METHOD(GiveEquipmentPack);
}

void CScriptbind_ItemSystem::CacheItemGeometry(mono::string itemClass)
{
	gEnv->pGame->GetIGameFramework()->GetIItemSystem()->CacheItemGeometry(ToCryString(itemClass));
}
	
void CScriptbind_ItemSystem::CacheItemSound(mono::string itemClass)
{
	gEnv->pGame->GetIGameFramework()->GetIItemSystem()->CacheItemSound(ToCryString(itemClass));
}

void CScriptbind_ItemSystem::GiveItem(EntityId playerId, mono::string itemClass)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		gEnv->pGame->GetIGameFramework()->GetIItemSystem()->GiveItem(pActor, ToCryString(itemClass), true, true, true);
}

void CScriptbind_ItemSystem::GiveEquipmentPack(EntityId playerId, mono::string equipmentPack)
{
	if(IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId))
		gEnv->pGame->GetIGameFramework()->GetIItemSystem()->GetIEquipmentManager()->GiveEquipmentPack(pActor, ToCryString(equipmentPack), true, true);
}