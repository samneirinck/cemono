/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Main module; handles all sub-systems within the CryMono dll.
//////////////////////////////////////////////////////////////////////////
// 20/11/2011 : Created by Filip 'i59' Lundgren (Based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_H__
#define __MONO_H__

#include <IMonoScriptSystem.h>
#include <IMonoDomain.h>
#include <IMonoScriptbind.h>

#include <MonoCommon.h>

#include <IFileChangeMonitor.h>
#include <IGameFramework.h>

struct IMonoScriptManager;
struct IMonoEntityManager;

struct SCVars;

class CFlowManager;
class CInput;

class CScriptDomain;
class CScriptObject;

class CScriptSystem
	: public IMonoScriptSystem
	, public IFileChangeListener
	, public IGameFrameworkListener
	, public ISystemEventListener
	, public IMonoScriptBind
{
	typedef std::map<const void *, const char *> TMethodBindings;
	typedef std::map<IMonoObject *, int> TScripts;

public:
	CScriptSystem();
	~CScriptSystem();

	// IMonoScriptSystem
	virtual bool IsInitialized() override { return m_pRootDomain != nullptr; }

	virtual void Release() override { delete this; }

	virtual void RegisterMethodBinding(const void *method, const char *fullMethodName) override;

	virtual IMonoObject *InstantiateScript(const char *scriptName, EMonoScriptFlags scriptType = eScriptFlag_Any, IMonoArray *pConstructorParameters = nullptr, bool throwOnFail = true) override;
	virtual void RemoveScriptInstance(int id, EMonoScriptFlags scriptType = eScriptFlag_Any) override;
	
	virtual IMonoObject *GetScriptManager() { return m_pScriptManager; }

	virtual IMonoAssembly *GetCryBraryAssembly() override { return m_pCryBraryAssembly; }
	virtual IMonoAssembly *GetCorlibAssembly() override;

	virtual IMonoDomain *GetRootDomain() override { return (IMonoDomain *)m_pRootDomain; }
	virtual IMonoDomain *CreateDomain(const char *name, bool setActive = false);

	virtual IMonoConverter *GetConverter() override { return m_pConverter; }

	virtual void RegisterFlownodes() { OnSystemEvent(ESYSTEM_EVENT_GAME_POST_INIT, 0, 0); }
	// ~IMonoScriptSystem

	// IFileChangeMonitor
	virtual void OnFileChange(const char *fileName);
	// ~IFileChangeMonitor

	// ~IGameFrameworkListener	  	
	virtual void OnPostUpdate(float fDeltaTime);
	virtual void OnSaveGame(ISaveGame* pSaveGame) {} 	
	virtual void OnLoadGame(ILoadGame* pLoadGame) {}  	
	virtual void OnLevelEnd(const char* nextLevel) {}  	
	virtual void OnActionEvent(const SActionEvent& event) {} 	
	// ~IGameFrameworkListener

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam);
	// ~ISystemEventListener

	CScriptDomain *TryGetDomain(MonoDomain *pAssembly);

	IMonoAssembly *GetDebugDatabaseCreator() { return m_pPdb2MdbAssembly; }

	CFlowManager *GetFlowManager() const { return m_pFlowManager; }

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "NativeScriptSystemMethods"; }
	// ~IMonoScriptBind

	// Externals
	static void UpdateScriptInstance(CScriptObject *pObject, mono::object newInstance);
	// ~Externals

	bool CompleteInit();

	void RegisterDefaultBindings();

	// The primary app domain, not really used for anything besides holding the script domain. Do *not* unload this at runtime, we cannot execute another root domain again without restarting.
	CScriptDomain *m_pRootDomain;
	std::vector<CScriptDomain *> m_domains;

	IMonoObject *m_pScriptManager;

	CFlowManager *m_pFlowManager;
	CInput *m_pInput;

	IMonoConverter *m_pConverter;

	IMonoAssembly *m_pCryBraryAssembly;
	IMonoAssembly *m_pPdb2MdbAssembly;

	SCVars *m_pCVars;

	// We temporarily store scriptbind methods here if developers attempt to register them prior to the script system has been initialized properly.
	TMethodBindings m_methodBindings;

	// ScriptBinds declared in this project are stored here to make sure they are destructed on shutdown.
	std::vector<std::shared_ptr<IMonoScriptBind>> m_localScriptBinds;
};

#endif //__MONO_H__