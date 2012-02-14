#include "StdAfx.h"
#include "ScriptBind_Renderer.h"

CScriptBind_Renderer::CScriptBind_Renderer()
{
	REGISTER_METHOD(GetViewCamera);
	REGISTER_METHOD(SetViewCamera);
}

MonoCamera CScriptBind_Renderer::GetViewCamera()
{
	MonoCamera cam;

	if(!gEnv)
		return cam;

	if(!gEnv->pSystem)
		return cam;

	CCamera cryCam = gEnv->pSystem->GetViewCamera();

	cam.Position = cryCam.GetPosition();
	cam.ViewDir = (Vec3)cryCam.GetAngles();
	cam.FieldOfView = cryCam.GetFov();

	return cam;
}

void CScriptBind_Renderer::SetViewCamera(MonoCamera cam)
{
	if(!gEnv)
		return;

	if(!gEnv->pSystem)
		return;

	CCamera cryCam;

	cryCam.SetPosition(cam.Position);
	cryCam.SetAngles((Ang3)cam.ViewDir);

	gEnv->pSystem->SetViewCamera(cryCam);
}