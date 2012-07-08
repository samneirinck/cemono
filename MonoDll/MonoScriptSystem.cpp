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
#include "Scriptbinds\Physics.h"
#include "Scriptbinds\Renderer.h"
#include "Scriptbinds\Debug.h"
#include "Scriptbinds\MaterialManager.h"
#include "Scriptbinds\ParticleSystem.h"
#include "Scriptbinds\ViewSystem.h"
#include "Scriptbinds\LevelSystem.h"
#include "Scriptbinds\UI.h"
#include "Scriptbinds\Entity.h"
#include "Scriptbinds\Network.h"
#include "Scriptbinds\Time.h"

#include "FlowManager.h"
#include "MonoInput.h"

#include "MonoCVars.h"

SCVars *g_pMonoCVars = 0;

CRYREGISTER_CLASS(CScriptSystem)

CScriptSystem::CScriptSystem() 
	: m_pRootDomain(nullptr)
	, m_pCryBraryAssembly(nullptr)
	, m_pPdb2MdbAssembly(nullptr)
	, m_pScriptManager(nullptr)
	, m_pInput(nullptr)
	, m_bHasPostInitialized(false)
{
	//CryLogAlways("Initializing Mono Script System");
	
	// We should look into storing mono binaries, configuration as well as scripts via CryPak.
	mono_set_dirs(PathUtils::GetLibPath(), PathUtils::GetConfigPath());

	string monoCmdOptions = "";

#ifndef _RELEASE
	// Prevents managed null reference exceptions causing crashes in unmanaged code
	// See: https://bugzilla.xamarin.com/show_bug.cgi?id=5963
	monoCmdOptions.append("--soft-breakpoints");
#endif

	// Commandline switch -DEBUG makes the process connect to the debugging server. Warning: Failure to connect to a debugging server WILL result in a crash.
	// This is currently a WIP feature which requires custom MonoDevelop extensions and other irritating things.
	const ICmdLineArg* arg = gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "DEBUG");
	if (arg != nullptr)
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
	for(auto it = m_assemblies.begin(); it != m_assemblies.end(); ++it)
		SAFE_RELEASE(*it);

	// Force garbage collection of all generations.
	mono_gc_collect(mono_gc_max_generation());

	if(gEnv->pSystem)
		gEnv->pGameFramework->UnregisterListener(this);

	if(IFileChangeMonitor *pFileChangeMonitor = gEnv->pFileChangeMonitor)
		pFileChangeMonitor->UnregisterListener(this);

	m_methodBindings.clear();

	m_scriptInstances.clear();

	SAFE_DELETE(m_pConverter);

	SAFE_RELEASE(m_pScriptManager);

	SAFE_DELETE(m_pCryBraryAssembly);

	SAFE_DELETE(m_pCVars);

	SAFE_RELEASE(m_pRootDomain);
}

struct SScriptManagerCreator : public IGameObjectExtensionCreatorBase
{
	CScriptManager *Create() { return new CScriptManager(); }

	void GetGameObjectExtensionRMIData(void **ppRMI, size_t *nCount) { CScriptManager::GetGameObjectExtensionRMIData(ppRMI, nCount); }
};

bool CScriptSystem::CompleteInit()
{
	CryLogAlways("		Initializing CryMono...");
	
	// Create root domain and determine the runtime version we'll be using.
	m_pRootDomain = new CScriptDomain(eRV_4_30319);

	CScriptArray::m_pDefaultElementClass = mono_get_object_class();

#ifndef _RELEASE
	m_pPdb2MdbAssembly = GetAssembly(PathUtils::GetMonoPath() + "bin\\pdb2mdb.dll");
#endif

	m_pCryBraryAssembly = GetAssembly(PathUtils::GetBinaryPath() + "CryBrary.dll");

	static SScriptManagerCreator scriptManagerCreator;

	IEntityClassRegistry::SEntityClassDesc scriptManagerClassDesc;
	scriptManagerClassDesc.flags |= ECLF_INVISIBLE;
	scriptManagerClassDesc.sName = "ScriptManager";
	gEnv->pGameFramework->GetIGameObjectSystem()->RegisterExtension("ScriptManager", &scriptManagerCreator, &scriptManagerClassDesc);

	CryLogAlways("		Registering default scriptbinds...");
	RegisterDefaultBindings();

	m_pScriptManager = m_pCryBraryAssembly->GetClass("ScriptManager", "CryEngine.Initialization")->CreateInstance();

	gEnv->pGameFramework->RegisterListener(this, "CryMono", eFLPriority_Game);

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);

	IMonoClass *pCryStats = m_pCryBraryAssembly->GetClass("CryStats", "CryEngine.Utilities");
	CryLogAlways("		Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + pCryStats->GetProperty("MemoryUsage")->Unbox<long>()) / 1024);

	return true;
}

void CScriptSystem::OnSystemEvent(ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam)
{
	switch(event)
	{
	case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			if(!m_bHasPostInitialized && gEnv->pGameFramework->GetIFlowSystem())
			{
				m_pScriptManager->CallMethod("PostInit");

				if(!gEnv->pEntitySystem->FindEntityByName("ScriptManager") && gEnv->bServer)
				{
					SEntitySpawnParams params;
					params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("ScriptManager");
					params.sName = "ScriptManager";
					params.nFlags = ENTITY_FLAG_SPAWNED | ENTITY_FLAG_UNREMOVABLE;
					params.vPosition = Vec3(0,0,0);
					gEnv->pEntitySystem->SpawnEntity(params);
				}

				m_bHasPostInitialized = true;
			}
		}
		break;
	}
}

void CScriptSystem::RegisterDefaultBindings()
{
	// Register what couldn't be registered earlier.
	if(m_methodBindings.size()>0)
	{
		for(TMethodBindings::iterator it = m_methodBindings.begin(); it != m_methodBindings.end(); ++it)
			RegisterMethodBinding((*it).first, (*it).second);
	}

#define RegisterBinding(T) m_localScriptBinds.push_back(std::shared_ptr<IMonoScriptBind>(new T()));
	RegisterBinding(CActorSystem);
	RegisterBinding(CScriptbind_3DEngine);
	RegisterBinding(CScriptbind_Physics);
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
	RegisterBinding(CUI);
	RegisterBinding(CScriptbind_Entity);
	RegisterBinding(CNetwork);

#define RegisterBindingAndSet(var, T) RegisterBinding(T); var = (T *)m_localScriptBinds.back().get();
	RegisterBindingAndSet(m_pFlowManager, CFlowManager);
	RegisterBindingAndSet(m_pInput, CInput);

#undef RegisterBindingAndSet
#undef RegisterBinding
}

bool CScriptSystem::InitializeSystems()
{
	IMonoClass *pClass = m_pCryBraryAssembly->GetClass("Network");

	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(gEnv->IsEditor());
	pArgs->Insert(gEnv->IsDedicated());
	pClass->CallMethodWithArray("InitializeNetworkStatics", pArgs, true);
	SAFE_RELEASE(pArgs);

	return true;
}

void CScriptSystem::OnPostUpdate(float fDeltaTime)
{
	// Updates all scripts and sets Time.FrameTime.
	m_pScriptManager->CallMethod("OnUpdate", fDeltaTime, gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), gEnv->pTimer->GetAsyncTime().GetMilliSeconds(), gEnv->pTimer->GetFrameRate(), gEnv->pTimer->GetTimeScale());
}

void CScriptSystem::OnFileChange(const char *fileName)
{
	if(g_pMonoCVars->mono_realtimeScripting == 0)
		return;

	const char *fileExt = PathUtil::GetExt(fileName);
	if(!strcmp(fileExt, "cs") || !strcmp(fileExt, "dll"))
		m_pScriptManager->CallMethod("OnReload");
}

void CScriptSystem::RegisterMethodBinding(const void *method, const char *fullMethodName)
{
	if(!IsInitialized())
		m_methodBindings.insert(TMethodBindings::value_type(method, fullMethodName));
	else
		mono_add_internal_call(fullMethodName, method);
}

IMonoObject *CScriptSystem::InstantiateScript(const char *scriptName, EMonoScriptFlags scriptType, IMonoArray *pConstructorParameters)
{
	IMonoObject *pResult = m_pScriptManager->CallMethod("CreateScriptInstance", scriptName, scriptType, pConstructorParameters);

	if(!pResult)
		MonoWarning("Failed to instantiate script %s", scriptName);
	else
		RegisterScriptInstance(pResult, pResult->GetProperty("ScriptId")->Unbox<int>());

	return pResult;
}

void CScriptSystem::RemoveScriptInstance(int id, EMonoScriptFlags scriptType)
{
	if(id==-1)
		return;

	for(TScripts::iterator it=m_scriptInstances.begin(); it != m_scriptInstances.end(); ++it)
	{
		if((*it).second==id)
		{
			m_scriptInstances.erase(it);

			break;
		}
	}

	m_pScriptManager->CallMethod("RemoveInstance", id, scriptType);
}

IMonoAssembly *CScriptSystem::GetCorlibAssembly()
{
	return CScriptAssembly::TryGetAssembly(mono_get_corlib());
}

IMonoAssembly *CScriptSystem::GetCryBraryAssembly()
{
	return m_pCryBraryAssembly;
}

const char *CScriptSystem::GetAssemblyPath(const char *currentPath, bool shadowCopy)
{
	if(shadowCopy)
		return PathUtils::GetTempPath().append(PathUtil::GetFile(currentPath));

	return currentPath;
}

MonoImage *CScriptSystem::GetAssemblyImage(const char *file)
{
	MonoAssembly *pMonoAssembly = mono_domain_assembly_open(mono_domain_get(), file);
	CRY_ASSERT(pMonoAssembly);

	return mono_assembly_get_image(pMonoAssembly);
}

IMonoAssembly *CScriptSystem::GetAssembly(const char *file, bool shadowCopy)
{
	const char *newPath = GetAssemblyPath(file, shadowCopy);

	for each(auto assembly in m_assemblies)
	{
		if(!strcmp(newPath, assembly->GetPath()))
			return assembly;
	}

	if(shadowCopy)
	{
		CopyFile(file, newPath, false);
		file = newPath;
	}

	string sAssemblyPath(file);
#ifndef _RELEASE
	if(sAssemblyPath.find("pdb2mdb")==-1)
	{
		if(IMonoAssembly *pDebugDatabaseCreator = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetDebugDatabaseCreator())
		{
			if(IMonoClass *pDriverClass = pDebugDatabaseCreator->GetClass("Driver", ""))
			{
				IMonoArray *pArgs = CreateMonoArray(1);
				pArgs->Insert(file);
				pDriverClass->CallMethodWithArray("Convert", pArgs, true);
				SAFE_RELEASE(pArgs);
			}
		}
	}
#endif

	CScriptAssembly *pAssembly = new CScriptAssembly(GetAssemblyImage(file), file);
	m_assemblies.push_back(pAssembly);
	return pAssembly;
}