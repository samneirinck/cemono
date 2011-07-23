#include "StdAfx.h"
#include "Mono.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include "PathUtils.h"
#include "Properties.h"

#include "MonoLogging.h"
#include "MonoCVars.h"

CMono::CMono()
	: m_pMonoDomain(0), m_pCryEngineAssembly(0)
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

	m_pMonoDomain = mono_jit_init("CryMono");
	if(!m_pMonoDomain)
	{
		GameWarning("Mono initialization failed!");
		return false;
	}

	mono_thread_attach(m_pMonoDomain);

	InitializeBindings();

	m_pCryEngineAssembly = LoadAssembly(CPathUtils::GetAssemblyPath() + "CryEngine.dll");

	MonoImage *pImage = mono_assembly_get_image(m_pCryEngineAssembly);
	InvokeFunc("Init", mono_class_from_name(pImage, "CryEngine", "Manager"));

	return result;
}

MonoAssembly *CMono::LoadAssembly(const char *fullPath)
{
	MonoAssembly *pAssembly = mono_domain_assembly_open(m_pMonoDomain, fullPath);
	if(!pAssembly)
		return false;
	/*else
		mono_jit_exec(m_pMonoDomain, pAssembly, NULL, NULL);
		*/

	return pAssembly;
}

MonoObject *CMono::InvokeFunc(string funcName, MonoClass *pClass, bool useInstance, void **args)
{
	MonoMethodDesc *pFooDesc = mono_method_desc_new (":" + funcName, false);
	MonoMethod* monoMethod = mono_method_desc_search_in_class(pFooDesc, pClass); 
    assert(monoMethod != NULL); //OK

	mono_method_desc_free (pFooDesc);

	MonoObject *pResult;
	if(useInstance)
	{
		 /* allocate memory for the object */
		MonoObject *pClassInstance = mono_object_new (m_pMonoDomain, pClass);
		/* execute the default argument-less constructor */
		mono_runtime_object_init (pClassInstance);

		pResult = mono_runtime_invoke(monoMethod, pClassInstance, args ? args : NULL, NULL);
	}
	else
		pResult = mono_runtime_invoke(monoMethod, NULL, args ? args : NULL, NULL);

	return pResult ? pResult : NULL;
}

bool CMono::InitializeBindings()
{
	AddBinding(new CMonoLogging());
	AddBinding(new CMonoCVars());

	return true;
}

void CMono::AddBinding(IMonoAPIBinding* pBinding)
{
	m_apiBindings.push_back(pBinding);
}