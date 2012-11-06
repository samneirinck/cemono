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

#include <MonoCommon.h>

struct IMonoClass;

namespace mono { class _object; typedef _object* object; }

/// <summary>
/// The IMonoObject class is used to wrap native mono objects of any type, and to
/// convert C++ types to the Mono equivalent.
/// </summary>
struct IMonoObject
{
public:
	inline IMonoObject *CallMethod(const char *funcName);

	template<typename P1> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1);

	template<typename P1, typename P2> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2);

	template<typename P1, typename P2, typename P3> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3);

	template<typename P1, typename P2, typename P3, typename P4> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4);

	template<typename P1, typename P2, typename P3, typename P4, typename P5> 
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5);

	template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	inline IMonoObject *CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6);

	inline IMonoObject *GetPropertyValue(const char *fieldName);
	inline void SetPropertyValue(const char *fieldName, IMonoObject *pNewValue);
	inline IMonoObject *GetFieldValue(const char *fieldName);
	inline void SetFieldValue(const char *fieldName, IMonoObject *pNewValue);

	/// <summary>
	/// Releases the object. Warning: also destructed in managed code!
	/// </summary>
	virtual void Release(bool triggerGC = true) = 0;

	/// <summary>
	/// Gets the unboxed object and casts it to the requested type. (class T)
	/// </summary>
	template <class T>
	T Unbox() { return *(T *)UnboxObject(); }

	/// <summary>
	/// Gets the type of this Mono object.
	/// </summary>
	virtual EMonoAnyType GetType() = 0;

	virtual MonoAnyValue GetAnyValue() = 0;
	
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

#include <IMonoClass.h>
#include <IMonoArray.h>

inline IMonoObject *IMonoObject::CallMethod(const char *funcName)
{
	return GetClass()->Invoke(this, funcName);
}

template<typename P1> 
inline IMonoObject *IMonoObject::CallMethod(const char *funcName, const P1 &p1)
{
	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(p1);

	IMonoObject *pResult = GetClass()->InvokeArray(this, funcName, pArgs);
	SAFE_RELEASE(pArgs);
	return pResult;
};

template<typename P1, typename P2> 
inline IMonoObject *IMonoObject::CallMethod(const char *funcName, const P1 &p1, const P2 &p2)
{
	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(p1);
	pArgs->Insert(p2);

	IMonoObject *pResult = GetClass()->InvokeArray(this, funcName, pArgs);
	SAFE_RELEASE(pArgs);
	return pResult;
};

template<typename P1, typename P2, typename P3> 
inline IMonoObject *IMonoObject::CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3)
{
	IMonoArray *pArgs = CreateMonoArray(3);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	
	IMonoObject *pResult = GetClass()->InvokeArray(this, funcName, pArgs);
	SAFE_RELEASE(pArgs);
	return pResult;
};

template<typename P1, typename P2, typename P3, typename P4> 
inline IMonoObject *IMonoObject::CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
{
	IMonoArray *pArgs = CreateMonoArray(4);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	pArgs->Insert(p4);
	
	IMonoObject *pResult = GetClass()->InvokeArray(this, funcName, pArgs);
	SAFE_RELEASE(pArgs);
	return pResult;
};

template<typename P1, typename P2, typename P3, typename P4, typename P5> 
inline IMonoObject *IMonoObject::CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
{
	IMonoArray *pArgs = CreateMonoArray(5);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	pArgs->Insert(p4);
	pArgs->Insert(p5);
	
	IMonoObject *pResult = GetClass()->InvokeArray(this, funcName, pArgs);
	SAFE_RELEASE(pArgs);
	return pResult;
};

template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline IMonoObject *IMonoObject::CallMethod(const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
{
	IMonoArray *pArgs = CreateMonoArray(6);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	pArgs->Insert(p4);
	pArgs->Insert(p5);
	pArgs->Insert(p6);
	
	IMonoObject *pResult = GetClass()->InvokeArray(this, funcName, pArgs);
	SAFE_RELEASE(pArgs);
	return pResult;
};

inline IMonoObject *IMonoObject::GetPropertyValue(const char *propertyName)
{
	return GetClass()->GetPropertyValue(this, propertyName);
}

inline void IMonoObject::SetPropertyValue(const char *propertyName, IMonoObject *pNewValue)
{
	GetClass()->SetPropertyValue(this, propertyName, pNewValue);
}

inline IMonoObject *IMonoObject::GetFieldValue(const char *fieldName)
{
	return GetClass()->GetFieldValue(this, fieldName);
}

inline void IMonoObject::SetFieldValue(const char *fieldName, IMonoObject *pNewValue)
{
	GetClass()->SetFieldValue(this, fieldName, pNewValue);
}

#endif //__I_MONO_OBJECT_H__