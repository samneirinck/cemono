#include "stdafx.h"
#include "CryScriptInstance.h"

CCryScriptInstance::CCryScriptInstance(mono::object scriptInstance, EMonoScriptFlags flags)
	: m_flags(flags)
{
	SetManagedObject((MonoObject *)scriptInstance);

	m_scriptId = GetPropertyValue("ScriptId")->Unbox<int>();

	g_pScriptSystem->AddListener(this);
}

CCryScriptInstance::~CCryScriptInstance()
{
	g_pScriptSystem->RemoveListener(this);

	m_scriptId = 0;
}

void CCryScriptInstance::OnReloadComplete()
{
	if(IMonoObject *pResult = g_pScriptSystem->GetScriptManager()->CallMethod("GetScriptInstanceById", m_scriptId, m_flags))
	{
		SetManagedObject((MonoObject *)pResult->GetManagedObject());
		pResult->Release(false);

		GetClass();
	}
}