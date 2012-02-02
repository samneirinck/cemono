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
	// Instantiate
	CMonoClass(MonoClass *pClass, IMonoArray *pConstructorArguments);
	// Only use as class (for now)
	CMonoClass(MonoClass *pClass) : m_pClass(pClass), m_pInstance(NULL), m_scriptId(-1), m_scriptType(0) {}
	CMonoClass(MonoClass *pClass, MonoObject *pInstance) : m_pClass(pClass), m_pInstance((mono::object)pInstance) { m_instanceHandle = mono_gchandle_new((MonoObject *)m_pInstance, false); }
	CMonoClass(int scriptId, int scriptType);
	~CMonoClass();

	// IMonoClass
	virtual const char *GetName() override { return mono_class_get_name(m_pClass); }
	virtual int GetScriptId() override { return m_scriptId; }
	virtual int GetScriptType() override { return m_scriptType; }

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

	int m_scriptId;
	int m_scriptType;
};

#endif //__MONO_CLASS_H__