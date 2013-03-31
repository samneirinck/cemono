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

	virtual mono::object CreateInstance(IMonoArray *pConstructorParams = nullptr) override;

	mono::object BoxObject(void *object, IMonoDomain *pDomain = nullptr) override;

	virtual void AddRef() override { ++m_refs; }

	virtual mono::object InvokeArray(mono::object pObject, const char *methodName, IMonoArray *params = nullptr, bool throwOnFail = true) override;
	virtual mono::object Invoke(mono::object pObject, const char *methodName, void **params = nullptr, int numParams = 0, bool throwOnFail = true) override;

	virtual void *GetMethodThunk(const char *methodName, int numParams) override;

	virtual mono::object GetPropertyValue(mono::object pObject, const char *propertyName, bool throwOnFail = true) override;
	virtual void SetPropertyValue(mono::object pObject, const char *propertyName, mono::object newValue, bool throwOnFail = true) override;
	virtual mono::object GetFieldValue(mono::object pObject, const char *fieldName, bool throwOnFail = true) override;
	virtual void SetFieldValue(mono::object pObject, const char *fieldName, mono::object newValue, bool throwOnFail = true) override;
	// ~IMonoClass

	// IMonoObject
	virtual void Release(bool triggerGC = true) override;

	virtual EMonoAnyType GetType() override { return eMonoAnyType_Unknown; }
	virtual MonoAnyValue GetAnyValue() override { return MonoAnyValue(); }

	virtual mono::object GetManagedObject() override { return CScriptObject::GetManagedObject(); }

	virtual IMonoClass *GetClass() override { return this; }

	virtual void *UnboxObject() override { return CScriptObject::UnboxObject(); }

	virtual const char *ToString() override { return CScriptObject::ToString(); }
	// ~IMonoObject

private:
	MonoMethod *GetMonoMethod(const char *name, IMonoArray *pArgs);
	MonoMethod *GetMonoMethod(const char *name, int numParams);

	MonoProperty *GetMonoProperty(const char *name, bool requireSetter = false, bool requireGetter = false);
	MonoClassField *GetMonoField(const char *name);

	string m_name;
	string m_namespace;

	CScriptAssembly *m_pDeclaringAssembly;

	int m_refs;
};

#endif //__MONO_CLASS_H__