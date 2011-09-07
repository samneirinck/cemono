#include "StdAfx.h"
#include "CemonoAssembly.h"

#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

CCemonoAssembly::CCemonoAssembly(MonoDomain* pDomain, const char* assemblyPath)
{
	CRY_ASSERT_MESSAGE(pDomain, "CCemonoAssembly::ctor Domain is NULL");
	CRY_ASSERT_MESSAGE(assemblyPath, "CCemonoAssembly::ctor assemblyPath is NULL");

	m_assemblyPath = assemblyPath;
	m_pDomain = pDomain;
	
	m_pAssembly = mono_domain_assembly_open(m_pDomain,assemblyPath);
	if (!m_pAssembly)
	{
		gEnv->pLog->LogError("Failed to create assembly from %s", assemblyPath);
		return;
	}
	m_pImage = mono_assembly_get_image(m_pAssembly);
	if (!m_pImage)
	{
		gEnv->pLog->LogError("Failed to get image from assembly %s", assemblyPath);
		return;
	}
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