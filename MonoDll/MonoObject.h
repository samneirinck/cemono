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

#endif //__MONO_OBJECT_H__