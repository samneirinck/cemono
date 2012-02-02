/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScriptBind automatical registration.
//////////////////////////////////////////////////////////////////////////
// 19/01/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_AUTOREG_SCRIPTBIND_H__
#define __I_MONO_AUTOREG_SCRIPTBIND_H__

#include <IMonoScriptBind.h>

/// <summary>
/// Used as an extension to IMonoScriptBind to easily "automatically" register scriptbinds.
/// Currently only used within CryMono.dll
/// </summary>
struct IMonoAutoRegScriptBind : public IMonoScriptBind
{
	IMonoAutoRegScriptBind()
	{
		m_pNext = 0;
		if (!m_pLast)
			m_pFirst = this;
		else
			m_pLast->m_pNext = this;
		m_pLast = this;
	}

	static IMonoAutoRegScriptBind *m_pFirst;
	static IMonoAutoRegScriptBind *m_pLast;
	IMonoAutoRegScriptBind *m_pNext;
};

#if (defined(WIN32) || defined(WIN64)) && defined(_LIB)
	#define CRY_EXPORT_STATIC_LINK_VARIABLE( Var ) \
		extern "C" { int lib_func_##Var() { return (int)&Var; } } \
		__pragma( message("#pragma comment(linker,\"/include:_lib_func_"#Var"\")") )
#else
	#define CRY_EXPORT_STATIC_LINK_VARIABLE( Var )
#endif

#define REGISTER_SCRIPTBIND(scriptbind) \
	scriptbind *g_AutoReg##scriptbind = new scriptbind(); \
	CRY_EXPORT_STATIC_LINK_VARIABLE( g_AutoReg##scriptbind );

#endif