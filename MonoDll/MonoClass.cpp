#include "StdAfx.h"
#include "MonoClass.h"

#include "MonoArray.h"
#include "MonoObject.h"

#include "MonoCVars.h"

#include <mono/metadata/debug-helpers.h>

CScriptClass::CScriptClass(MonoClass *pClass, IMonoArray *pConstructorArguments)
	: m_pClass(pClass)
	, m_pInstance(NULL)
{
	if (!m_pClass)
	{
		gEnv->pLog->LogError("Mono class object creation failed!");

		Release();
	}

	Instantiate(pConstructorArguments);
}

CScriptClass::~CScriptClass()
{
	mono_gchandle_free(m_instanceHandle);

	gEnv->pMonoScriptSystem->RemoveScriptInstance(m_scriptId);

	m_pInstance = 0;
	m_pClass = 0;
}

int CScriptClass::GetScriptId()
{
	if(IMonoObject *pScriptId = GetProperty("ScriptId"))
		return pScriptId->Unbox<int>();

	return -1;
}

void CScriptClass::Instantiate(IMonoArray *pConstructorParams)
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

	if(IMonoObject *pScriptId = GetProperty("ScriptId"))
		m_scriptId = pScriptId->Unbox<int>();
}

void CScriptClass::OnReload(MonoClass *pNewClass, mono::object pNewInstance)
{
	m_pClass = pNewClass;
	m_pInstance = pNewInstance;

	m_instanceHandle = mono_gchandle_new((MonoObject *)m_pInstance, false);
}

IMonoObject *CScriptClass::CallMethod(const char *methodName, IMonoArray *pParams, bool _static)
{
	if(!_static && !m_pInstance)
		CryLogAlways("[Warning] Attempting to invoke non-static method %s on non-instantiated class %s!", methodName, GetName());

	if(MonoMethod *pMethod = GetMethod(methodName, _static))
	{
		MonoObject *pException = NULL;
		MonoObject *pResult = NULL;

		MonoArray *params = pParams ? (MonoArray *)(mono::array)*pParams : NULL;

		try
		{
			pResult = mono_runtime_invoke_array(pMethod, _static ? NULL : m_pInstance, params, &pException);
		}
		catch(char *str)
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Exception was raised when invoking method %s: %s", methodName, str);
		}

		if(pException)
			HandleException(pException);
		else if(pResult)
			return *(mono::object)(pResult);
	}

	return NULL;
}

MonoMethod *CScriptClass::GetMethod(const char *methodName, bool bStatic)
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

IMonoObject *CScriptClass::GetProperty(const char *propertyName)
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

void CScriptClass::SetProperty(const char *propertyName, IMonoObject *pNewValue)
{
	if(MonoProperty *pProperty = mono_class_get_property_from_name(m_pClass, propertyName))
	{
		void *args[1];
		args[0] = static_cast<CScriptObject *>(pNewValue)->GetMonoObject();

		return mono_property_set_value(pProperty, m_pInstance, args, NULL);
	}
}

IMonoObject *CScriptClass::GetField(const char *fieldName)
{
	if(MonoClassField *pField = mono_class_get_field_from_name(m_pClass, fieldName))
	{
		MonoObject *fieldValue = mono_field_get_value_object(mono_domain_get(), pField, (MonoObject *)m_pInstance);

		if(fieldValue)
			return *(mono::object)fieldValue;
	}

	return NULL;
}

void CScriptClass::SetField(const char *fieldName, IMonoObject *pNewValue)
{
	if(MonoClassField *pField = mono_class_get_field_from_name(m_pClass, fieldName))
		return mono_field_set_value((MonoObject *)m_pInstance, pField, static_cast<CScriptObject *>(pNewValue)->GetMonoObject());
}

void CScriptClass::HandleException(MonoObject *pException)
{
	MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());
	MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, NULL, NULL);
	if(g_pMonoCVars->mono_exceptionsTriggerFatalErrors)
		CryFatalError(ToCryString((mono::string)exceptionString));
	if(g_pMonoCVars->mono_exceptionsTriggerMessageBoxes)
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, ToCryString((mono::string)exceptionString));
	else
		CryLogAlways(ToCryString((mono::string)exceptionString));
}