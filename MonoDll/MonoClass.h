#ifndef __MONO_CLASS_H__
#define __MONO_CLASS_H__

#include <IMonoClass.h>

struct IMonoObject;
struct IMonoArray;

class CMonoClass : public IMonoClass
{
public:
	CMonoClass(MonoObject *pClassObject, IMonoArray *pConstructorArguments = NULL);
	CMonoClass(MonoClass *pClass) : m_pClass(pClass) {}
	~CMonoClass();

	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool _static = false) override;

	MonoClass *GetClass() { return m_pClass; }

private:
	MonoMethod *GetMethodFromClassAndMethodName(MonoClass* pClass, const char* methodName);

	MonoObject *m_pInstance;
	MonoClass *m_pClass;

	int m_instanceHandle;
};

#endif //__MONO_CLASS_H__