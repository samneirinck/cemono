#include "stdafx.h"
#include "Cemono.h"
#include <CryExtension/Impl/ClassWeaver.h>


CRYREGISTER_CLASS(CCemono)

CCemono::CCemono() : m_pMonoDomain(0)
{
}

CCemono::~CCemono()
{
}

bool CCemono::Init()
{
	bool result = true;

	mono_set_dirs(MonoPathUtils::GetLibPath(),MonoPathUtils::GetConfigPath());

	if (!InitializeDomain())
	{
		return false;
	}
	
	return result;
}

void CCemono::Shutdown()
{

}

void CCemono::AddClassBinding(ICemonoClassBinding* pBinding)
{
	std::vector<ICemonoMethodBinding> methodBindings = pBinding->GetMethods();
	std::vector<ICemonoMethodBinding>::iterator it;
	
	// Get all methods registered in this class
	for (it = methodBindings.begin(); it != methodBindings.end(); ++it)
	{
		// Construct the full method name
		// Typically something like CryEngine.API.Console._LogAlways
		string fullName = pBinding->GetNamespace();
		if (strcmp(pBinding->GetNamespaceExtension(), ""))
		{
			fullName.append(".");
			fullName.append(pBinding->GetNamespaceExtension());
		}
		fullName.append(".");
		fullName.append(pBinding->GetClassName());
		fullName.append("::");
		fullName.append((*it).methodName);

		mono_add_internal_call(fullName, (*it).method);
	}
	

	m_classBindings.push_back(pBinding);
}

bool CCemono::InitializeDomain()
{
	// Create root domain
	m_pMonoDomain = mono_jit_init_version("Cemono Root", "v4.0.30319");
	if(!m_pMonoDomain)
	{
		GameWarning("Mono initialization failed!");
		return false;
	}

	return (m_pMonoDomain != NULL);
}