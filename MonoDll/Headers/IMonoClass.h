/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScript interface for external projects, i.e. CryGame.
// Represents a managed type.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_SCRIPT_H__
#define __I_MONO_SCRIPT_H__

#include <IMonoScriptSystem.h>
#include <IMonoArray.h>

struct IMonoObject;

/// <summary>
/// Reference to a Mono class, used to call static methods and etc.
/// </summary>
/// <example>
/// IMonoScript *pCryNetwork = gEnv->pMonoScriptSystem->GetCustomClass("CryNetwork", "CryEngine");
/// </example>
struct IMonoClass
	: public IMonoObject
{
public:
	/// <summary>
	/// Gets the class name.
	/// </summary>
	virtual const char *GetName() = 0;

	/// <summary>
	/// Gets the namespace the class resides in.
	/// </summary>
	virtual const char *GetNamespace() = 0;

	/// <summary>
	/// Instantiates the class, if not already instantiated.
	/// </summary>
	virtual IMonoObject *CreateInstance(IMonoArray *pConstructorParams = NULL) = 0;
};

#endif //__I_MONO_SCRIPT_H__