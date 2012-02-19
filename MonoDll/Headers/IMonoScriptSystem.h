/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScriptSystem interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 20/11/2011 : Created by Filip 'i59' Lundgren (Based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_SCRIPT_SYSTEM_H__
#define __I_MONO_SCRIPT_SYSTEM_H__

#include <MonoAnyValue.h>

#include <CryExtension/ICryUnknown.h>
#include <IEngineModule.h>

struct IMonoMethodBinding;
struct IMonoObject;
struct IMonoClass;
struct IMonoArray;

struct IMonoAssembly;

struct IMonoEntityManager;

struct IMonoConverter;

enum EMonoScriptType
{
	EMonoScriptType_NULL = -1,

	EMonoScriptType_GameRules,
	EMonoScriptType_FlowNode,
	EMonoScriptType_StaticEntity,
	EMonoScriptType_Entity,
	EMonoScriptType_Actor,
	EMonoScriptType_EditorForm,
	EMonoScriptType_Unknown,
};

/// <summary>
/// The main module in CryMono; initializes mono domain and handles calls to C# scripts.
/// </summary>
struct IMonoScriptSystem : public IEngineModule
{
	CRYINTERFACE_DECLARE(IMonoScriptSystem, 0x86169744ce38420f, 0x9768a98386be991f)

	// IEngineModule
	virtual const char *GetName() { return "CryMono"; }
	virtual const char *GetCategory() { return "CryEngine"; }

	virtual bool Initialize( SSystemGlobalEnvironment &env,const SSystemInitParams &initParams ) { return true; }
	// ~IEngineModule

	/// <summary>
	/// Initializes the Mono runtime.
	/// Called prior to CryGame initialization; resides within CGameStartup::Init in the sample project.
	/// </summary>
	virtual bool Init() = 0 ;
	/// <summary>
	/// Registers default Mono bindings and initializes CryBrary.dll. (Scripts are compiled after this is called)
	/// Called post-CryGame initialization; resides within CGameStartup::Init in the sample project.
	/// </summary>
	virtual void PostInit() = 0;

	/// <summary>
	/// Reloads CryBrary.dll and initializes script complilation.
	/// Automatically called when a script, plugin or CryBrary itself is modified.
	/// </summary>
	virtual bool Reload() = 0;

	/// <summary>
	/// Deletes script system instance; cleans up mono objects etc.
	/// Called from the dll which implements CryMono on engine shutdown (CGameStartup destructor within the sample project)
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Updates the system, once per frame.
	/// </summary>
	virtual void Update(float frameTime) = 0;

	virtual IMonoEntityManager *GetEntityManager() const = 0;
	
	/// <summary>
	/// Registers a method binding, called from IMonoScriptBind.
	/// </summary>
	/// <param name="fullMethodName">i.e. "CryEngine.GameRulesSystem::GetPlayerId"</param>
	virtual void RegisterMethodBinding(const void *method, const char *fullMethodName) = 0;

	/// <summary>
	/// Instantiates a script (with constructor parameters if supplied) of type and name
	/// This assumes that the script was present in a .dll in Plugins or within a .cs file when PostInit was called.
	/// </summary>
	virtual int InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters = nullptr) = 0;
	/// <summary>
	/// Gets the instantied script with the supplied id.
	/// </summary>
	virtual IMonoClass *GetScriptById(int id) = 0;
	/// <summary>
	/// Removes and destructs an instantiated script with the supplied id if found.
	/// </summary>
	virtual void RemoveScriptInstance(int id) = 0;

	/// <summary>
	/// Gets a pointer to the CryBrary assembly containing all default CryMono types.
	/// </summary>
	virtual IMonoAssembly *GetCryBraryAssembly() = 0;
	/// <summary>
	/// Loads an Mono assembly and returns a fully initialized IMonoAssembly.
	/// </summary>
	virtual IMonoAssembly *LoadAssembly(const char *assemblyPath) = 0;

	/// <summary>
	/// Retrieves an instance of the IMonoConverter; a class used to easily convert C# types to C++ and the other way around.
	/// </summary>
	virtual IMonoConverter *GetConverter() = 0;

	/// <summary>
	/// Entry point of the dll, used to set up CryMono.
	/// </summary>
	typedef void *(*TEntryFunction)(ISystem* pSystem);
};

typedef boost::shared_ptr<IMonoScriptSystem> IMonoPtr;

#endif //__I_MONO_SCRIPT_SYSTEM_H__