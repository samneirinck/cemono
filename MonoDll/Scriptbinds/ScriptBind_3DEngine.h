/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// 3DEngine scriptbind
//////////////////////////////////////////////////////////////////////////
// 13/01/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_RENDERER__
#define __SCRIPTBIND_RENDERER__

#include <MonoCommon.h>
#include <IMonoAutoRegScriptBind.h>

class CScriptBind_3DEngine : public IMonoAutoRegScriptBind
{
public:
	CScriptBind_3DEngine();
	~CScriptBind_3DEngine() {}

	// IMonoScriptBind
	virtual void Release() { delete this; }

	virtual const char *GetClassName() { return "Engine"; }
	// ~IMonoScriptBind
};

#endif //__SCRIPTBIND_RENDERER__