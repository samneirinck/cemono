#include "StdAfx.h"
#include "MonoAssembly.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

#include "PathUtils.h"
#include "MonoScriptSystem.h"

#include <MonoClass.h>

CScriptAssembly::~CScriptAssembly()
{
	m_pAssembly = 0;
	m_pImage = 0;
}

const char *CScriptAssembly::Relocate(const char *originalAssemblyPath)
{
	string newAssemblyPath = PathUtils::GetTempPath() + PathUtil::GetFile(originalAssemblyPath);

	CopyFile(originalAssemblyPath, newAssemblyPath, false);

#ifndef _RELEASE
	CopyFile(string(originalAssemblyPath).append(".mdb"), newAssemblyPath.append(".mdb"), false);
#endif

	return newAssemblyPath.c_str();
}

IMonoClass *CScriptAssembly::InstantiateClass(const char *className, const char *nameSpace, IMonoArray *pConstructorArguments)
{
	// Get class
	MonoClass *pClass = GetClassFromName(nameSpace, className);
	if (!pClass)
	{
		MonoWarning("Tried to create an instance of non-existent class %s.%s", nameSpace, className);
		return NULL;
	}

	return new CScriptClass(pClass, pConstructorArguments);
}

IMonoClass *CScriptAssembly::GetCustomClass(const char *className, const char *nameSpace)
{ 
	if(MonoClass *monoClass = GetClassFromName(nameSpace, className))
		return new CScriptClass(monoClass);

	MonoWarning("Failed to get class %s.%s", nameSpace, className);
	return NULL;
}

MonoClass *CScriptAssembly::GetClassFromName(const char* nameSpace, const char* className)
{
	return mono_class_from_name(m_pImage, nameSpace, className);
}