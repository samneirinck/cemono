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
	for each(auto pClass in m_classes)
		delete pClass;

	m_pDomain->OnAssemblyReleased(this);

	m_pObject = 0;
}

void CScriptAssembly::Release(bool triggerGC)
{
	if(m_classes.empty())
		delete this;
}

void CScriptAssembly::OnClassReleased(CScriptClass *pClass)
{
	stl::find_and_erase(m_classes, pClass);
}

IMonoClass *CScriptAssembly::GetClass(const char *className, const char *nameSpace)
{ 
	if(MonoClass *monoClass = mono_class_from_name((MonoImage *)m_pObject, nameSpace, className))
		return TryGetClass(monoClass);

	MonoWarning("Failed to get class %s.%s", nameSpace, className);
	return nullptr;
}

CScriptClass *CScriptAssembly::TryGetClass(MonoClass *pMonoClass)
{
	CRY_ASSERT(pMonoClass);

	for each(auto pClass in m_classes)
	{
		if((MonoClass *)pClass->GetManagedObject() == pMonoClass)
		{
			pClass->AddRef();
			return pClass;
		}
	}

	CScriptClass *pScriptClass = new CScriptClass(pMonoClass, this);
	m_classes.push_back(pScriptClass);
	pScriptClass->AddRef();

	return pScriptClass;
}