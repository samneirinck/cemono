#include "StdAfx.h"
#include "MonoClass.h"

#include "MonoArray.h"
#include "MonoObject.h"

#include <mono/metadata/debug-helpers.h>

CMonoClass::CMonoClass(MonoObject *pClassObject, IMonoArray *pConstructorArguments)
{
	if (!pClassObject)
	{
		gEnv->pLog->LogError("Mono class object creation failed!");
		return;
	}

	// We need this to allow the GC to collect the class object later on.
	m_instanceHandle = mono_gchandle_new(pClassObject, true);

	m_pClass = mono_object_get_class(pClassObject);
	m_pInstance = pClassObject;

	if(pConstructorArguments)
		CallMethod(".ctor(string)", pConstructorArguments);
	else
		mono_runtime_object_init(pClassObject);
}

CMonoClass::~CMonoClass()
{
	mono_gchandle_free(m_instanceHandle);

	SAFE_DELETE(m_pClass);
}

IMonoObject *CMonoClass::CallMethod(const char *methodName, IMonoArray *params, bool _static)
{
	if (m_pClass)
	{
		if (MonoMethod* pMethod = GetMethodFromClassAndMethodName(m_pClass, methodName))
			return new CMonoObject(mono_runtime_invoke_array(pMethod, _static ? NULL : m_pInstance, params ? static_cast<CMonoArray *>(params)->GetMonoArray() : NULL, NULL));
	}

	return NULL;
}

MonoMethod* CMonoClass::GetMethodFromClassAndMethodName(MonoClass* pClass, const char* methodName)
{
	MonoMethodDesc* pMethodDescription = mono_method_desc_new(":" + (string)methodName, false);
	if (pMethodDescription)
	{
		return mono_method_desc_search_in_class(pMethodDescription, pClass);
		mono_method_desc_free(pMethodDescription);
	}

	return NULL;
}