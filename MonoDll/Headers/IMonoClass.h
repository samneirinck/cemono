/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoClass interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_CLASS__
#define __I_MONO_CLASS__

struct IMonoObject;
struct IMonoArray;

/// <summary>
/// Reference to a Mono class, used to call static methods and etc.
/// </summary>
/// <example>
/// IMonoClass *pCryNetwork = gEnv->pMonoScriptSystem->GetCustomClass("CryNetwork", "CryEngine");
/// </example>
struct IMonoClass
{
public:
	/// <summary>
	/// Invokes a method on the specified class.
	/// </summary>
	/// <example>
	/// CallMethod("InitializeNetwork", pParameters, true");
	/// </example>
	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool _static = false) = 0;
};

#endif //__I_MONO_CLASS__