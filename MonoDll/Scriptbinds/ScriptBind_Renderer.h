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

struct MonoCamera
{
	Vec3 ViewDir;
	Vec3 Position;

	float FieldOfView;
};

class CScriptBind_Renderer : public IMonoScriptBind
{
public:
	CScriptBind_Renderer();
	~CScriptBind_Renderer() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "Renderer"; }
	// ~IMonoScriptBind

protected:

	static MonoCamera GetViewCamera();
	static void SetViewCamera(MonoCamera);

	static int GetWidth();
	static int GetHeight();
};

#endif //__SCRIPTBIND_RENDERER__