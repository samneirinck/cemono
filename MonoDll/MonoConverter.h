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

class CConverter : public IMonoConverter
{
	typedef std::map<ECommonManagedTypes, IMonoClass *> TPreStoredTypes;
public:
	CConverter();
	~CConverter();

	// IMonoConverter
	virtual void Reset() override;

	virtual const char *ToString(mono::string monoString) override { if(!monoString) return ""; return mono_string_to_utf8((MonoString *)monoString); }
	virtual mono::string ToMonoString(const char *string) override { return (mono::string)mono_string_new(mono_domain_get(), string); }

	virtual IMonoArray *CreateArray(int size) override;
	virtual IMonoArray *ToArray(mono::array arr) override;

	virtual IMonoObject *CreateObject(MonoAnyValue &value) override;

	virtual IMonoObject *ToObject(mono::object obj) override;

	virtual IMonoClass *GetCommonClass(ECommonManagedTypes commonType) override { return m_preStoredTypes[commonType]; }

	virtual IMonoObject *ToManagedType(ECommonManagedTypes commonType, void *object) override;
	virtual IMonoObject *ToManagedType(IMonoClass *pTo, void *object) override;

	virtual IMonoClass *ToClass(IMonoObject *pObject) override;
	// ~IMonoConverter

	// Store commonly accessed types for quick access.
	TPreStoredTypes m_preStoredTypes;
};

#endif //__MONO_CONVERTER_H__