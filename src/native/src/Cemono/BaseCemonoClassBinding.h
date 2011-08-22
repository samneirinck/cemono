#pragma once

#include "Headers/ICemonoClassBinding.h"
#include <mono/jit/jit.h>

#define REGISTER_METHOD(method) RegisterMethod(#method, method)

class BaseCemonoClassBinding : public ICemonoClassBinding
{
public:
	// ICemonoClassBinding interface
	virtual const char* GetNamespace() { return "CryEngine"; }
	virtual const char* GetNamespaceExtension() { return  "API"; } // i.e. "FlowSystem" if your class is located in namespace "CryEngine.FlowSystem"
	virtual const char* GetClassName() = 0;
	virtual const std::vector<ICemonoMethodBinding> GetMethods() { return m_methods; }
	// -ICemonoClassBinding 

protected:
	void RegisterMethod(const char* name, const void* method)
	{
		ICemonoMethodBinding binding;
		
		binding.methodName = name;
		binding.method = method;
		
		m_methods.push_back(binding);
	}
	
	std::vector<ICemonoMethodBinding> m_methods;
};