#include "stdafx.h"
#include "MonoScriptSystem.h"

#include "MonoPathUtils.h"
#include "MonoAssembly.h"
#include "MonoString.h"
#include "MonoArray.h"
#include "MonoClass.h"
#include "MonoScript.h"
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

// Bindings
#include "Scriptbinds\ActorSystem.h"
#include "Scriptbinds\GameRulesSystem.h"
#include "Scriptbinds\ItemSystem.h"
#include "Scriptbinds\Console.h"

#include "EntityManager.h"
#include "CallbackHandler.h"

#ifndef _RELEASE
#include "MonoTester.h"
#endif

CRYREGISTER_CLASS(CMonoScriptSystem)

CMonoScriptSystem::CMonoScriptSystem() 
	: m_pMonoDomain(NULL)
	, m_bDebugging(false)
	, m_pBclAssembly(NULL)
	, m_pManagerAssembly(NULL)
	, m_pCallbackHandler(0)
{
	gEnv->pMonoScriptSystem = this;
}

CMonoScriptSystem::~CMonoScriptSystem()
{
	m_scripts.clear();
	SAFE_DELETE(m_callParams.pArray);

	SAFE_DELETE(m_pTester);
	SAFE_DELETE(m_pCallbackHandler);

	SAFE_DELETE(m_pManagerClass);
	SAFE_DELETE(m_pManagerAssembly);
	SAFE_DELETE(m_pBclAssembly);

	if (m_pMonoDomain)
		mono_jit_cleanup(m_pMonoDomain);

	SAFE_DELETE(m_pMonoDomain);

	gEnv->pMonoScriptSystem = NULL;
}

bool CMonoScriptSystem::Init()
{
	CryLog("    Initializing CryMono ...");

	// We should look into CryPak for this (as well as c# scripts), in case it's possible to read them while they're pak'd.
	mono_set_dirs(CMonoPathUtils::GetLibPath(), CMonoPathUtils::GetConfigPath());
	
	if (!InitializeDomain())
		return false;

	RegisterDefaultBindings();
	
	if (!InitializeBaseClassLibraries())
		return false;

	if(!InitializeManager())
		return false;

	CryLog("    Initializing CryMono done, MemUsage=1337Kb");

	return true;
}

void CMonoScriptSystem::Update()
{
	//m_pCallbackHandler->InvokeCallback("Update", "Manager");
	m_pManagerClass->CallMethod("Update");
}

void CMonoScriptSystem::RegisterScriptBind(IMonoScriptBind *pScriptBind)
{	
	// Get all methods registered in this class
	std::vector<IMonoMethodBinding> methodBindings = pScriptBind->GetMethods();
	std::for_each( begin(methodBindings), end(methodBindings), [&]( IMonoMethodBinding& binding) {
		// Construct the full method name
		// Typically something like CryEngine.API.Console._LogAlways
		string fullName = pScriptBind->GetNamespace();
		if (strcmp(pScriptBind->GetNamespaceExtension(), ""))
		{
			fullName.append(".");
			fullName.append(pScriptBind->GetNamespaceExtension());
		}
		fullName.append(".");
		fullName.append(pScriptBind->GetClassName());
		fullName.append("::");
		
		fullName.append(binding.methodName);

		mono_add_internal_call(fullName, binding.method);
	});
}

int CMonoScriptSystem::InstantiateScript(EMonoScriptType scriptType, const char *scriptName)
{
	CMonoArray args(2);
	args.InsertObject(CreateMonoObject<int>(scriptType));
	args.InsertString(scriptName);

	IMonoObject *pResult = m_pManagerClass->CallMethod("InstantiateClass", &args);

	m_scripts.push_back(new CMonoScript(pResult->Unbox<int>(), scriptName, scriptType));
	delete pResult;

	return m_scripts.at(m_scripts.size()-1)->GetId();
}

void CMonoScriptSystem::RemoveScriptInstance(EMonoScriptType scriptType, const char* scriptName)
{
	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if(!strcmp((*it)->GetName(), scriptName) && (*it)->GetType()==scriptType)
			m_scripts.erase(it);
	}
}

IMonoScript *CMonoScriptSystem::GetScriptById(int id)
{
	for(TScripts::iterator it=m_scripts.begin(); it != m_scripts.end(); ++it)
	{
		if((*it)->GetId()==id)
			return (*it);
	}

	return NULL;
}

bool CMonoScriptSystem::InitializeDomain()
{
	// Create root domain 
	m_pMonoDomain = mono_jit_init_version("CryMono", "v4.0.30128");
	if(!m_pMonoDomain)
	{
		GameWarning("Mono initialization failed!");
		return false;
	}

	return (m_pMonoDomain != NULL);
}

void CMonoScriptSystem::RegisterDefaultBindings()
{
	m_pCallbackHandler = new CMonoCallbackHandler();
	m_pEntityManager = new CEntityManager();

	RegisterScriptBind(new CScriptBind_Console());
	RegisterScriptBind(new CScriptBind_ItemSystem());
	RegisterScriptBind(new CScriptBind_GameRules());
	RegisterScriptBind(new CScriptBind_ActorSystem());
	RegisterScriptBind(m_pEntityManager);
}

IMonoEntityManager *CMonoScriptSystem::GetEntityManager() const
{
	return m_pEntityManager;
}

bool CMonoScriptSystem::InitializeBaseClassLibraries()
{
	string bclPath = CMonoPathUtils::GetBinaryPath() + "BaseLibrary.dll";
	m_pBclAssembly = new CMonoAssembly(m_pMonoDomain, bclPath);

	return m_pBclAssembly != NULL;
}

bool CMonoScriptSystem::InitializeManager()
{	
	string managerPath = CMonoPathUtils::GetBinaryPath() + "Manager.dll";
	m_pManagerAssembly = new CMonoAssembly(m_pMonoDomain, managerPath);
	if (!m_pManagerAssembly)
		return false;

#ifndef _RELEASE
	m_pTester = new CMonoTester();
#endif

	m_pManagerClass = m_pManagerAssembly->InstantiateClass("CryMono", "Manager");

	return true;
}

IMonoClass *CMonoScriptSystem::GetCustomClass(const char *className, const char *nameSpace, bool baseClassAssembly)
{ 
	return new CMonoClass(mono_class_from_name(static_cast<CMonoAssembly *>(baseClassAssembly ? m_pBclAssembly : m_pManagerAssembly)->GetImage(), nameSpace, className));
}

IMonoClass *CMonoScriptSystem::InstantiateClass(const char *className, const char *nameSpace, IMonoArray *pConstructorParameters)
{
	return m_pManagerAssembly->InstantiateClass(nameSpace, className, pConstructorParameters);
}

void CMonoScriptSystem::PushFuncParamAny(const SMonoAnyValue &any)
{
	IMonoObject *pObject = NULL;

	switch(any.type)
	{
	case MONOTYPE_BOOL:
		{
			pObject = CreateMonoObject<bool>(any.b);
		}
		break;
	case MONOTYPE_INT:
		{
			pObject = CreateMonoObject<int>((int)any.number);
		}
		break;
	case MONOTYPE_UINT:
		{
			pObject = CreateMonoObject<unsigned int>((unsigned int)any.number);
		}
		break;
	case MONOTYPE_FLOAT:
		{
			pObject = CreateMonoObject<float>(any.number);
		}
		break;
	case MONOTYPE_VEC3:
		{
			pObject = CreateMonoObject<Vec3>(Vec3(any.number, any.num2, any.num3));
		}
		break;
	case MONOTYPE_ANG3:
		{
			pObject = CreateMonoObject<Ang3>(Ang3(any.number, any.num2, any.num3)); 
		}
		break;
	case MONOTYPE_STRING:
		{
			m_callParams.pArray->InsertString(any.str);

			return;
		}
		break;
	case MONOTYPE_NULL:
		{
			return;
		}
		break;
	}
	 
	m_callParams.pArray->InsertObject(pObject);
	SAFE_DELETE(pObject);
}

void CMonoScriptSystem::BeginCall(int scriptId, const char *funcName, int numArgs) 
{
	m_callParams = SMonoCallParams(scriptId, funcName, CreateMonoArray(numArgs));
}

IMonoObject *CMonoScriptSystem::EndCall()
{
	IMonoScript *pScript = GetScriptById(m_callParams.scriptId);
	IMonoObject *pResult = pScript->InvokeMethod(m_callParams.funcName, m_callParams.pArray);

	SAFE_DELETE(m_callParams.pArray);

	return pResult;
}