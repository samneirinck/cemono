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

#include <IMonoObject.h>

namespace mono 
{
	class _string; typedef _string* string; 
	class _array; typedef _array* array;
};

struct IMonoObject;

struct MonoAnyValue;

/// <summary>
/// Used to wrap arrays sent from C#, and also used when passing arrays of elements from C++.
/// 
/// Creating an IMonoArray: IMonoArray *pMyArray = CreateMonoArray(arraySize);
/// Converting an mono::array: IMonoArray *pConvertedArray = *(mono::array)monoArray;
/// </summary>
struct IMonoArray : public CSerializable
{
public:
	/// <summary>
	/// Deletes the array and everything contained within it.
	/// Note: This also deletes the C# array.
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Clears the array of all its elements.
	/// Note that the pre-determined size set when the array was created remains.
	/// </summary>
	virtual void Clear() = 0;
	/// <summary>
	/// Retrieves the size of the array.
	/// </summary>
	virtual int GetSize() const = 0;

	/// <summary>
	/// Retrieves an IMonoObject at the selected index of the array.
	/// </summary>
	virtual IMonoObject *GetItem(int index) = 0;
	/// <summary>
	/// Retrieves an unboxed IMonoObject at the selected index of the array.
	/// </summary>
	template <class T>
	T GetItemUnboxed(int index) { return GetItem(index)->Unbox<T>(); }
	/// <summary>
	/// Retrieves a string at the selected index of the array.
	/// </summary>
	virtual const char *GetItemString(int index) = 0;
	/// <summary>
	/// Retrieves an IMonoArray at the selected index of the array.
	/// </summary>
	virtual IMonoArray *GetItemArray(int index) = 0;

	/// <summary>
	/// Inserts an MonoAnyValue object into the array.
	/// </summary>
	virtual void Insert(MonoAnyValue value) = 0;
	/// <summary>
	/// Inserts an IMonoObject into the array
	/// </summary>
	virtual void Insert(IMonoObject *pObject) = 0;
	/// <summary>
	/// Inserts an IMonoArray into the array.
	/// </summary>
	virtual void Insert(IMonoArray *pArray) = 0;

	/// <summary>
	/// Inserts a mono string into the array.
	/// </summary>
	virtual void InsertString(mono::string string) = 0;

	/// <summary>
	/// Inserts a mono object into the array.
	/// </summary>
	virtual void InsertObject(mono::object obj) = 0;

	/// <summary>
	/// Retrieves the mono array, can be passed directly to C#.
	/// </summary>
	virtual mono::array GetMonoArray() = 0;

	/// <summary>
	/// Simple overloaded operator to allow direct casting to Mono type array.
	/// </summary>
	operator mono::array() const
	{
		return const_cast<IMonoArray *>(this)->GetMonoArray();
	}
};

#endif //__I_MONO_ARRAY_H__