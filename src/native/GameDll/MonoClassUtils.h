#pragma once

#include "StdAfx.h"
#include "Mono.h"

class CMonoClassUtils
{
public:
	static MonoObject* CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass)
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

	static MonoObject* CreateInstanceOf(MonoClass* pClass)
	{
		return CreateInstanceOf(mono_domain_get(), pClass);
	}

};