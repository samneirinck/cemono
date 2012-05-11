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
	class _array; typedef _array *array; 
	class _object; typedef _object *object;
}

struct IMonoArray;
struct IMonoObject;
struct IMonoClass;
struct IMonoAssembly;

struct MonoAnyValue;

enum ECommonManagedTypes
{
	// Vec3 struct
	eCMT_Vec3 = 0,
	// unsigned int, do not supply an object of the SMonoEntityId struct.
	eCMT_EntityId,
	// Standard hit info struct declared in CryGame.
	eCMT_HitInfo
};

/// <summary>
/// Used to create and convert C++ / C# values.
/// </summary>
struct IMonoConverter
{
	virtual void Reset() = 0;

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
	/// Converts a mono array to a IMonoArray. (To provide GetSize, GetItem etc functionality.)
	/// </summary>
	virtual IMonoArray *ToArray(mono::array arr) = 0;

	/// <summary>
	/// Creates a IMonoObject out of a MonoAnyValue.
	/// </summary>
	virtual IMonoObject *CreateObject(MonoAnyValue &value) = 0;
	/// <summary>
	/// Converts an mono object to a IMonoObject.
	/// </summary>
	virtual IMonoObject *ToObject(mono::object obj) = 0;
	/// <summary>
	/// Converts an IMonoObject to a class, if valid mono object is contained within.
	/// Allows for invoking methods, properties etc.
	/// </summary>
	virtual IMonoClass *ToClass(IMonoObject *pObject) = 0;

	virtual IMonoClass *GetCommonClass(ECommonManagedTypes commonType) = 0;
	virtual IMonoObject *ToManagedType(ECommonManagedTypes commonType, void *object) = 0;

	/// <summary>
	/// Converts an object into the specified managed type.
	/// </summary>
	virtual IMonoObject *ToManagedType(IMonoClass *pTo, void *object) = 0;

	virtual IMonoAssembly *LoadAssembly(const char *assemblyPath) = 0;
};

#endif //__I_MONO_CONVERTER_H__