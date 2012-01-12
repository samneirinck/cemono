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

struct IMonoObject : public CMonoSerializable
{
public:
	template <class T>
	T Unbox() { return *(T *)UnboxObject(); }

	virtual IMonoObject *GetProperty(const char *propertyName) = 0;

	// CMonoSerializable
	virtual void Serialize(TSerialize ser) override
	{
		value = GetAnyValue();

		CMonoSerializable::Serialize(ser);
	}
	// ~CMonoSerializable

	virtual MonoAnyType GetType() = 0;
	virtual MonoAnyValue GetAnyValue() = 0;

private:
	virtual void *UnboxObject() = 0;
};

#endif //__I_MONO_OBJECT_H__