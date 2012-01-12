/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera polar coordinates, the basis of the camera coordinate system

-------------------------------------------------------------------------
History:
- 01:2009 : Created By Jan Müller

*************************************************************************/

#include "StdAfx.h"
#include "CameraPolar.h"

Vec3 CCameraPolar::GetDir() const
{
	Vec3 vDir = Vec3Constants<float>::fVec3_OneY;

	//assume normalized vector and compute coordinates
	float fSinPitch = cry_sinf(m_fPitch);
	vDir.x = -cry_sinf(m_fYaw) * fSinPitch;
	vDir.y = cry_cosf(m_fYaw) * fSinPitch;
	vDir.z = cry_cosf(m_fPitch);

	//directions have to be normalized
	CRY_ASSERT(fabsf(vDir.len() - 1.0f) < g_fCamError);

	return vDir;
}

void CCameraPolar::Set(const Vec3 &vDir)
{
	//directions must be normalized
	CRY_ASSERT(fabsf(vDir.len() - 1.0f) < g_fCamError);

	//yaw in 0 .. 2PI, starting at FORWARD (0,1,0)
	m_fYaw = cry_acosf(vDir.y);
	if(vDir.x > 0)
		m_fYaw = gf_PI2 - m_fYaw;

	//pitch in 0 .. PI, starting at UP (0,0,1)
	m_fPitch = fabsf(cry_acosf(vDir.z));
	m_fPitch = clamp(m_fPitch, g_fCameraPolarMinPitch, g_fCameraPolarMaxPitch);
}

void CCameraPolar::Set(float fYaw, float fPitch)
{
	m_fYaw = fYaw;
	Wrap2PI(m_fYaw);
	m_fPitch = fPitch;
	m_fPitch = clamp(m_fPitch, g_fCameraPolarMinPitch, g_fCameraPolarMaxPitch);
}
