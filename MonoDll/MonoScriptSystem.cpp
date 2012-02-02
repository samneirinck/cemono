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

#include "EntityManager.h"
#include "FlowManager.h"
#include "CallbackHandler.h"

#ifndef _RELEASE
#include "MonoTester.h"
#endif

IMonoAutoRegScriptBind *IMonoAutoRegScriptBind::m_pFirst = NULL;
IMonoAutoRegScriptBind *IMonoAutoRegScriptBind::m_pLast = NULL;
CRYREGISTER_CLASS(CMonoScriptSystem)

CMonoScriptSystem::CMonoScriptSystem() 
	: m_pMonoDomain(NULL)
	, m_pLibraryAssembly(NULL)
	, m_pCallbackHandler(NULL)
{
	CryLog("Initializing Mono Script System");

	//system(CMonoPathUtils::GetMonoPath() + "bin\\pdb2mdb.exe" + " " + CMonoPathUtils::GetBinaryPath() + "CryBrary.dll");

	// We should look into CryPak for this (as well as c# scripts), in case it's possible to read them while they're pak'd.
	mono_set_dirs(CMonoPathUtils::GetLibPath(), CMonoPathUtils::GetConfigPath());

	string monoCmdOptions = "";

	// Commandline switch -CEMONO_DEBUG makes the process connect to the debugging server
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

	for(TScripts::iterator it = m_scripts.begin(); it != m_scripts.end(); ++it)
		SAFE_DELETE((*it));

	m_scripts.clear();
	m_scriptBinds.clear();

	SAFE_DELETE(m_pConverter);
	SAFE_DELETE(m_pCallbackHandler);

	SAFE_DELETE(m_pCryConsole);
	SAFE_DELETE(m_pScriptCompiler);
	SAFE_DELETE(m_pLibraryAssembly);

	SAFE_DELETE(m_pEntityManager);
	SAFE_DELETE(m_pFlowManager);

	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	gEnv->pMonoScriptSystem = NULL;
}

bool CMonoScriptSystem::Init()
{
	CryLog("    Initializing CryMono...");
	
	if (!InitializeDomain())
		return false;

	m_pLibraryAssembly = LoadAssembly(CMonoPathUtils::GetBinaryPath() + "CryBrary.dll");
	if (!m_pLibraryAssembly)
		return false;

	m_pCryConsole = m_pLibraryAssembly->InstantiateClass("CryEngine", "Console");

	m_pScriptCompiler = m_pLibraryAssembly->InstantiateClass("CryEngine", "ScriptCompiler");

	CryLog("    Registering default scriptbinds...");
	RegisterDefaultBindings();

	CryLog("    Initializing subsystems...");
	InitializeSystems();

	CryLog("    Compiling scripts...");
	m_pScriptCompiler->CallMethod("Initialize");

	CryModuleMemoryInfo memInfo;
	CryModuleGetMemoryInfo(&memInfo);
	CryLog("    Initializing CryMono done, MemUsage=%iKb", (memInfo.allocated + m_pLibraryAssembly->GetCustomClass("CryStats", "CryEngine.Utils")->GetProperty("MemoryUsage")->Unbox<long>()) / 1024);

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
	if(m_scriptBinds.size()>0)
	{
		for(std::vector<IMonoScriptBind *>::iterator it = m_scriptBinds.begin(); it != m_scriptBinds.end(); ++it)
			RegisterScriptBind((*it));

		m_scriptBinds.clear();
	}

	IMonoAutoRegScriptBind *pScriptBind = IMonoAutoRegScriptBind::m_pFirst;
	while(pScriptBind)
	{
		RegisterScriptBind(pScriptBind);

		pScriptBind = pScriptBind->m_pNext;
	}

	m_pCallbackHandler = new CMonoCallbackHandler();
	m_pEntityManager = new CEntityManager();
	m_pFlowManager = new CFlowManager();

	// We need these later.
	m_pEntityManager->AddRef();
	m_pFlowManager->AddRef();

	RegisterScriptBind(m_pEntityManager);
	RegisterScriptBind(m_pFlowManager);
}

bool CMonoScriptSystem::InitializeSystems()
{
#ifndef _RELEASE
	CMonoTester *pTester = new CMonoTester();
	pTester->AddRef();

	pTester->CommenceTesting();
	SAFE_RELEASE(pTester);
#endif

	IMonoClass *pClass = m_pLibraryAssembly->GetCustomClass("CryNetwork");
	IMonoArray *pArray = GetConverter()->CreateArray(2);
	pArray->Insert(gEnv->IsEditor());
	pArray->Insert(gEnv->IsDedicated());
	pClass->CallMethod("InitializeNetworkStatics", pArray, true);
	SAFE_DELETE(pClass);
	SAFE_DELETE(pArray);

	return true;
}

void CMonoScriptSystem::Update(float frameTime)
{
	//m_pCallbackHandler->InvokeCallback("Update", "Manager");

	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(frameTime);
	m_pScriptCompiler->CallMethod("OnUpdate", pArgs);
}

void CMonoScriptSystem::OnFileChange(const char *sFilename)
{
	IMonoArray *pParams = CreateMonoArray(1);
	pParams->Insert(sFilename);

	m_pScriptCompiler->CallMethod("OnFileChange", pParams);
	SAFE_DELETE(pParams);
}

void CMonoScriptSystem::RegisterScriptBind(IMonoScriptBind *pScriptBind)
{
	if(!IsInitialized())
	{
		m_scriptBinds.push_back(pScriptBind);

		return;
	}

	pScriptBind->AddRef();

	string namespaceName = pScriptBind->GetNamespace();
	if (strcmp(pScriptBind->GetNamespaceExtension(), ""))
	{
		namespaceName.append(".");
		namespaceName.append(pScriptBind->GetNamespaceExtension());
	}

	string fullName = (namespaceName + ".").append(pScriptBind->GetClassName()).append("::");

	std::vector<IMonoMethodBinding> methodBindings = pScriptBind->GetMethods();
#ifdef SCRIPTBIND_GENERATION
	IMonoArray *pMethods = CreateMonoArray(methodBindings.size());
#endif

	for(std::vector<IMonoMethodBinding>::iterator it = methodBindings.begin(); it != methodBindings.end(); ++it)
	{
#ifdef SCRIPTBIND_GENERATION
		InternalCallMethod *pInternalCall = new InternalCallMethod(ToMonoString((*it).methodName), ToMonoString((*it).returnType), ToMonoString((*it).parameters));
		IMonoObject *pConvertedInternalCall = GetConverter()->CreateObjectOfCustomType(pInternalCall, "InternalCallMethod", "CryEngine");
		pMethods->Insert(pConvertedInternalCall);
		SAFE_DELETE(pInternalCall);
#endif

		mono_add_internal_call(fullName + (*it).methodName, (*it).method);
	}

#ifdef SCRIPTBIND_GENERATION
	IMonoArray *pArray = CreateMonoArray(3);
	pArray->Insert(namespaceName);
	pArray->Insert(pScriptBind->GetClassName());
	pArray->Insert(pMethods);

	m_pScriptCompiler->CallMethod("RegisterScriptbind", pArray);
#endif

	//pScriptBind->Release();
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
		SAFE_DELETE(pArray);
		SAFE_DELETE(pClass);
	}
	
	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it)->GetScriptType()==scriptType && !strcmp((*it)->GetName(), scriptName))
		{
			if(scriptType==EMonoScriptType_GameRules)
				m_scripts.erase(it);

			break;
		}
	}

	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(scriptName);
	pArgs->Insert(pConstructorParameters);

	int scriptId = m_pScriptCompiler->CallMethod("InstantiateScript", pArgs)->Unbox<int>();
	SAFE_DELETE(pArgs);

	if(scriptId!=-1)
		m_scripts.push_back(new CMonoClass(scriptId, scriptType));

	return scriptId;
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
			SAFE_DELETE(pArgs);

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
	return new CMonoAssembly(m_pMonoDomain, assemblyPath);
}