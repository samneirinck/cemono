#include "stdafx.h"
#include "MonoScriptSystem.h"

#include "PathUtils.h"
#include "MonoAssembly.h"
#include "MonoCommon.h"
#include "MonoArray.h"
#include "MonoClass.h"
#include "MonoObject.h"
#include "MonoDomain.h"

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
#include "Scriptbinds\Debug.h"
#include "Scriptbinds\MaterialManager.h"
#include "Scriptbinds\ParticleSystem.h"
#include "Scriptbinds\ViewSystem.h"
#include "Scriptbinds\LevelSystem.h"

#include "EntityManager.h"
#include "FlowManager.h"
#include "MonoInput.h"

#include "CallbackHandler.h"

#include "MonoCVars.h"

SCVars *g_pMonoCVars = 0;

CRYREGISTER_CLASS(CScriptSystem)

CScriptSystem::CScriptSystem() 
	: m_pRootDomain(NULL)
	, m_pCryBraryAssembly(NULL)
	, m_pCallbackHandler(NULL)
	, m_pPdb2MdbAssembly(NULL)
	, m_pScriptManager(NULL)
	, m_pScriptDomain(NULL)
	, m_AppDomainSerializer(NULL)
	, m_pInput(NULL)
	, m_bReloading(false)
	, m_bLastCompilationSuccess(false)
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
		return;

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

	for each(auto script in m_scripts)
	{
		if(script.first)
			script.first->Release();
	}

	m_scripts.clear();

	SAFE_DELETE(m_pConverter);
	SAFE_DELETE(m_pCallbackHandler);

	SAFE_RELEASE(m_AppDomainSerializer);
	SAFE_RELEASE(m_pScriptManager);

	SAFE_RELEASE(m_pCryBraryAssembly);

	SAFE_DELETE(m_pCVars);

	for each(auto scriptbind in m_localScriptBinds)
		SAFE_DELETE(scriptbind);

	m_localScriptBinds.clear();
	m_scriptReloadListeners.clear();

	SAFE_RELEASE(m_pScriptDomain);
	SAFE_RELEASE(m_pRootDomain);

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
	
	// Create root domain and determine the runtime version we'll be using.
	m_pRootDomain = new CScriptDomain(eRV_4_30319);

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
	if(!m_pScriptManager)
		gEnv->pSystem->Quit();
	else
		m_pScriptManager->CallMethod("PostInit");
}

bool CScriptSystem::Reload(bool initialLoad)
{
	PreReload();

	for each(auto listener in m_scriptReloadListeners)
		listener->OnPreScriptReload(initialLoad);

	// Reload is split into Reload & DoReload to make sure we don't call PreReload multiple times.
	return DoReload(initialLoad);
}

void CScriptSystem::PreReload()
{
	// Don't allow another reload to commence while we're already reloading.
	m_bReloading = true;

	// Force dump of instance data if last script reload was successful. (Otherwise we'd override the existing script dump with an invalid one)
	if(m_bLastCompilationSuccess)
		m_AppDomainSerializer->CallMethod("DumpScriptData");
}

bool CScriptSystem::DoReload(bool initialLoad)
{
	// Make sure the new script domain is created under root
	m_pRootDomain->SetActive(true);

	// The script domain as to which all loaded assemblies and scripts will be contained within.
	IMonoDomain *pNewScriptDomain = new CScriptDomain("ScriptDomain");
	pNewScriptDomain->SetActive(true);

	IMonoAssembly *pNewCryBraryAssembly = new CScriptAssembly(PathUtils::GetBinaryPath() + "CryBrary.dll");
	if(!pNewCryBraryAssembly)
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Failed to load CryBrary.dll");
		return false;
	}

	if(initialLoad)
		CryLogAlways("		Initializing subsystems...");

	InitializeSystems(pNewCryBraryAssembly);

	IMonoClass *pNewScriptManager = pNewCryBraryAssembly->InstantiateClass("ScriptManager", "CryEngine.Initialization");

	for each(auto listener in m_scriptReloadListeners)
		listener->OnPreScriptCompilation(!initialLoad);

	if(initialLoad)
		CryLogAlways("		Loading plugins...");

	IMonoObject *pInitializationResult = pNewScriptManager->CallMethod("LoadPlugins");

	m_bLastCompilationSuccess = pInitializationResult ? pInitializationResult->Unbox<bool>() : false;

	for each(auto listener in m_scriptReloadListeners)
		listener->OnPostScriptCompilation(!initialLoad, m_bLastCompilationSuccess);

	if(m_bLastCompilationSuccess)
	{
		SAFE_RELEASE(m_AppDomainSerializer);
		SAFE_RELEASE(m_pScriptManager);
		SAFE_RELEASE(m_pCryBraryAssembly);

		SAFE_RELEASE(m_pScriptDomain);

		m_pScriptDomain = pNewScriptDomain;

		m_pCryBraryAssembly = pNewCryBraryAssembly;
		m_pScriptManager = pNewScriptManager;

		PostReload(initialLoad);
	}
	else
	{
		m_pRootDomain->SetActive();

		SAFE_RELEASE(pNewCryBraryAssembly);
		SAFE_RELEASE(m_AppDomainSerializer);
		SAFE_RELEASE(pNewScriptManager);
		
		SAFE_DELETE(pNewScriptDomain);

		if(!initialLoad)
		{
			//Cancel, Try Again, Continue
			switch(CryMessageBox("Script compilation failed, check log for more information. Do you want to continue by reloading the last successful script compilation?", "Script compilation failed!", 0x00000006L))
			{
			case 2: // cancel (quit)
				gEnv->pSystem->Quit();
				return false;
			case 10: // try again (recompile)
				return DoReload(initialLoad);
			case 11: // continue (load previously functional script domain)
				m_pScriptDomain->SetActive();
				break;
			}
		}
		else
		{
			//Cancel, Retry
			switch(CryMessageBox("Script compilation failed, check log for more information.", "Script compilation failed!", 0x00000005L))
			{
			case 2: // cancel (quit)
				return false;
			case 4: // retry (recompile)
				return DoReload(initialLoad);
			}
		}
	}

	return true;
}

void CScriptSystem::PostReload(bool initialLoad)
{
	m_AppDomainSerializer = m_pCryBraryAssembly->InstantiateClass("AppDomainSerializer", "CryEngine.Serialization");

	m_pConverter->Reset();

	// Nodes won't get recompiled if we forget this.
	if(!initialLoad)
	{
		PostInit();

		m_AppDomainSerializer->CallMethod("TrySetScriptData");

		// Since we've destructed all previous scripts, assemblies and script domains, pointers to these are now invalid.
		// Iterate through all scripts and get the new script instance objects.
		for each(auto script in m_scripts)
		{
			IMonoArray *pParams = CreateMonoArray(1);
			pParams->Insert(script.second);
			if(IMonoObject *pScriptInstance = m_pScriptManager->CallMethod("GetScriptInstanceById", pParams, true))
			{
				mono::object monoObject = pScriptInstance->GetMonoObject();

				MonoClass *pMonoClass = mono_object_get_class((MonoObject *)monoObject);
				if(pMonoClass && mono_class_get_name(pMonoClass))
					static_cast<CScriptClass *>(script.first)->OnReload(pMonoClass, monoObject);
			}

			SAFE_RELEASE(pParams);
		}
	}

	m_bReloading = false;

	for each(auto listener in m_scriptReloadListeners)
		listener->OnPostScriptReload(initialLoad);
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
	RegisterBinding(CScriptbind_Debug);
	RegisterBinding(CTime);
	RegisterBinding(CScriptbind_MaterialManager);
	RegisterBinding(CScriptbind_ParticleSystem);
	RegisterBinding(CScriptbind_ViewSystem);
	RegisterBinding(CLevelSystem);

#define RegisterBindingAndSet(var, T) RegisterBinding(T); var = (T *)m_localScriptBinds.back();
	RegisterBindingAndSet(m_pCallbackHandler, CCallbackHandler);
	RegisterBindingAndSet(m_pEntityManager, CEntityManager);
	RegisterBindingAndSet(m_pFlowManager, CFlowManager);
	RegisterBindingAndSet(m_pInput, CInput);

#undef RegisterBindingAndSet
#undef RegisterBinding
}

bool CScriptSystem::InitializeSystems(IMonoAssembly *pCryBraryAssembly)
{
	IMonoClass *pClass = pCryBraryAssembly->GetCustomClass("CryNetwork");
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
	IMonoArray *pArray = CreateMonoArray(1);
	pArray->Insert(fDeltaTime);
	m_pScriptManager->CallMethod("OnUpdate", pArray, true);
	SAFE_RELEASE(pArray);
}

void CScriptSystem::OnFileChange(const char *fileName)
{
	if(m_bReloading)
		return;

	const char *fileExt = PathUtil::GetExt(fileName);
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

IMonoClass *CScriptSystem::InstantiateScript(const char *scriptName, EMonoScriptType scriptType, IMonoArray *pConstructorParameters)
{
	IMonoArray *pArray = CreateMonoArray(3);
	pArray->Insert(scriptName);
	pArray->Insert(scriptType);
	pArray->Insert(pConstructorParameters);
	IMonoObject *pResult = m_pScriptManager->CallMethod("InstantiateScript", pArray, true);
	SAFE_RELEASE(pArray);

	auto *pScript = pResult ? pResult->Unbox<IMonoClass *>() : NULL;

	if(pScript)
		m_scripts.insert(TScripts::value_type(pScript, pScript->GetScriptId()));
	else
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Failed to instantiate script %s", scriptName);

	return pScript;
}

void CScriptSystem::RemoveScriptInstance(int id, EMonoScriptType scriptType)
{
	if(id==-1)
		return;

	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it).second==id)
		{
			m_scripts.erase(it);

			break;
		}
	}

	CallMonoScript<void>(m_pScriptManager, "RemoveInstance", id, scriptType);
}