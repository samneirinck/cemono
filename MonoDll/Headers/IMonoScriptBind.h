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
	IMonoMethodBinding(const char *funcName, const void *func, const char *retType, const char *params)
		: methodName(funcName), method(func), returnType(retType), parameters(params) {}

	const char *methodName;
	const void *method;

	const char *parameters;
	const char *returnType;
};

/// <summary>
/// Simple pre-processor method used to quickly register methods within scriptbinds.
/// We add _'s before the method name to easily distinguish between standard methods and externals (scriptbinds) in C#.
/// </summary>
#define REGISTER_METHOD(method) RegisterMethod(GetMethodBindingFor##method##())

/// <summary>
/// Macro used to declare mono scriptbinds, needed to generate CryScriptbinds.dll.
/// </summary>
#define MonoMethod(retType, method, ...) \
	static retType method(##__VA_ARGS__); \
	public: \
	static IMonoMethodBinding GetMethodBindingFor##method##()  { return IMonoMethodBinding("_" #method, method, #retType, #__VA_ARGS__); } \
	protected: \

/// <summary>
/// Same as MonoMethod, except doesn't apply "_" to method name. Useful when we want the method to be accessible and look good outside of the CryBrary assembly.
/// </summary>
#define ExposedMonoMethod(retType, method, ...) \
	static retType method(##__VA_ARGS__); \
	public: \
	static IMonoMethodBinding GetMethodBindingFor##method##()  { return IMonoMethodBinding(#method, method, #retType, #__VA_ARGS__); } \
	protected: \

/// <summary>
/// </summary>
struct IMonoScriptBind
{
	/// <summary>
	/// Called after the methods in this scriptbind has been collected.
	/// </summary>
	virtual void Release() = 0; // { delete this; }
	/// <summary>
	/// </summary>
	virtual void AddRef() {}

	/// <summary>
	/// The namespace in which the Mono class this scriptbind is tied to resides in; returns "CryEngine" by default if not overridden.
	/// </summary>
	virtual const char *GetNamespace() { return "CryEngine"; }
	/// <summary>
	/// Extends the namespace, i.e. "FlowSystem" if your class is located in namespace "CryEngine.FlowSystem".
	/// </summary>
	virtual const char *GetNamespaceExtension() const { return ""; } 
	/// <summary>
	/// The Mono class which this scriptbind is tied to. Unlike GetNameSpace and GetNameSpaceExtension, this has no default value and MUST be set.
	/// </summary>
	virtual const char *GetClassName() = 0;

	/// <summary>
	/// Returns a vector containing the binded methods this ScriptBind contains.
	/// </summary>
	virtual const std::vector<IMonoMethodBinding> GetMethods() const { return m_methods; }
protected:
	/// <summary>
	/// Pushes a binded method into the array; actually registered by MonoScriptSystem using GetMethods() during IMonoScriptSystem::PostInit().
	/// </summary>
	virtual void RegisterMethod(IMonoMethodBinding method) { m_methods.push_back(method); }

	/// <summary>
	/// The actual vector containing binded methods.
	/// </summary>
	std::vector<IMonoMethodBinding> m_methods;
};

#endif //__IMONOSCRIPTBIND_H__