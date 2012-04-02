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

	if(MonoMethod *pMethod = GetMethod(methodName, pParams, _static))
	{
		// If overridden, get the "new" method.
		if (m_pInstance && !_static)
			pMethod = mono_object_get_virtual_method((MonoObject *)m_pInstance, pMethod);

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
	else
		CryLogAlways("[Warning] Failed to get method %s", methodName);

	return NULL;
}

MonoMethod *CScriptClass::GetMethod(const char *methodName, IMonoArray *pArgs, bool bStatic)
{
	if(g_pMonoCVars->mono_useExperimentalMethodFinding)
	{
		MonoMethodSignature *pSignature = NULL;

		void *pIterator = NULL;

		MonoType *pClassType = mono_class_get_type(m_pClass);
		MonoClass *pClass = m_pClass;
		MonoMethod *pCurMethod = NULL;

		while (pClass != NULL)
		{
			pCurMethod = mono_class_get_methods(pClass, &pIterator);
			if(pCurMethod == NULL)
			{
				pClass = mono_class_get_parent(pClass);
				if(pClass == mono_get_object_class())
					break;

				continue;
			}

			if(strcmp(mono_method_get_name(pCurMethod), methodName))
				continue;

			//if(bStatic != (mono_method_get_flags(pCurMethod, NULL) & METHOD_ATTRIBUTE_STATIC) > 0)
				//continue;

			pSignature = mono_method_signature(pCurMethod);

			int signatureParamCount = mono_signature_get_param_count(pSignature);
			int suppliedArgsCount = pArgs ? pArgs->GetSize() : 0;

			if(suppliedArgsCount > 0)
			{
				void *pIter = NULL;

				MonoType *pType = NULL;
				for(int i = 0; i < signatureParamCount; i++)
				{
					pType = mono_signature_get_params(pSignature, &pIter);

					if(IMonoObject *pItem = pArgs->GetItem(i))
					{
						MonoAnyType anyType = pItem->GetType();
						MonoTypeEnum monoType = (MonoTypeEnum)mono_type_get_type(pType);

						if(monoType == MONO_TYPE_BOOLEAN && anyType != MONOTYPE_BOOL)
							break;
						else if(monoType == MONO_TYPE_I4 && anyType != MONOTYPE_INT)
							break;
						else if(monoType == MONO_TYPE_U4 && anyType != MONOTYPE_UINT)
							break;
						else if(monoType == MONO_TYPE_I2 && anyType != MONOTYPE_SHORT)
							break;
						else if(monoType == MONO_TYPE_U2 && anyType != MONOTYPE_USHORT)
							break;
						else if(monoType == MONO_TYPE_STRING && anyType != MONOTYPE_STRING && anyType != MONOTYPE_WSTRING)
							break;
					}
					else if(suppliedArgsCount - 1 < i)
						return pCurMethod;

					if(i >= suppliedArgsCount - 1)
						return pCurMethod;
				}
			}
			else if(signatureParamCount == 0)
				return pCurMethod; // The args desired to be sent and the method signature's num args were both 0, we can safely return this MonoMethod object.
		}

		return NULL;
	}

	// Old method, to be removed when the new implementation is stable.
	int numParams = pArgs ? pArgs->GetSize() : 0;
	MonoMethod *pMethod = NULL;

	if(m_pClass)
	{
		MonoMethodDesc *pMethodDesc = mono_method_desc_new(":" + (string)methodName, false);
		MonoClass *pClass = m_pClass;

		while (pClass != NULL && pMethod == NULL) 
		{
			// TODO: Accurate method signature matching; currently several methods with the same name and amount of parameters will break.
			if(numParams > -1)
			{
				if(!strcmp(methodName, "OnClientConnect"))
				{
					// Fugly temporary solution to get optional args to work.
					for(int i = numParams; pMethod == NULL && i >= 0; i++)
						pMethod = mono_class_get_method_from_name(pClass, methodName, i);
				}
				else
				{
				// Fugly temporary solution to get optional args to work.
				for(int i = numParams; pMethod == NULL && i >= 0; i--)
					pMethod = mono_class_get_method_from_name(pClass, methodName, i);
				}
			}
			else
				pMethod = mono_method_desc_search_in_class(pMethodDesc, pClass); 
			if (!pMethod) 
				pClass = mono_class_get_parent(pClass);
		}

		mono_method_desc_free(pMethodDesc);

		// If overridden, get the "new" method.
		if (m_pInstance && !bStatic && pMethod)
			pMethod = mono_object_get_virtual_method((MonoObject *)m_pInstance, pMethod);
	}

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