/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2012.
//////////////////////////////////////////////////////////////////////////
// IMonoDomain interface to handle script / app domains.
//////////////////////////////////////////////////////////////////////////
// 03/05/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPT_DOMAIN_H_
#define __SCRIPT_DOMAIN_H_

#include <IMonoDomain.h>

class CScriptDomain : public IMonoDomain
{
public:
	// Create root domain
	CScriptDomain(ERuntimeVersion runtimeVersion = eRV_2_50727);
	CScriptDomain(const char *name, bool setActive = false);
	~CScriptDomain();

	// IMonoDomain
	virtual void Release() override { delete this; }

	virtual bool SetActive(bool force = false)  override;
	virtual bool IsActive() override { return m_pDomain == mono_domain_get(); }

	virtual bool IsRoot() override { return m_bRootDomain; }
	// ~IMonoDomain

private:

	MonoDomain *m_pDomain;

	bool m_bRootDomain;
};

#endif //__SCRIPT_DOMAIN_H_