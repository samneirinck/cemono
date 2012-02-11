/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoAssembly interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_ASSEMBLY__
#define __I_MONO_ASSEMBLY__

struct IMonoClass;
struct IMonoArray;

/// <summary>
/// Reference to a Mono assembly; used to for example instantiate classes contained within a C# dll.
/// </summary>
struct IMonoAssembly
{
public:
	/// <summary>
	/// Deletes the assembly and cleans up used objects.
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Instantiates a class within the assembly.
	/// </summary>
	/// <example>
	/// IMonoClass *pClass = pAssembly->InstantiateClass("CryEngine", "MyClass");
	/// </example>
	virtual IMonoClass *InstantiateClass(const char *nameSpace, const char *className, IMonoArray *pConstructorArguments = NULL) = 0;
	/// <summary>
	/// Gets a custom C# class from within the assembly.
	/// Note: This does not construct an new instance of the class, only returns an uninitialized IMonoClass. To instantiate a class, see IMonoAssembly::InstantiateClass
	/// </summary>
	/// <example>
	/// IMonoClass *pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Vec3");
	/// </example>
	virtual IMonoClass *GetCustomClass(const char *className, const char *nameSpace = "CryEngine") = 0;
};

#endif //__I_MONO_ASSEMBLY__`	