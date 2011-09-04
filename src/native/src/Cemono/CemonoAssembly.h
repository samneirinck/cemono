#pragma once

#include <mono/jit/jit.h>

class CCemonoAssembly
{
public:
	CCemonoAssembly(const char* assemblyPath);
	virtual ~CCemonoAssembly(void);
	MonoObject* CreateInstanceOf(const char* nameSpace, const char* className);
	MonoObject* CallMethod(MonoObject* pObjectInstance, const char* methodName);
	//MonoObject* CallMethod(MonoObject* pObjectInstance, MonoClass* pClass, const char* methodName);

protected:
	MonoClass* GetClassFromName(const char* nameSpace, const char* className);
	MonoClass* GetClassFromObject(MonoObject* pObjectInstance);

	MonoMethod* GetMethodFromClassAndMethodName(MonoClass* pClass, const char* methodName);
	

	const char* m_assemblyPath;
	std::map<const char*, MonoClass*> m_monoClasses;
	std::map<MonoClass*, std::map<const char*,MonoMethod*>> m_monoMethods;
	MonoAssembly* m_pAssembly;
	MonoImage* m_pImage;
	MonoDomain* m_pDomain;
};

