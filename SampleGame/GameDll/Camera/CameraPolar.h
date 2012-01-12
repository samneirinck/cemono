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

#ifndef CAMERA_POLAR_H
#define CAMERA_POLAR_H

#include "CameraCommon.h"

//maximum/minimum pitch to prevent camera flip-over/-under
const static float g_fCameraPolarMaxPitch = gf_PI - g_fCamError;
const static float g_fCameraPolarMinPitch = g_fCamError;

// Polar coordinates
// imaging a 3D point being represented by two angles :
// yaw/theta is the rotation around the height axis in 360°/2PI
// pitch/phi is the rotation around the "forward" axis in 180°/PI
// a CameraPolar represents any single 3d point on a unified sphere around (0,0,0)
class CCameraPolar 
{
public:

	CCameraPolar() : m_fYaw(0.0f), m_fPitch(0.0f)
	{
	}

	CCameraPolar(float fYaw, float fPitch)
	{
		Set(fYaw, fPitch);
	}

	CCameraPolar(const Vec3 &vDir)
	{
		Set(vDir);
	}

	//get polar coordinates
	float GetYaw() const { return m_fYaw; }
	float GetPitch() const { return m_fPitch; }
	//get a direction vector (normalized) in the direction of the polar
	Vec3 GetDir() const;

	//set polar coordinates
	void Set(float fYaw, float fPitch);
	//set polar coordinates from a direction (normalized point)
	void Set(const Vec3 &vDir);
	void SetYaw(float fYaw)
	{
		m_fYaw = fYaw;
		Wrap2PI(m_fYaw);
	}
	void SetPitch(float fPitch)
	{
		m_fPitch = clamp(fPitch, g_fCameraPolarMinPitch, g_fCameraPolarMaxPitch);
	}

private:

	float m_fYaw;
	float m_fPitch;
};

#endif