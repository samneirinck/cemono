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