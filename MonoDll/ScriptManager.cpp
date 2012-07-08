#include "StdAfx.h"
#include "ScriptManager.h"

#include <IMonoScriptSystem.h>

#include <IMonoArray.h>
#include <IMonoObject.h>
#include <IMonoAssembly.h>

#include "MonoClass.h"

#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>

CScriptManager::CScriptManager()
{
}

CScriptManager::~CScriptManager()
{
}

bool CScriptManager::ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params )
{
	return true;
}

bool CScriptManager::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	if (!GetGameObject()->BindToNetwork())
		return false;

	return true;
}