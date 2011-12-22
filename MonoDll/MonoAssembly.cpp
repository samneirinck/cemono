#include "StdAfx.h"
#include "MonoAssembly.h"

#include "MonoClass.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

CMonoAssembly::CMonoAssembly(MonoDomain* pDomain, const char* assemblyPath)
{
	CRY_ASSERT_MESSAGE(pDomain, "CMonoAssembly::ctor Domain is NULL");
	CRY_ASSERT_MESSAGE(assemblyPath, "CMonoAssembly::ctor assemblyPath is NULL");

	m_assemblyPath = assemblyPath;
	
	m_pAssembly = mono_domain_assembly_open(pDomain,assemblyPath);
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

IMonoClass *CMonoAssembly::InstantiateClass(const char *nameSpace, const char *className, IMonoArray *pConstructorArguments)
{
	// Get class
	MonoClass *pClass = GetClassFromName(nameSpace, className);
	if (!pClass)
	{
		gEnv->pLog->LogError("Tried to create an instance of a NULL class");
		return NULL;
	}

	return new CMonoClass(mono_object_new(mono_domain_get(), pClass), pConstructorArguments);
}

MonoClass *CMonoAssembly::GetClassFromName(const char* nameSpace, const char* className)
{
	return mono_class_from_name(m_pImage, nameSpace, className);
}