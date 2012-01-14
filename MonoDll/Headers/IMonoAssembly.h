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
	/// Instantiates a class within the assembly.
	/// </summary>
	/// <example>
	/// IMonoClass *pClass = InstantiateClass("CryEngine", "MyClass");
	/// </example>
	virtual IMonoClass *InstantiateClass(const char *nameSpace, const char *className, IMonoArray *pConstructorArguments = NULL) = 0;
};

#endif //__I_MONO_ASSEMBLY__`	