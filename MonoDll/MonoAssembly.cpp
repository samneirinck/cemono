#include "StdAfx.h"
#include "MonoAssembly.h"

#include "MonoScriptSystem.h"
#include "MonoDomain.h"
#include "MonoException.h"

#include "PathUtils.h"

#include <MonoClass.h>

#include <mono/mini/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/exception.h>

CScriptAssembly::CScriptAssembly(CScriptDomain *pDomain, MonoImage *pImage, const char *path, bool nativeAssembly)
	: m_pDomain(pDomain)
	, m_bNative(nativeAssembly) // true if this assembly was loaded via C++.
	, m_bDestroying(false)
{
	CRY_ASSERT(pImage);
	m_pObject = (MonoObject *)pImage;
	m_pClass = NULL;

	m_path = string(path);
}

CScriptAssembly::~CScriptAssembly()
{
	m_bDestroying = true;

	for(auto it = m_classes.begin(); it != m_classes.end(); ++it)
		delete *it;

	m_classes.clear();

	m_pObject = 0;
}

void CScriptAssembly::Release(bool triggerGC)
{
	if(m_classes.empty())
	{
		m_pDomain->OnAssemblyReleased(this);
		// delete assembly should only be directly done by this method and the CScriptDomain dtor, otherwise Release.
		delete this;
	}
}

void CScriptAssembly::OnClassReleased(CScriptClass *pClass)
{
	if(!m_bDestroying)
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

IMonoException *CScriptAssembly::_GetException(const char *nameSpace, const char *exceptionClass, const char *message)
{
	MonoException *pException;
	if(message != nullptr)
		pException = mono_exception_from_name_msg((MonoImage *)m_pObject, nameSpace, exceptionClass, message);
	else
		pException = mono_exception_from_name((MonoImage *)m_pObject, nameSpace, exceptionClass);

	return new CScriptException(pException);
}