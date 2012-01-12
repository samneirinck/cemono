/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoArray interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_ARRAY_H__
#define __I_MONO_ARRAY_H__

#include <MonoSerializable.h>

namespace mono 
{
	class _string; typedef _string* string; 
	class _array; typedef _array* array;
};

struct IMonoObject;

struct MonoAnyValue;
struct HitInfo;
struct ExplosionInfo;

struct IMonoArray : public CMonoSerializable
{
public:
	virtual void Clear() = 0;
	virtual int GetSize() const = 0;

	virtual IMonoObject *GetItem(int index) = 0;
	template <class T>
	T GetItemUnboxed(int index) { return GetItem(index)->Unbox<T>(); }
	virtual const char *GetItemString(int index) = 0;
	virtual IMonoArray *GetItemArray(int index) = 0;

	virtual void Insert(MonoAnyValue value) = 0;

	virtual void Insert(IMonoObject *pObject) = 0;
	virtual void Insert(IMonoArray *pArray) = 0;

	virtual void InsertString(mono::string string) = 0;
	virtual void InsertArray(mono::array array) = 0;

	virtual mono::array GetMonoArray() = 0;

	// Operator overloads to easily cast to mono types
	operator mono::array() const
	{
		return const_cast<IMonoArray *>(this)->GetMonoArray();
	}
};

#endif //__I_MONO_ARRAY_H__