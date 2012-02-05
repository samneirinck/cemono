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
#include "EntityManager.h"

#include <IMonoScriptSystem.h>

#include <MonoCommon.h>

#include <CryExtension/Impl/ClassWeaver.h>

#include <IFileChangeMonitor.h>

struct IMonoClass;
struct IMonoMethodBinding;

class CMonoTester;
class CMonoScript;

class CFlowManager;
class CMonoCallbackHandler;

struct InternalCallMethod
{
	InternalCallMethod(mono::string Name, mono::string RetType, mono::string params)
		: name(Name), returnType(RetType), parameters(params) {}

	mono::string name;
	mono::string returnType;

	mono::string parameters;
};

class CMonoScriptSystem : public IMonoScriptSystem, public IFileChangeListener
{
	CRYINTERFACE_SIMPLE(IMonoScriptSystem);
	
	CRYGENERATE_SINGLETONCLASS(CMonoScriptSystem, "CryMono", 0xc37b8ad5d62f47de, 0xa8debe525ff0fc8a)

	typedef std::vector<IMonoClass *> TScripts;
	typedef std::map<string, IMonoMethodBinding> TMethodBindings;

public:
	// IMonoScriptSystem
	virtual bool Init() override;
	virtual void PostInit() override;
	virtual void Release() override { delete this; }

	virtual void Update(float frameTime) override;

	virtual IMonoEntityManager *GetEntityManager() const { return m_pEntityManager; }

	virtual void RegisterMethodBinding(IMonoMethodBinding binding, const char *classPath) override;
	virtual void RegisterMethodBindings(std::vector<IMonoMethodBinding> methodBindings, const char *classPath) override;
	
	virtual int InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters = nullptr) override;
	virtual IMonoClass *GetScriptById(int id) override;
	virtual void RemoveScriptInstance(int id) override;
	
	virtual IMonoAssembly *GetCryBraryAssembly() override { return m_pLibraryAssembly; }
	virtual IMonoAssembly *LoadAssembly(const char *assemblyPath) override;

	virtual IMonoConverter *GetConverter() override { return m_pConverter; }
	// ~IMonoScriptSystem

	// IFileChangeMonitor
	virtual void OnFileChange(const char* sFilename);
	// ~IFileChangeMonitor

	IMonoClass *GetScriptCompilerClass() const { return m_pScriptCompiler; }

	CMonoCallbackHandler *GetCallbackHandler() const { return m_pCallbackHandler; }
	CFlowManager *GetFlowManager() const { return m_pFlowManager; }

	bool IsInitialized() { return m_pMonoDomain != NULL; }

private:
	bool InitializeDomain();
	void RegisterDefaultBindings();
	bool InitializeSystems();

	MonoDomain *m_pMonoDomain;
	IMonoAssembly *m_pLibraryAssembly;

	IMonoClass *m_pCryConsole;
	IMonoClass *m_pScriptCompiler;

	IMonoConverter *m_pConverter;

	CEntityManager *m_pEntityManager;
	CFlowManager *m_pFlowManager;
	CMonoCallbackHandler *m_pCallbackHandler;
	CMonoTester *m_pTester;

	TScripts m_scripts;
	TMethodBindings m_methodBindings;

	// ScriptBinds declared in this project.
	std::vector<IMonoScriptBind *> m_localScriptBinds;
};

#endif //__MONO_H__