#pragma once
#include "Mono.h"

class MonoAPIBinding
{
private:
protected:
	const char* GetNamespace()
	{
		return "CryEngine.API";
	}
	virtual const char* GetClassName() = 0;

public:
	void RegisterAPIBinding(const char *name, const void* method)
	{
		string fullName = GetNamespace();
		fullName.append(".");
		fullName.append(GetClassName());
		fullName.append("::");
		fullName.append(name);

		mono_add_internal_call(fullName, method);
	}	
};