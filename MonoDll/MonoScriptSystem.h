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

#include <MonoCommon.h>

#include <CryExtension/Impl/ClassWeaver.h>
#include <IFileChangeMonitor.h>
#include <IGameFramework.h>

struct IMonoScriptBind;

struct IMonoScriptManager;
struct IMonoEntityManager;

struct SCVars;

class CFlowManager;
class CInput;

class CScriptSystem
	: public IMonoScriptSystem
	, public IFileChangeListener
	, public IGameFrameworkListener
	, public ISystemEventListener
{
	// CryExtension
	CRYINTERFACE_SIMPLE(IMonoScriptSystem);

	CRYGENERATE_SINGLETONCLASS(CScriptSystem, "CryMono", 0xc37b8ad5d62f47de, 0xa8debe525ff0fc8a)
	// ~CryExtension

	typedef std::map<const void *, const char *> TMethodBindings;
	typedef std::map<IMonoClass *, int> TScripts;
	typedef std::vector<IMonoScriptSystemListener *> TScriptCompilationListeners;

public:
	// IMonoScriptSystem
	virtual bool Reload(bool initialLoad = false) override;
	virtual bool IsReloading() override { return m_bReloading; }

	virtual void Release() override { delete this; }

	virtual void RegisterMethodBinding(const void *method, const char *fullMethodName) override;

	virtual IMonoClass *InstantiateScript(const char *scriptName, EMonoScriptType scriptType = eScriptType_Unknown, IMonoArray *pConstructorParameters = nullptr) override;
	virtual void RemoveScriptInstance(int id, EMonoScriptType scriptType = eScriptType_Unknown) override;
	
	virtual IMonoAssembly *GetCryBraryAssembly() override { return m_pCryBraryAssembly; }

	virtual IMonoDomain *GetRootDomain() override { return m_pRootDomain; }

	virtual IMonoConverter *GetConverter() override { return m_pConverter; }

	virtual void RegisterListener(IMonoScriptSystemListener *pListener) override { stl::push_back_unique(m_scriptReloadListeners, pListener); }
	virtual void UnregisterListener(IMonoScriptSystemListener *pListener) override { stl::find_and_erase(m_scriptReloadListeners, pListener); }
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

	IMonoAssembly *GetDebugDatabaseCreator() { return m_pPdb2MdbAssembly; }

	CFlowManager *GetFlowManager() const { return m_pFlowManager; }

	bool IsInitialized() { return m_pRootDomain != NULL; }

protected:
	bool CompleteInit();
	bool InitializeSystems(IMonoAssembly *pCryBraryAssembly);

	void PreReload();
	bool DoReload(bool initialLoad);

	void RegisterDefaultBindings();

	// The primary app domain, not really used for anything besides holding the script domain. Do *not* unload this at runtime, we cannot execute another root domain again without restarting.
	IMonoDomain *m_pRootDomain;

	// The app domain in which we load scripts into. Killed and reloaded on script reload.
	IMonoDomain *m_pScriptDomain;

	IMonoClass *m_pScriptManager;
	// Hard pointer to the AppDomainSerializer class to quickly dump and restore scripts.
	IMonoClass *m_AppDomainSerializer;

	// Map containing all scripts and their id's for quick access.
	TScripts m_scripts;

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

	TScriptCompilationListeners m_scriptReloadListeners;

	// If true, the last script reload was successful. This is necessary to make sure we don't override with invalid script dumps.
	bool m_bLastCompilationSuccess;
	// True when currently recompiling scripts / serializing app domain.
	bool m_bReloading;
};

#endif //__MONO_H__