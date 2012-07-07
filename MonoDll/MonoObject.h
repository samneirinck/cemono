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

class CScriptClass;

class CScriptObject
	: public IMonoObject
{
protected:
	CScriptObject() {}

public:
	CScriptObject(MonoObject *object);
	CScriptObject(MonoObject *object, IMonoArray *pConstructorParams);
	virtual ~CScriptObject();

	MonoClass *GetMonoClass();

	// IMonoObject
	virtual IMonoObject *CallMethodWithArray(const char *methodName, IMonoArray *params = nullptr, bool bStatic = false);

	virtual IMonoObject *GetProperty(const char *propertyName, bool bStatic = false);
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic = false);
	virtual IMonoObject *GetField(const char *fieldName, bool bStatic = false);
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic = false);

	virtual void Release() override { delete this; }

	virtual EMonoAnyType GetType();

	virtual mono::object GetManagedObject() override { return (mono::object)m_pObject; }

	virtual IMonoClass *GetClass();
	// ~IMonoObject

	// IMonoScriptSystemListener
	virtual void OnPreScriptCompilation(bool isReload) {}
	virtual void OnPostScriptCompilation(bool isReload, bool compilationSuccess) {}

	virtual void OnPreScriptReload(bool initialLoad) {}
	virtual void OnPostScriptReload(bool initialLoad);
	// ~IMonoScriptSystemListener

	static void HandleException(MonoObject *pException);

protected:
	virtual void *UnboxObject() override { return mono_object_unbox(m_pObject); }
	// ~IMonoObject

	MonoObject *m_pObject;
	IMonoClass *m_pClass;

	int m_objectHandle;
	// CryScriptInstance.ScriptId
	int m_scriptId;
};

#endif //__MONO_OBJECT_H__