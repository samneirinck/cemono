#include "StdAfx.h"
#include "CemonoAssembly.h"
#include <mono/metadata/debug-helpers.h>


CCemonoAssembly::CCemonoAssembly(const char* assemblyPath)
{
	m_assemblyPath = assemblyPath;
	
	m_pAssembly = mono_domain_assembly_open(mono_domain_get(),assemblyPath);
}


CCemonoAssembly::~CCemonoAssembly(void)
{
}

MonoObject* CCemonoAssembly::CreateInstanceOf(const char* nameSpace, const char* className)
{
	// Get class
	MonoClass* pClass = GetClassFromName(nameSpace, className);
	if (!pClass)
	{
		gEnv->pLog->LogError("Tried to create an instance of a NULL class");
		return NULL;
	}

	MonoObject* pObject = mono_object_new(m_pDomain, pClass);
	if (!pObject)
	{
		gEnv->pLog->LogError("Failed to create mono object for %s.%s", nameSpace, className);
		return NULL;
	}

	mono_runtime_object_init(pObject);

	return pObject;
}

MonoObject* CCemonoAssembly::CallMethod(MonoObject* pObjectInstance, const char* methodName)
{
	MonoObject* pReturnObject = NULL;
	MonoClass *pClass = GetClassFromObject(pObjectInstance);
	if (pClass)
	{
		MonoMethod* pMethod = GetMethodFromClassAndMethodName(pClass, methodName);
		if (pMethod)
		{
			pReturnObject = mono_runtime_invoke(pMethod, pObjectInstance, NULL, NULL);
		}
	}
	return pReturnObject;
}

MonoClass* CCemonoAssembly::GetClassFromObject(MonoObject* pObjectInstance)
{
	return mono_object_get_class(pObjectInstance);
}

MonoClass* CCemonoAssembly::GetClassFromName(const char* nameSpace, const char* className)
{
	string fullClassName = string(nameSpace).append(".").append(className);
	if (m_monoClasses.find(fullClassName) == m_monoClasses.end())
	{
		m_monoClasses[fullClassName] = mono_class_from_name(m_pImage, nameSpace, className);
	}
	return m_monoClasses[fullClassName];
}

MonoMethod* CCemonoAssembly::GetMethodFromClassAndMethodName(MonoClass* pClass, const char* methodName)
{
	if (m_monoMethods.find(pClass) == m_monoMethods.end())
	{
		m_monoMethods[pClass] = std::map<const char*,MonoMethod*>();
	}
	
	std::map<const char*, MonoMethod*> methodMappings = m_monoMethods[pClass];
	if (methodMappings.find(methodName) == methodMappings.end())
	{
		MonoMethod* pMethod = NULL;
		MonoMethodDesc* pMethodDescription = mono_method_desc_new(methodName, false);
		if (pMethodDescription)
		{
			pMethod = mono_method_desc_search_in_class(pMethodDescription, pClass);
			mono_method_desc_free(pMethodDescription);
		}

		methodMappings[methodName] = pMethod;
	}

	return methodMappings[methodName];
}