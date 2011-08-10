#pragma once

#include "StdAfx.h"
#include "Mono.h"

class CMonoClassUtils
{
public:
	static MonoObject *CallMethod(const char *funcName, string _className, string _nameSpace, MonoImage *pImage, MonoObject *pInstance = NULL, void **args = NULL);
	static MonoObject *CallMethod(const char *funcName, MonoClass *pClass, MonoObject *pInstance = NULL, void **args = NULL);

	static MonoObject* CreateInstanceOf(MonoDomain* pDomain, MonoClass* pClass);
	static MonoObject* CreateInstanceOf(MonoClass* pClass);
	static MonoClass* GetClassByName(const char* nameSpace, const char* className);
};