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

class CScriptAssembly;

class CScriptDomain : public IMonoDomain
{
public:
	// Create root domain
	CScriptDomain(ERuntimeVersion runtimeVersion = eRV_2_50727);
	CScriptDomain(const char *name, bool setActive = false);
	CScriptDomain(MonoDomain *pMonoDomain) : m_pDomain(pMonoDomain), m_bRootDomain(false) {}
	~CScriptDomain();

	// IMonoDomain
	virtual void Release() override { delete this; }

	virtual bool SetActive(bool force = false)  override;
	virtual bool IsActive() override { return m_pDomain == mono_domain_get(); }

	virtual bool IsRoot() override { return m_bRootDomain; }

	virtual IMonoAssembly *LoadAssembly(const char *file, bool shadowCopy = false, bool convertPdbToMdb = true) override;
	// ~IMonoDomain

	MonoDomain *GetMonoDomain() { return m_pDomain; }

	CScriptAssembly *TryGetAssembly(MonoImage *pImage);
	void OnAssemblyReleased(CScriptAssembly *pAssembly);

private:
	MonoDomain *m_pDomain;

	std::vector<CScriptAssembly *> m_assemblies;

	bool m_bRootDomain;
};

#endif //__SCRIPT_DOMAIN_H_