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

struct IMonoScriptManager;

struct IMonoMethodBinding;

struct IMonoObject;
struct IMonoClass;
struct IMonoArray;
struct IMonoAssembly;

struct IMonoEntityManager;

struct IMonoConverter;

/// <summary>
/// The main module in CryMono; initializes mono domain and handles calls to C# scripts.
/// </summary>
struct IMonoScriptSystem : ICryUnknown
{
	CRYINTERFACE_DECLARE(IMonoScriptSystem, 0x86169744ce38420f, 0x9768a98386be991f)

	/// <summary>
	/// Reloads CryBrary.dll and initializes script complilation.
	/// Automatically called when a script, plugin or CryBrary itself is modified.
	/// </summary>
	virtual bool Reload(bool initialLoad = false) = 0;

	/// <summary>
	/// Deletes script system instance; cleans up mono objects etc.
	/// Called from the dll which implements CryMono on engine shutdown (CGameStartup destructor within the sample project)
	/// </summary>
	virtual void Release() = 0;

	virtual IMonoScriptManager *GetScriptManager() = 0;

	virtual IMonoEntityManager *GetEntityManager() const = 0;
	
	/// <summary>
	/// Registers a method binding, called from IMonoScriptBind.
	/// </summary>
	/// <param name="fullMethodName">i.e. "CryEngine.GameRulesSystem::GetPlayerId"</param>
	virtual void RegisterMethodBinding(const void *method, const char *fullMethodName) = 0;

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