////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2010.
// -------------------------------------------------------------------------
//  File name:   LookAim_Helper.h
//  Version:     v1.00
//  Created:     25/01/2010 by Sven Van Soom
//  Description: Helper class for setting up and updating Looking and Aiming
//
//		Currently it's only a very thin wrapper adding blending to the 
//		LookAtSimple pose modifier to allow combined looking & aiming. It still
//		uses the old LookIK system when available.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include <ICryAnimation.h>
#include "LookAim_Helper.h"
#include "Player.h"
#include <CryExtension/CryCreateClassInstance.h>


CLookAim_Helper::CLookAim_Helper()
: m_initialized(false),
  m_canUseLookAtSimple(false),
	m_canUseLookAtComplex(false),
	m_availableHandsForFiring ((1<<IItem::eIH_Left) | (1<<IItem::eIH_Right))
{
}


void CLookAim_Helper::UpdateLook(CPlayer* pPlayer, ICharacterInstance* pCharacter, bool bEnabled, f32 FOV, const Vec3& targetGlobal, const uint32 lookIKLayer, const f32 *customBlends /*= NULL*/)
{
	if (!m_initialized)
	{
		Init(pPlayer, pCharacter);
	}

	bool useLookAtComplex;
	bool useLookAtSimple;

	if (m_canUseLookAtComplex)
	{
		// for now just use the old 'complex' look at method until we sort out how to properly blend old and new look at
		useLookAtComplex = true;
		useLookAtSimple = false;
	}
	else
	{
		useLookAtComplex = true; // for backwards compatibility reasons we still update the old look-at even when m_canUseLookAtComplex is false
		useLookAtSimple = m_canUseLookAtSimple;
	}

	// ---------------------------
	// Complex (old style) Look-At
	// ---------------------------

	ISkeletonPose * pSkeletonPose = pCharacter->GetISkeletonPose();
	
	pSkeletonPose->SetLookIK(useLookAtComplex && bEnabled, FOV, targetGlobal, lookIKLayer);

	// ---------------------------
	// Simple Head-Only Look-At
	// ---------------------------

	if (m_canUseLookAtSimple)
	{
		float frameTime = gEnv->pTimer->GetFrameTime();

		// Fade In/Out the Weight
		m_lookAtWeight = bEnabled ? CLAMP(m_lookAtWeight + (frameTime * m_lookAtFadeInSpeed), 0.0f, 1.0f) : CLAMP(m_lookAtWeight - (frameTime * m_lookAtFadeOutSpeed), 0.0f, 1.0f);

		// Blend To The Target
		if (targetGlobal.IsValid())
		{
			m_lookAtTargetGlobal = targetGlobal;
		}
		SmoothCD(m_lookAtInterpolatedTargetGlobal, m_lookAtTargetRate, frameTime, m_lookAtTargetGlobal, m_lookAtTargetSmoothTime);

		// Push the LookAtSimple PoseModifier
		if (useLookAtSimple && (m_lookAtWeight > 0.0f))
		{
			m_lookAtSimple->SetTargetGlobal(m_lookAtInterpolatedTargetGlobal);
			m_lookAtSimple->SetWeight(m_lookAtWeight);
			pCharacter->GetISkeletonAnim()->PushLayer(cryinterface_cast<IAnimationPoseModifier>(m_lookAtSimple));
		}
	}
}


void CLookAim_Helper::Init(CPlayer* pPlayer, ICharacterInstance* pCharacter)
{
	if (m_initialized)
		return;

	m_initialized = true;

	// Looking
	SActorParams* params = pPlayer->GetActorParams();
	int16 lookAtSimpleHeadJoint = pCharacter->GetISkeletonPose()->GetJointIDByName(params->lookAtSimpleHeadBoneName);

	m_canUseLookAtSimple = (lookAtSimpleHeadJoint != -1);
	m_canUseLookAtComplex = params->canUseComplexLookIK;

	if (m_canUseLookAtSimple)
	{
		if (!m_lookAtSimple.get())
			CryCreateClassInstance<AnimPoseModifier::CLookAtSimple>("AnimationPoseModifier_LookAtSimple", m_lookAtSimple);

		m_lookAtWeight = 1.0;
		m_lookAtFadeInSpeed = 2.0f; // fade in in 0.5 second(s)
		m_lookAtFadeOutSpeed = 2.0f; // fade out in 0.5 second(s)
		m_lookAtTargetSmoothTime = 0.1f; // smoothly blend towards target in this amount of seconds
		m_lookAtInterpolatedTargetGlobal.zero();
		m_lookAtTargetRate.zero();
		m_lookAtTargetGlobal.zero();
		m_lookAtSimple->SetJointId(lookAtSimpleHeadJoint);
	}

	// Aiming
	m_availableHandsForFiring = (1<<IItem::eIH_Left) | (1<<IItem::eIH_Right);
	m_lastAimPoseAnimID = 0;
	m_lastAimYaw = 0.0f;
	m_aimIsSwitchingArms = false;
	m_vLastAimTarget.Set(0.0f, 0.0f, 0.0f);
}