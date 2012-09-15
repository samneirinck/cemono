#include "StdAfx.h"
#include "MonoClass.h"

#include "MonoScriptSystem.h"

#include "MonoArray.h"
#include "MonoObject.h"
#include "MonoAssembly.h"

#include "MonoCVars.h"

CScriptClass::CScriptClass(MonoClass *pClass, CScriptAssembly *pDeclaringAssembly)
	: m_pDeclaringAssembly(pDeclaringAssembly)
	, m_refs(0)
{
	CRY_ASSERT(pClass);

	m_pObject = (MonoObject *)pClass; 
	m_objectHandle = -1;
	m_pClass = NULL;
	m_scriptId = -1;

	m_name = string(mono_class_get_name(pClass));
	m_namespace = string(mono_class_get_namespace(pClass));
}

CScriptClass::~CScriptClass()
{
	m_name.clear();
	m_namespace.clear();

	// Remove this class from the assembly's class registry, and decrement its release counter.
	m_pDeclaringAssembly->OnClassReleased(this);
}

IMonoObject *CScriptClass::CreateInstance(IMonoArray *pConstructorParams)
{
	MonoObject *pInstance = mono_object_new(mono_domain_get(), (MonoClass *)m_pObject);

	return new CScriptObject(pInstance, pConstructorParams);
}

IMonoObject *CScriptClass::InvokeArray(IMonoObject *pObject, const char *methodName, IMonoArray *pParams)
{
	MonoMethod *pMethod = GetMonoMethod(methodName, pParams);
	CRY_ASSERT(pMethod);

	MonoObject *pException = nullptr;
	MonoObject *pResult = mono_runtime_invoke_array(pMethod, pObject ? pObject->GetManagedObject() : nullptr, pParams ? (MonoArray *)pParams->GetManagedObject() : nullptr, &pException);

	if(pException)
		HandleException(pException);
	else if(pResult)
		return *(mono::object)(pResult);

	return nullptr;
}

IMonoObject *CScriptClass::Invoke(IMonoObject *pObject, const char *methodName, void **pParams, int numParams)
{
	MonoMethod *pMethod = GetMonoMethod(methodName, numParams);
	CRY_ASSERT(pMethod);

	MonoObject *pException = nullptr;
	MonoObject *pResult = mono_runtime_invoke(pMethod, pObject ? pObject->GetManagedObject() : nullptr, pParams, &pException);

	if(pException)
		HandleException(pException);
	else if(pResult)
		return *(mono::object)(pResult);

	return nullptr;
}

MonoMethod *CScriptClass::GetMonoMethod(const char *methodName, IMonoArray *pArgs)
{
	MonoMethodSignature *pSignature = nullptr;

	void *pIterator = 0;

	MonoClass *pClass = (MonoClass *)m_pObject;
	MonoType *pClassType = mono_class_get_type(pClass);
	MonoMethod *pCurMethod = nullptr;

	int suppliedArgsCount = pArgs ? pArgs->GetSize() : 0;

	while (pClass != nullptr)
	{
		pCurMethod = mono_class_get_methods(pClass, &pIterator);
		if(pCurMethod == nullptr)
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
			//if(bStatic != (mono_method_get_flags(pCurMethod, nullptr) & METHOD_ATTRIBUTE_STATIC) > 0)
				//continue;

			void *pIter = nullptr;

			MonoType *pType = nullptr;
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

	MonoWarning("Failed to get method %s in class %s", methodName, GetName());
	return nullptr;
}

MonoMethod *CScriptClass::GetMonoMethod(const char *methodName, int numParams)
{
	MonoMethodSignature *pSignature = nullptr;

	void *pIterator = 0;

	MonoClass *pClass = (MonoClass *)m_pObject;
	MonoType *pClassType = mono_class_get_type(pClass);
	MonoMethod *pCurMethod = nullptr;

	while (pClass != nullptr)
	{
		pCurMethod = mono_class_get_methods(pClass, &pIterator);
		if(pCurMethod == nullptr)
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
		if(bCorrectName && signatureParamCount == numParams)
			return pCurMethod;
	}

	MonoWarning("Failed to get method %s in class %s", methodName, GetName());
	return nullptr;
}

IMonoObject *CScriptClass::GetPropertyValue(IMonoObject *pObject, const char *propertyName)
{
	if(MonoProperty *pProperty = GetMonoProperty(propertyName))
	{
		MonoObject *pException = nullptr;

		MonoObject *propertyValue = mono_property_get_value(pProperty, pObject ? pObject->GetManagedObject() : nullptr, nullptr, &pException);

		if(pException)
			HandleException(pException);
		else if(propertyValue)
			return *(mono::object)propertyValue;
	}

	return nullptr;
}

void CScriptClass::SetPropertyValue(IMonoObject *pObject, const char *propertyName, IMonoObject *pNewValue)
{
	if(MonoProperty *pProperty = GetMonoProperty(propertyName))
	{
		void *args[1];
		args[0] = pNewValue ? pNewValue->GetManagedObject() : nullptr;

		mono_property_set_value(pProperty, pObject ? pObject->GetManagedObject() : nullptr, args, nullptr);
	}
}

IMonoObject *CScriptClass::GetFieldValue(IMonoObject *pObject, const char *fieldName)
{
	if(MonoClassField *pField = GetMonoField(fieldName))
	{
		MonoObject *fieldValue = mono_field_get_value_object(mono_domain_get(), pField, (MonoObject *)(pObject ? pObject->GetManagedObject() : nullptr));

		if(fieldValue)
			return *(mono::object)fieldValue;

		return nullptr;
	}

	return nullptr;
}

void CScriptClass::SetFieldValue(IMonoObject *pObject, const char *fieldName, IMonoObject *pNewValue)
{
	if(MonoClassField *pField = GetMonoField(fieldName))
		mono_field_set_value((MonoObject *)(pObject ? pObject->GetManagedObject() : nullptr), pField, pNewValue ? pNewValue->GetManagedObject() : nullptr);
}

MonoProperty *CScriptClass::GetMonoProperty(const char *name)
{
	return mono_class_get_property_from_name((MonoClass *)m_pObject, name);
}

MonoClassField *CScriptClass::GetMonoField(const char *name)
{
	return mono_class_get_field_from_name((MonoClass *)m_pObject, name);
}

IMonoObject *CScriptClass::BoxObject(void *object)
{
	return *(mono::object)mono_value_box(mono_domain_get(), (MonoClass *)m_pObject, object);
}