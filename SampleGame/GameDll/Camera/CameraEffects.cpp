/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera effects

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#include "StdAfx.h"
#include "CameraEffects.h"
#include "Player.h"
#include "CameraInputHelper.h"

/*void CCameraEffects::CamShake()
{
	IActor *pClient = g_pGame->GetIGameFramework()->GetClientActor();
	CPlayer *pHero = static_cast<CPlayer*>(pClient);
	if(!pHero)
		return;

	CScreenEffects *pScreenFX = pHero->GetScreenEffects();
	if(!pScreenFX)
		return;

	pScreenFX->CamShake(Vec3(0.0f,0.0f,0.0f), Vec3(cry_frand(),cry_frand(),cry_frand()), 10.5f, 0.2f, 4.0f);
}*/

void CCameraEffects::UpdateTreeTransparency(const Vec3 &camPos, const Vec3 &targetPos)
{
	if(!g_pGameCVars->cl_enable_tree_transparency)
		return;

	static ICVar *pTreeTransCvar(gEnv->pConsole->GetCVar("e_VegetationAlphaBlendNear"));
	//get some minimum distance plus the camera distance
	float camDist = max(0.0f, -0.5f + (camPos - targetPos).len());/*1.0f + (camPos - targetPos).len()*/;
	if(pTreeTransCvar)
	{
		//set effect distance
		pTreeTransCvar->Set(camDist);
	}
}

void CCameraEffects::UpdateDOF()
{
	// Kevin - Below to be migrated to Flowgraph camera to avoid conflict with DOF logic preexisting

	/*CPlayer *pHero = CPlayer::GetHero();
	if(!pHero)
		return;

	//adding camera dof blur depending on rotation speed
	//unfortunately this stuff is currently FG controlled, which overwrites the code settings when active
	I3DEngine *pEngine = gEnv->p3DEngine;
	float fRotationBlur = min(fabsf(pHero->GetCameraInputHelper()->GetYawDelta()) * 25.0f, 1.0f);
	if(fRotationBlur > 0.3f)
	{
		pEngine->SetPostEffectParam("Dof_Active", 1.0f);
		pEngine->SetPostEffectParam("Dof_FocusMax", 100.0f);
		pEngine->SetPostEffectParam("Dof_FocusMin", 50.0f);
		pEngine->SetPostEffectParam("Dof_FocusLimit", 100.0f);
		pEngine->SetPostEffectParam("Dof_FocusRange", 250.0f - fRotationBlur*200.0f);
	}*/
}