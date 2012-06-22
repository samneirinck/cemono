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

#include <IMonoObject.h>

#include <mono/mini/jit.h>

class CScriptObject
	: public IMonoObject
{
protected:
	CScriptObject() {}

public:
	CScriptObject(MonoObject *object);
	CScriptObject(MonoObject *object, IMonoArray *pConstructorParams);
	virtual ~CScriptObject() { if(m_objectHandle != -1) mono_gchandle_free(m_objectHandle); m_pObject = 0; }

	MonoClass *GetMonoClass() { return mono_object_get_class(m_pObject); }

	void SetObject(mono::object object) { m_pObject = (MonoObject *)object; }

	// IMonoObject
	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool bStatic = false);

	virtual IMonoObject *GetProperty(const char *propertyName, bool bStatic = false);
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic = false);
	virtual IMonoObject *GetField(const char *fieldName, bool bStatic = false);
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic = false);

	virtual void Release() override { delete this; }

	virtual EMonoAnyType GetType();

	virtual mono::object GetManagedObject() override { return (mono::object)m_pObject; }

	virtual IMonoClass *GetClass();

	static void HandleException(MonoObject *pException);

protected:
	virtual void *UnboxObject() override { return mono_object_unbox(m_pObject); }
	// ~IMonoObject

	MonoObject *m_pObject;
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
		MonoWarning("Trying to create MonoObject using a MonoString");
	if(std::is_same<T, const char *>::value)
		MonoWarning("Trying to create MonoObject using string");//return mono_value_box(mono_domain_get(), mono_get_string_class(), ToMonoString((const char *)t));
	else
	{
		if(MonoClass *pClass = GetMonoClassOfType<T>(t))
			return *(mono::object)mono_value_box(mono_domain_get(), pClass, &t);
	}

	MonoWarning("Failed to create IMonoObject, try using IMonoConverter::CreateObjectOfCustomType instead.");
	return NULL;
}

#endif //__MONO_OBJECT_H__