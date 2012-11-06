#include "StdAfx.h"
#include "MonoAssembly.h"

#include "MonoScriptSystem.h"
#include "MonoDomain.h"
#include "PathUtils.h"

#include <MonoClass.h>

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>

#include <Windows.h>

CScriptAssembly::CScriptAssembly(CScriptDomain *pDomain, MonoImage *pImage, const char *path, bool nativeAssembly)
	: m_pDomain(pDomain)
	, m_bNative(nativeAssembly) // true if this assembly was loaded via C++.
{
	CRY_ASSERT(pImage);
	m_pObject = (MonoObject *)pImage;

	m_path = string(path);
}

CScriptAssembly::~CScriptAssembly()
{
	for each(auto classPair in m_classRegistry)
		classPair.first->Release();

	m_pDomain->OnAssemblyReleased(this);

	m_pObject = 0;
}

void CScriptAssembly::Release(bool triggerGC)
{
	if(m_classRegistry.empty())
		delete this;
}

void CScriptAssembly::OnClassReleased(CScriptClass *pClass)
{
	m_classRegistry.erase(pClass);
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
			pair.first->AddRef();
			return pair.first;
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
CScriptClass *CScriptAssembly::TryGetClassFromRegistry(MonoClass *pClass)
{
	CRY_ASSERT(pClass);

	MonoImage *pImage = mono_class_get_image(pClass);

	MonoDomain *pMonoDomain = mono_object_get_domain((MonoObject *)pClass);
	
	if(CScriptDomain *pDomain = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->TryGetDomain(pMonoDomain))
	{
		if(auto pAssembly = pDomain->TryGetAssembly(pImage))
			return pAssembly->TryGetClass(pClass);
	}

	return NULL;
}