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

/// <summary>
/// Used to store mono methods for quick accessibility.
/// </summary>
struct IMonoMethodBinding
{
	IMonoMethodBinding(const char *funcName, const void *func)
		: methodName(funcName), method(func) {}

	const char* methodName;
	const void* method;
};

/// <summary>
/// Simple pre-processor method used to quickly register methods within scriptbinds.
/// We add _'s before the method name to easily distinguish between standard methods and externals (scriptbinds) in C#.
/// </summary>
#define REGISTER_METHOD(method) RegisterMethod("_" #method, method)

/// <summary>
/// </summary>
struct IMonoScriptBind
{
	/// <summary>
	/// The namespace in which the Mono class this scriptbind is tied to resides in; returns "CryEngine" by default if not overridden.
	/// </summary>
	virtual const char* GetNamespace() { return "CryEngine"; }
	/// <summary>
	/// Extends the namespace, i.e. "FlowSystem" if your class is located in namespace "CryEngine.FlowSystem".
	/// </summary>
	virtual const char* GetNamespaceExtension() const { return ""; } 
	/// <summary>
	/// The Mono class which this scriptbind is tied to. Unlike GetNameSpace and GetNameSpaceExtension, this has no default value and MUST be set.
	/// </summary>
	virtual const char* GetClassName() = 0;

	/// <summary>
	/// Returns a vector containing the binded methods this ScriptBind contains.
	/// </summary>
	virtual const std::vector<IMonoMethodBinding> GetMethods() const { return m_methods; }
protected:
	/// <summary>
	/// Pushes a binded method into the array; actually registered by MonoScriptSystem using GetMethods() during IMonoScriptSystem::PostInit().
	/// </summary>
	virtual void RegisterMethod(const char *name, const void *method) { m_methods.push_back(IMonoMethodBinding(name, method)); }

	/// <summary>
	/// The actual vector containing binded methods.
	/// </summary>
	std::vector<IMonoMethodBinding> m_methods;
};

#endif //__IMONOSCRIPTBIND_H__