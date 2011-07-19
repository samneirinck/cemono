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

