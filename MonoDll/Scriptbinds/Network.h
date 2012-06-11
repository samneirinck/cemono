/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// CryENGINE Network scriptbind
//////////////////////////////////////////////////////////////////////////
// 10/06/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_NETWORK_H__
#define __SCRIPTBIND_NETWORK_H__

#include <MonoCommon.h>
#include <IMonoScriptbind.h>

class CNetwork
	: public IMonoScriptBind
{
public:
	CNetwork();
	~CNetwork();

	// IMonoScriptbind
	virtual const char *GetClassName() { return "Network"; }
	// ~IMonoScriptbind
};

#endif __SCRIPTBIND_NETWORK_H__