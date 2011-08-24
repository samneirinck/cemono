#pragma once

#include "StdAfx.h"
#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

class MonoClassUtils
{
public:
	static MonoObject *CallMethod(string funcName, string _className, string _nameSpace, MonoImage *pImage, MonoObject *pInstance = NULL, void **args = NULL);
	static MonoObject *CallMethod(string funcName, MonoClass *pClass, MonoObject *pInstance = NULL, void **args = NULL);

	static MonoObject* CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass);
	static MonoObject* CreateInstanceOf(MonoClass* pClass);
	static MonoObject* CreateInstanceOfWithComplexConstructor(MonoDomain *pDomain, MonoClass* pClass, void** constructorParameters);
	static MonoClass* GetClassByName(const char* nameSpace, const char* className);
	static MonoObject* CallMethod(MonoObject* pObjectInstance, const char* methodName);
	static MonoObject* CallMethod(MonoObject* pObjectInstance, MonoMethod* pMethod);
	static MonoObject* CallMethod(MonoObject* pObjectInstance, const char* methodName, void** args);
};