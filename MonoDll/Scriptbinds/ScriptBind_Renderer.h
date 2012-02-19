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

	static CCamera ToCryCamera(MonoCamera camera);
	static MonoCamera ToMonoCamera(CCamera camera);
protected:
	// External methods
	static MonoCamera GetViewCamera();
	static void SetViewCamera(MonoCamera);

	static int GetWidth();
	static int GetHeight();

	// Texture handling
	static int LoadTexture(mono::string path);
	static void DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId);

	static int CreateRenderTarget(int width, int height, ETEX_Format texFormat = eTF_A8R8G8B8);
	static void DestroyRenderTarget(int textureId);
	static void SetRenderTarget(int textureId);
	// ~External methods
};

#endif //__SCRIPTBIND_RENDERER__