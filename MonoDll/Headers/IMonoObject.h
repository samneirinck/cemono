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

#include <MonoSerializable.h>

/// <summary>
/// The IMonoObject class is used to wrap native mono objects of any type, and to
/// convert C++ types to the Mono equivalent.
/// </summary>
struct IMonoObject : public CMonoSerializable
{
public:
	/// <summary>
	/// Gets the unboxed object and casts it to the requested type. (class T)
	/// </summary>
	template <class T>
	T Unbox() { return *(T *)UnboxObject(); }

	/// <summary>
	/// Gets a property within this Mono object as an IMonoObject.
	/// </summary>
	/// <example>
	/// [C#] public float X { get; set; }
	/// [C++] float X = GetProperty("X")->Unbox<float>();
	/// </example>
	virtual IMonoObject *GetProperty(const char *propertyName) = 0;

	// CMonoSerializable
	/// <summary>
	/// Allows serialization of mono objects.
	/// </summary>
	virtual void Serialize(TSerialize ser) override
	{
		value = GetAnyValue();

		CMonoSerializable::Serialize(ser);
	}
	// ~CMonoSerializable

	/// <summary>
	/// Gets the type of this Mono object.
	/// </summary>
	virtual MonoAnyType GetType() = 0;
	/// <summary>
	/// Gets the value of this object as an MonoAnyValue.
	/// </summary>
	virtual MonoAnyValue GetAnyValue() = 0;

private:
	/// <summary>
	/// Unboxes the object and returns it as a void pointer. (Use Unbox() method to easily cast to the C++ type)
	/// </summary>
	virtual void *UnboxObject() = 0;
};

#endif //__I_MONO_OBJECT_H__