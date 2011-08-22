#pragma once
#include <vector>

struct ICemonoMethodBinding
{
	const char* methodName;
	const void* method;
};

struct ICemonoClassBinding
{
	virtual const char* GetNamespace() = 0;
	virtual const char* GetNamespaceExtension() = 0;
	virtual const char* GetClassName() = 0;
	virtual const std::vector<ICemonoMethodBinding> GetMethods() = 0;
};