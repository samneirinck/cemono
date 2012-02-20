#include "StdAfx.h"
#include "MonoAssembly.h"

#include "MonoClass.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

CMonoAssembly::CMonoAssembly(const char *assemblyPath)
{
	string path = assemblyPath;

	TCHAR tempPath[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);

	string newAssemblyPath = tempPath + path.substr(path.find_last_of("\\") + 1);
	CopyFile(assemblyPath, newAssemblyPath, false);

	m_assemblyPath = newAssemblyPath;
	
	m_pAssembly = mono_domain_assembly_open(mono_domain_get(), newAssemblyPath);
	if (!m_pAssembly)
	{
		gEnv->pLog->LogError("Failed to create assembly from %s", assemblyPath);
		
		delete this;
	}
	m_pImage = mono_assembly_get_image(m_pAssembly);
	if (!m_pImage)
	{
		gEnv->pLog->LogError("Failed to get image from assembly %s", assemblyPath);
		
		delete this;
	}
}

CMonoAssembly::~CMonoAssembly()
{
	m_assemblyPath = 0;

	m_pAssembly = 0;
	m_pImage = 0;
}

IMonoClass *CMonoAssembly::InstantiateClass(const char *className, const char *nameSpace, IMonoArray *pConstructorArguments)
{
	// Get class
	MonoClass *pClass = GetClassFromName(nameSpace, className);
	if (!pClass)
	{
		gEnv->pLog->LogError("Tried to create an instance of non-existent class %s in namespace %s", className, nameSpace);
		return NULL;
	}

	return new CMonoClass(pClass, pConstructorArguments);
}

IMonoClass *CMonoAssembly::GetCustomClass(const char *className, const char *nameSpace)
{ 
	if(MonoClass *monoClass = mono_class_from_name(GetImage(), nameSpace, className))
		return new CMonoClass(monoClass);

	return NULL;
}

MonoClass *CMonoAssembly::GetClassFromName(const char* nameSpace, const char* className)
{
	return mono_class_from_name(m_pImage, nameSpace, className);
}