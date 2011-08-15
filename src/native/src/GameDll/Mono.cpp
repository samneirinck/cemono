#include "StdAfx.h"
#include "Mono.h"
#include <mono\metadata\debug-helpers.h>

#include "LoggingBinding.h"
#include "ConsoleBinding.h"
#include "EntitySystemBinding.h"
#include "FGPluginManager.h"

CMono::CMono()
	: m_pMonoDomain(0),	m_pManagerAssembly(0), m_pManagerObject(0), m_pBclAssembly(0)
{
	// Set up directories

	mono_set_dirs(CMonoPathUtils::GetLibPath(), CMonoPathUtils::GetConfigPath());
}

CMono::~CMono()
{
	// Clean up mono
	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	// Clean up bindings
	std::vector<MonoAPIBinding*>::iterator it;
	for (it = m_apiBindings.begin(); it != m_apiBindings.end(); ++it)
	{
		delete *it;
	}
}

bool CMono::Init()
{
	bool initResult = true;

	if (!InitializeDomain())
		return false;

	// Register bindings
	InitializeBindings();

	if (!InitializeBaseClassLibraries())
		return false;

	if (!InitializeManager())
		return false;

	mono_thread_attach(m_pMonoDomain);

	m_pFGPluginManager = new CFGPluginManager();

	return initResult;
}

bool CMono::InitializeDomain()
{
	// Create root domain
	m_pMonoDomain = mono_jit_init("cemono");
	if(!m_pMonoDomain)
	{
		GameWarning("Mono initialization failed!");
		return false;
	}

	return (m_pMonoDomain != NULL);
}

bool CMono::InitializeManager()
{
	// Open assembly in domain
	m_pManagerAssembly = LoadAssembly(CMonoPathUtils::GetAssemblyPath() + "Cemono.Manager.dll");

	if (!m_pManagerAssembly)
		return false;

	MonoImage* pMonoImage = mono_assembly_get_image(m_pManagerAssembly);
	if (!pMonoImage)
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
	AddBinding(new CConsoleBinding());
	AddBinding(new CEntitySystemBinding());
	return true;
}

void CMono::AddBinding(MonoAPIBinding* pBinding)
{
	m_apiBindings.push_back(pBinding);
}


bool CMono::InitializeBaseClassLibraries()
{
	m_pBclAssembly = LoadAssembly(CMonoPathUtils::GetAssemblyPath() + "Cemono.Bcl.dll");
	
	if (!m_pBclAssembly)
	{
		CryError("Failed to initialize base class libraries, assembly=NULL");
		return false;
	} 
	else
	{
		m_pBclImage = mono_assembly_get_image(m_pBclAssembly);

		return m_pBclImage != NULL;
	}
}