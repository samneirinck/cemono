#include "stdafx.h"
#include "MonoScriptSystem.h"

#include "MonoPathUtils.h"
#include "MonoAssembly.h"
#include "MonoCommon.h"
#include "MonoArray.h"
#include "MonoClass.h"
#include "MonoObject.h"

#include <mono/mini/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>

#include <ICmdLine.h>

#include "MonoConverter.h"

// Bindings
#include "Scriptbinds\ItemSystem.h"
#include "Scriptbinds\Console.h"
#include "Scriptbinds\ScriptBind_GameRules.h"
#include "Scriptbinds\ActorSystem.h"
#include "Scriptbinds\ScriptBind_3DEngine.h"
#include "Scriptbinds\ScriptBind_PhysicalWorld.h"
#include "Scriptbinds\ScriptBind_Renderer.h"
#include "Scriptbinds\ScriptBind_StaticEntity.h"

#include "EntityManager.h"
#include "FlowManager.h"
#include "MonoInput.h"
#include "MonoTester.h"

#include "CallbackHandler.h"

#ifndef _RELEASE
#include "MonoTester.h"
#endif

CRYREGISTER_CLASS(CMonoScriptSystem)

CMonoScriptSystem::CMonoScriptSystem() 
	: m_pMonoDomain(NULL)
	, m_pLibraryAssembly(NULL)
	, m_pCallbackHandler(NULL)
{
	CryLogAlways("Initializing Mono Script System");

	//system(CMonoPathUtils::GetMonoPath() + "bin\\pdb2mdb.exe" + " " + CMonoPathUtils::GetBinaryPath() + "CryBrary.dll");

	// We should look into CryPak for this (as well as c# scripts), in case it's possible to read them while they're pak'd.
	mono_set_dirs(CMonoPathUtils::GetLibPath(), CMonoPathUtils::GetConfigPath());

	string monoCmdOptions = "";

	// Commandline switch -DEBUG makes the process connect to the debugging server. Warning: Failure to connect to a  debugging server WILL result in a crash.
	const ICmdLineArg* arg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "DEBUG");
	if (arg != NULL)
		monoCmdOptions.append("--debugger-agent=transport=dt_socket,address=127.0.0.1:65432 ");

	char *options = new char[monoCmdOptions.size() + 1];
	strcpy(options, monoCmdOptions.c_str());
	mono_jit_parse_options(1, &options);

#ifndef _RELEASE
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

	m_pConverter = new CMonoConverter();

	gEnv->pMonoScriptSystem = this;

	if(gEnv->IsEditor())
		gEnv->pFileChangeMonitor->RegisterListener(this, "scripts\\");

	if(!Init())
		CryFatalError("Failed to initialize CryMono, aborting..");
}

CMonoScriptSystem::~CMonoScriptSystem()
{
	gEnv->pFileChangeMonitor->UnregisterListener(this);

	m_scripts.clear();
	m_methodBindings.clear();

	SAFE_DELETE(m_pConverter);
	SAFE_DELETE(m_pCallbackHandler);

	SAFE_DELETE(m_pCryConsole);
	SAFE_DELETE(m_pScriptCompiler);
	SAFE_DELETE(m_pLibraryAssembly);

	m_localScriptBinds.clear();

	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	gEnv->pMonoScriptSystem = NULL;
}

bool CMonoScriptSystem::Init()
{
	CryLogAlways("    Initializing CryMono...");
	
	if (!InitializeDomain())
		return false;

	m_pLibraryAssembly = LoadAssembly(CMonoPathUtils::GetBinaryPath() + "CryBrary.dll");
	if (!m_pLibraryAssembly)
		return false;

	m_pCryConsole = m_pLibraryAssembly->InstantiateClass("CryEngine", "Console");

	//We don't instantiate this directly because we need the correct AppDomain for the compiler in C#
	m_pScriptCompiler = m_pLibraryAssembly->GetCustomClass("ScriptCompiler")->CallMethod("SetupCompiler", NULL, true)->Unbox<IMonoClass *>();

	CryLogAlways("    Registering default scriptbinds...");
	RegisterDefaultBindings();

	CryLogAlways("    Initializing subsystems...");
	InitializeSystems();

	CryLogAlways("    Compiling scripts...");
	m_pScriptCompiler->CallMethod("Initialize");

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);
	CryLogAlways("    Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + m_pLibraryAssembly->GetCustomClass("CryStats", "CryEngine.Utils")->GetProperty("MemoryUsage")->Unbox<long>()) / 1024);

	return true;
}

void CMonoScriptSystem::PostInit()
{
	m_pScriptCompiler->CallMethod("PostInit");
}

bool CMonoScriptSystem::InitializeDomain()
{
	// Create root domain
	m_pMonoDomain = mono_jit_init_version("CryMono", "v4.0.30319");
	if(!m_pMonoDomain)
	{
		GameWarning("Mono initialization failed!");
		return false;
	}

	return m_pMonoDomain != NULL;
}

void CMonoScriptSystem::RegisterDefaultBindings()
{
	// Register what couldn't be registered earlier.
	if(m_methodBindings.size()>0)
	{
		for(TMethodBindings::iterator it = m_methodBindings.begin(); it != m_methodBindings.end(); ++it)
			RegisterMethodBinding((*it).second, (*it).first);
	}

#define RegisterBinding(T) m_localScriptBinds.push_back((IMonoScriptBind *)new T());
	RegisterBinding(CScriptBind_ActorSystem);
	RegisterBinding(CScriptBind_3DEngine);
	RegisterBinding(CScriptBind_PhysicalWorld);
	RegisterBinding(CScriptBind_Renderer);
	RegisterBinding(CScriptBind_Console);
	RegisterBinding(CScriptBind_ItemSystem);
	RegisterBinding(CScriptBind_Inventory);
	RegisterBinding(CScriptBind_GameRules);
	RegisterBinding(CScriptBind_StaticEntity);
	RegisterBinding(CMonoInput);
	RegisterBinding(CMonoTester);

#define RegisterBindingAndSet(var, T) RegisterBinding(T); var = (T *)m_localScriptBinds.back();
	RegisterBindingAndSet(m_pCallbackHandler, CMonoCallbackHandler);
	RegisterBindingAndSet(m_pEntityManager, CEntityManager);
	RegisterBindingAndSet(m_pFlowManager, CFlowManager);

#ifndef _RELEASE
	RegisterBindingAndSet(m_pTester, CMonoTester);
#endif

#undef RegisterBinding
}

bool CMonoScriptSystem::InitializeSystems()
{
#ifndef _RELEASE
	m_pTester->CommenceTesting();
	SAFE_DELETE(m_pTester);
#endif

	IMonoClass *pClass = m_pLibraryAssembly->GetCustomClass("CryNetwork");
	IMonoArray *pArray = GetConverter()->CreateArray(2);
	pArray->Insert(gEnv->IsEditor());
	pArray->Insert(gEnv->IsDedicated());
	pClass->CallMethod("InitializeNetworkStatics", pArray, true);
	SAFE_DELETE(pClass);
	SAFE_RELEASE(pArray);

	return true;
}

void CMonoScriptSystem::Update(float frameTime)
{
	//m_pCallbackHandler->InvokeCallback("Update", "Manager");

	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(frameTime);
	m_pScriptCompiler->CallMethod("OnUpdate", pArgs);
	SAFE_RELEASE(pArgs);
}

void CMonoScriptSystem::OnFileChange(const char *sFilename)
{
	IMonoArray *pParams = CreateMonoArray(1);
	pParams->Insert(sFilename);

	m_pScriptCompiler->CallMethod("OnFileChange", pParams);
	SAFE_RELEASE(pParams);
}

void CMonoScriptSystem::RegisterMethodBinding(IMonoMethodBinding binding, const char *classPath)
{
	mono_add_internal_call(classPath + (string)binding.methodName, binding.method);
}

void CMonoScriptSystem::RegisterMethodBindings(std::vector<IMonoMethodBinding> newBindings, const char *classPath)
{
	if(newBindings.size()<=0)
		return;

	for each(auto newBinding in newBindings)
	{
		for each(auto storedBinding in m_methodBindings)
		{
			// Check if it already exists
			if(storedBinding.second.methodName==newBinding.methodName)
				return;
			
			if(!IsInitialized())
				m_methodBindings.insert(TMethodBindings::value_type(classPath, newBinding));
		}
	}

	if(!IsInitialized()) // MonoDomain not initialized yet, we'll register them later.
		return;

	for each(auto binding in newBindings)
		RegisterMethodBinding(binding, classPath);
}

int CMonoScriptSystem::InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters)
{
	if(scriptType==EMonoScriptType_GameRules)
	{
		IMonoClass *pClass = m_pLibraryAssembly->GetCustomClass("CryNetwork");
		IMonoArray *pArray = GetConverter()->CreateArray(3);
		pArray->Insert(gEnv->bMultiplayer);
		pArray->Insert(gEnv->IsClient());
		pArray->Insert(gEnv->bServer);
		pClass->CallMethod("InitializeNetwork", pArray, true);
		SAFE_RELEASE(pArray);
		SAFE_DELETE(pClass);
	}
	
	/*for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it)->GetScriptType()==scriptType && !strcmp((*it)->GetName(), scriptName))
		{
			if(scriptType==EMonoScriptType_GameRules)
				m_scripts.erase(it);

			break;
		}
	}*/

	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(scriptName);
	pArgs->Insert(pConstructorParameters);

	if(IMonoObject *pScriptInstance = m_pScriptCompiler->CallMethod("InstantiateScript", pArgs))
		m_scripts.push_back(pScriptInstance->Unbox<IMonoClass *>());
		
	SAFE_RELEASE(pArgs);
	
	return m_scripts.back()->GetScriptId();
}

void CMonoScriptSystem::RemoveScriptInstance(int id)
{
	if(id==-1)
		return;

	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it)->GetScriptId()==id)
		{
			IMonoArray *pArgs = CreateMonoArray(2);
			pArgs->Insert(id);
			pArgs->Insert((*it)->GetName());

			m_pScriptCompiler->CallMethod("RemoveInstance", pArgs);
			SAFE_RELEASE(pArgs);

			SAFE_DELETE((*it));
			m_scripts.erase(it);

			break;
		}
	}
}

IMonoClass *CMonoScriptSystem::GetScriptById(int id)
{
	if(id==-1)
		return NULL;

	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it)->GetScriptId()==id)
			return (*it);
	}

	return NULL;
}

IMonoAssembly *CMonoScriptSystem::LoadAssembly(const char *assemblyPath)
{
	return new CMonoAssembly(assemblyPath);
}