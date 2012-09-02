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

/// <summary>
/// Converts a C++ string to the C# equivalent.
/// </summary>
inline const char *ToCryString(mono::string monoString)
{
	if(!monoString)
		return "";

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
static IMonoArray *CreateMonoArray(int numArgs, IMonoClass *pElementClass = nullptr)
{
	return gEnv->pMonoScriptSystem->GetConverter()->CreateArray(numArgs, pElementClass);
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

	class _object
	{
	public:
		operator IMonoObject *() const
		{
			return gEnv->pMonoScriptSystem->GetConverter()->ToObject(const_cast<_object *>(this));
		}

		operator IMonoArray *() const
		{
			return gEnv->pMonoScriptSystem->GetConverter()->ToArray(const_cast<_object *>(this));
		}
	};

	typedef _string* string;
	typedef _object* object;
}; 

#endif //__MONO_COMMON_H__