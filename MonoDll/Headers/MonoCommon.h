/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Common CryMono types and methods.
//
// DO NOT INCLUDE IN CRYMONO INTERFACES!
//////////////////////////////////////////////////////////////////////////
// 11/01/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_COMMON_H__
#define __MONO_COMMON_H__

namespace mono 
{
	class _string; typedef _string* string; 
	class _array; typedef _array* array;
	class _object; typedef _object* object;

	struct entityId
	{
		entityId() : id(0) {}
		entityId(EntityId Id) : id(Id) {}

		EntityId id;
	};

	struct pointer
	{
		pointer(void *ptr)
			: pPointer(ptr) {}

		void *pPointer;
	};
};

#include <IMonoArray.h>

#include <IMonoScriptSystem.h>
#include <IMonoConverter.h>

#include <IMonoClass.h>

/// <summary>
/// Converts a C++ string to the C# equivalent.
/// </summary>
inline const char *ToCryString(mono::string monoString)
{
	return gEnv->pMonoScriptSystem->GetConverter()->ToString(monoString);
}

/// <summary>
/// Converts a C# string to the C++ equivalent.
/// </summary>
inline mono::string ToMonoString(const char *cryString)
{
	return gEnv->pMonoScriptSystem->GetConverter()->ToMonoString(cryString);
}

/// <summary>
/// Creates an IMonoArray with the specified size.
/// </summary>
static IMonoArray *CreateMonoArray(int numArgs)
{
	if(numArgs<1)
		return NULL;

	return gEnv->pMonoScriptSystem->GetConverter()->CreateArray(numArgs);
}

/// <summary>
/// Wrapped 'helpers' used to easily convert native mono objects to IMonoArray's, strings etc.
/// </summary>
namespace mono
{
	/// <summary>
	/// Mono String; used in scriptbind parameters and when invoking Mono scripts.
	/// </summary>
	class _string
	{
	public:
		/// <summary>
		/// Allows direct casting from mono::string to const char *, no more manual ToCryString calls, woo!
		/// </summary>
		operator const char*() const
		{
			return ToCryString(const_cast<_string *>(this));
		}

		/// <summary>
		/// Allows direct casting from mono::string to CryStringT<char> (string).
		/// </summary>
		operator CryStringT<char>() const
		{
			return (CryStringT<char>)ToCryString(const_cast<_string *>(this));
		}
	};

	/// <summary> 
	/// Mono Array; used in scriptbind parameters and when invoking Mono scripts to handle C# arrays.
	/// </summary>
	class _array
	{
	public:
		operator IMonoArray *() const
		{
			return gEnv->pMonoScriptSystem->GetConverter()->ToArray(const_cast<_array *>(this));
		}
	};

	class _object
	{
	public:
		operator IMonoObject *() const
		{
			return gEnv->pMonoScriptSystem->GetConverter()->ToObject(const_cast<_object *>(this));
		}
	};

	typedef _string* string;
	typedef _array* array;
	typedef _object* object;
}; 

template <typename TResult>
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName) { return IMonoClass::CallMethod<TResult>(pScriptClass, funcName); }

template<typename TResult, typename P1> 
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName, const P1 &p1)
{
	IMonoArray *pArgs = CreateMonoArray(1);
	pArgs->Insert(p1);

	return IMonoClass::CallMethod<TResult>(pScriptClass, funcName, pArgs, true);
};

template<typename TResult, typename P1, typename P2> 
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName, const P1 &p1, const P2 &p2)
{
	IMonoArray *pArgs = CreateMonoArray(2);
	pArgs->Insert(p1);
	pArgs->Insert(p2);

	return IMonoClass::CallMethod<TResult>(pScriptClass, funcName, pArgs);
};

template<typename TResult, typename P1, typename P2, typename P3> 
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3)
{
	IMonoArray *pArgs = CreateMonoArray(3);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	
	return IMonoClass::CallMethod<TResult>(pScriptClass, funcName, pArgs, true);
};

template<typename TResult, typename P1, typename P2, typename P3, typename P4> 
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
{
	IMonoArray *pArgs = CreateMonoArray(4);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	pArgs->Insert(p4);
	
	return IMonoClass::CallMethod<TResult>(pScriptClass, funcName, pArgs, true);
};

template<typename TResult, typename P1, typename P2, typename P3, typename P4, typename P5> 
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
{
	IMonoArray *pArgs = CreateMonoArray(5);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	pArgs->Insert(p4);
	pArgs->Insert(p5);
	
	return IMonoClass::CallMethod<TResult>(pScriptClass, funcName, pArgs, true);
};

template<typename TResult, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline TResult CallMonoScript(IMonoClass *pScriptClass, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
{
	IMonoArray *pArgs = CreateMonoArray(6);
	pArgs->Insert(p1);
	pArgs->Insert(p2);
	pArgs->Insert(p3);
	pArgs->Insert(p4);
	pArgs->Insert(p5);
	pArgs->Insert(p6);
	
	return IMonoClass::CallMethod<TResult>(pScriptClass, funcName, pArgs, true);
};

#endif //__MONO_COMMON_H__