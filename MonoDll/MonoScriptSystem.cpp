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
#include <mono/metadata/mono-gc.h>

#include <ICmdLine.h>
#include <ISystem.h>

#include "ScriptManager.h"

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

CRYREGISTER_CLASS(CScriptSystem)

CScriptSystem::CScriptSystem() 
	: m_pMonoDomain(NULL)
	, m_pCryBraryAssembly(NULL)
	, m_pCallbackHandler(NULL)
	, m_pPdb2MdbAssembly(NULL)
	, m_pScriptManager(NULL)
{
	CryLogAlways("Initializing Mono Script System");

	// We should look into CryPak for this (as well as c# scripts), in case it's possible to read them while they're pak'd.
	mono_set_dirs(PathUtils::GetLibPath(), PathUtils::GetConfigPath());

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

	m_pConverter = new CConverter();

	gEnv->pMonoScriptSystem = this;

	if(gEnv->IsEditor())
		gEnv->pFileChangeMonitor->RegisterListener(this, "scripts\\");

	if(!CompleteInit())
		CryFatalError("Failed to initialize CryMono, aborting..");
}

CScriptSystem::~CScriptSystem()
{
	mono_gc_collect(mono_gc_max_generation());

	gEnv->pFileChangeMonitor->UnregisterListener(this);

	m_methodBindings.clear();

	SAFE_DELETE(m_pConverter);
	SAFE_DELETE(m_pCallbackHandler);

	SAFE_RELEASE(m_pCryBraryAssembly);

	m_localScriptBinds.clear();

	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	gEnv->pMonoScriptSystem = NULL;
}

bool CScriptSystem::CompleteInit()
{
	CryLogAlways("    Initializing CryMono...");
	
	if (!InitializeDomain())
		return false;

#ifndef _RELEASE
	m_pPdb2MdbAssembly = new CScriptAssembly(PathUtils::GetMonoPath() + "bin\\pdb2mdb.dll");
#endif

	REGISTER_FACTORY(gEnv->pGameFramework, "ScriptManager", CScriptManager, false);

	if(!Reload(true))
		return false;

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);
	CryLogAlways("    Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + m_pCryBraryAssembly->GetCustomClass("CryStats", "CryEngine.Utils")->GetProperty("MemoryUsage")->Unbox<long>()) / 1024);

	return true;
}

bool CScriptSystem::Reload(bool initialLoad)
{
	// Store the state of current instances to apply them to the reloaded scripts at the end.
	if(!initialLoad)
	{
		CryLogAlways("C# modifications detected on disk, initializing CryBrary reload");

		m_pScriptManager->Release();
	}

	SEntitySpawnParams params;
	params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Default");
	params.nFlags = ENTITY_FLAG_SERVER_ONLY;
	params.id = 1337;

	if(auto pEntity = gEnv->pEntitySystem->SpawnEntity(params))
	{
		IGameObject *pGameObject = gEnv->pGameFramework->GetIGameObjectSystem()->CreateGameObjectForEntity(pEntity->GetId());

		m_pScriptManager = static_cast<IMonoScriptManager *>(pGameObject->AcquireExtension("ScriptManager"));
	}

	m_pCryBraryAssembly = LoadAssembly(PathUtils::GetBinaryPath() + "CryBrary.dll");
	if (!m_pCryBraryAssembly)
		return false;

	CryLogAlways("    Registering default scriptbinds...");
	RegisterDefaultBindings();

	CryLogAlways("    Initializing subsystems...");
	InitializeSystems();

	static_cast<CScriptManager *>(m_pScriptManager)->CompileScripts();

	// Nodes won't get recompiled if we forget this.
	if(!initialLoad)
		GetFlowManager()->Reset();

	return true;
}

bool CScriptSystem::InitializeDomain()
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

void CScriptSystem::RegisterDefaultBindings()
{
	// Register what couldn't be registered earlier.
	if(m_methodBindings.size()>0)
	{
		for(TMethodBindings::iterator it = m_methodBindings.begin(); it != m_methodBindings.end(); ++it)
			RegisterMethodBinding((*it).first, (*it).second);
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
	RegisterBinding(CInput);
	RegisterBinding(CTester);

#define RegisterBindingAndSet(var, T) RegisterBinding(T); var = (T *)m_localScriptBinds.back();
	RegisterBindingAndSet(m_pCallbackHandler, CCallbackHandler);
	RegisterBindingAndSet(m_pEntityManager, CEntityManager);
	RegisterBindingAndSet(m_pFlowManager, CFlowManager);

#ifndef _RELEASE
	RegisterBindingAndSet(m_pTester, CTester);
#endif

#undef RegisterBindingAndSet
#undef RegisterBinding
}

bool CScriptSystem::InitializeSystems()
{
#ifndef _RELEASE
	//m_pTester->CommenceTesting();
	//SAFE_DELETE(m_pTester);
#endif

	IMonoClass *pClass = m_pCryBraryAssembly->GetCustomClass("CryNetwork");
	IMonoArray *pArray = GetConverter()->CreateArray(2);
	pArray->Insert(gEnv->IsEditor());
	pArray->Insert(gEnv->IsDedicated());
	pClass->CallMethod("InitializeNetworkStatics", pArray, true);
	SAFE_RELEASE(pClass);
	SAFE_RELEASE(pArray);

	return true;
}

void CScriptSystem::OnFileChange(const char *sFilename)
{
	string fileName = sFilename;
	const char *fileExt = fileName.substr(fileName.find_last_of(".") + 1);

	if(!strcmp(fileExt, "cs") || !strcmp(fileExt, "dll"))
		Reload();
}

void CScriptSystem::RegisterMethodBinding(const void *method, const char *fullMethodName)
{
	if(!IsInitialized())
		m_methodBindings.insert(TMethodBindings::value_type(method, fullMethodName));
	else
		mono_add_internal_call(fullMethodName, method);
}

IMonoAssembly *CScriptSystem::LoadAssembly(const char *assemblyPath)
{
	return new CScriptAssembly(assemblyPath);
}