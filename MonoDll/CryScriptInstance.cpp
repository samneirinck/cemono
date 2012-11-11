#include "stdafx.h"
#include "CryScriptInstance.h"

CCryScriptInstance::CCryScriptInstance(mono::object scriptInstance)
{
	SetManagedObject((MonoObject *)scriptInstance);

	m_scriptId = GetPropertyValue("ScriptId")->Unbox<int>();

	gEnv->pMonoScriptSystem->AddListener(this);
}

CCryScriptInstance::~CCryScriptInstance()
{
	gEnv->pMonoScriptSystem->RemoveListener(this);

	m_scriptId = 0;
}

void CCryScriptInstance::OnReloadComplete()
{
	if(IMonoObject *pResult = gEnv->pMonoScriptSystem->GetScriptManager()->CallMethod("GetScriptInstanceById", m_scriptId, eScriptFlag_Any))
	{
		SetManagedObject((MonoObject *)pResult->GetManagedObject());
		pResult->Release(false);

		GetClass();
	}
}