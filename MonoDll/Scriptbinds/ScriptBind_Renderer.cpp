#include "StdAfx.h"
#include "ScriptBind_Renderer.h"

#include <IViewSystem.h>
#include <IGameFramework.h>

CScriptBind_Renderer::CScriptBind_Renderer()
{
	REGISTER_METHOD(GetViewCamera);
	REGISTER_METHOD(SetViewCamera);

	REGISTER_METHOD(GetWidth);
	REGISTER_METHOD(GetHeight);

	REGISTER_METHOD(LoadTexture);
	REGISTER_METHOD(DrawTextureToScreen);

	REGISTER_METHOD(CreateRenderTarget);
	REGISTER_METHOD(DestroyRenderTarget);
	REGISTER_METHOD(SetRenderTarget);
}

CCamera CScriptBind_Renderer::ToCryCamera(MonoCamera cam)
{
	CCamera cryCam = gEnv->pSystem->GetViewCamera();

	cryCam.SetPosition(cam.Position);
	cryCam.SetAngles((Ang3)cam.Angles);
	cryCam.SetFrustum(GetWidth(), GetHeight(), DEG2RAD(cam.FieldOfView));

	return cryCam;
}

MonoCamera CScriptBind_Renderer::ToMonoCamera(CCamera cryCam)
{
	MonoCamera cam;

	cam.Position = cryCam.GetPosition();
	cam.Angles = (Vec3)cryCam.GetAngles();
	cam.FieldOfView = cryCam.GetFov();

	return cam;
}

// Externals below
MonoCamera CScriptBind_Renderer::GetViewCamera()
{
	MonoCamera cam;

	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetActiveView())
		{
			auto viewParams = *pView->GetCurrentParams();

			cam.Angles = Vec3(Ang3(viewParams.rotation));
			cam.Position = viewParams.position;
			cam.FieldOfView = viewParams.fov;
		}
	}

	return cam;
}

void CScriptBind_Renderer::SetViewCamera(MonoCamera cam)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetActiveView())
		{
			auto viewParams = *pView->GetCurrentParams();

			viewParams.fov = DEG2RAD(cam.FieldOfView);
			viewParams.rotation = Quat(Ang3(cam.Angles));
			viewParams.position = cam.Position;

			pView->SetCurrentParams(viewParams);
		}
	}
}

int CScriptBind_Renderer::GetWidth()
{
	return gEnv->pRenderer->GetWidth();
}

int CScriptBind_Renderer::GetHeight()
{
	return gEnv->pRenderer->GetHeight();
}

int CScriptBind_Renderer::LoadTexture(mono::string texturePath)
{
	if(ITexture *pTexture = gEnv->pRenderer->EF_LoadTexture(ToCryString(texturePath)))
		return pTexture->GetTextureID();

	return -1;
}

void CScriptBind_Renderer::DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a, float z)
{
	// Could expose the optional args later.
	gEnv->pRenderer->Draw2dImage(xpos, ypos, width, height, textureId, s0, t0, s1, t1, angle, r, g, b, a, z);
}

int CScriptBind_Renderer::CreateRenderTarget(int width, int height, ETEX_Format texFormat)
{
	return gEnv->pRenderer->CreateRenderTarget(width, height, texFormat);
}

void CScriptBind_Renderer::DestroyRenderTarget(int textureId)
{
	gEnv->pRenderer->DestroyRenderTarget(textureId);
}

void CScriptBind_Renderer::SetRenderTarget(int textureId)
{
	gEnv->pRenderer->SetRenderTarget(textureId);
}