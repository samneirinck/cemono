/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoObject interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_OBJECT_H__
#define __I_MONO_OBJECT_H__

#include <IMonoScriptSystem.h>
#include <IMonoConverter.h>

struct IMonoClass;

namespace mono { class _object; typedef _object* object; }

/// <summary>
/// The IMonoObject class is used to wrap native mono objects of any type, and to
/// convert C++ types to the Mono equivalent.
/// </summary>
struct IMonoObject
{
public:
	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool bStatic = false) = 0;

	virtual IMonoObject *GetProperty(const char *propertyName, bool bStatic = false) = 0;
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic = false) = 0;
	virtual IMonoObject *GetField(const char *fieldName, bool bStatic = false) = 0;
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic = false) = 0;

	template <typename TResult>
	static TResult CallMethod(IMonoObject *pInvokable, const char *funcName, IMonoArray *pArgs = NULL)
	{
		if(IMonoObject *pResult = pInvokable->CallMethod(funcName, pArgs))
		{
			TResult result = pResult->Unbox<TResult>();

			SAFE_RELEASE(pResult);
			return result;
		}

		return (TResult)0;
	}

	template <>
	static void CallMethod(IMonoObject *pInvokable, const char *funcName, IMonoArray *pArgs)
	{
		pInvokable->CallMethod(funcName, pArgs);
	}

	/// <summary>
	/// Deletes the object. Warning: Also deleted in C#!
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Gets the unboxed object and casts it to the requested type. (class T)
	/// </summary>
	template <class T>
	T Unbox() { return *(T *)UnboxObject(); }

	/// <summary>
	/// Gets the type of this Mono object.
	/// </summary>
	virtual EMonoAnyType GetType() = 0;
	
	/// <summary>
	/// Returns the object as it is seen in managed code, can be passed directly across languages.
	/// </summary>
	virtual mono::object GetManagedObject() = 0;

	virtual IMonoClass *GetClass() = 0;

private:
	/// <summary>
	/// Unboxes the object and returns it as a void pointer. (Use Unbox() method to easily cast to the C++ type)
	/// </summary>
	virtual void *UnboxObject() = 0;
};

#endif //__I_MONO_OBJECT_H__