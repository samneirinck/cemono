#include "StdAfx.h"
#include "MonoClass.h"

#include "MonoArray.h"
#include "MonoObject.h"

#include "MonoCVars.h"

#include <mono/metadata/debug-helpers.h>

CScriptClass::CScriptClass(MonoClass *pClass, IMonoArray *pConstructorArguments)
	: m_pClass(pClass)
	, m_pInstance(NULL)
	, m_scriptId(-1)
{
	Instantiate(pConstructorArguments);
}

CScriptClass::CScriptClass(MonoClass *pClass, mono::object instance)
	: m_pClass(pClass)
	, m_pInstance(instance) 
	, m_scriptId(-1)
{
	m_instanceHandle = mono_gchandle_new((MonoObject *)m_pInstance, false);

	GetScriptId();
}

CScriptClass::~CScriptClass()
{
	mono_gchandle_free(m_instanceHandle);

	if(gEnv->pMonoScriptSystem)
		gEnv->pMonoScriptSystem->RemoveScriptInstance(m_scriptId);

	m_pInstance = 0;
	m_pClass = 0;
}

int CScriptClass::GetScriptId()
{
	if(m_scriptId == -1)
	{
		if(IMonoObject *pScriptId = GetProperty("ScriptId"))
			m_scriptId = pScriptId->Unbox<int>();
	}

	return m_scriptId;
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

	GetScriptId();
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
		MonoObject *pException = NULL;

		MonoObject *pResult = mono_runtime_invoke_array(pMethod, _static ? NULL : m_pInstance, pParams ? (MonoArray *)(mono::array)*pParams : NULL, &pException);

		if(pException)
			HandleException(pException);
		else if(pResult)
			return *(mono::object)(pResult);
	}
	else
		CryLogAlways("[Warning] Failed to get method %s in object of class %s", methodName, GetName());


	return NULL;
}

MonoMethod *CScriptClass::GetMethod(const char *methodName, IMonoArray *pArgs, bool bStatic)
{
	MonoMethodSignature *pSignature = NULL;

	void *pIterator = 0;

	MonoType *pClassType = mono_class_get_type(m_pClass);
	MonoClass *pClass = m_pClass;
	MonoMethod *pCurMethod = NULL;

	int suppliedArgsCount = pArgs ? pArgs->GetSize() : 0;

	while (pClass != NULL)
	{
		pCurMethod = mono_class_get_methods(pClass, &pIterator);
		if(pCurMethod == NULL)
		{
			pClass = mono_class_get_parent(pClass);
			if(pClass == mono_get_object_class())
				break;

			pIterator = 0;
			continue;
		}

		pSignature = mono_method_signature(pCurMethod);
		int signatureParamCount = mono_signature_get_param_count(pSignature);

		bool bCorrectName = !strcmp(mono_method_get_name(pCurMethod), methodName);
		if(bCorrectName && signatureParamCount == 0 && suppliedArgsCount == 0)
			return pCurMethod;
		else if(bCorrectName && signatureParamCount >= suppliedArgsCount)
		{
			//if(bStatic != (mono_method_get_flags(pCurMethod, NULL) & METHOD_ATTRIBUTE_STATIC) > 0)
				//continue;

			void *pIter = NULL;

			MonoType *pType = NULL;
			for(int i = 0; i < signatureParamCount; i++)
			{
				pType = mono_signature_get_params(pSignature, &pIter);

				if(IMonoObject *pItem = pArgs->GetItem(i))
				{
					EMonoAnyType anyType = pItem->GetType();
					MonoTypeEnum monoType = (MonoTypeEnum)mono_type_get_type(pType);

					if(monoType == MONO_TYPE_BOOLEAN && anyType != eMonoAnyType_Boolean)
						break;
					else if(monoType == MONO_TYPE_I4 && anyType != eMonoAnyType_Integer)
						break;
					else if(monoType == MONO_TYPE_U4 && anyType != eMonoAnyType_UnsignedInteger)
						break;
					else if(monoType == MONO_TYPE_I2 && anyType != eMonoAnyType_Short)
						break;
					else if(monoType == MONO_TYPE_U2 && anyType != eMonoAnyType_UnsignedShort)
						break;
					else if(monoType == MONO_TYPE_STRING && anyType != eMonoAnyType_String)
						break;
				}

				if(i + 1 == suppliedArgsCount)
					return pCurMethod;
			}
		}
	}

	return NULL;
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
		args[0] = pNewValue->GetMonoObject();

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
		return mono_field_set_value((MonoObject *)m_pInstance, pField, pNewValue->GetMonoObject());
}

void CScriptClass::HandleException(MonoObject *pException)
{
	MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());
	MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, NULL, NULL);
	if(g_pMonoCVars->mono_exceptionsTriggerFatalErrors)
		CryFatalError(ToCryString((mono::string)exceptionString));
	if(g_pMonoCVars->mono_exceptionsTriggerMessageBoxes)
		CryMessageBox(ToCryString((mono::string)exceptionString), "CryMono exception was raised", 0x00000000L);
	else
		CryLogAlways(ToCryString((mono::string)exceptionString));
}