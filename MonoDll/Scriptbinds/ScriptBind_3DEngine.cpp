#include "StdAfx.h"
#include "ScriptBind_3DEngine.h"

CScriptBind_3DEngine::CScriptBind_3DEngine()
{
	REGISTER_METHOD(RenderWorld);
}

void CScriptBind_3DEngine::RenderWorld(int renderFlags, MonoCamera camera)
{
	gEnv->p3DEngine->RenderWorld(renderFlags, &CScriptBind_Renderer::ToCryCamera(camera), 1, "monoCam");
}