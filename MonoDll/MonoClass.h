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

#include <MonoCommon.h>
#include <IMonoClass.h>

struct IMonoArray;

class CScriptClass 
	: public CScriptObject
	, public IMonoClass
{
	typedef std::map<MonoClass *, CScriptClass *> TClassMap;
	CScriptClass(MonoClass *pClass);
public:
	static CScriptClass *TryGetClass(MonoClass *pClass);
	virtual ~CScriptClass() {}

	// IMonoClass
	virtual const char *GetName() override { return mono_class_get_name((MonoClass *)m_pObject); }
	virtual const char *GetNamespace() override { return mono_class_get_namespace((MonoClass *)m_pObject); }

	virtual IMonoObject *CreateInstance(IMonoArray *pConstructorParams = NULL) override;

	IMonoObject *BoxObject(void *object) override;
	// ~IMonoClass

	// IMonoObject
	virtual void Release() override { delete this; }

	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool bStatic = false) { return CScriptObject::CallMethod(methodName, params, true); }

	virtual IMonoObject *GetProperty(const char *propertyName, bool bStatic = false) override { return CScriptObject::GetProperty(propertyName, true); }
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic = false) override { CScriptObject::SetProperty(propertyName, pNewValue, true); }
	virtual IMonoObject *GetField(const char *fieldName, bool bStatic = false) override { return CScriptObject::GetField(fieldName, true); }
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic = false) override { CScriptObject::SetField(fieldName, pNewValue, true); }

	virtual EMonoAnyType GetType() override { return eMonoAnyType_Class; }

	virtual mono::object GetManagedObject() override { return CScriptObject::GetManagedObject(); }

	virtual IMonoClass *GetClass() override { return CScriptObject::GetClass(); }

	virtual void *UnboxObject() override { return CScriptObject::UnboxObject(); }
	// ~IMonoObject

	MonoMethod *GetMonoMethod(const char *name, IMonoArray *pArgs);
	MonoProperty *GetMonoProperty(const char *name);
	MonoClassField *GetMonoField(const char *name);

	static TClassMap m_classRegistry;
};

#endif //__MONO_CLASS_H__