#include "stdafx.h"
#include "Cemono.h"
#include "MonoClassUtils.h"

#include <CryExtension/Impl/ClassWeaver.h>
#include <mono/metadata/mono-debug.h>
#include <ICmdLine.h>



// Bindings
#include "ConsoleBinding.h"
#include "LoggingBinding.h"
#include "FlowSystemBinding.h"

CRYREGISTER_CLASS(CCemono)

CCemono::CCemono() : m_pMonoDomain(0), m_bDebugging(false)
{
}

CCemono::~CCemono()
{
	if (m_pMonoDomain)
	{
		mono_jit_cleanup(m_pMonoDomain);
	}

	std::vector<ICemonoClassBinding*>::iterator it;
	for (it = m_classBindings.begin(); it != m_classBindings.end(); ++it)
	{
		delete *it;
	}
}

bool CCemono::Init()
{
	CryLog("Cemono initialization");
	CryLog("    Initializing Cemono ...");
	bool result = true;

	mono_set_dirs(MonoPathUtils::GetLibPath(),MonoPathUtils::GetConfigPath());

	// Commandline switch -CEMONO_DEBUG makes the process connect to the debugging server
	const ICmdLineArg* arg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "CEMONO_DEBUG");
	if (arg != NULL)
	{
		m_bDebugging = true;
		char* options = "--debugger-agent=transport=dt_socket,address=127.0.0.1:65432";
		
		mono_jit_parse_options(1, &options);
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
	}
	if (!InitializeDomain())
	{
		return false;
	}

	RegisterDefaultBindings();
	
	if (!InitializeBaseClassLibraries())
		return false;

	if (!InitializeManager())
		return false;

	CryLog("    Initializing Cemono done, MemUsage=1337Kb" );


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

void CCemono::RegisterDefaultBindings()
{
	AddClassBinding(new CConsoleBinding());
	AddClassBinding(new CLoggingBinding());
	AddClassBinding(new CFlowSystemBinding());
}

bool CCemono::InitializeBaseClassLibraries()
{
	string bclPath = MonoPathUtils::GetCemonoLibPath() + "Cemono.Bcl.dll";
	m_pBclAssembly = mono_domain_assembly_open(m_pMonoDomain, bclPath);
	
	if (m_pBclAssembly == NULL)
	{
		//CryError("Failed to initialize base class libraries, assembly=NULL");
		CryFatalError("no base class libraries");
		return false;
	} else {
		m_pBclImage = mono_assembly_get_image(m_pBclAssembly);

		return m_pBclAssembly != NULL;
	}
	return true;
}

bool CCemono::InitializeManager()
{
	// Open assembly in domain
	m_pManagerAssembly = mono_domain_assembly_open(m_pMonoDomain, MonoPathUtils::GetCemonoLibPath() + "Cemono.Manager.dll");

	if (m_pManagerAssembly == NULL)
	{
		return false;
	}

	MonoImage* pMonoImage = mono_assembly_get_image(m_pManagerAssembly);
	if (pMonoImage == NULL)
	{
		CryFatalError("Failed to load mono manager image");
		return false;
	}

	MonoClass* pClass = mono_class_from_name(pMonoImage, "Cemono", "Manager");
	if (!pClass)
	{
		CryFatalError("Failed to find cemono Manager class");
		return false;
	}

	m_pManagerObject = MonoClassUtils::CreateInstanceOf(m_pMonoDomain, pClass);
	return true;
}


void CCemono::GetMemoryStatistics(ICrySizer * s) const
{
	s->Add(*this);
	
	//s->Add(m_classBindings);
	//s->Add(*m_pBclAssembly);
	//s->Add(*m_pBclImage);
	//s->Add(*m_pManagerAssembly);
	//s->Add(*m_pManagerObject);
	//s->Add(*m_pMonoDomain);

}
