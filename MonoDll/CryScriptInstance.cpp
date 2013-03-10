#include "stdafx.h"
#include "CryScriptInstance.h"

CCryScriptInstance::CCryScriptInstance(EMonoScriptFlags flags)
	: m_flags(flags)
	, m_scriptId(0)
{
	g_pScriptSystem->AddListener(this);
}

CCryScriptInstance::~CCryScriptInstance()
{
	if(g_pScriptSystem)
		g_pScriptSystem->RemoveListener(this);
}

void CCryScriptInstance::Release(bool triggerGC)
{
	delete this;
}

void CCryScriptInstance::SetManagedObject(MonoObject *newObject, bool allowGC)
{
	CScriptObject::SetManagedObject(newObject, allowGC);

	m_scriptId = GetPropertyValue("ScriptId")->Unbox<int>();
}

void CCryScriptInstance::OnReloadStart()
{
	m_pClass = nullptr;
	m_pObject = nullptr;

	m_objectHandle = -1;
}

void CCryScriptInstance::OnReloadComplete()
{
	if(m_scriptId == 0)
		return;

	IMonoObject *pScriptManager = g_pScriptSystem->GetScriptManager();

	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(m_scriptId);
	pArgs->Insert(m_flags);

	if(mono::object result = pScriptManager->GetClass()->InvokeArray(pScriptManager->GetManagedObject(), "GetScriptInstanceById", pArgs))
	{
		SetManagedObject((MonoObject *)result, true);
	}
	else
		MonoWarning("Failed to locate script instance %i after reload!", m_scriptId);
}