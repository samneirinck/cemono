#include "stdafx.h"
#include "MonoScriptSystem.h"

#include "PathUtils.h"
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
#include "Scriptbinds\GameRules.h"
#include "Scriptbinds\ActorSystem.h"
#include "Scriptbinds\3DEngine.h"
#include "Scriptbinds\PhysicalWorld.h"
#include "Scriptbinds\Renderer.h"
#include "Scriptbinds\StaticEntity.h"
#include "Scriptbinds\Debug.h"
#include "Scriptbinds\Scriptbind_UI.h"
#include "Scriptbinds\MaterialManager.h"
#include "Scriptbinds\ParticleSystem.h"
#include "Scriptbinds\ViewSystem.h"

#include "EntityManager.h"
#include "FlowManager.h"
#include "MonoInput.h"

#include "CallbackHandler.h"

#include "MonoCVars.h"

SCVars *g_pMonoCVars = 0;

CRYREGISTER_CLASS(CScriptSystem)

CScriptSystem::CScriptSystem() 
	: m_pMonoDomain(NULL)
	, m_pCryBraryAssembly(NULL)
	, m_pCallbackHandler(NULL)
	, m_pPdb2MdbAssembly(NULL)
	, m_pScriptManager(NULL)
	, m_AppDomainSerializer(NULL)
	, m_pInput(NULL)
	, m_bLastCompilationSuccess(true)
	, m_pUIScriptBind(NULL)
{
	CryLogAlways("Initializing Mono Script System");

	// We should look into storing mono binaries, configuration as well as scripts via CryPak.
	mono_set_dirs(PathUtils::GetLibPath(), PathUtils::GetConfigPath());

	string monoCmdOptions = "";

	// Commandline switch -DEBUG makes the process connect to the debugging server. Warning: Failure to connect to a debugging server WILL result in a crash.
	// This is currently a WIP feature which requires custom MonoDevelop extensions and other irritating things.
	const ICmdLineArg* arg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "DEBUG");
	if (arg != NULL)
		monoCmdOptions.append("--debugger-agent=transport=dt_socket,address=127.0.0.1:65432,embedding=1");

	char *options = new char[monoCmdOptions.size() + 1];
	strcpy(options, monoCmdOptions.c_str());

	// Note: iPhone requires AOT compilation, this can be enforced via mono options. TODO: Get Crytek to add CryMobile support to the Free SDK.
	mono_jit_parse_options(1, &options);

#ifndef _RELEASE
	// Required for mdb's to load for detailed stack traces etc.
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

	m_pConverter = new CConverter();

	gEnv->pMonoScriptSystem = this;

	m_pCVars = new SCVars();
	g_pMonoCVars = m_pCVars;

	if(!CompleteInit())
		CryFatalError("Failed to initialize CryMono, aborting..");

	if(IFileChangeMonitor *pFileChangeMonitor = gEnv->pFileChangeMonitor)
		pFileChangeMonitor->RegisterListener(this, "scripts\\");
}

CScriptSystem::~CScriptSystem()
{
	// Force garbage collection of all generations.
	mono_gc_collect(mono_gc_max_generation());

	if(IFileChangeMonitor *pFileChangeMonitor = gEnv->pFileChangeMonitor)
		pFileChangeMonitor->UnregisterListener(this);

	m_methodBindings.clear();

	SAFE_DELETE(m_pConverter);
	SAFE_DELETE(m_pCallbackHandler);
	SAFE_DELETE(m_pUIScriptBind);

	SAFE_RELEASE(m_pCryBraryAssembly);
	SAFE_DELETE(m_pCVars);

	m_localScriptBinds.clear();

	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	gEnv->pMonoScriptSystem = NULL;
}

#define REGISTER_GAME_OBJECT_EXTENSION(framework, name)\
	{\
		struct C##name##Creator : public IGameObjectExtensionCreatorBase\
		{\
		C##name *Create()\
			{\
			return new C##name();\
			}\
			void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
			{\
			C##name::GetGameObjectExtensionRMIData( ppRMI, nCount );\
			}\
		};\
		static C##name##Creator _creator;\
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, NULL);\
	}

bool CScriptSystem::CompleteInit()
{
	CryLogAlways("		Initializing CryMono...");
	
	if (!InitializeDomain())
		return false;

#ifndef _RELEASE
	m_pPdb2MdbAssembly = new CScriptAssembly(PathUtils::GetMonoPath() + "bin\\pdb2mdb.dll");
#endif
	
	// WIP ScriptManager game object, to be used for CryMono RMI support etc in the future.
	REGISTER_GAME_OBJECT_EXTENSION(gEnv->pGameFramework, ScriptManager);

	CryLogAlways("		Registering default scriptbinds...");
	RegisterDefaultBindings();

	if(!Reload(true))
		return false;

	gEnv->pGameFramework->RegisterListener(this, "CryMono", eFLPriority_Game);

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);
	CryLogAlways("		Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + m_pCryBraryAssembly->GetCustomClass("CryStats", "CryEngine.Utilities")->GetProperty("MemoryUsage")->Unbox<long>()) / 1024);

	return true;
}

void CScriptSystem::PostInit()
{
	//GetFlowManager()->Reset();

	m_pScriptManager->CallMethod("PostInit");
}

bool CScriptSystem::Reload(bool initialLoad)
{
	MonoDomain *pPrevScriptDomain = NULL;
	IMonoAssembly *pPrevCryBraryAssembly = m_pCryBraryAssembly;

	// Store the state of current instances to apply them to the reloaded scripts at the end.
	if(!initialLoad)
	{
		CryLogAlways("C# modifications detected on disk, initializing CryBrary reload");

		 // Force dump of instance data if last script reload was successful. (Otherwise we'd override the existing script dump with an invalid one)
		if(m_bLastCompilationSuccess)
			m_AppDomainSerializer->CallMethod("DumpScriptData");

		// Store the previous script domain so we can either restore to it (in case of compilation failure) or fully destruct it.
		pPrevScriptDomain = mono_domain_get();
 	
		mono_domain_set(mono_get_root_domain(), false);

		SAFE_RELEASE(m_pScriptManager);
		SAFE_RELEASE(m_AppDomainSerializer);
	}

	// This will lead to all ScriptDomains using the same friendly name, but doesn't look like it's causing any issues. TODO: Investigate to be sure.
	m_pScriptDomain = mono_domain_create_appdomain("ScriptDomain", NULL);
	mono_domain_set(m_pScriptDomain, false);

	m_pCryBraryAssembly = LoadAssembly(PathUtils::GetBinaryPath() + "CryBrary.dll");
	if (!m_pCryBraryAssembly)
		return false;

	CryLogAlways("		Initializing subsystems...");
	InitializeSystems();

	m_pScriptManager = m_pCryBraryAssembly->InstantiateClass("ScriptManager", "CryEngine.Initialization");

	CryLogAlways("		Compiling scripts...");
	IMonoObject *pInitializationResult = m_pScriptManager->CallMethod("Initialize");

	m_bLastCompilationSuccess = pInitializationResult->Unbox<bool>();

	if(m_bLastCompilationSuccess)
	{
		SAFE_RELEASE(pPrevCryBraryAssembly);

		UnloadDomain(pPrevScriptDomain);
	}
	else if(!initialLoad && g_pMonoCVars->mono_revertScriptsOnError)
	{
		// Compilation failed or something, revert to the old script domain.
		UnloadDomain(m_pScriptDomain);

		mono_domain_set(pPrevScriptDomain, false);

		SAFE_RELEASE(m_pCryBraryAssembly);
		m_pCryBraryAssembly = pPrevCryBraryAssembly;
		
		SAFE_RELEASE(m_AppDomainSerializer);
		SAFE_RELEASE(m_pScriptManager);

		//m_AppDomainSerializer = m_pCryBraryAssembly->InstantiateClass("AppDomainSerializer", "CryEngine.Serialization");
		m_pScriptManager = m_pCryBraryAssembly->InstantiateClass("ScriptManager", "CryEngine.Initialization");
	}

	m_AppDomainSerializer = m_pCryBraryAssembly->InstantiateClass("AppDomainSerializer", "CryEngine.Serialization");

	m_pInput->Reset();
	m_pConverter->Reset();
	m_pUIScriptBind->OnReset();

	// Nodes won't get recompiled if we forget this.
	if(!initialLoad)
	{
		PostInit();

		m_AppDomainSerializer->CallMethod("TrySetScriptData");

		// All CScriptClass objects now contain invalid pointers, this will force an update.
		for each(auto script in m_scripts)
		{
			IMonoArray *pParams = CreateMonoArray(1);
			pParams->Insert(script.second);
			if(IMonoObject *pScriptInstance = m_pScriptManager->CallMethod("GetScriptInstanceById", pParams))
			{
				mono::object monoObject = pScriptInstance->GetMonoObject();

				MonoClass *pMonoClass = mono_object_get_class((MonoObject *)monoObject);
				if(pMonoClass && mono_class_get_name(pMonoClass))
					static_cast<CScriptClass *>(script.first)->OnReload(pMonoClass, monoObject);
			}

			SAFE_RELEASE(pParams);
		}
	}

	return true;
}

void CScriptSystem::UnloadDomain(MonoDomain *pDomain)
{
	if(pDomain)
	{
		mono_domain_finalize(pDomain, -1);

		MonoObject *pException;
		try
		{
			mono_domain_try_unload(pDomain, &pException);
		}
		catch(char *ex)
		{
			CryLogAlways("[MonoWarning] An exception was raised during ScriptDomain unload: %s", ex);
		}

		if(pException)	
		{			
			CryLogAlways("[MonoWarning] An exception was raised during ScriptDomain unload:");
			MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());		
			MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, NULL, NULL);		
			CryLogAlways(ToCryString((mono::string)exceptionString));
		}
	}
}

bool CScriptSystem::InitializeDomain()
{
	// Create root domain and determines the runtime version we'll be using.
	// Currently supported runtimes; (See domain.c) v2.0.50215, v2.0.50727, v4.0.20506, v4.0.30128, v4.0.30319
	// In case mono_jit_init is used instead of mono_jit_init_version; default runtime version is v2.0.50727 (most stable version)
	m_pMonoDomain = mono_jit_init_version("CryMono", "v4.0.30319");
	if(!m_pMonoDomain)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Mono initialization failed!");
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
	RegisterBinding(CActorSystem);
	RegisterBinding(CScriptbind_3DEngine);
	RegisterBinding(CScriptbind_PhysicalWorld);
	RegisterBinding(CScriptbind_Renderer);
	RegisterBinding(CScriptbind_Console);
	RegisterBinding(CScriptbind_ItemSystem);
	RegisterBinding(CGameRules);
	RegisterBinding(CScriptbind_StaticEntity);
	RegisterBinding(CScriptbind_Debug);
	RegisterBinding(CTime);
	RegisterBinding(CScriptbind_MaterialManager);
	RegisterBinding(CScriptbind_ParticleSystem);
	RegisterBinding(CScriptbind_ViewSystem);

#define RegisterBindingAndSet(var, T) RegisterBinding(T); var = (T *)m_localScriptBinds.back();
	RegisterBindingAndSet(m_pUIScriptBind, CScriptbind_UI);
	RegisterBindingAndSet(m_pCallbackHandler, CCallbackHandler);
	RegisterBindingAndSet(m_pEntityManager, CEntityManager);
	RegisterBindingAndSet(m_pFlowManager, CFlowManager);
	RegisterBindingAndSet(m_pInput, CInput);

#undef RegisterBindingAndSet
#undef RegisterBinding
}

bool CScriptSystem::InitializeSystems()
{
	IMonoClass *pClass = m_pCryBraryAssembly->GetCustomClass("CryNetwork");
	IMonoArray *pArray = CreateMonoArray(2);
	pArray->Insert(gEnv->IsEditor());
	pArray->Insert(gEnv->IsDedicated());
	pClass->CallMethod("InitializeNetworkStatics", pArray, true);
	SAFE_RELEASE(pClass);
	SAFE_RELEASE(pArray);

	return true;
}

void CScriptSystem::OnPostUpdate(float fDeltaTime)
{
	// Updates all scripts and sets Time.FrameTime.
	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(fDeltaTime);
	m_pScriptManager->CallMethod("OnUpdate", pArgs);
	SAFE_RELEASE(pArgs);
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

int CScriptSystem::InstantiateScript(const char *scriptName, IMonoArray *pConstructorParameters)
{
	// TODO: Find a new and better way to set Network.IsMultiplayer, IsClient & IsServer. Currently always false!
	/*if(scriptType==EMonoScriptType_GameRules)
	{
		IMonoClass *pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("CryNetwork");
		IMonoArray *pArray = CreateMonoArray(3);
		pArray->Insert(gEnv->bMultiplayer);
		pArray->Insert(gEnv->IsClient());
		pArray->Insert(gEnv->bServer);
		pClass->CallMethod("InitializeNetwork", pArray, true);
		SAFE_RELEASE(pArray);
		SAFE_RELEASE(pClass);
	}*/

	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(scriptName);
	pArgs->Insert(pConstructorParameters);

	int scriptId = -1; // Always returns -1 if unsuccessful, mayhaps change this to 0 and uint?
	if(IMonoObject *pScriptInstance = m_pScriptManager->CallMethod("InstantiateScript", pArgs))
	{
		IMonoClass *pScript = pScriptInstance->Unbox<IMonoClass *>();
		scriptId = pScript->GetScriptId();
		m_scripts.insert(TScripts::value_type(pScript, scriptId));
	}
		
	SAFE_RELEASE(pArgs);
	
	return scriptId;
}

void CScriptSystem::RemoveScriptInstance(int id)
{
	if(id==-1)
		return;

	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it).second==id)
		{
			IMonoArray *pArgs = CreateMonoArray(1);
			pArgs->Insert(id);

			m_pScriptManager->CallMethod("RemoveInstance", pArgs);
			SAFE_RELEASE(pArgs);

			m_scripts.erase(it);

			break;
		}
	}
}

IMonoClass *CScriptSystem::GetScriptById(int id)
{
	if(id==-1)
		return NULL;

	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it).second==id)
			return (*it).first;
	}

	return NULL;
}

IMonoAssembly *CScriptSystem::LoadAssembly(const char *assemblyPath)
{
	return new CScriptAssembly(assemblyPath);
}