/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2010.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Controls script variables coming from track view to add some 
control/feedback during cutscenes

-------------------------------------------------------------------------
History:
- 28:04:2010   Created by Benito Gangoso Rodriguez

*************************************************************************/

#include "StdAfx.h"
#include "CinematicInput.h"
#include "Player.h"
#include "PlayerInput.h"
#include "GameActions.h"
#include <IForceFeedbackSystem.h>
#include <IMovieSystem.h>
#include <IViewSystem.h>
#include "GameCVars.h"
#include "GameUtils.h"

CCinematicInput::CCinematicInput()
: m_currentRawInputAngles(0.0f, 0.0f, 0.0f)
, m_cutsceneRunningCount(0)
, m_cutscenesNoPlayerRunningCount(0)
{

}

CCinematicInput::~CCinematicInput()
{
	//Make sure filter are disabled on level unload
	if (g_pGameActions)
	{
		g_pGameActions->FilterCutsceneNoPlayer()->Enable(false);
	}
}

void CCinematicInput::OnBeginCutScene( int cutSceneFlags )
{
	m_cutsceneRunningCount++;

	if (cutSceneFlags & IAnimSequence::NO_PLAYER)
	{
		if (m_cutscenesNoPlayerRunningCount == 0)
		{
			DisablePlayerForCutscenes();
		}
		m_cutscenesNoPlayerRunningCount++;
	}
}

void CCinematicInput::OnEndCutScene( int cutSceneFlags )
{
	m_cutsceneRunningCount = max(m_cutsceneRunningCount - 1, 0);
	if (m_cutsceneRunningCount == 0)
	{
		ClearCutSceneScriptVariables();
	}

	if (cutSceneFlags & IAnimSequence::NO_PLAYER)
	{
		m_cutscenesNoPlayerRunningCount = max(m_cutscenesNoPlayerRunningCount - 1, 0);
		if (m_cutscenesNoPlayerRunningCount == 0)
		{
			ReEnablePlayerAfterCutscenes();
		}
	}
}

void CCinematicInput::Update( float frameTime )
{
	IScriptSystem* pScriptSystem = gEnv->pScriptSystem;
	if (pScriptSystem)
	{
		UpdateForceFeedback(pScriptSystem, frameTime);
		UpdateAdditiveCameraInput(pScriptSystem, frameTime);
	}
}

void CCinematicInput::ClearCutSceneScriptVariables()
{
	IScriptSystem* pScriptSystem = gEnv->pScriptSystem;
	if (pScriptSystem)
	{
		pScriptSystem->SetGlobalValue("Cinematic_RumbleA", 0.0f);
		pScriptSystem->SetGlobalValue("Cinematic_RumbleB", 0.0f);

		pScriptSystem->SetGlobalValue("Cinematic_CameraLookUp", 0.0f);
		pScriptSystem->SetGlobalValue("Cinematic_CameraLookDown", 0.0f);
		pScriptSystem->SetGlobalValue("Cinematic_CameraLookLeft", 0.0f);
		pScriptSystem->SetGlobalValue("Cinematic_CameraLookRight", 0.0f);
	}
}

void CCinematicInput::UpdateForceFeedback( IScriptSystem* pScriptSystem, float frameTime )
{
	float rumbleA = 0.0f, rumbleB = 0.0f;

	pScriptSystem->GetGlobalValue("Cinematic_RumbleA", rumbleA);
	pScriptSystem->GetGlobalValue("Cinematic_RumbleB", rumbleB);

	if ((rumbleA + rumbleB) > 0.0f)
	{
		g_pGame->GetIGameFramework()->GetIForceFeedbackSystem()->AddFrameCustomForceFeedback(rumbleA, rumbleB);
	}
}

void CCinematicInput::UpdateAdditiveCameraInput( IScriptSystem* pScriptSystem, float frameTime )
{
	float lookUpLimit = 0.0f, lookDownLimit = 0.0f;
	float lookLeftLimit = 0.0f, lookRightLimit = 0.0f;

	pScriptSystem->GetGlobalValue("Cinematic_CameraLookUp", lookUpLimit);
	pScriptSystem->GetGlobalValue("Cinematic_CameraLookDown", lookDownLimit);
	pScriptSystem->GetGlobalValue("Cinematic_CameraLookLeft", lookLeftLimit);
	pScriptSystem->GetGlobalValue("Cinematic_CameraLookRight", lookRightLimit);

	lookUpLimit = DEG2RAD(lookUpLimit);
	lookDownLimit = DEG2RAD(lookDownLimit);
	lookLeftLimit = DEG2RAD(lookLeftLimit);
	lookRightLimit = DEG2RAD(lookRightLimit);

	CActor* pClientActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor());
	if (pClientActor)
	{
		CRY_ASSERT(pClientActor->GetActorClass() == CPlayer::GetActorClassType());
		CPlayer* pClientPlayer = static_cast<CPlayer*>(pClientActor);

		IPlayerInput* pIPlayerInput = pClientPlayer->GetPlayerInput();
		if(pIPlayerInput && pIPlayerInput->GetType() == IPlayerInput::PLAYER_INPUT)
		{
			CPlayerInput * pPlayerInput = static_cast<CPlayerInput*>(pIPlayerInput);

			Ang3 rawMouseInput = pPlayerInput->GetRawMouseInput() * 0.25f;
			rawMouseInput.z = - rawMouseInput.z;
			rawMouseInput.x *= (g_pGameCVars->cl_invertMouse == 0) ? 1.0f : -1.0f;
			
			Ang3 rawControllerInput = pPlayerInput->GetRawControllerInput() + rawMouseInput;
			//Yaw angle (Z axis)
			rawControllerInput.z = -clamp((float)__fsel(rawControllerInput.z, rawControllerInput.z * lookRightLimit, rawControllerInput.z * lookLeftLimit), -lookLeftLimit, lookRightLimit);		
			//Pitch angle (X axis)
			rawControllerInput.x *= (g_pGameCVars->cl_invertController == 0) ? 1.0f : -1.0f;
			rawControllerInput.x = clamp((float)__fsel(rawControllerInput.x, rawControllerInput.x * lookUpLimit, rawControllerInput.x * lookDownLimit), -lookDownLimit, lookUpLimit);
			//No roll allowed
			rawControllerInput.y = 0.0f;

			Interpolate(m_currentRawInputAngles, rawControllerInput, 2.5f, frameTime);

			IView* pActiveView = g_pGame->GetIGameFramework()->GetIViewSystem()->GetActiveView();
			if (pActiveView)
			{
				pActiveView->SetFrameAdditiveCameraAngles(m_currentRawInputAngles);
			}
		}
	}
}

void CCinematicInput::ReEnablePlayerAfterCutscenes()
{
	CActor* pClientActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor());
	if (pClientActor)
	{
		CRY_ASSERT(pClientActor->GetActorClass() == CPlayer::GetActorClassType());
		CPlayer* pClientPlayer = static_cast<CPlayer*>(pClientActor);

		pClientPlayer->OnEndCutScene();
	}

	g_pGameActions->FilterCutsceneNoPlayer()->Enable(false);
}

void CCinematicInput::DisablePlayerForCutscenes()
{
	CActor* pClientActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor());
	if (pClientActor)
	{
		CRY_ASSERT(pClientActor->GetActorClass() == CPlayer::GetActorClassType());
		CPlayer* pClientPlayer = static_cast<CPlayer*>(pClientActor);

		pClientPlayer->OnBeginCutScene();
	}

	g_pGameActions->FilterCutsceneNoPlayer()->Enable(true);
}
