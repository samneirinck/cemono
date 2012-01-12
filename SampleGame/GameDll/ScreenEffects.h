// ScreenEffects - Allows for simultaneous and queued blending of effects
//  John Newfield
//  
//  23-1-2008: Refactored by Benito G.R.

#ifndef _SCREEN_EFFECTS_H_
#define _SCREEN_EFFECTS_H_

#pragma once

#include "BlendTypes.h"
#include "BlendedEffect.h"
#include "BlendNode.h"

//-ScreenEffects------------------------
class CScreenEffects
{

public:

	//Some prefenided group IDs
	enum ScreenEffectsGroupId
	{
		eSFX_GID_RBlur			= 1,
		eSFX_GID_SmokeBlur	= 2,
		eSFX_GID_ZoomIn			= 3,
		eSFX_GID_ZoomOut		= 4,
		eSFX_GID_HitReaction= 5,
		eSFX_GID_Last
	};

	CScreenEffects(IActor *owner);
	virtual ~CScreenEffects(void);
	// Start a blend
	virtual void StartBlend(IBlendedEffect *effect, IBlendType *blendType, float speed, int blendGroup);
	virtual bool HasJobs(int blendGroup);
	virtual int GetUniqueID();

	// Reset a blend group (do not delete the group)
	virtual void ResetBlendGroup(int blendGroup, bool resetScreen = false);
	virtual void ResetAllBlendGroups(bool resetScreen = false);

	// Clear a blend group (deletes running blends)
	virtual void ClearBlendGroup(int blendGroup, bool resetScreen = false);
	virtual void ClearAllBlendGroups(bool resetScreen = false);

	// Reset the screen to default values
	virtual void ResetScreen();
	virtual void Update(float frameTime);
	virtual void PostUpdate(float frameTime);

	// Camera shake
	virtual void CamShake(Vec3 shiftShake, Vec3 rotateShake, float freq, float shakeTime, float randomness = 0, int shakeID = 5);

	virtual void EnableBlends(bool enable) {m_enableBlends = enable;};
	virtual void EnableBlends(bool enable, int blendGroup);

	// Update x/y coords
	virtual void SetUpdateCoords(const char *coordsXname, const char *coordsYname, Vec3 pos);

	// Util
	virtual float GetCurrentFOV();

	void GetMemoryUsage(ICrySizer * s) const;

private:

	// Maps blend group IDs to blend groups
	std::map<int, CBlendGroup*> m_blends;
	std::map<int, bool> m_enabledGroups;
	int     m_curUniqueID;
	bool    m_enableBlends;
	bool    m_updatecoords;
	string  m_coordsXname;
	string  m_coordsYname;
	Vec3    m_coords3d;
	IActor* m_ownerActor;
};

#endif