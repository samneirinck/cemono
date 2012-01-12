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

struct IMonoConverter
{
	virtual const char *ToString(mono::string monoString) = 0;
	virtual mono::string ToMonoString(const char *string) = 0;

	virtual IMonoArray *CreateArray(int size) = 0;
	virtual IMonoArray *ToArray(mono::array arr) = 0;

	virtual IMonoObject *CreateObject(MonoAnyValue &value) = 0;
	virtual IMonoObject *CreateObjectOfCustomType(void *object, const char *typeName, const char *namespaceName = "CryEngine") = 0;
};

#endif //__I_MONO_CONVERTER_H__