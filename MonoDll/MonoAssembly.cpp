#include "StdAfx.h"
#include "MonoAssembly.h"

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

#include "PathUtils.h"
#include "MonoScriptSystem.h"

#include <MonoClass.h>

std::vector<CScriptAssembly *> CScriptAssembly::m_assemblies = std::vector<CScriptAssembly *>();

CScriptAssembly::CScriptAssembly(MonoImage *pImage, const char *path)
	: m_pImage(pImage)
	, m_path(path) 
{
	CRY_ASSERT(pImage);

	m_assemblies.push_back(this);
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

IMonoClass *CScriptAssembly::TryGetClass(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	for each(auto pair in m_classRegistry)
	{
		if(pair.second == pClass)
			return pair.first;
	}

	IMonoClass *pScriptClass = new CScriptClass(pClass);
	m_classRegistry.insert(TClassMap::value_type(pScriptClass, pClass));
	return pScriptClass;
}

///////////////////////////////////////////////////////////////////
// Statics
///////////////////////////////////////////////////////////////////
 
CScriptAssembly *CScriptAssembly::TryGetAssembly(MonoImage *pImage)
{
	CRY_ASSERT(pImage);

	for each(auto assembly in m_assemblies)
	{
		if(assembly->GetImage() == pImage)
			return assembly;
	}

	// TODO: Get assembly path
	return new CScriptAssembly(pImage, "");
}

IMonoClass *CScriptAssembly::TryGetClassFromRegistry(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	return TryGetAssembly(mono_class_get_image(pClass))->TryGetClass(pClass);
}