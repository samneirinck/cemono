/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Used as a base to allow serialization for Mono objects.
//////////////////////////////////////////////////////////////////////////
// 31/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_SERIALIZABLE__
#define __MONO_SERIALIZABLE__

#include <MonoAnyValue.h>

class CMonoSerializable
{
public:
	CMonoSerializable() : value(0) {}

	virtual void Serialize(TSerialize ser)
	{
		// the value of value is set in the parent, i.e. IMonoObject::Serialize.

		ser.BeginGroup("CMonoSerializable");
		ser.Value("monoAnyValue", value);
		ser.EndGroup();
	}

protected:
	MonoAnyValue value;
};

#endif //__MONO_SERIALIZABLE__