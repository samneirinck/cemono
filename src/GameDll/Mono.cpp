#include "StdAfx.h"
#include "Mono.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "MonoPathUtils.h"

#include "MonoProperties.h"

CMono::CMono()
	: m_pMonoDomain(0)
{
	// Set up directories


	mono_set_dirs(CMonoPathUtils::GetLibPath(), CMonoPathUtils::GetConfigPath());
}

CMono::~CMono()
{
	// Clean up mono
	if(m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);
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

	MonoAssembly *assembly;
	assembly = mono_domain_assembly_open(m_pMonoDomain, CMonoPathUtils::GetAssemblyPath() + "CryEngine.dll");
	if(!assembly)
	{
		GameWarning("Failed to open mono assembly");
		return false;
	}
	else
		mono_jit_exec(m_pMonoDomain, assembly, 0, NULL);

	mono_thread_attach(m_pMonoDomain);

	return result;
}