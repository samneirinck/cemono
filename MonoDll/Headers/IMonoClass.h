/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoClass interface for external projects, i.e. CryGame.
// Represents a managed type.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_CLASS_H__
#define __I_MONO_CLASS_H__

#include <IMonoObject.h>

struct IMonoAssembly;
struct IMonoArray;

/// <summary>
/// Reference to a Mono class, used to call static methods and etc.
/// </summary>
/// <example>
/// IMonoScript *pCryNetwork = g_pScriptSystem->GetCustomClass("CryNetwork", "CryEngine");
/// </example>
struct IMonoClass
	: public IMonoObject
{
public:
	virtual void AddRef() = 0;

	/// <summary>
	/// Gets the class name.
	/// </summary>
	virtual const char *GetName() = 0;

	/// <summary>
	/// Gets the namespace the class resides in.
	/// </summary>
	virtual const char *GetNamespace() = 0;

	/// <summary>
	/// Gets the assembly that declares this type.
	/// </summary>
	virtual IMonoAssembly *GetAssembly() = 0;

	/// <summary>
	/// Instantiates the class, if not already instantiated.
	/// </summary>
	virtual IMonoObject *CreateInstance(IMonoArray *pConstructorParams = nullptr) = 0;

	/// <summary>
	/// Boxes a type to be received as a managed type.
	/// </summary>
	virtual IMonoObject *BoxObject(void *object, IMonoDomain *pDomain = nullptr) = 0;

	/// <summary>
	/// TODO
	/// </summary>
	virtual IMonoObject *InvokeArray(IMonoObject *pObject, const char *methodName, IMonoArray *params = nullptr) = 0;
	/// <summary>
	/// TODO
	/// </summary>
	virtual IMonoObject *Invoke(IMonoObject *pObject, const char *methodName, void **params = nullptr, int numParams = 0) = 0;

	/// <summary>
	/// Gets a managed function as a native function pointer. See example for use case.
	/// Much faster than standard invoke!
	/// </summary>
	/// <example>
	/// typedef int (*GetHashCode) (mono::object obj);
	///
	/// GetHashCode func = pObjectClass->GetMethodThunk("GetHashCode", 0);
	/// int hashCode = func(myObject);
	/// </example>
	virtual void *GetMethodThunk(const char *methodName, int numParams) = 0;

	/// <summary>
	/// TODO
	/// </summary>
	virtual IMonoObject *GetPropertyValue(IMonoObject *pObject, const char *propertyName) = 0;
	/// <summary>
	/// TODO
	/// </summary>
	virtual void SetPropertyValue(IMonoObject *pObject, const char *propertyName, mono::object newValue) = 0;
	/// <summary>
	/// TODO
	/// </summary>
	virtual IMonoObject *GetFieldValue(IMonoObject *pObject, const char *fieldName) = 0;
	/// <summary>
	/// TODO
	/// </summary>
	virtual void SetFieldValue(IMonoObject *pObject, const char *fieldName, mono::object newValue) = 0;
};

#endif //__I_MONO_CLASS_H__