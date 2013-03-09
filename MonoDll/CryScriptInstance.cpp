#include "stdafx.h"
#include "CryScriptInstance.h"

CCryScriptInstance::CCryScriptInstance(mono::object scriptInstance, EMonoScriptFlags flags)
	: m_flags(flags)
{
	SetManagedObject((MonoObject *)scriptInstance, true);

	m_scriptId = GetPropertyValue("ScriptId")->Unbox<int>();

	g_pScriptSystem->AddListener(this);
}

CCryScriptInstance::~CCryScriptInstance()
{
	if(g_pScriptSystem)
		g_pScriptSystem->RemoveListener(this);

	m_scriptId = 0;
}

void CCryScriptInstance::OnReloadStart()
{
	m_pClass = nullptr;
	m_pObject = nullptr;

	m_objectHandle = -1;
}

void CCryScriptInstance::OnReloadComplete()
{
	IMonoObject *pScriptManager = g_pScriptSystem->GetScriptManager();

	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(m_scriptId);
	pArgs->Insert(m_flags);

	if(mono::object result = pScriptManager->GetClass()->InvokeArray(pScriptManager->GetManagedObject(), "GetScriptInstanceById", pArgs))
	{
		SetManagedObject((MonoObject *)result, true);
	}
	else
		CryFatalError("[CryMono] Failed to locate script instance %i after reload!", m_scriptId);
}