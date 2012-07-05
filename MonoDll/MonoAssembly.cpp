#include "StdAfx.h"
#include "MonoAssembly.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

#include "PathUtils.h"
#include "MonoScriptSystem.h"

#include <MonoClass.h>

CScriptAssembly::CScriptAssembly(MonoImage *pImage, const char *path, bool nativeAssembly)
	: m_bNative(nativeAssembly) // true if this assembly was loaded via C++.
{
	CRY_ASSERT(pImage);
	m_pObject = (MonoObject *)pImage;

	m_path = string(path);
}

CScriptAssembly::~CScriptAssembly()
{
	CScriptSystem *pScriptSystem = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem);
	stl::find_and_erase(pScriptSystem->m_assemblies, this);

	m_pObject = 0;
}

IMonoClass *CScriptAssembly::GetClass(const char *className, const char *nameSpace)
{ 
	if(MonoClass *monoClass = mono_class_from_name((MonoImage *)m_pObject, nameSpace, className))
		return TryGetClass(monoClass);

	MonoWarning("Failed to get class %s.%s", nameSpace, className);
	return nullptr;
}

CScriptClass *CScriptAssembly::TryGetClass(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	for each(auto pair in m_classRegistry)
	{
		if(pair.second == pClass)
		{
			return pair.first;
			pair.first->AddRef();
		}
	}

	CScriptClass *pScriptClass = new CScriptClass(pClass, this);
	m_classRegistry.insert(TClassMap::value_type(pScriptClass, pClass));
	pScriptClass->AddRef();

	return pScriptClass;
}

///////////////////////////////////////////////////////////////////
// Statics
///////////////////////////////////////////////////////////////////
 
CScriptAssembly *CScriptAssembly::TryGetAssembly(MonoImage *pImage)
{
	CRY_ASSERT(pImage);

	CScriptSystem *pScriptSystem = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem);

	for each(auto assembly in pScriptSystem->m_assemblies)
	{
		if(assembly->GetImage() == pImage)
		{
			assembly->AddRef();
			return assembly;
		}
	}

	// This assembly was loaded from managed code.
	CScriptAssembly *pAssembly = new CScriptAssembly(pImage, mono_image_get_filename(pImage), false);
	pScriptSystem->m_assemblies.push_back(pAssembly);
	pAssembly->AddRef();

	return pAssembly;
}

CScriptClass *CScriptAssembly::TryGetClassFromRegistry(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	MonoImage *pImage = mono_class_get_image(pClass);

	if(auto pAssembly = TryGetAssembly(pImage))
		return pAssembly->TryGetClass(pClass);

	return NULL;
}