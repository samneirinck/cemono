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

struct SViewParams;

struct MonoCamera
{
	Vec3 Angles;
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
	// External methods

	static unsigned int CreateView();
	static void RemoveView(unsigned int viewId);

	static unsigned int GetActiveView();
	static void SetActiveView(unsigned int viewId);

	static SViewParams GetViewParams(unsigned int viewId);
	static void SetViewParams(unsigned int viewId, SViewParams);

	static int GetWidth();
	static int GetHeight();

	// Texture handling
	static int LoadTexture(mono::string path);
	static void DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a, float z);

	static int CreateRenderTarget(int width, int height, ETEX_Format texFormat = eTF_A8R8G8B8);
	static void DestroyRenderTarget(int textureId);
	static void SetRenderTarget(int textureId);
	// ~External methods
};

#endif //__SCRIPTBIND_RENDERER__