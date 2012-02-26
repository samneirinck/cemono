#include "StdAfx.h"
#include "ScriptManager.h"

#include <IMonoScriptSystem.h>

#include <IMonoClass.h>
#include <IMonoArray.h>
#include <IMonoObject.h>
#include <IMonoAssembly.h>

#include "MonoClass.h"

#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>

CScriptManager::CScriptManager()
	: m_pScriptDomain(NULL)
	, m_pScriptManager(NULL)
{
}


CScriptManager::~CScriptManager()
{
	gEnv->pConsole->RemoveCommand("mono_dump_state");

	// Force dump of instance data.
	m_AppDomainSerializer->CallMethod("DumpScriptData");

	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_finalize(m_pScriptDomain, -1);

	MonoObject *pException;
	mono_domain_try_unload(m_pScriptDomain, &pException);

	if(pException)
	{	
		CryLogAlways("[MonoWarning] An exception was raised during ScriptDomain unload:");

		MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());
		MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, NULL, NULL);
		CryLogAlways(ToCryString((mono::string)exceptionString));
	}

	m_scripts.clear();

	SAFE_RELEASE(m_pScriptManager);

	mono_gc_collect(mono_gc_max_generation());
}

bool CScriptManager::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);

	REGISTER_COMMAND("mono_dump_state", CmdDumpMonoState, VF_NULL, "");

	m_pScriptDomain = mono_domain_create_appdomain("ScriptDomain", NULL);
	mono_domain_set(m_pScriptDomain, false);

	return true;
}

void CScriptManager::CompileScripts()
{
	m_AppDomainSerializer = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("AppDomainSerializer", "CryEngine.Utils");

	CryLogAlways("    Compiling scripts...");
	m_pScriptManager = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("ScriptCompiler");
	m_pScriptManager->CallMethod("Initialize");

	CryLogAlways("    Checking for dumped script data...");
	m_AppDomainSerializer->CallMethod("TrySetScriptData");

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

	m_pScriptManager->CallMethod("PostInit");
}

void CScriptManager::PostInit(IGameObject *pGameObject)
{
}

void CScriptManager::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	m_pScriptManager->CallMethod("Update");
}

void CScriptManager::PostUpdate(float frameTime)
{
	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(frameTime);

	m_pScriptManager->CallMethod("PostUpdate", pArgs);

	SAFE_RELEASE(pArgs);
}

int CScriptManager::InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters)
{
	if(scriptType==EMonoScriptType_GameRules)
	{
		IMonoClass *pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("CryNetwork");
		IMonoArray *pArray = CreateMonoArray(3);
		pArray->Insert(gEnv->bMultiplayer);
		pArray->Insert(gEnv->IsClient());
		pArray->Insert(gEnv->bServer);
		pClass->CallMethod("InitializeNetwork", pArray, true);
		SAFE_RELEASE(pArray);
		SAFE_RELEASE(pClass);
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

	int scriptId = -1;
	if(IMonoObject *pScriptInstance = m_pScriptManager->CallMethod("InstantiateScript", pArgs))
	{
		IMonoClass *pScript = pScriptInstance->Unbox<IMonoClass *>();
		scriptId = pScript->GetScriptId();
		m_scripts.insert(TScripts::value_type(pScript, scriptId));
	}
		
	SAFE_RELEASE(pArgs);
	
	return scriptId;
}

void CScriptManager::RemoveScriptInstance(int id)
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

			m_pScriptManager->CallMethod("RemoveInstance", pArgs);
			SAFE_RELEASE(pArgs);

			m_scripts.erase(it);

			break;
		}
	}
}

IMonoClass *CScriptManager::GetScriptById(int id)
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

void CScriptManager::CmdDumpMonoState(IConsoleCmdArgs *cmdArgs)
{
	gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("AppDomainSerializer", "CryEngine.Utils")->CallMethod("DumpScriptData");
}