// This is the main DLL file.

#include "stdafx.h"
#include <vector>
#include <Windows.h>
#include <string>
#include <msclr\marshal.h>

#include "Cemono.Debugging.h"
#include <ICemonoClassBinding.h>
#include <mono\jit\jit.h>

ICemonoClassBinding** m_bindings;
int m_numBindings;

ICemonoClassBinding* GetClassBindingByName(const char* name)
{
	for (int i = 0; i < m_numBindings; i++)
	{
		ICemonoClassBinding* binding = m_bindings[i];

		if (strcmp(binding->GetClassName(),name) == 0)
			return binding;
	}
	return NULL;
}
const void* GetMethodByName(ICemonoClassBinding* classBinding, const char* name)
{
	const ICemonoMethodBinding* methodBindings = classBinding->GetMethodsArray();
	for (int i = 0; i < classBinding->GetNumMethods(); i++)
	{
		auto methodBinding = methodBindings[i];

		if (strcmp(methodBinding.methodName, name) == 0)
			return methodBinding.method;
	}

	return NULL;
}

const void* GetMethod(const char* className, const char* methodName)
{
	auto classBinding = GetClassBindingByName(className);
	if (classBinding)
	{
		return GetMethodByName(classBinding, methodName);
	}

	return NULL;
}

template <class T1, class TResult> TResult ExecuteFunction(const char* className, const char* methodName, T1 arg)
{
	auto method = GetMethod(className, methodName);

	return ((TResult (*)(T1))method)(arg);
}

template <> bool ExecuteFunction(const char* className, const char* methodName, const char* arg)
{
	auto str = mono_string_new(mono_domain_get(), arg);

	return ExecuteFunction<MonoString*, bool>(className, methodName, str);
}

template <> int ExecuteFunction(const char* className, const char* methodName, const char* arg)
{
	auto str = mono_string_new(mono_domain_get(), arg);

	return ExecuteFunction<MonoString*, int>(className, methodName, str);
}

template <> float ExecuteFunction(const char* className, const char* methodName, const char* arg)
{
	auto str = mono_string_new(mono_domain_get(), arg);

	return ExecuteFunction<MonoString*, float>(className, methodName, str);
}

template <> const char* ExecuteFunction(const char* className, const char* methodName, const char* arg)
{
	auto str = mono_string_new(mono_domain_get(), arg);
	auto returnValue = ExecuteFunction<MonoString*, MonoString*>(className, methodName, str);

	char* charValue = mono_string_to_utf8(returnValue);

	char *szRet = (char *)::CoTaskMemAlloc(strlen(charValue)+1); 
	strcpy(szRet, charValue);
	mono_free(charValue);

	return szRet;

	return "";
}

template <class T1, class T2> void ExecuteAction(const char* className, const char* methodName, T1 arg1, T2 arg2)
{
	auto method = GetMethod(className, methodName);

	((void (*)(T1, T2))method)(arg1, arg2);
}

template <> void ExecuteAction(const char* className, const char* methodName, const char* arg1, int arg2)
{
	auto method = GetMethod(className, methodName);
	auto str1 = mono_string_new(mono_domain_get(), arg1);

	((void (*)(MonoString*, int))method)(str1, arg2);
}

template <> void ExecuteAction(const char* className, const char* methodName, const char* arg1, float arg2)
{
	auto method = GetMethod(className, methodName);
	auto str1 = mono_string_new(mono_domain_get(), arg1);

	((void (*)(MonoString*, float))method)(str1, arg2);
}

template <> void ExecuteAction(const char* className, const char* methodName, const char* arg1, const char* arg2)
{
	auto method = GetMethod(className, methodName);
	auto str1 = mono_string_new(mono_domain_get(), arg1);
	auto str2 = mono_string_new(mono_domain_get(), arg2);

	((void (*)(MonoString*, MonoString*))method)(str1, str2);
}

template <class T1, class T2, class T3> void ExecuteAction(const char* className, const char* methodName, T1 arg1, T2 arg2, T3 arg3)
{
	auto method = GetMethod(className, methodName);

	((void (*)(T1, T2, T3))method)(arg1, arg2, arg3);
}

template <class T1, class T2, class T3, class T4> void ExecuteAction(const char* className, const char* methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
	auto method = GetMethod(className, methodName);

	((void (*)(T1, T2, T3, T4))method)(arg1, arg2, arg3, arg4);
}

template <> void ExecuteAction(const char* className, const char* methodName, const char* arg1, int arg2, int arg3, const char* arg4)
{
	auto method = GetMethod(className, methodName);
	auto str1 = mono_string_new(mono_domain_get(), arg1);
	auto str4 = mono_string_new(mono_domain_get(), arg4);

	((void (*)(MonoString*, int, int, MonoString*))method)(str1, arg2, arg3, str4);
}

template <> void ExecuteAction(const char* className, const char* methodName, const char* arg1, float arg2, int arg3, const char* arg4)
{
	auto method = GetMethod(className, methodName);
	auto str1 = mono_string_new(mono_domain_get(), arg1);
	auto str4 = mono_string_new(mono_domain_get(), arg4);

	((void (*)(MonoString*, float, int, MonoString*))method)(str1, arg2, arg3, str4);
}

template <> void ExecuteAction(const char* className, const char* methodName, const char* arg1, const char* arg2, int arg3, const char* arg4)
{
	auto method = GetMethod(className, methodName);
	auto str1 = mono_string_new(mono_domain_get(), arg1);
	auto str2 = mono_string_new(mono_domain_get(), arg2);
	auto str4 = mono_string_new(mono_domain_get(), arg4);

	((void (*)(MonoString*, MonoString*, int, MonoString*))method)(str1, str2, arg3, str4);
}

//RegisterMethod(#method, method)
#define FUNCTION(arg,result,className,methodName) __declspec(dllexport) result methodName(arg arg1) { return ExecuteFunction<arg,result>(className,#methodName,arg1); }
#define ACTION2(arg1,arg2,className,methodName) __declspec(dllexport) void methodName(arg1 arg_1, arg2 arg_2) { ExecuteAction<arg1,arg2>(className,#methodName,arg_1, arg_2); }
#define ACTION3(arg1,arg2,arg3,className,methodName) __declspec(dllexport) void methodName(arg1 arg_1, arg2 arg_2, arg3 arg_3) { ExecuteAction<arg1,arg2,arg3>(className,#methodName,arg_1, arg_2, arg_3); }
#define ACTION4(arg1,arg2,arg3,arg4,className,methodName) __declspec(dllexport) void methodName(arg1 arg_1, arg2 arg_2, arg3 arg_3, arg4 arg_4) { ExecuteAction<arg1,arg2,arg3,arg4>(className,#methodName,arg_1, arg_2, arg_3, arg_4); }



extern "C"
{
	 __declspec(dllexport) void InitCemonoDebugging(int numBindings, ICemonoClassBinding** classBindings)
	 {
		 m_bindings = classBindings;
		 m_numBindings = numBindings;

		 // This is not the correct place to initialize the manager, move out!
		 Cemono::Manager^ manager = gcnew Cemono::Manager();
	 }

	 // Console
	FUNCTION(const char*, bool, "Console", _GetCVar)
	FUNCTION(const char*, const char*, "Console", _GetCVarHelpText)
	FUNCTION(const char*, int, "Console", _GetCVarValueInt)
	FUNCTION(const char*, float, "Console", _GetCVarValueFloat)
	FUNCTION(const char*, const char*, "Console", _GetCVarValueString)
	ACTION2(const char*, int, "Console", _SetCVarValueInt)
	ACTION2(const char*, float, "Console", _SetCVarValueFloat)
	ACTION2(const char*, const char*, "Console", _SetCVarValueString)
	FUNCTION(const char*, int, "Console", _GetCVarFlags)
	ACTION2(const char*, int, "Console", _SetCVarFlags)
	ACTION4(const char*, int, int, const char*, "Console", _RegisterCVarInt)
	ACTION4(const char*, float, int, const char*, "Console", _RegisterCVarFloat)
	ACTION4(const char*, const char*, int, const char*, "Console", _RegisterCVarString)
}