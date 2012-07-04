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

namespace mono { class _object; typedef _object* object; }

/// <summary>
/// Reference to a Mono assembly; used to for example instantiate classes contained within a C# dll.
/// </summary>
struct IMonoAssembly
{
public:
	/// <summary>
	/// Gets a custom C# class from within the assembly.
	/// Note: This does not construct an new instance of the class, only returns an uninitialized IMonoScript. To instantiate a class, see IMonoAssembly::InstantiateClass
	/// </summary>
	/// <example>
	/// IMonoScript *pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Vec3");
	/// </example>
	virtual IMonoClass *GetClass(const char *className, const char *nameSpace = "CryEngine") = 0;

	/// <summary>
	/// Gets the name of this assembly.
	/// </summary>
	virtual const char *GetName() = 0;

	/// <summary>
	/// Gets the full path to this assembly on disk.
	/// </summary>
	virtual const char *GetPath() = 0;

	/// <summary>
	/// Determines if this assembly was loaded from C++.
	/// </summary>
	virtual bool IsNative() = 0;

	virtual mono::object GetManagedObject() = 0;
};

#endif //__I_MONO_ASSEMBLY__`	