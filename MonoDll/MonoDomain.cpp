#include "StdAfx.h"
#include "MonoDomain.h"

#include <MonoCommon.h>

#include <mono/metadata/debug-helpers.h>

CScriptDomain::CScriptDomain(ERuntimeVersion runtimeVersion)
	: m_bRootDomain(true)
{
	const char *version = "v2.0.50727";
	switch(runtimeVersion)
	{
	case eRV_2_50215:
		version = "v2.0.50215";
		break;
	case eRV_2_50727:
		break;

	case eRV_4_20506:
		version = "v4.0.20506";
		break;
	case eRV_4_30128:
		version = "v4.0.30128";
		break;
	case eRV_4_30319:
		version = "v4.0.30319";
		break;
	}

	// Crashing on this line is an indicator of mono being incorrectly configured, Make sure Bin(32/64)/mono.exe, Bin(32/64)/mono-2.0.dll & Engine/Mono are up-to-date.
	m_pDomain = mono_jit_init_version("CryMono", version);
	if(!m_pDomain)
		CryFatalError("Failed to initialize root domain with runtime version %s!", runtimeVersion);
}

CScriptDomain::CScriptDomain(const char *name, bool setActive)
	: m_bRootDomain(false)
{
	m_pDomain = mono_domain_create_appdomain(const_cast<char *>(name), nullptr);

	if(setActive)
		SetActive();
}

CScriptDomain::~CScriptDomain()
{
	if(m_bRootDomain)
		mono_jit_cleanup(m_pDomain);
	else
	{
		if(m_pDomain == mono_domain_get())
			mono_domain_set(mono_get_root_domain(), false);

		mono_domain_finalize(m_pDomain, 2);

		MonoObject *pException;
		try
		{
			mono_domain_try_unload(m_pDomain, &pException);
		}
		catch(char *ex)
		{
			MonoWarning("An exception was raised during ScriptDomain unload: %s", ex);
		}

		if(pException)	
		{			
			MonoWarning("An exception was raised during ScriptDomain unload:");
			MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());		
			MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, nullptr, nullptr);		
			CryLogAlways(ToCryString((mono::string)exceptionString));
		}
	}
}

bool CScriptDomain::SetActive(bool force)
{
	return mono_domain_set(m_pDomain, force) == 1;
}