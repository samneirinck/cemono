#include "StdAfx.h"
#include "Mono.h"
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-debug.h>

#include "LoggingBinding.h"
#include "ConsoleBinding.h"

#include <stdio.h>
#include <stdlib.h>

FILE* stream;
CMono::CMono()
	: m_pMonoDomain(0),	m_pManagerAssembly(0), m_pManagerObject(0), m_pBclAssembly(0)
{
	//freopen("c:\\temp\\myfile.txt", "a", stderr);

	setvbuf(stderr,NULL,_IONBF, 100);
	fprintf(stderr, "Hello");
	// Set up directories
	mono_set_dirs(CMonoPathUtils::GetAssemblyPath(), CMonoPathUtils::GetConfigurationPath());
	char* options = "--debugger-agent=transport=dt_socket,address=127.0.0.1:51740,loglevel=1,logfile=c:\\temp\\dbglog.txt,timeout=10000";
	//char* options = "--profile=log:calls";
	mono_jit_parse_options(1, &options);
}

CMono::~CMono()
{
	// Clean up mono
	if (m_pMonoDomain != NULL)
	{
		mono_jit_cleanup(m_pMonoDomain);
	}

	// Clean up bindings
	std::vector<IMonoAPIBinding*>::iterator it;
	for (it = m_apiBindings.begin(); it != m_apiBindings.end(); ++it)
	{
		delete *it;
	}

}



bool CMono::Init()
{
	bool initializationResult = true;

	if (!InitializeDomain())
		return false;

	// Register bindings
	InitializeBindings();

	if (!InitializeBaseClassLibraries())
		return false;

	if (!InitializeManager())
		return false;



	mono_thread_attach(m_pMonoDomain);

	return initializationResult;
}

bool CMono::InitializeDomain()
{
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);

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
	AddBinding(new CConsoleBinding());
	return true;
}

void CMono::AddBinding(IMonoAPIBinding* pBinding)
{
	m_apiBindings.push_back(pBinding);
}


bool CMono::InitializeBaseClassLibraries()
{
	string bclPath = CMonoPathUtils::GetCemonoAssemblyPath() + "Cemono.Bcl.dll";
	m_pBclAssembly = mono_domain_assembly_open(m_pMonoDomain, bclPath);
	
	if (m_pBclAssembly == NULL)
	{
		CryError("Failed to initialize base class libraries, assembly=NULL");
		return false;
	} else {
		m_pBclImage = mono_assembly_get_image(m_pBclAssembly);

		return m_pBclAssembly != NULL;
	}

}