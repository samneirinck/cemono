#include "StdAfx.h"
#include "MonoAssembly.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

#include "PathUtils.h"
#include "MonoScriptSystem.h"

#include <MonoClass.h>

CScriptAssembly::CScriptAssembly(const char *assemblyPath)
{
#ifndef _RELEASE
	string path = assemblyPath;
	if(path.find("pdb2mdb")==-1)
	{
		TCHAR tempPath[MAX_PATH];
		GetTempPath(MAX_PATH, tempPath);

		int lastDirectoryIndex = path.find_last_of("\\");
		if(path.find_last_of("//") < lastDirectoryIndex)
			lastDirectoryIndex = path.find_last_of("//");

		string newAssemblyPath = tempPath + path.substr(lastDirectoryIndex + 1);
		CopyFile(assemblyPath, newAssemblyPath, false);

		if(IMonoAssembly *pDebugDatabaseCreator = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetDebugDatabaseCreator())
		{
			if(IMonoClass *pDriverClass = pDebugDatabaseCreator->GetCustomClass("Driver", ""))
			{
				IMonoArray *pArgs = CreateMonoArray(1);
				pArgs->Insert(assemblyPath);
				pDriverClass->CallMethod("Convert", pArgs, true);

				SAFE_RELEASE(pArgs);
				SAFE_RELEASE(pDriverClass);
			}
		}

		CopyFile(path.append(".mdb"), newAssemblyPath + ".mdb", false);

		assemblyPath = newAssemblyPath;
	}
#endif

	m_assemblyPath = assemblyPath;

	m_pAssembly = mono_domain_assembly_open(mono_domain_get(), m_assemblyPath);
	if (!m_pAssembly)
	{
		gEnv->pLog->LogError("Failed to create assembly from %s", assemblyPath);
		
		Release();
	}

	m_pImage = mono_assembly_get_image(m_pAssembly);
	if (!m_pImage)
	{
		gEnv->pLog->LogError("Failed to get image from assembly %s", assemblyPath);
		
		Release();
	}
}

CScriptAssembly::~CScriptAssembly()
{
	m_assemblyPath = 0;

	m_pAssembly = 0;
	m_pImage = 0;
}

IMonoClass *CScriptAssembly::InstantiateClass(const char *className, const char *nameSpace, IMonoArray *pConstructorArguments)
{
	// Get class
	MonoClass *pClass = GetClassFromName(nameSpace, className);
	if (!pClass)
	{
		gEnv->pLog->LogError("Tried to create an instance of non-existent class %s in namespace %s", className, nameSpace);
		return NULL;
	}

	return new CScriptClass(pClass, pConstructorArguments);
}

IMonoClass *CScriptAssembly::GetCustomClass(const char *className, const char *nameSpace)
{ 
	if(MonoClass *monoClass = GetClassFromName(nameSpace, className))
		return new CScriptClass(monoClass);

	return NULL;
}

MonoClass *CScriptAssembly::GetClassFromName(const char* nameSpace, const char* className)
{
	return mono_class_from_name(m_pImage, nameSpace, className);
}