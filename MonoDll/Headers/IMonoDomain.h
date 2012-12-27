/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2012.
//////////////////////////////////////////////////////////////////////////
// IMonoDomain interface to handle script / app domains.
//////////////////////////////////////////////////////////////////////////
// 03/05/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_DOMAIN_H__
#define __I_MONO_DOMAIN_H__

struct IMonoAssembly;

struct IMonoDomain
{
	/// <summary>
	/// Deletes and cleans used objects in the domain.
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Sets this domain as the currently active one.
	/// </summary>
	virtual bool SetActive(bool force = false) = 0;
	/// <summary>
	/// Determines if this domain is currently active.
	/// </summary>
	virtual bool IsActive() = 0;

	/// <summary>
	/// If true, this is the primary domain.
	/// </summary>
	virtual bool IsRoot() = 0;

	/// <summary>
	/// Loads an assembly into the domain.
	/// </summary>
	virtual IMonoAssembly *LoadAssembly(const char *file, bool shadowCopy = false, bool convertPdbToMdb = true) = 0;
};

/// <summary>
/// Runtime version enum used when creating root domains.
/// When updating Mono version, verify runtime versions in domain.c, and update CScriptDomain(ERuntimeVersion) constructor accordingly.
/// </summary>
enum ERuntimeVersion
{
	eRV_2_50215,
	/// <summary>
	/// The default / most stable runtime.
	/// </summary>
	eRV_2_50727,

	eRV_4_20506,
	eRV_4_30128,
	eRV_4_30319
};

#endif //__I_MONO_DOMAIN_H__