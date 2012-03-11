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
#include "Scriptbinds\ScriptBind_Debug.h"

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
	, m_AppDomainSerializer(NULL)
	, m_pInput(NULL)
	, m_bLastCompilationSuccess(true)
{
	CryLogAlways("Initializing Mono Script System");

	// We should look into CryPak for this (as well as c# scripts), in case it's possible to read them while they're pak'd.
	mono_set_dirs(PathUtils::GetLibPath(), PathUtils::GetConfigPath());

	string monoCmdOptions = "";

	// Commandline switch -DEBUG makes the process connect to the debugging server. Warning: Failure to connect to a	debugging server WILL result in a crash.
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
	
	REGISTER_GAME_OBJECT_EXTENSION(gEnv->pGameFramework, ScriptManager);

	CryLogAlways("		Registering default scriptbinds...");
	RegisterDefaultBindings();

	if(!Reload(true))
		return false;

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);
	CryLogAlways("		Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + m_pCryBraryAssembly->GetCustomClass("CryStats", "CryEngine.Utils")->GetProperty("MemoryUsage")->Unbox<long>()) / 1024);

	return true;
}

void CScriptSystem::PostInit()
{
	//GetFlowManager()->Reset();

	m_pScriptManager->CallMethod("PostInit", true);
}

bool CScriptSystem::Reload(bool initialLoad)
{
	MonoDomain *pPrevScriptDomain = NULL;
	IMonoAssembly *pPrevCryBraryAssembly = m_pCryBraryAssembly;

	// Store the state of current instances to apply them to the reloaded scripts at the end.
	if(!initialLoad)
	{
		CryLogAlways("C# modifications detected on disk, initializing CryBrary reload");

		 // Force dump of instance data if last script reload was successful. (Otherwise we'll override the existing script dump with an invalid one)
		if(m_bLastCompilationSuccess)
			m_AppDomainSerializer->CallMethod("DumpScriptData");

		pPrevScriptDomain = mono_domain_get();
 	
		mono_domain_set(mono_get_root_domain(), false);

		SAFE_DELETE(m_pScriptManager);
		SAFE_DELETE(m_AppDomainSerializer);
	}

	m_pScriptDomain = mono_domain_create_appdomain("ScriptDomain", NULL);
	mono_domain_set(m_pScriptDomain, false);

	m_pCryBraryAssembly = LoadAssembly(PathUtils::GetBinaryPath() + "CryBrary.dll");
	if (!m_pCryBraryAssembly)
		return false;

	m_AppDomainSerializer = m_pCryBraryAssembly->InstantiateClass("AppDomainSerializer", "CryEngine.Serialization");

	CryLogAlways("		Initializing subsystems...");
	InitializeSystems();

	CryLogAlways("		Compiling scripts...");
	m_pScriptManager = m_pCryBraryAssembly->GetCustomClass("ScriptCompiler");

	IMonoObject *pInitializationResult = m_pScriptManager->CallMethod("Initialize", true);

	m_bLastCompilationSuccess = pInitializationResult->Unbox<bool>();

	if(m_bLastCompilationSuccess)
	{
		SAFE_DELETE(pPrevCryBraryAssembly);

		UnloadDomain(pPrevScriptDomain);
	}
	else if(!initialLoad)
	{
		// Compilation failed or something, revert to the old script domain.
		UnloadDomain(m_pScriptDomain);

		mono_domain_set(pPrevScriptDomain, false);

		SAFE_DELETE(m_pCryBraryAssembly);
		m_pCryBraryAssembly = pPrevCryBraryAssembly;
		
		SAFE_DELETE(m_AppDomainSerializer);
		SAFE_DELETE(m_pScriptManager);

		m_AppDomainSerializer = m_pCryBraryAssembly->InstantiateClass("AppDomainSerializer", "CryEngine.Serialization");
		m_pScriptManager = m_pCryBraryAssembly->GetCustomClass("ScriptCompiler");
	}

	m_pInput->Reset();
	m_pConverter->Reset();

	// Nodes won't get recompiled if we forget this.
	if(!initialLoad)
	{
		PostInit();

		m_AppDomainSerializer->CallMethod("TrySetScriptData");

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

	gEnv->pGameFramework->RegisterListener(this, "CryMono", eFLPriority_Game);

	return true;
}

void CScriptSystem::UnloadDomain(MonoDomain *pDomain)
{
	if(pDomain)
	{
		mono_domain_finalize(pDomain, -1);

		MonoObject *pException;
		mono_domain_try_unload(pDomain, &pException);

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
	RegisterBinding(CScriptBind_GameRules);
	RegisterBinding(CScriptBind_StaticEntity);
	RegisterBinding(CScriptBind_Debug);
	RegisterBinding(CScriptBind_Time);
	RegisterBinding(CTester);

#define RegisterBindingAndSet(var, T) RegisterBinding(T); var = (T *)m_localScriptBinds.back();
	RegisterBindingAndSet(m_pCallbackHandler, CCallbackHandler);
	RegisterBindingAndSet(m_pEntityManager, CEntityManager);
	RegisterBindingAndSet(m_pFlowManager, CFlowManager);
	RegisterBindingAndSet(m_pInput, CInput);

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

void CScriptSystem::OnPostUpdate(float fDeltaTime)
{
	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(fDeltaTime);
	m_pScriptManager->CallMethod("OnUpdate", pArgs, true);
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

	int scriptId = -1;
	if(IMonoObject *pScriptInstance = m_pScriptManager->CallMethod("InstantiateScript", pArgs, true))
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
			IMonoArray *pArgs = CreateMonoArray(2);
			pArgs->Insert(id);
			pArgs->Insert((*it).first->GetName());

			m_pScriptManager->CallMethod("RemoveInstance", pArgs, true);
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