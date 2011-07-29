#pragma once

#include "StdAfx.h"
#include "Mono.h"

class CMonoClassUtils
{
public:
	static MonoObject* CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass);
	static MonoObject* CreateInstanceOf(MonoClass* pClass);
	static MonoClass* GetClassByName(const char* nameSpace, const char* className);
	static MonoObject* CallMethod(MonoObject* pObjectInstance, const char* methodName);
	static MonoObject* CallMethod(MonoObject* pObjectInstance, MonoMethod* pMethod);
	static MonoObject* CallMethod(MonoObject* pObjectInstance, const char* methodName, void** args);
};