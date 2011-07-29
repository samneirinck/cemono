#include "StdAfx.h"
#include "MonoClassUtils.h"
#include "IValidator.h"

MonoObject* CMonoClassUtils::CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass)
{
	if (pClass == NULL)
	{
		CryError("Tried to create an instance of a NULL class");
		return NULL;
	}

	MonoObject* pObject = mono_object_new(pDomain, pClass);
	if (pObject == NULL)
	{
		CryError("Failed to create mono object");
		return NULL;
	}

	mono_runtime_object_init(pObject);

	return pObject;
}

MonoObject* CMonoClassUtils::CreateInstanceOf(MonoClass* pClass)
{
	return CreateInstanceOf(mono_domain_get(), pClass);
}

MonoClass* CMonoClassUtils::GetClassByName(const char* nameSpace, const char* className)
{
	MonoClass* pClass = mono_class_from_name(g_pMono->GetBclImage(), nameSpace, className);
	if (pClass == NULL)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find class %s in %s", className, nameSpace);
	}
	return pClass;
}

MonoObject* CMonoClassUtils::CallMethod(MonoObject* pObjectInstance, const char* methodName)
{
	return CallMethod(pObjectInstance, methodName, NULL);
}

MonoObject* CMonoClassUtils::CallMethod(MonoObject* pObjectInstance, MonoMethod* pMethod)
{
	MonoClass *pClass;
	MonoObject *pReturnObject = NULL;

	pClass = mono_object_get_class(pObjectInstance);
	if (pClass)
	{
			if (pMethod)
			{
				pReturnObject = mono_runtime_invoke(pMethod, pObjectInstance, NULL, NULL);
			} 
	} 

	return NULL;
}

MonoObject* CMonoClassUtils::CallMethod(MonoObject* pObjectInstance, const char* methodName, void** args)
{
	MonoClass *pClass;
	MonoMethod *pMethod;
	MonoObject *pReturnObject = NULL;
	MonoMethodDesc *desc = NULL;

	pClass = mono_object_get_class(pObjectInstance);
	if (pClass)
	{
		desc = mono_method_desc_new(methodName, false);

		if (desc)
		{
			pMethod = mono_method_desc_search_in_class(desc, pClass);
			if (pMethod)
			{
				pReturnObject = mono_runtime_invoke(pMethod, pObjectInstance, ((args != NULL) ? args : NULL), NULL);
			} 
			else 
			{
				CryError("Failed to find method %s", methodName);
			}
		} 
		else 
		{
			CryError("Invalid method description %s", methodName);
		}
	} 
	else 
	{
		CryError("Failed to find class while trying to invoke %s", methodName);
	}

	// Clean up
	if (desc)
		mono_method_desc_free(desc);


	return NULL;
}