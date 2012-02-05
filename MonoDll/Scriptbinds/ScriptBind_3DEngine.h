/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// 3DEngine scriptbind
//////////////////////////////////////////////////////////////////////////
// 13/01/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_3DENGINE__
#define __SCRIPTBIND_3DENGINE__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

class CScriptBind_3DEngine : public IMonoScriptBind
{
public:
	CScriptBind_3DEngine();
	~CScriptBind_3DEngine() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "Engine"; }
	// ~IMonoScriptBind
};

#endif //__SCRIPTBIND_3DENGINE__