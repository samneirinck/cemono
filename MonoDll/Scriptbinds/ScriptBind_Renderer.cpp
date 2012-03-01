#include "StdAfx.h"
#include "ScriptBind_Renderer.h"

#include <IViewSystem.h>
#include <IGameFramework.h>

CScriptBind_Renderer::CScriptBind_Renderer()
{
	REGISTER_METHOD(CreateView);
	REGISTER_METHOD(RemoveView);

	REGISTER_METHOD(GetActiveView);
	REGISTER_METHOD(SetActiveView);

	REGISTER_METHOD(GetViewParams);
	REGISTER_METHOD(SetViewParams);

	REGISTER_METHOD(GetWidth);
	REGISTER_METHOD(GetHeight);

	REGISTER_METHOD(ScreenToWorld);

	REGISTER_METHOD(DrawTextToScreen);

	REGISTER_METHOD(LoadTexture);
	REGISTER_METHOD(DrawTextureToScreen);

	REGISTER_METHOD(CreateRenderTarget);
	REGISTER_METHOD(DestroyRenderTarget);
	REGISTER_METHOD(SetRenderTarget);
}

// Externals below
unsigned int CScriptBind_Renderer::CreateView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetViewId(pViewSystem->CreateView());

	return 0;
}

void CScriptBind_Renderer::DrawTextToScreen(float xpos, float ypos, float fontSize, mono::array color, bool center, mono::string text)
{
	IMonoArray *pArray = *color;
	if(pArray->GetSize() != 4)
		return;

	float actualColor[] = { 
		pArray->GetItemUnboxed<float>(0), 
		pArray->GetItemUnboxed<float>(1),
		pArray->GetItemUnboxed<float>(2),
		pArray->GetItemUnboxed<float>(3),
	};

	gEnv->pRenderer->Draw2dLabel(xpos, ypos, fontSize, actualColor, center, ToCryString(text));
}

void CScriptBind_Renderer::RemoveView(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetViewByEntityId(viewId))
			pViewSystem->RemoveView(viewId);
	}
}

unsigned int CScriptBind_Renderer::GetActiveView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetActiveViewId();

	return 0;
}

void CScriptBind_Renderer::SetActiveView(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetViewByEntityId(viewId))
			pViewSystem->SetActiveView(viewId);
	}
}

SViewParams CScriptBind_Renderer::GetViewParams(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetActiveView())
			return *pView->GetCurrentParams();
	}

	return SViewParams();
}

void CScriptBind_Renderer::SetViewParams(unsigned int viewId, SViewParams viewParams)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetActiveView())
		{
			viewParams.SaveLast();

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

Vec3 CScriptBind_Renderer::ScreenToWorld(int x, int y)
{
	auto camMatrix = gEnv->pRenderer->GetCamera().GetMatrix();

	auto aspectRatio = GetWidth() / GetHeight();

	auto dx = (x / (GetWidth() * 0.5f) - 1) / aspectRatio;
	auto dy = 1 - y / (GetHeight() * 0.5f);

	auto xr = camMatrix * Vec3(1,0,0);
	auto yr = camMatrix * Vec3(0,1,0);
	auto zr = camMatrix * Vec3(0,0,1);

	xr *= (float)aspectRatio;
	zr *= -(0.5 * aspectRatio * tan(0.5 * gEnv->pRenderer->GetCamera().GetFov()));

	return (camMatrix.GetTranslation(), dx * xr + dy * yr + zr);
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