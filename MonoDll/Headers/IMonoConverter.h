/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Handles converting between C# and C++ types and objects.
//////////////////////////////////////////////////////////////////////////
// 11/01/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_CONVERTER_H__
#define __I_MONO_CONVERTER_H__

namespace mono 
{ 
	class _string; typedef _string *string;
	class _object; typedef _object *object;
}

struct IMonoArray;
struct IMonoObject;
struct IMonoAssembly;
struct IMonoClass;

struct MonoAnyValue;

/// <summary>
/// Used to create and convert C++ / C# values.
/// </summary>
struct IMonoConverter
{
	/// <summary>
	/// Converts a mono string to a const char *.
	/// </summary>
	virtual const char *ToString(mono::string monoString) = 0;
	/// <summary>
	/// Converts a const char * to a mono string.
	/// </summary>
	virtual mono::string ToMonoString(const char *string) = 0;

	/// <summary>
	/// Creates an IMonoArray with the pre-determined size.
	/// If element is NULL, uses object class.
	/// </summary>
	/// <example>
	/// [Native C++]
	/// IMonoClass *pElementClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetClass("Vec3");
	/// CreateArray(3, pElementClass);
	/// 
	/// [Managed]
	/// Vec3[] myElements;
	/// </example>
	virtual IMonoArray *CreateArray(int size, IMonoClass *pElementClass = NULL) = 0;
	/// <summary>
	/// Converts a mono array to a IMonoArray. (To provide GetSize, GetItem etc functionality.)
	/// </summary>
	virtual IMonoArray *ToArray(mono::object arr) = 0;

	/// <summary>
	/// Creates a IMonoObject out of a MonoAnyValue.
	/// </summary>
	virtual IMonoObject *CreateObject(MonoAnyValue &value) = 0;
	/// <summary>
	/// Converts an mono object to a IMonoObject.
	/// </summary>
	virtual IMonoObject *ToObject(mono::object obj) = 0;
};

#endif //__I_MONO_CONVERTER_H__