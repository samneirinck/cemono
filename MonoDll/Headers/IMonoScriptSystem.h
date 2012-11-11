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

struct IMonoScriptManager;

struct IMonoObject;
struct IMonoArray;
struct IMonoAssembly;
struct IMonoDomain;

struct IMonoEntityManager;
struct IMonoConverter;
struct IMonoScriptEventListener;

/// <summary>
/// Script flags are passed to IMonoScriptSystem::InstantiateScript and RemoveScriptInstance as a way to identify scripts more effectively, and to solve the issue with scripts being of multiple types.
/// </summary>
enum EMonoScriptFlags
{
	/// <summary>
	/// Scripts not inheriting from CryScriptInstance will utilize this script type.
	/// </summary>
	eScriptFlag_Any = 1,
	/// <summary>
	/// Scripts inheriting from CryScriptInstance, but no other CryMono base script will be linked to this script type.
	/// </summary>
	eScriptFlag_CryScriptInstance = 2,
	/// <summary>
	/// Scripts directly inheriting from BaseGameRules will utilize this script type.
	/// </summary>
	eScriptFlag_GameRules = 4,
	/// <summary>
	/// Scripts directly inheriting from FlowNode will utilize this script type.
	/// </summary>
	eScriptFlag_FlowNode = 8,
	/// <summary>
	/// Scripts directly inheriting from Entity will utilize this script type.
	/// </summary>
	eScriptFlag_Entity = 16,
	/// <summary>
	/// Scripts directly inheriting from Actor will utilize this script type.
	/// </summary>
	eScriptFlag_Actor = 32,
	/// <summary>
	/// </summary>
	eScriptFlag_UIEventSystem = 64,
	eScriptFlag_ScriptCompiler = 128,
};

/// <summary>
/// The main module in CryMono; initializes mono domain and handles calls to C# scripts.
/// </summary>
struct IMonoScriptSystem
{
	/// <summary>
	/// Returns true when the root domain has been initialized.
	/// </summary>
	virtual bool IsInitialized() = 0;

	/// <summary>
	/// Used to start script recompilation / serialization.
	/// </summary>
	virtual void Reload() = 0;

	virtual void AddListener(IMonoScriptEventListener *pListener) = 0;
	virtual void RemoveListener(IMonoScriptEventListener *pListener) = 0;

	/// <summary>
	/// Deletes script system instance; cleans up mono objects etc.
	/// Called from the dll which implements CryMono on engine shutdown (CGameStartup destructor within the sample project)
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Registers a method binding, called from IMonoScriptBind.
	/// </summary>
	/// <param name="fullMethodName">i.e. "CryEngine.GameRulesSystem::GetPlayerId"</param>
	virtual void RegisterMethodBinding(const void *method, const char *fullMethodName) = 0;

	/// <summary>
	/// Instantiates a script (with constructor parameters if supplied) of type and name
	/// This assumes that the script was present in a .dll in Plugins or within a .cs file when PostInit was called.
	/// </summary>
	virtual IMonoObject *InstantiateScript(const char *scriptName, EMonoScriptFlags scriptType = eScriptFlag_Any, IMonoArray *pConstructorParameters = nullptr, bool throwOnFail = true) = 0;
	/// <summary>
	/// Removes and destructs an instantiated script with the supplied id if found.
	/// </summary>
	virtual void RemoveScriptInstance(int id, EMonoScriptFlags scriptType = eScriptFlag_Any) = 0;

	virtual IMonoObject *GetScriptManager() = 0;

	/// <summary>
	/// Gets a pointer to the CryBrary assembly containing all default managed CryMono types.
	/// </summary>
	virtual IMonoAssembly *GetCryBraryAssembly() = 0;

	/// <summary>
	/// Gets the core assembly, containing the System namespace etc.
	/// </summary>
	virtual IMonoAssembly *GetCorlibAssembly() = 0;

	/// <summary>
	/// Gets the root domain created on script system initialization.
	/// </summary>
	virtual IMonoDomain *GetRootDomain() = 0;

	/// <summary>
	/// Creates a new app domain.
	/// </summary>
	virtual IMonoDomain *CreateDomain(const char *name, bool setActive = false) = 0;

	/// <summary>
	/// Retrieves an instance of the IMonoConverter; a class used to easily convert C# types to C++ and the other way around.
	/// </summary>
	virtual IMonoConverter *GetConverter() = 0;

	/// <summary>
	/// If called prior to default CryMono flownode registration time (IGameFramework PostInit); flownodes are immediately registered.
	/// </summary>
	virtual void RegisterFlownodes() = 0;
};

struct IMonoScriptEventListener
{
	virtual void OnReloadStart() = 0;
	virtual void OnReloadComplete() = 0;
};

#endif //__I_MONO_SCRIPT_SYSTEM_H__