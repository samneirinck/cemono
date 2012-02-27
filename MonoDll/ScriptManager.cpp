#include "StdAfx.h"
#include "ScriptManager.h"

#include <IMonoScriptSystem.h>

#include <IMonoClass.h>
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
	CryLogAlways("CScriptManager::ReloadExtension");

	return true;
}

bool CScriptManager::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	GetGameObject()->EnablePostUpdates(this);

	return true;
}

void CScriptManager::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}

void CScriptManager::PostUpdate(float frameTime)
{
	/*IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(frameTime);

	m_pScriptManager->CallMethod("OnUpdate", pArgs);

	SAFE_RELEASE(pArgs);*/
}

