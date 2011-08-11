#pragma once
#include "Mono.h"

class MonoAPIBinding
{
private:
protected:
	const char* GetNamespace() { return "CryEngine"; }

	virtual const char* GetNamespaceExtension() { return  ""; } // i.e. "API" if your class is located in namespace "CryEngine.API"
	virtual const char* GetClassName() = 0;

public:
	void RegisterAPIBinding(const char *name, const void* method)
	{
		string fullName = GetNamespace();

		if(strcmp(GetNamespaceExtension(), ""))
			fullName.append((string)"." + GetNamespaceExtension());

		fullName.append(".");
		fullName.append(GetClassName());
		fullName.append("::");
		fullName.append(name);

		mono_add_internal_call(fullName, method);
	}
};