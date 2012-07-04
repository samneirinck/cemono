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
	virtual IMonoObject *CallMethodWithArray(const char *methodName, IMonoArray *params = nullptr, bool bStatic = false) = 0;

	virtual IMonoObject *GetProperty(const char *propertyName, bool bStatic = false) = 0;
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic = false) = 0;
	virtual IMonoObject *GetField(const char *fieldName, bool bStatic = false) = 0;
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic = false) = 0;

	inline IMonoObject *CallMethod(const char *funcName)
	{
		return CallMethodWithArray(funcName);
	}

	template<typename P1> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1)
	{
		IMonoArray *pArgs = CreateMonoArray(1);
		pArgs->Insert(p1);

		IMonoObject *pResult = CallMethodWithArray(funcName, pArgs);
		SAFE_RELEASE(pArgs);
		return pResult;
	};

	template<typename P1, typename P2> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2)
	{
		IMonoArray *pArgs = CreateMonoArray(2);
		pArgs->Insert(p1);
		pArgs->Insert(p2);

		IMonoObject *pResult = CallMethodWithArray(funcName, pArgs);
		SAFE_RELEASE(pArgs);
		return pResult;
	};

	template<typename P1, typename P2, typename P3> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3)
	{
		IMonoArray *pArgs = CreateMonoArray(3);
		pArgs->Insert(p1);
		pArgs->Insert(p2);
		pArgs->Insert(p3);
	
		IMonoObject *pResult = CallMethodWithArray(funcName, pArgs);
		SAFE_RELEASE(pArgs);
		return pResult;
	};

	template<typename P1, typename P2, typename P3, typename P4> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
	{
		IMonoArray *pArgs = CreateMonoArray(4);
		pArgs->Insert(p1);
		pArgs->Insert(p2);
		pArgs->Insert(p3);
		pArgs->Insert(p4);
	
		IMonoObject *pResult = CallMethodWithArray(funcName, pArgs);
		SAFE_RELEASE(pArgs);
		return pResult;
	};

	template<typename P1, typename P2, typename P3, typename P4, typename P5> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
	{
		IMonoArray *pArgs = CreateMonoArray(5);
		pArgs->Insert(p1);
		pArgs->Insert(p2);
		pArgs->Insert(p3);
		pArgs->Insert(p4);
		pArgs->Insert(p5);
	
		IMonoObject *pResult = CallMethodWithArray(funcName, pArgs);
		SAFE_RELEASE(pArgs);
		return pResult;
	};

	template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
	{
		IMonoArray *pArgs = CreateMonoArray(6);
		pArgs->Insert(p1);
		pArgs->Insert(p2);
		pArgs->Insert(p3);
		pArgs->Insert(p4);
		pArgs->Insert(p5);
		pArgs->Insert(p6);
	
		IMonoObject *pResult = CallMethodWithArray(funcName, pArgs);
		SAFE_RELEASE(pArgs);
		return pResult;
	};

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