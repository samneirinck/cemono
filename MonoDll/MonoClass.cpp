#include "StdAfx.h"
#include "MonoClass.h"

#include "MonoArray.h"
#include "MonoObject.h"

#include "MonoCVars.h"

CScriptClass::CScriptClass(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	m_pObject = (MonoObject *)pClass; 
	m_objectHandle = -1;
}

IMonoObject *CScriptClass::CreateInstance(IMonoArray *pConstructorParams)
{
	MonoObject *pInstance = mono_object_new(mono_domain_get(), (MonoClass *)m_pObject);

	return new CScriptObject(pInstance, pConstructorParams);
}

MonoMethod *CScriptClass::GetMonoMethod(const char *methodName, IMonoArray *pArgs)
{
	MonoMethodSignature *pSignature = NULL;

	void *pIterator = 0;

	MonoClass *pClass = (MonoClass *)m_pObject;
	MonoType *pClassType = mono_class_get_type(pClass);
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

	MonoWarning("Failed to get method %s in class %s", methodName, GetName());
	return NULL;
}

MonoProperty *CScriptClass::GetMonoProperty(const char *name)
{
	MonoProperty *pProperty = mono_class_get_property_from_name((MonoClass *)m_pObject, name);
	if(!pProperty)
		MonoWarning("Failed to get property %s in class %s", name, GetName());

	return pProperty;
}

MonoClassField *CScriptClass::GetMonoField(const char *name)
{
	MonoClassField *pField = mono_class_get_field_from_name((MonoClass *)m_pObject, name);
	if(!pField)
		MonoWarning("Failed to get field %s in class %s", name, GetName());

	return pField;
}