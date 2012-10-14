/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Handles converting between C# and C++ types and objects.
//////////////////////////////////////////////////////////////////////////
// 11/01/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_CONVERTER_H__
#define __MONO_CONVERTER_H__

#include <IMonoConverter.h>

struct IMonoClass;

class CConverter 
	: public IMonoConverter
{
public:
	CConverter() {}
	~CConverter() {}

	// IMonoConverter
	virtual const char *ToString(mono::string monoString) override { return mono_string_to_utf8((MonoString *)monoString); }
	virtual mono::string ToMonoString(const char *string) override { return (mono::string)mono_string_new(mono_domain_get(), string); }

	virtual IMonoArray *CreateArray(int size, IMonoClass *pElementClass = nullptr) override;
	virtual IMonoArray *ToArray(mono::object arr) override;

	virtual mono::object BoxAnyValue(MonoAnyValue &value) override;

	virtual IMonoObject *ToObject(mono::object obj, bool allowGC = true) override;
	// ~IMonoConverter
};

#endif //__MONO_CONVERTER_H__