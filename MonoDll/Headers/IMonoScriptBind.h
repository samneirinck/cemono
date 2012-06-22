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

#include <IMonoScriptSystem.h>
#include <IMonoAssembly.h>

/// <summary>
/// Simple pre-processor method used to quickly register methods within scriptbinds.
/// We add _'s before the method name to easily distinguish between standard methods and externals (scriptbinds) in C#.
/// </summary>
#define REGISTER_METHOD(method) gEnv->pMonoScriptSystem->RegisterMethodBinding(method, (GetNamespace() + (string)".").append(GetClassName()).append("::").append("_" #method))

#define REGISTER_EXPOSED_METHOD(method) gEnv->pMonoScriptSystem->RegisterMethodBinding(method, (GetNamespace() + (string)".").append(GetClassName()).append("::").append(#method))

/// <summary>
/// </summary>
struct IMonoScriptBind
{
	IMonoScriptBind() : m_pClass(NULL) { }

	/// <summary>
	/// The namespace in which the Mono class this scriptbind is tied to resides in; returns "CryEngine" by default if not overridden.
	/// </summary>
	virtual const char *GetNamespace() { return "CryEngine"; }
	/// <summary>
	/// The Mono class which this scriptbind is tied to. Unlike GetNameSpace and GetNameSpaceExtension, this has no default value and MUST be set.
	/// </summary>
	virtual const char *GetClassName() = 0;
	/// <summary>
	/// Returns the CryBrary class for this scriptbind.
	/// </summary>
	IMonoClass *GetClass()
	{
		if(!m_pClass)
			m_pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetClass(GetClassName(), GetNamespace());

		return m_pClass;
	}

private:
	IMonoClass *m_pClass;
};

#endif //__IMONOSCRIPTBIND_H__