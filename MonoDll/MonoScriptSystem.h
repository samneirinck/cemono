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

#include "MonoArray.h"

#include <IMonoScriptSystem.h>

#include <MonoCommon.h>

#include <CryExtension/Impl/ClassWeaver.h>
#include <IFileChangeMonitor.h>
#include <IGameFramework.h>

struct IMonoMethodBinding;
struct IMonoScriptBind;

struct IMonoScriptManager;
struct IMonoEntityManager;

class CTester;
class CFlowManager;
class CCallbackHandler;

class CScriptSystem
	: public IMonoScriptSystem
	, public IFileChangeListener
{
	CRYINTERFACE_SIMPLE(IMonoScriptSystem);

	CRYGENERATE_SINGLETONCLASS(CScriptSystem, "CryMono", 0xc37b8ad5d62f47de, 0xa8debe525ff0fc8a)

	typedef std::map<const void *, const char *> TMethodBindings;
	typedef std::map<IMonoClass *, int> TScripts;

public:
	// IMonoScriptSystem
	virtual bool Reload(bool initialLoad = false) override;

	virtual void Release() override { delete this; }

	virtual IMonoEntityManager *GetEntityManager() const override { return m_pEntityManager; }

	virtual void RegisterMethodBinding(const void *method, const char *fullMethodName) override;

	virtual int InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters = nullptr) override;  	
	virtual IMonoClass *GetScriptById(int id) override;
	virtual void RemoveScriptInstance(int id) override;
	
	virtual IMonoAssembly *GetCryBraryAssembly() override { return m_pCryBraryAssembly; }
	virtual IMonoAssembly *LoadAssembly(const char *assemblyPath) override;

	virtual IMonoConverter *GetConverter() override { return m_pConverter; }
	// ~IMonoScriptSystem

	// IFileChangeMonitor
	virtual void OnFileChange(const char* sFilename);
	// ~IFileChangeMonitor

	IMonoAssembly *GetDebugDatabaseCreator() { return m_pPdb2MdbAssembly; }

	CCallbackHandler *GetCallbackHandler() const { return m_pCallbackHandler; }
	CFlowManager *GetFlowManager() const { return m_pFlowManager; }

	bool IsInitialized() { return m_pMonoDomain != NULL; }

protected:
	bool CompleteInit();
	bool InitializeDomain();
	void RegisterDefaultBindings();
	bool InitializeSystems();

	MonoDomain *m_pMonoDomain;

	// The app domain in which we load scripts into. Killed and reloaded on script reload.
	MonoDomain *m_pScriptDomain;

	IMonoClass *m_pScriptManager;
	IMonoClass *m_AppDomainSerializer;

	TScripts m_scripts;

	IMonoEntityManager *m_pEntityManager;

	IMonoConverter *m_pConverter;

	CFlowManager *m_pFlowManager;
	CCallbackHandler *m_pCallbackHandler;
	CTester *m_pTester;

	IMonoAssembly *m_pCryBraryAssembly;
	IMonoAssembly *m_pPdb2MdbAssembly;

	TMethodBindings m_methodBindings;

	// ScriptBinds declared in this project.
	std::vector<IMonoScriptBind *> m_localScriptBinds;
};

#endif //__MONO_H__