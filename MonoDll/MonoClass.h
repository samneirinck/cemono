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

#include <MonoCommon.h>
#include <IMonoClass.h>

struct IMonoObject;
struct IMonoArray;

class CMonoClass : public IMonoClass
{
public:
	// Instantiate a class right away.
	CMonoClass(MonoClass *pClass, IMonoArray *pConstructorArguments);
	// No instance provided, can only be used to invoke / get static members. Instantiation is possible using the Instantiate method.
	CMonoClass(MonoClass *pClass) : m_pClass(pClass), m_pInstance(NULL) {}
	// Set up using an existing instance.
	CMonoClass(MonoClass *pClass, MonoObject *pInstance) : m_pClass(pClass), m_pInstance((mono::object)pInstance) { m_instanceHandle = mono_gchandle_new((MonoObject *)m_pInstance, false); }
	~CMonoClass();

	// IMonoClass
	virtual const char *GetName() override { return mono_class_get_name(m_pClass); }
	virtual int GetScriptId() override;

	virtual void Instantiate(IMonoArray *pConstructorParams = NULL) override;

	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool _static = false) override;

	virtual IMonoObject *GetProperty(const char *propertyName) override;
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue) override;
	virtual IMonoObject *GetField(const char *fieldName) override;
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue) override;
	// ~IMonoClass

	MonoClass *GetMonoClass() { return m_pClass; }

private:
	MonoMethod *GetMethod(const char *methodName, bool bStatic = false);
	void HandleException(MonoObject *pException);

	mono::object m_pInstance;
	MonoClass *m_pClass;

	int m_instanceHandle;
};

#endif //__MONO_CLASS_H__