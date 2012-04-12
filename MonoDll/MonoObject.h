/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Wrapper for the MonoObject for less intensively ugly code and
// better workflow.
//////////////////////////////////////////////////////////////////////////
// 17/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_OBJECT_H__
#define __MONO_OBJECT_H__

#include "MonoCommon.h"
#include "MonoClass.h"

#include "MonoScriptSystem.h"

#include <IMonoObject.h>
#include <mono/mini/jit.h>

class CScriptObject : public IMonoObject
{
public:
	CScriptObject(mono::object object);
	virtual ~CScriptObject() { mono_gchandle_free(m_objectHandle); m_pObject = 0; }

	MonoClass *GetMonoClass() { return mono_object_get_class((MonoObject *)m_pObject); }

	// IMonoObject
	virtual void Release() override { delete this; }

	virtual MonoAnyType GetType() override;
	virtual MonoAnyValue GetAnyValue() override;

	virtual mono::object GetMonoObject() override { return m_pObject; }

private:
	virtual void *UnboxObject() override { return mono_object_unbox((MonoObject *)m_pObject); }
	// ~IMonoObject

protected:
	mono::object m_pObject;

	int m_objectHandle;
};

template <typename T>
static MonoClass *GetMonoClassOfType(T t)
{
	if(std::is_same<T, int>::value)
		return mono_get_int32_class();
	else if(std::is_same<T, uint32>::value)
		return mono_get_uint32_class();
	else if(std::is_same<T, short>::value)
		return mono_get_int16_class();
	else if(std::is_same<T, uint16>::value)
		return mono_get_uint16_class();
	else if(std::is_same<T, float>::value)
		return mono_get_single_class();
	else if(std::is_same<T, bool>::value)
		return mono_get_boolean_class();

	return NULL;
}

template <typename T>
static IMonoObject *CreateMonoObject(T t)
{
	if(std::is_same<T, MonoString *>::value)
		CryLogAlways("[Warning] Trying to create MonoObject using a MonoString");
	if(std::is_same<T, const char *>::value)
		CryLogAlways("[Warning] Trying to create MonoObject using string");//return mono_value_box(mono_domain_get(), mono_get_string_class(), ToMonoString((const char *)t));
	else
	{
		if(MonoClass *pClass = GetMonoClassOfType<T>(t))
			return *(mono::object)mono_value_box(mono_domain_get(), pClass, &t);
	}

	CryLogAlways("Failed to create IMonoObject, try using IMonoConverter::CreateObjectOfCustomType instead.");
	return NULL;
}

#endif //__MONO_OBJECT_H__