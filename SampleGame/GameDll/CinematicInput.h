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

#pragma once

#ifndef _CINEMATIC_INPUT_H_
#define _CINEMATIC_INPUT_H_

class CCinematicInput
{
public:

	CCinematicInput();
	~CCinematicInput();

	void OnBeginCutScene(int cutSceneFlags);
	void OnEndCutScene(int cutSceneFlags);

	void Update(float frameTime);

	ILINE bool IsAnyCutSceneRunning() const 
	{ 
		return (m_cutsceneRunningCount > 0); 
	}

private:

	void UpdateForceFeedback(IScriptSystem* pScriptSystem, float frameTime);
	void UpdateAdditiveCameraInput(IScriptSystem* pScriptSystem, float frameTime);

	void ClearCutSceneScriptVariables();
	void DisablePlayerForCutscenes();
	void ReEnablePlayerAfterCutscenes();

	Ang3	m_currentRawInputAngles;
	int		m_cutsceneRunningCount;
	int		m_cutscenesNoPlayerRunningCount;
};

#endif