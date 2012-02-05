/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Renderer scriptbind
//////////////////////////////////////////////////////////////////////////
// 13/01/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_RENDERER__
#define __SCRIPTBIND_RENDERER__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

class CScriptBind_Renderer : public IMonoScriptBind
{
public:
	CScriptBind_Renderer();
	~CScriptBind_Renderer() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "Renderer"; }
	// ~IMonoScriptBind
};

#endif //__SCRIPTBIND_RENDERER__