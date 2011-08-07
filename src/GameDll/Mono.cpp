#include "StdAfx.h"
#include "Mono.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <mono/metadata/debug-helpers.h>

#include "PathUtils.h"
#include "Properties.h"

#include "MonoLogging.h"

#include "FGPSHandler.h"

CMono::CMono()
	: m_pMonoDomain(0)
	, m_pCryEngineAssembly(0)
	, m_bInitialized(false)
{
	// Set up directories
	mono_set_dirs(CPathUtils::GetLibPath(), CPathUtils::GetConfigPath());
}

CMono::~CMono()
{
	// Clean up mono
	if(m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	// Clean up bindings
	std::vector<IMonoAPIBinding*>::iterator it;
	for (it = m_apiBindings.begin(); it != m_apiBindings.end(); ++it)
	{
		delete *it;
	}
}

bool CMono::Init()
{
	bool result = true;

	m_pMonoDomain = mono_jit_init("cemono");
	if(!m_pMonoDomain)
	{
		GameWarning("Mono initialization failed!");
		return false;
	}

	mono_thread_attach(m_pMonoDomain);

	InitializeBindings();

	m_pCryEngineAssembly = LoadAssembly(CPathUtils::GetAssemblyPath() + "CryEngine.dll");

	m_pFGPluginManager = new CFGPluginManager();

	m_bInitialized = result;

	return result;
}

MonoAssembly *CMono::LoadAssembly(const char *fullPath)
{
	MonoAssembly *pAssembly = mono_domain_assembly_open(m_pMonoDomain, fullPath);
	if(!pAssembly)
		return false;

	return pAssembly;
}

bool CMono::InitializeBindings()
{
	AddBinding(new CMonoLogging());

	return true;
}

void CMono::AddBinding(IMonoAPIBinding* pBinding)
{
	m_apiBindings.push_back(pBinding);
}

MonoObject *CMono::InvokeFunc(string funcName, MonoClass *pClass, MonoObject *pInstance, void **args)
{
	MonoMethodDesc *pFooDesc = mono_method_desc_new (":" + funcName, false);

	MonoMethod* monoMethod = mono_method_desc_search_in_class(pFooDesc, pClass); 
    assert(monoMethod != NULL); //OK

	mono_method_desc_free (pFooDesc);

	MonoObject *pResult = mono_runtime_invoke(monoMethod, pInstance ? pInstance : NULL, args, NULL);

	return pResult ? pResult : NULL;
}

MonoObject *CMono::InvokeFunc(string funcName, string _className, string _nameSpace, MonoImage *pImage, MonoObject *pInstance, void **args)
{
	MonoClass *pClass;

	if(pImage)
		pClass = mono_class_from_name(pImage, _nameSpace, _className);
	else
		pClass = mono_class_from_name(GetNativeLibraryImage(), _nameSpace, _className);

	return InvokeFunc(funcName, pClass, pInstance, args);
}