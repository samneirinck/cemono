#include "StdAfx.h"
#include "ItemSystem.h"

#include "MonoString.h"
#include <IGameFramework.h>
#include <IItemSystem.h>

CScriptBind_ItemSystem::CScriptBind_ItemSystem()
{
	REGISTER_METHOD(CacheItemGeometry);
	REGISTER_METHOD(CacheItemSound);
}

void CScriptBind_ItemSystem::CacheItemGeometry(MonoString *itemClass)
{
	gEnv->pGame->GetIGameFramework()->GetIItemSystem()->CacheItemGeometry(ToCryString(itemClass));
}
	
void CScriptBind_ItemSystem::CacheItemSound(MonoString *itemClass)
{
	gEnv->pGame->GetIGameFramework()->GetIItemSystem()->CacheItemSound(ToCryString(itemClass));
}