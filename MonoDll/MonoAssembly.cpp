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

CScriptAssembly::CScriptAssembly(MonoImage *pImage)
	: m_pImage(pImage)
{
	CRY_ASSERT(m_pImage);

	m_assemblies.push_back(this);
}

CScriptAssembly::CScriptAssembly(const char *path, bool push_back)
	:  m_path(path) 
{
	MonoAssembly *pMonoAssembly = mono_domain_assembly_open(mono_domain_get(), path);
	CRY_ASSERT(pMonoAssembly);

	m_pImage = mono_assembly_get_image(pMonoAssembly);
	CRY_ASSERT(m_pImage);
	
	if(push_back)
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

CScriptClass *CScriptAssembly::TryGetClass(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	for each(auto pair in m_classRegistry)
	{
		if(pair.second == pClass)
			return pair.first;
	}

	CScriptClass *pScriptClass = new CScriptClass(pClass);
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
	return new CScriptAssembly(pImage);
}

CScriptClass *CScriptAssembly::TryGetClassFromRegistry(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	MonoImage *pImage = mono_class_get_image(pClass);
	if(auto pAssembly = TryGetAssembly(pImage))
		return pAssembly->TryGetClass(pClass);

	return NULL;
}