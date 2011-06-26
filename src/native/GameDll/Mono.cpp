#include "StdAfx.h"
#include "Mono.h"

#include "LoggingBinding.h"


CMono::CMono()
	: m_pMonoDomain(0),	m_pManagerAssembly(0), m_pManagerObject(0)
{
	// Set up directories
	mono_set_dirs(CMonoPathUtils::GetAssemblyPath(), CMonoPathUtils::GetConfigurationPath());
}

CMono::~CMono()
{
	if (m_pMonoDomain != NULL)
	{
		mono_jit_cleanup(m_pMonoDomain);
	}
}



bool CMono::Init()
{
	bool initializationResult = true;

	if (!InitializeDomain())
		return false;



	// Register bindings
	InitializeBindings();

	if (!InitializeManager())
		return false;



	mono_thread_attach(m_pMonoDomain);

	return initializationResult;
}

bool CMono::InitializeDomain()
{
	// Create root domain
	m_pMonoDomain = mono_jit_init_version("Cemono Root", "v4.0.30319");

	return (m_pMonoDomain != NULL);
}

bool CMono::InitializeManager()
{
	// Open assembly in domain
	m_pManagerAssembly = mono_domain_assembly_open(m_pMonoDomain, CMonoPathUtils::GetCemonoAssemblyPath() + "Cemono.Manager.dll");

	if (m_pManagerAssembly == NULL)
	{
		return false;
	}

	MonoImage* pMonoImage = mono_assembly_get_image(m_pManagerAssembly);
	if (pMonoImage == NULL)
	{
		CryError("Failed to load mono manager image");
		return false;
	}

	MonoClass* pClass = mono_class_from_name(pMonoImage, "Cemono", "Manager");
	if (!pClass)
	{
		CryError("Failed to find cemono Manager class");
		return false;
	}

	m_pManagerObject = CMonoClassUtils::CreateInstanceOf(m_pMonoDomain, pClass);

	return true;
}

bool CMono::InitializeBindings()
{
	AddBinding(new CLoggingBinding());
	return true;
}

void CMono::AddBinding(IMonoAPIBinding* pBinding)
{
	m_apiBindings.push_back(pBinding);
}
