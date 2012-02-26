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
#include <MonoSerializable.h>

/// <summary>
/// The IMonoObject class is used to wrap native mono objects of any type, and to
/// convert C++ types to the Mono equivalent.
/// </summary>
struct IMonoObject : public CSerializable
{
public:
	/// <summary>
	/// Deletes the object. Warning: Also deleted in C#!
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Gets the unboxed object and casts it to the requested type. (class T)
	/// </summary>
	template <class T>
	T Unbox() { return *(T *)UnboxObject(); }

	/// <summary>
	/// </summary>
	template <>
	IMonoClass *Unbox() { return gEnv->pMonoScriptSystem->GetConverter()->ToClass(this); }

	// CSerializable
	/// <summary>
	/// Allows serialization of mono objects.
	/// </summary>
	virtual void Serialize(TSerialize ser) override
	{
		value = GetAnyValue();

		CSerializable::Serialize(ser);
	}
	// ~CSerializable

	/// <summary>
	/// Gets the type of this Mono object.
	/// </summary>
	virtual MonoAnyType GetType() = 0;
	/// <summary>
	/// Gets the value of this object as an MonoAnyValue.
	/// </summary>
	virtual MonoAnyValue GetAnyValue() = 0;
	
	/// <summary>
	/// </summary>
	virtual mono::object GetMonoObject() = 0;

	/// <summary>
	/// Simple overloaded operator to allow direct casting to Mono type object.
	/// </summary>
	operator mono::object() const
	{
		return const_cast<IMonoObject *>(this)->GetMonoObject();
	}

private:
	/// <summary>
	/// Unboxes the object and returns it as a void pointer. (Use Unbox() method to easily cast to the C++ type)
	/// </summary>
	virtual void *UnboxObject() = 0;
};

#endif //__I_MONO_OBJECT_H__