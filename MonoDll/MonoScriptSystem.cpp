#include "stdafx.h"
#include "MonoScriptSystem.h"

#include "MonoAssembly.h"
#include "MonoCommon.h"
#include "MonoArray.h"
#include "MonoClass.h"
#include "MonoObject.h"
#include "MonoDomain.h"

#include "CryScriptInstance.h"

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

#include "MonoConverter.h"

// Bindings
#include "Scriptbinds\Console.h"
#include "Scriptbinds\GameRules.h"
#include "Scriptbinds\ActorSystem.h"
#include "Scriptbinds\3DEngine.h"
#include "Scriptbinds\Physics.h"
#include "Scriptbinds\Renderer.h"
#include "Scriptbinds\Debug.h"
#include "Scriptbinds\MaterialManager.h"
#include "Scriptbinds\ParticleSystem.h"
#include "Scriptbinds\ViewSystem.h"
#include "Scriptbinds\LevelSystem.h"
#include "Scriptbinds\Entity.h"
#include "Scriptbinds\Network.h"
#include "Scriptbinds\Time.h"
#include "Scriptbinds\ScriptTable.h" 
#include "Scriptbinds\CrySerialize.h"

#include "FlowManager.h"
#include "MonoInput.h"

#include "MonoCVars.h"
#include "PathUtils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef GetClassName

SCVars *g_pMonoCVars = 0;
CScriptSystem *g_pScriptSystem = 0;

CScriptSystem::CScriptSystem() 
	: m_pRootDomain(nullptr)
	, m_pCryBraryAssembly(nullptr)
	, m_pPdb2MdbAssembly(nullptr)
	, m_pScriptManager(nullptr)
	, m_pScriptDomain(nullptr)
	, m_bReloading(false)
	, m_bDetectedChanges(false)
	, m_bQuitting(false)
{
	CryLogAlways("Initializing Mono Script System");

#ifndef PLUGIN_SDK
	gEnv->pMonoScriptSystem = this;
#endif;

	g_pScriptSystem = this;

	m_pCVars = new SCVars();
	g_pMonoCVars = m_pCVars;
	
	// We should look into storing mono binaries, configuration as well as scripts via CryPak.
	mono_set_dirs(PathUtils::GetMonoLibPath(), PathUtils::GetMonoConfigPath());
	
#ifndef _RELEASE
	// Enable Mono signal handling
	// Makes sure that Mono sends back exceptions it tries to handle, for CE crash handling.
	mono_set_signal_chaining(true);
#endif

	string monoCmdOptions = "";

	if(auto *pArg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "monoArgs"))
		monoCmdOptions.append(pArg->GetValue());
	// Commandline switch -DEBUG makes the process connect to the debugging server. Warning: Failure to connect to a debugging server WILL result in a crash.
	// This is currently a WIP feature which requires custom MonoDevelop extensions and other irritating things.
	const ICmdLineArg* arg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "DEBUG");
	if (arg != nullptr)
		monoCmdOptions.append("--debugger-agent=transport=dt_socket,address=127.0.0.1:65432,embedding=1");
#ifndef _RELEASE
	else if(g_pMonoCVars->mono_softBreakpoints) // Soft breakpoints not compatible with debugging server
	{
		CryLogAlways("		[Performance Warning] Mono soft breakpoints are enabled!");

		// Prevents managed null reference exceptions causing crashes in unmanaged code
		// See: https://bugzilla.xamarin.com/show_bug.cgi?id=5963
		monoCmdOptions.append("--soft-breakpoints");
	}
#endif

	char *options = new char[monoCmdOptions.size() + 1];
	strcpy(options, monoCmdOptions.c_str());

	// Note: iPhone requires AOT compilation, this can be enforced via mono options. TODO: Get Crytek to add CryMobile support to the Free SDK.
	mono_jit_parse_options(1, &options);

#ifndef _RELEASE
	// Required for mdb's to load for detailed stack traces etc.
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
#endif

	m_pConverter = new CConverter();

	if(!CompleteInit())
		return;

	if(IFileChangeMonitor *pFileChangeMonitor = gEnv->pFileChangeMonitor)
		pFileChangeMonitor->RegisterListener(this, "scripts\\");
}

CScriptSystem::~CScriptSystem()
{
	m_bQuitting = true;

	for(auto it = m_localScriptBinds.begin(); it != m_localScriptBinds.end(); ++it)
		delete (*it);
	m_localScriptBinds.clear();

	SAFE_RELEASE(m_pScriptManager);

	// Force garbage collection of all generations.
	mono_gc_collect(mono_gc_max_generation());

	for(auto it = m_domains.rbegin(); it != m_domains.rend(); ++it)
		(*it)->Release();
	m_domains.clear();

	if(gEnv->pSystem)
	{
		// We destroy the script system from the shutdown event, so can't unregister while the system event listener list is iterating.
		// Should be the last event sent, so not unregistering shouldn't be an issue.
		//gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
		gEnv->pGameFramework->UnregisterListener(this);
	}

	if(IFileChangeMonitor *pFileChangeMonitor = gEnv->pFileChangeMonitor)
		pFileChangeMonitor->UnregisterListener(this);

	m_methodBindings.clear();

	SAFE_DELETE(m_pConverter);

	SAFE_DELETE(m_pCVars);

	g_pScriptSystem = nullptr;
}

bool CScriptSystem::CompleteInit()
{
	CryLogAlways("		Initializing CryMono ...");
	
	// Create root domain and determine the runtime version we'll be using.
	m_pRootDomain = new CScriptDomain(eRV_4_30319);
	m_domains.push_back(m_pRootDomain);

	CScriptArray::m_pDefaultElementClass = mono_get_object_class();

#ifndef _RELEASE
	m_pPdb2MdbAssembly = m_pRootDomain->LoadAssembly(PathUtils::GetMonoPath() + "bin\\pdb2mdb.dll");
#endif

	RegisterDefaultBindings();

	m_bFirstReload = true;
	Reload();
	m_bFirstReload = false;

	gEnv->pGameFramework->RegisterListener(this, "CryMono", eFLPriority_Game);

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);

	IMonoClass *pCryStats = m_pCryBraryAssembly->GetClass("CryStats", "CryEngine.Utilities");
	CryLogAlways("		Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + pCryStats->GetPropertyValue(NULL, "MemoryUsage")->Unbox<long>()) / 1024);

	return true;
}

void CScriptSystem::Reload()
{
	if(g_pMonoCVars->mono_realtimeScripting == 0 || m_bReloading)
		return;

	m_bReloading = true;

	if(!m_bFirstReload)
	{
		for each(auto listener in m_listeners)
			listener->OnReloadStart();

		m_pScriptManager->CallMethod("Serialize");
	}

	IMonoDomain *pScriptDomain = CreateDomain("ScriptDomain", true);

	IMonoAssembly *pCryBraryAssembly = pScriptDomain->LoadAssembly(PathUtils::GetBinaryPath() + "CryBrary.dll");

	IMonoArray *pCtorParams = CreateMonoArray(2);
	pCtorParams->InsertAny(m_bFirstReload);
	pCtorParams->InsertMonoString(ToMonoString(PathUtils::GetConfigPath()));

	IMonoObject *pScriptManager = pCryBraryAssembly->GetClass("ScriptManager", "CryEngine.Initialization")->CreateInstance(pCtorParams);

	auto result = pScriptManager->CallMethod("Initialize", m_bFirstReload)->Unbox<EScriptReloadResult>();
	switch(result)
	{
	case EScriptReloadResult_Success:
		{
			// revert previous domain
			if(!m_bFirstReload)
				m_pScriptDomain->Release();

			m_pScriptDomain = pScriptDomain;
			m_pScriptManager = pScriptManager;
			m_pCryBraryAssembly = pCryBraryAssembly;

			if(!m_bFirstReload)
				m_pScriptManager->CallMethod("Deserialize");

			// Set Network.Editor etc.
			IMonoClass *pClass = m_pCryBraryAssembly->GetClass("Network");

			IMonoArray *pArgs = CreateMonoArray(2);
			pArgs->Insert(gEnv->IsEditor());
			pArgs->Insert(gEnv->IsDedicated());
			pClass->InvokeArray(NULL, "InitializeNetworkStatics", pArgs);
			SAFE_RELEASE(pArgs);

			for each(auto listener in m_listeners)
				listener->OnReloadComplete();

			if(!m_bFirstReload && gEnv->IsEditor())
				gEnv->pFlowSystem->ReloadAllNodeTypes();
		}
		break;
	case EScriptReloadResult_Retry:
		Reload();
		return;
	case EScriptReloadResult_Revert:
		{
			pScriptDomain->Release();
			m_pScriptDomain->SetActive();
		}
		break;
	case EScriptReloadResult_Abort:
		gEnv->pSystem->Quit();
		break;
	}

	m_bReloading = false;
}

void CScriptSystem::RegisterDefaultBindings()
{
	// Register what couldn't be registered earlier.
	if(m_methodBindings.size()>0)
	{
		for(TMethodBindings::iterator it = m_methodBindings.begin(); it != m_methodBindings.end(); ++it)
			RegisterMethodBinding((*it).first, (*it).second);
	}

#define RegisterBinding(T) m_localScriptBinds.push_back(new T());
	RegisterBinding(CScriptbind_ActorSystem);
	RegisterBinding(CScriptbind_3DEngine);
	RegisterBinding(CScriptbind_Physics);
	RegisterBinding(CScriptbind_Renderer);
	RegisterBinding(CScriptbind_Console);
	RegisterBinding(CScriptbind_GameRules);
	RegisterBinding(CScriptbind_Debug);
	RegisterBinding(CScriptbind_Time);
	RegisterBinding(CScriptbind_MaterialManager);
	RegisterBinding(CScriptbind_ParticleSystem);
	RegisterBinding(CScriptbind_ViewSystem);
	RegisterBinding(CScriptbind_LevelSystem);
	RegisterBinding(CScriptbind_Entity);
	RegisterBinding(CScriptbind_Network);
	RegisterBinding(CScriptbind_ScriptTable);
	RegisterBinding(CScriptbind_CrySerialize);
	RegisterBinding(CScriptbind_Input);

	m_pFlowManager = new CFlowManager();
	m_pFlowManager->AddRef();

#undef RegisterBindingAndSet
#undef RegisterBinding
}

void CScriptSystem::EraseBinding(IMonoScriptBind *pScriptBind)
{
	if(!m_bQuitting)
		stl::find_and_erase(m_localScriptBinds, pScriptBind);
}

void CScriptSystem::OnPostUpdate(float fDeltaTime)
{
	// Updates all scripts and sets Time.FrameTime.
	m_pScriptManager->CallMethod("OnUpdate", fDeltaTime, gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), gEnv->pTimer->GetAsyncTime().GetMilliSeconds(), gEnv->pTimer->GetFrameRate(), gEnv->pTimer->GetTimeScale());
}

void CScriptSystem::OnFileChange(const char *fileName)
{
	if(g_pMonoCVars->mono_realtimeScriptingDetectChanges == 0)
		return;

	const char *fileExt = PathUtil::GetExt(fileName);
	if(!strcmp(fileExt, "cs") || !strcmp(fileExt, "dll"))
	{
		if(!GetFocus())
		{
			m_bDetectedChanges = true;
			return;
		}

		Reload();
	}
}

void CScriptSystem::OnSystemEvent(ESystemEvent event,UINT_PTR wParam,UINT_PTR lparam)
{
	switch(event)
	{
	case ESYSTEM_EVENT_CHANGE_FOCUS:
		{
			if(wParam != 0 && m_bDetectedChanges)
			{
				Reload();
				m_bDetectedChanges = false;
			}
		}
		break;
	case ESYSTEM_EVENT_SHUTDOWN:
		{
			Release();
		}
		break;
	}
}

void CScriptSystem::RegisterFlownodes()
{
	if(m_pScriptManager && gEnv->pGameFramework->GetIFlowSystem())
		m_pScriptManager->CallMethod("RegisterFlownodes");
}

void CScriptSystem::RegisterMethodBinding(const void *method, const char *fullMethodName)
{
	if(!IsInitialized())
		m_methodBindings.insert(TMethodBindings::value_type(method, fullMethodName));
	else
		mono_add_internal_call(fullMethodName, method);
}

IMonoObject *CScriptSystem::InstantiateScript(const char *scriptName, EMonoScriptFlags scriptType, IMonoArray *pConstructorParameters, bool throwOnFail)
{
	IMonoObject *pResult = m_pScriptManager->CallMethod("CreateScriptInstance", scriptName, scriptType, pConstructorParameters, throwOnFail);

	if(!pResult)
		return nullptr;

	mono::object instance = pResult->GetManagedObject();
	pResult->Release(false);

	return new CCryScriptInstance(instance);
}

void CScriptSystem::RemoveScriptInstance(int id, EMonoScriptFlags scriptType)
{
	if(id==-1)
		return;

	m_pScriptManager->CallMethod("RemoveInstance", id, scriptType);
}


IMonoAssembly *CScriptSystem::GetCorlibAssembly()
{
	return m_pRootDomain->TryGetAssembly(mono_get_corlib());
}

IMonoDomain *CScriptSystem::CreateDomain(const char *name, bool setActive)
{
	CScriptDomain *pDomain = new CScriptDomain(name, setActive);
	m_domains.push_back(pDomain);

	return pDomain;
}

IMonoDomain *CScriptSystem::GetActiveDomain()
{
	return TryGetDomain(mono_domain_get());
}

CScriptDomain *CScriptSystem::TryGetDomain(MonoDomain *pMonoDomain)
{
	for each(auto domain in m_domains)
	{
		if(domain->GetMonoDomain() == pMonoDomain)
			return domain;
	}

	CScriptDomain *pDomain = new CScriptDomain(pMonoDomain);
	m_domains.push_back(pDomain);

	return pDomain;
}

void CScriptSystem::OnDomainReleased(CScriptDomain *pDomain)
{
	if(!m_bQuitting)
		stl::find_and_erase(m_domains, pDomain);
}
