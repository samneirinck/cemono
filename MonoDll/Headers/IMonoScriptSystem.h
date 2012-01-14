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

#include <IMonoArray.h>

#include <CryExtension/ICryUnknown.h>

struct IMonoScriptBind;
struct IMonoScript;
struct IMonoObject;
struct IMonoArray;

struct IMonoAssembly;
struct IMonoClass;

struct IMonoEntityManager;

struct IMonoConverter;

enum EMonoScriptType
{
	EMonoScriptType_NULL = 0,

	EMonoScriptType_GameRules,
	EMonoScriptType_FlowNode,
	EMonoScriptType_Entity,
	EMonoScriptType_Actor
};

/// <summary>
/// The main module in CryMono; initializes mono domain and handles calls to C# scripts.
/// </summary>
struct IMonoScriptSystem : public ICryUnknown
{
	CRYINTERFACE_DECLARE(IMonoScriptSystem, 0x86169744ce38420f, 0x9768a98386be991f)

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
	/// Deletes script system instance; cleans up mono objects etc.
	/// Called from the dll which implements CryMono on engine shutdown (CGameStartup destructor within the sample project)
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Updates the system, once per frame.
	/// </summary>
	virtual void Update() = 0;

	virtual IMonoEntityManager *GetEntityManager() const = 0;
	
	/// <summary>
	/// Registers a Mono scriptbind which inherits from IMonoScriptBind, and the methods contained within.
	/// Note that binded methods also have to be declared as externals within your C# assembly.
	/// </summary>
	virtual void RegisterScriptBind(IMonoScriptBind *pScriptBind) = 0;

	/// <summary>
	/// Instantiates a script (with constructor parameters if supplied) of type and name
	/// This assumes that the script was present in a .dll in Plugins or within a .cs file when PostInit was called.
	/// </summary>
	virtual int InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters = nullptr) = 0;
	/// <summary>
	/// Gets the instantied script with the supplied id.
	/// </summary>
	virtual IMonoScript *GetScriptById(int id) = 0;
	/// <summary>
	/// Removes and destructs an instantiated script with the supplied id if found.
	/// </summary>
	virtual void RemoveScriptInstance(int id) = 0;

	/// <summary>
	/// Loads an Mono assembly and returns a fully initialized IMonoAssembly.
	/// </summary>
	virtual IMonoAssembly *LoadAssembly(const char *assemblyPath) = 0;

	/// <summary>
	/// Invokes a script method with the supplied args.
	/// </summary>
	virtual IMonoObject *CallScript(int scriptId, const char *funcName, IMonoArray *pArgs = NULL) = 0;
	template <typename T> T CallScript(int scriptId, const char *funcName, IMonoArray *pArgs = NULL)
	{
		if(IMonoObject *pObject = CallScript(scriptId, funcName, pArgs))
			return pObject->Unbox<T>();

		return default(T);
	}

	/// <summary>
	/// Gets a custom C# class from within the specified assembly.
	/// Uses CryBrary if assembly is NULL.
	/// </summary>
	/// <example>
	/// gEnv->pMonoScriptSystem->GetCustomClass("MyClass")->CallMethod("Initialize");
	/// </example>
	virtual IMonoClass *GetCustomClass(const char *className, const char *nameSpace = "CryEngine", IMonoAssembly *pAssembly = NULL) = 0;
	/// <summary>
	/// Instantiates a class created in C#.
	/// </summary>
	virtual IMonoClass *InstantiateClass(const char *className, const char *nameSpace = "CryEngine", IMonoArray *pConstructorParameters = NULL) = 0;

	/// <summary>
	/// Retrieves an instance of the IMonoConverter; a class used to easily convert C# types to C++ and the other way around.
	/// </summary>
	virtual IMonoConverter *GetConverter() = 0;

	/// <summary>
	/// Entry point of the dll, used to set up CryMono.
	/// </summary>
	typedef void *(*TEntryFunction)(ISystem* pSystem);
};


static IMonoObject *CallMonoScript(int scriptId, const char *funcName)
{
	if(scriptId==-1)
		return NULL;

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, NULL);
};
template<typename P1> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1)
{
	if(scriptId==-1)
		return NULL;

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(1);
	pArray->Insert(p1);

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, pArray);
};
template<typename P1, typename P2> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2)
{
	if(scriptId==-1)
		return NULL;

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(2);
	pArray->Insert(p1);
	pArray->Insert(p2);

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, pArray);
};
template<typename P1, typename P2, typename P3> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3)
{
	if(scriptId==-1)
		return NULL;

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(3);
	pArray->Insert(p1);
	pArray->Insert(p2);
	pArray->Insert(p3);

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, pArray);
};
template<typename P1, typename P2, typename P3, typename P4> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
{
	if(scriptId==-1)
		return NULL;

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(4);
	pArray->Insert(p1);
	pArray->Insert(p2);
	pArray->Insert(p3);
	pArray->Insert(p4);

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, pArray);
};
template<typename P1, typename P2, typename P3, typename P4, typename P5> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
{
	if(scriptId==-1)
		return NULL;

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(5);
	pArray->Insert(p1);
	pArray->Insert(p2);
	pArray->Insert(p3);
	pArray->Insert(p4);
	pArray->Insert(p5);

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, pArray);
};
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
{
	if(scriptId==-1)
		return NULL;

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(6);
	pArray->Insert(p1);
	pArray->Insert(p2);
	pArray->Insert(p3);
	pArray->Insert(p4);
	pArray->Insert(p5);
	pArray->Insert(p6);

	return gEnv->pMonoScriptSystem->CallScript(scriptId, funcName, pArray);
};

typedef boost::shared_ptr<IMonoScriptSystem> IMonoPtr;

#endif //__I_MONO_SCRIPT_SYSTEM_H__