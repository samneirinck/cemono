/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Wrapper for the MonoClass for less intensively ugly code and
// better workflow.
//////////////////////////////////////////////////////////////////////////
// 17/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_CLASS_H__
#define __MONO_CLASS_H__

#include "MonoObject.h"
#include "MonoAssembly.h"

#include <MonoCommon.h>
#include <IMonoClass.h>

struct IMonoArray;


class CScriptClass 
	: public CScriptObject
	, public IMonoClass
{
public:
	CScriptClass(MonoClass *pClass, CScriptAssembly *pDeclaringAssembly);
	virtual ~CScriptClass();

	// IMonoClass
	virtual const char *GetName() override { return m_name.c_str(); }
	virtual const char *GetNamespace() override { return m_namespace.c_str(); }

	virtual IMonoAssembly *GetAssembly() { return m_pDeclaringAssembly; }

	virtual IMonoObject *CreateInstance(IMonoArray *pConstructorParams = nullptr) override;

	IMonoObject *BoxObject(void *object) override;

	virtual void AddRef() override { ++m_refs; }

	virtual IMonoObject *InvokeArray(IMonoObject *pObject, const char *methodName, IMonoArray *params = nullptr) override;
	virtual IMonoObject *Invoke(IMonoObject *pObject, const char *methodName, void **params = nullptr, int numParams = 0) override;

	virtual IMonoObject *GetPropertyValue(IMonoObject *pObject, const char *propertyName) override;
	virtual void SetPropertyValue(IMonoObject *pObject, const char *propertyName, mono::object newValue) override;
	virtual IMonoObject *GetFieldValue(IMonoObject *pObject, const char *fieldName) override;
	virtual void SetFieldValue(IMonoObject *pObject, const char *fieldName, mono::object newValue) override;
	// ~IMonoClass

	// IMonoObject
	virtual void Release(bool triggerGC = true) override;

	virtual EMonoAnyType GetType() override { return eMonoAnyType_Class; }
	virtual MonoAnyValue GetAnyValue() override { return MonoAnyValue(); }

	virtual mono::object GetManagedObject() override { return CScriptObject::GetManagedObject(); }

	virtual IMonoClass *GetClass() override { return this; }

	virtual void *UnboxObject() override { return CScriptObject::UnboxObject(); }

	virtual const char *ToString() override { return CScriptObject::ToString(); }
	// ~IMonoObject

private:
	MonoMethod *GetMonoMethod(const char *name, IMonoArray *pArgs);
	MonoMethod *GetMonoMethod(const char *name, int numParams);

	MonoProperty *GetMonoProperty(const char *name);
	MonoClassField *GetMonoField(const char *name);

	string m_name;
	string m_namespace;

	CScriptAssembly *m_pDeclaringAssembly;

	int m_refs;
};

#endif //__MONO_CLASS_H__