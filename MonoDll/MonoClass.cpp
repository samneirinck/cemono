#include "StdAfx.h"
#include "MonoClass.h"

#include "MonoArray.h"
#include "MonoObject.h"

#include <mono/metadata/debug-helpers.h>

CMonoClass::CMonoClass(MonoClass *pClass, IMonoArray *pConstructorArguments)
	: m_scriptId(-1)
	, m_scriptType(0)
	, m_pClass(pClass)
	, m_pInstance(NULL)
{
	if (!m_pClass)
	{
		gEnv->pLog->LogError("Mono class object creation failed!");

		delete this;
	}

	Instantiate(pConstructorArguments);
}

CMonoClass::CMonoClass(int scriptId, int scriptType)
	: m_scriptId(scriptId)
	, m_scriptType(scriptType)
	, m_pInstance(NULL)
{
	IMonoClass *pScriptCompiler = static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetScriptCompilerClass();

	if(pScriptCompiler)
	{
		IMonoArray *pArgs = new CMonoArray(1);
		pArgs->Insert(scriptId);

		IMonoObject *pInstanceResult = pScriptCompiler->CallMethod("GetScriptInstanceById", pArgs);
		if(pInstanceResult)
		{
			m_pInstance = static_cast<CMonoObject *>(pInstanceResult)->GetMonoObject();
			m_pClass = mono_object_get_class((MonoObject *)m_pInstance);

			m_instanceHandle = mono_gchandle_new((MonoObject *)m_pInstance, false);
		}
		else
		{
			gEnv->pLog->LogError("Attempted to create CMonoClass using an invalid scriptId");

			delete this;
		}
	}
}

CMonoClass::~CMonoClass()
{
	mono_gchandle_free(m_instanceHandle);

	static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->RemoveScriptInstance(m_scriptId);

	m_pClass = NULL;
}

void CMonoClass::Instantiate(IMonoArray *pConstructorParams)
{
	if(m_pInstance)
	{
		CryLogAlways("[Warning] Attempted to instantiate mono class with existing instance");
		return;
	}

	m_pInstance = (mono::object)mono_object_new(mono_domain_get(), m_pClass);

	// We need this to allow the GC to collect the class object later on.
	m_instanceHandle = mono_gchandle_new((MonoObject *)m_pInstance, false);

	if(pConstructorParams)
		CallMethod(".ctor(string)", pConstructorParams);
	else
		mono_runtime_object_init((MonoObject *)m_pInstance);
}

IMonoObject *CMonoClass::CallMethod(const char *methodName, IMonoArray *params, bool _static)
{
	if(MonoMethod *pMethod = GetMethod(methodName, _static))
	{
		MonoObject *pException = NULL;
		MonoObject *pResult = NULL;

		if(params)
		{
			try
			{
				pResult = mono_runtime_invoke_array(pMethod, _static ? NULL : m_pInstance, (MonoArray *)(mono::array)*params, &pException);
			}
			catch(char *str)
			{
				CryLogAlways("Exception was raised when invoking method %s: %s", methodName, str);
			}
		}
		else
		{
			try
			{
				pResult = mono_runtime_invoke(pMethod, _static ? NULL : m_pInstance, NULL, &pException);
			}
			catch(char *str)
			{
				CryLogAlways("Exception was raised when invoking method %s: %s", methodName, str);
			}
		}

		if(pException)
			HandleException(pException);
		else if(pResult)
			return *(mono::object)(pResult);
	}

	return NULL;
}

MonoMethod *CMonoClass::GetMethod(const char *methodName, bool bStatic)
{
	MonoMethod *pMethod = NULL;
	
	if(m_pClass)
	{
		MonoMethodDesc *pMethodDesc = mono_method_desc_new(":" + (string)methodName, false);
		MonoClass *pClass = m_pClass;

		while (pClass != NULL && pMethod == NULL) 
		{ 
			pMethod = mono_method_desc_search_in_class(pMethodDesc, pClass); 
			if (!pMethod) 
				pClass = mono_class_get_parent(pClass);
		}

		mono_method_desc_free(pMethodDesc);
	}

	// If overridden, get the "new" method.
	if (m_pInstance && !bStatic && pMethod)
        pMethod = mono_object_get_virtual_method((MonoObject *)m_pInstance, pMethod); 

	return pMethod;
}

IMonoObject *CMonoClass::GetProperty(const char *propertyName)
{
	if(MonoProperty *pProperty = mono_class_get_property_from_name(m_pClass, propertyName))
	{
		MonoObject *pException = NULL;

		MonoObject *propertyValue = mono_property_get_value(pProperty, m_pInstance, NULL, &pException);

		if(pException)
			HandleException(pException);
		else if(propertyValue)
			return *(mono::object)propertyValue;
	}

	return NULL;
}

void CMonoClass::SetProperty(const char *propertyName, IMonoObject *pNewValue)
{
	if(MonoProperty *pProperty = mono_class_get_property_from_name(m_pClass, propertyName))
	{
		void *args[1];
		args[0] = static_cast<CMonoObject *>(pNewValue)->GetMonoObject();

		return mono_property_set_value(pProperty, m_pInstance, args, NULL);
	}
}

IMonoObject *CMonoClass::GetField(const char *fieldName)
{
	if(MonoClassField *pField = mono_class_get_field_from_name(m_pClass, fieldName))
	{
		MonoObject *fieldValue = mono_field_get_value_object(mono_domain_get(), pField, (MonoObject *)m_pInstance);

		if(fieldValue)
			return *(mono::object)fieldValue;
	}

	return NULL;
}

void CMonoClass::SetField(const char *fieldName, IMonoObject *pNewValue)
{
	if(MonoClassField *pField = mono_class_get_field_from_name(m_pClass, fieldName))
		return mono_field_set_value((MonoObject *)m_pInstance, pField, static_cast<CMonoObject *>(pNewValue)->GetMonoObject());
}

void CMonoClass::HandleException(MonoObject *pException)
{
	CryLogAlways("[MonoWarning] Class %s raised an exception:", mono_class_get_name(m_pClass));

	MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());
	MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, NULL, NULL);
	CryLogAlways(ToCryString((mono::string)exceptionString));
}