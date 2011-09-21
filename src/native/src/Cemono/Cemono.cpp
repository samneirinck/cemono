#include "stdafx.h"
#include "Cemono.h"
#include "MonoClassUtils.h"

#include <CryExtension/Impl/ClassWeaver.h>
#include <mono/metadata/mono-debug.h>
#include <ICmdLine.h>

using namespace std;

// Bindings
#include "ConsoleBinding.h"
#include "LoggingBinding.h"
#include "FlowSystemBinding.h"
#include "EntitySystemBinding.h"

CRYREGISTER_CLASS(CCemono)

CCemono::CCemono() : m_pMonoDomain(0), m_bDebugging(false), m_pBclAssembly(0), m_pManagerAssembly(0)
{
}

CCemono::~CCemono()
{
	if (m_pMonoDomain)
	{
		mono_jit_cleanup(m_pMonoDomain);
	}

	SAFE_DELETE(m_pManagerAssembly);
	SAFE_DELETE(m_pBclAssembly);
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

void CCemono::AddClassBinding(std::shared_ptr<ICemonoClassBinding> pBinding)
{
	vector<ICemonoMethodBinding> methodBindings = pBinding->GetMethods();
	
	// Get all methods registered in this class
	for_each( begin(methodBindings), end(methodBindings), [&]( ICemonoMethodBinding& binding) {
		// Construct the full method name
		// Typically something like CryEngine.API.Console._LogAlways
		ce::string fullName = pBinding->GetNamespace();
		if (strcmp(pBinding->GetNamespaceExtension(), ""))
		{
			fullName.append(".");
			fullName.append(pBinding->GetNamespaceExtension());
		}
		fullName.append(".");
		fullName.append(pBinding->GetClassName());
		fullName.append("::");
		fullName.append(binding.methodName);

		mono_add_internal_call(fullName, binding.method);
	});

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
	
	AddClassBinding(std::make_shared<CConsoleBinding>());
	AddClassBinding(std::make_shared<CLoggingBinding>());
	AddClassBinding(std::make_shared<CFlowSystemBinding>());
	AddClassBinding(std::make_shared<CEntitySystemBinding>());
}

bool CCemono::InitializeBaseClassLibraries()
{
	ce::string bclPath = MonoPathUtils::GetCemonoLibPath() + "Cemono.Bcl.dll";
	m_pBclAssembly = new CCemonoAssembly(m_pMonoDomain, bclPath);
	return m_pBclAssembly != NULL;
}

bool CCemono::InitializeManager()
{
	ce::string managerPath = MonoPathUtils::GetCemonoLibPath() + "Cemono.Manager.dll";
	m_pManagerAssembly = new CCemonoAssembly(m_pMonoDomain, managerPath);
	if (m_pManagerAssembly == NULL)
	{
		return false;
	}

	m_pManagerObject = m_pManagerAssembly->CreateInstanceOf("Cemono", "Manager");
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

void CCemono::GetMemoryUsage(ICrySizer* pSizer) const
{
	
}