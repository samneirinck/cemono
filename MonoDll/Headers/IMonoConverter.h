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
	class _string; typedef _string* string;
	class _array; typedef _array* array; 
}

struct IMonoArray;
struct IMonoObject;
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
	/// </summary>
	virtual IMonoArray *CreateArray(int size) = 0;
	/// <summary>
	/// Converts a mono array to a IMonoArray (To provide GetSize, GetItem etc functionality.)
	/// </summary>
	virtual IMonoArray *ToArray(mono::array arr) = 0;

	/// <summary>
	/// Creates a IMonoObject out of a MonoAnyValue.
	/// </summary>
	virtual IMonoObject *CreateObject(MonoAnyValue &value) = 0;
	/// <summary>
	/// </summary>
	virtual IMonoObject *CreateObjectOfCustomType(void *object, const char *typeName, const char *namespaceName = "CryEngine") = 0;
};

#endif //__I_MONO_CONVERTER_H__