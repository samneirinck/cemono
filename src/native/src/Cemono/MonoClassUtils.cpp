#include "StdAfx.h"
#include "MonoClassUtils.h"
#include <IValidator.h>

MonoObject* MonoClassUtils::CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass)
{
	if (!pClass)
	{
		gEnv->pLog->LogError("Tried to create an instance of a NULL class");
		return NULL;
	}

	MonoObject* pObject = mono_object_new(pDomain, pClass);
	if (!pObject)
	{
		gEnv->pLog->LogError("Failed to create mono object");
		return NULL;
	}

	mono_runtime_object_init(pObject);

	return pObject;
}

MonoObject* MonoClassUtils::CreateInstanceOf(MonoClass* pClass)
{
	return CreateInstanceOf(mono_domain_get(), pClass);
}

MonoClass* MonoClassUtils::GetClassByName(const char* nameSpace, const char* className)
{
	/*MonoClass* pClass = mono_class_from_name(g_pMono->GetBclImage(), nameSpace, className);
	if (!pClass)
	{
		gEnv->pLog->LogError("Could not find class %s in %s", className, nameSpace);
	}
	return pClass;*/
	return NULL;
}

MonoObject* MonoClassUtils::CallMethod(MonoObject* pObjectInstance, const char* methodName)
{
	return CallMethod(pObjectInstance, methodName, NULL);
}

MonoObject* MonoClassUtils::CallMethod(MonoObject* pObjectInstance, MonoMethod* pMethod)
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

MonoObject* MonoClassUtils::CallMethod(MonoObject* pObjectInstance, const char* methodName, void** args)
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
				gEnv->pLog->LogError("Failed to find method %s", methodName);
			}
		} 
		else 
		{
			gEnv->pLog->LogError("Invalid method description %s", methodName);
		}
	} 
	else 
	{
		gEnv->pLog->LogError("Failed to find class while trying to invoke %s", methodName);
	}

	// Clean up
	if (desc)
		mono_method_desc_free(desc);


	return NULL;
}


MonoObject *MonoClassUtils::CallMethod(string funcName, MonoClass *pClass, MonoObject *pInstance, void **args)
{
	MonoMethodDesc *pFooDesc = mono_method_desc_new (":" + funcName, false);

	MonoMethod* monoMethod = mono_method_desc_search_in_class(pFooDesc, pClass); 
	assert(monoMethod != NULL); //OK

	mono_method_desc_free (pFooDesc);

	return mono_runtime_invoke(monoMethod, pInstance, args, NULL);
}

MonoObject *MonoClassUtils::CallMethod(string funcName, string _className, string _nameSpace, MonoImage *pImage, MonoObject *pInstance, void **args)
{
	MonoClass *pClass = mono_class_from_name(pImage, _nameSpace, _className);

	return CallMethod(funcName, pClass, pInstance, args);
}