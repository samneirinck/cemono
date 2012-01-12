/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScriptBind interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 20/11/2011 : Created by Filip 'i59' Lundgren (Based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __IMONOSCRIPTBIND_H__
#define __IMONOSCRIPTBIND_H__

#include <vector>

struct IMonoMethodBinding
{
	IMonoMethodBinding(const char *funcName, const void *func)
		: methodName(funcName), method(func) {}

	const char* methodName;
	const void* method;
};

#define REGISTER_METHOD(method) RegisterMethod("_" #method, method)

struct IMonoScriptBind
{
	// Returns "CryEngine" by default.
	virtual const char* GetNamespace() { return "CryEngine"; }
	// Extends the namespace, i.e. "FlowSystem" if your class is located in namespace "CryEngine.FlowSystem".
	virtual const char* GetNamespaceExtension() const { return ""; } 
	virtual const char* GetClassName() = 0;

	virtual const std::vector<IMonoMethodBinding> GetMethods() const { return m_methods; }
protected:
	virtual void RegisterMethod(const char *name, const void *method) { m_methods.push_back(IMonoMethodBinding(name, method)); }

	std::vector<IMonoMethodBinding> m_methods;
};

#endif //__IMONOSCRIPTBIND_H__