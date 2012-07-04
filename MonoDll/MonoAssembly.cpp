#include "StdAfx.h"
#include "MonoAssembly.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

#include "PathUtils.h"
#include "MonoScriptSystem.h"

#include <MonoClass.h>

CScriptAssembly::CScriptAssembly(MonoImage *pImage, const char *path)
	: m_pImage(pImage)
{
	CRY_ASSERT(m_pImage);

	m_path = string(path);
}

CScriptAssembly::~CScriptAssembly()
{
	m_pImage = 0;
}

IMonoClass *CScriptAssembly::GetClass(const char *className, const char *nameSpace)
{ 
	if(MonoClass *monoClass = mono_class_from_name(m_pImage, nameSpace, className))
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
			return pair.first;
	}

	CScriptClass *pScriptClass = new CScriptClass(pClass, this);
	m_classRegistry.insert(TClassMap::value_type(pScriptClass, pClass));
	return pScriptClass;
}

///////////////////////////////////////////////////////////////////
// Statics
///////////////////////////////////////////////////////////////////
 
CScriptAssembly *CScriptAssembly::TryGetAssembly(MonoImage *pImage)
{
	CRY_ASSERT(pImage);

	for each(auto assembly in static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->m_assemblies)
	{
		if(assembly->GetImage() == pImage)
			return assembly;
	}

	// TODO: Get assembly path
	return new CScriptAssembly(pImage, mono_image_get_filename(pImage));
}

CScriptClass *CScriptAssembly::TryGetClassFromRegistry(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	MonoImage *pImage = mono_class_get_image(pClass);

	if(auto pAssembly = TryGetAssembly(pImage))
		return pAssembly->TryGetClass(pClass);

	return NULL;
}