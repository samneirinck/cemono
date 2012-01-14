/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScript interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 02/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __IMONOSCRIPT_H__
#define __IMONOSCRIPT_H__

enum EMonoScriptType;

struct IMonoArray;
struct IMonoObject;

typedef int MonoScriptId;

/// <summary>
/// Created for each Mono class instance (Entities, FlowNodes & GameRules) and can be used to invoke methods
/// This can be retrieved using IMonoScriptSystem::GetScriptById.
/// To invoke script methods, utilize the global CallMonoScript method.
/// </summary>
struct IMonoScript
{
	/// <summary>
	/// Retrieves this script's identifier.
	/// </summary>
	virtual MonoScriptId GetId() = 0;
	/// <summary>
	/// Retrieves the name of this script class.
	/// </summary>
	virtual const char *GetName() = 0;
	/// <summary>
	/// Returns the script type.
	/// </summary>
	virtual EMonoScriptType GetType() = 0;

	/// <summary>
	/// Invokes a method on this script instance, with the specified args if provided.
	/// </summary>
	virtual IMonoObject *InvokeMethod(const char *func, IMonoArray *pArgs = NULL) = 0;
	template <typename T>
	T InvokeMethod(const char *func, IMonoArray *pArgs = NULL)
	{
		if(IMonoObject *pObject = InvokeMethod(func, pArgs))
			return pObject->Unbox<T>();

		return (T)0;
	}
};

#endif //__IMONOSCRIPT_H__