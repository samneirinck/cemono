#include "StdAfx.h"
#include "MonoClassUtils.h"
#include "IValidator.h"

MonoObject* CMonoClassUtils::CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass)
{
	if (!pClass)
	{
		CryError("Tried to create an instance of a NULL class");
		return NULL;
	}

	MonoObject* pObject = mono_object_new(pDomain, pClass);
	if (!pObject)
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
	if (!pClass)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find class %s in %s", className, nameSpace);
	}
	return pClass;
}

MonoObject *CMonoClassUtils::CallMethod(string funcName, MonoClass *pClass, MonoObject *pInstance, void **args)
{
	MonoMethodDesc *pFooDesc = mono_method_desc_new (":" + funcName, false);

	MonoMethod* monoMethod = mono_method_desc_search_in_class(pFooDesc, pClass); 
    assert(monoMethod != NULL); //OK

	mono_method_desc_free (pFooDesc);

	return mono_runtime_invoke(monoMethod, pInstance, args, NULL);
}

MonoObject *CMonoClassUtils::CallMethod(string funcName, string _className, string _nameSpace, MonoImage *pImage, MonoObject *pInstance, void **args)
{
	MonoClass *pClass = mono_class_from_name(pImage, _nameSpace, _className);

	return CallMethod(funcName, pClass, pInstance, args);
}