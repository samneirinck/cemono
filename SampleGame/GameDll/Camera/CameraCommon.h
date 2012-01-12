/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera helper functions

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#ifndef CAMERA_COMMON_H
#define CAMERA_COMMON_H

//init statics
const static float g_fCamError = 0.0001f;
const static float gf_PIHalf = gf_PI * 0.5f;

// Spherical coordinate system.
struct SSpherical 
{
	float m_fYaw;
	float m_fPitch;
	float m_fDist;

	SSpherical() : m_fYaw(0.0f), m_fPitch(0.0f), m_fDist(0.0f)
	{
	}

	SSpherical(float fYaw, float fPitch)
	{
		Set(fYaw, fPitch, 1.0f);
	}

	void Set(float fYaw, float fPitch, float fDist)
	{
		m_fYaw = fYaw;
		m_fPitch = fPitch;
		m_fDist = fDist;
	}
};

//wrap angle to 0..2PI
inline void Wrap2PI(float &fYaw)
{
	//wrap to 0..2PI
	/*if(fYaw > gf_PI2)
	{
	int iPiDiv = int(fYaw / gf_PI2);
	fYaw = fYaw - iPiDiv * gf_PI2;
	}
	else if(fYaw < 0.0f)
	{
	int iPiDiv = int(fYaw / -gf_PI2);
	fYaw = fYaw + iPiDiv * gf_PI2;
	fYaw = gf_PI2 - fYaw;
	}*/
	if(fYaw >= gf_PI2)
		fYaw -= gf_PI2;
	else if(fYaw <= 0.0f)
		fYaw = gf_PI2 + fYaw;
}

// Spherical coordinates conversion
inline Vec3 SphericalToCartesian(float fYaw, float fPitch, float fDist)
{
	Vec3 vPos;
	float fSinPitch = cry_sinf(fPitch);
	vPos.x = fDist * cry_cosf(fYaw) * fSinPitch;
	vPos.y = fDist * cry_sinf(fYaw) * fSinPitch;
	vPos.z = fDist * cry_cosf(fPitch);

	return vPos;
}

inline void CartesianToSpherical(const Vec3 &vPos, float &fYaw, float &fPitch, float &fDist)
{
	fYaw = cry_atan2f(vPos.y,vPos.x);
	fDist = vPos.len();
	float distPitch = cry_fabsf(fDist) > 0.0001f ? fDist : fDist >=0 ? 0.0001f : -0.0001f;
	float fDot = vPos.z / distPitch;
	fDot = clamp(fDot, -1.0f, 1.0f);
	fPitch = cry_acosf(fDot);
}

inline Vec3 SphericalToCartesian(const SSpherical &sphCoord)
{
	return SphericalToCartesian(sphCoord.m_fYaw, sphCoord.m_fPitch, sphCoord.m_fDist);
}

inline void CartesianToSpherical(const Vec3 &vPos, SSpherical &sph)//float &yaw,float &pitch,float &dist)
{
	CartesianToSpherical(vPos, sph.m_fYaw, sph.m_fPitch, sph.m_fDist);
}

//update step for interpolation from "A" to "B" in "t" seconds
inline float InterpolateTo(float fFromValue, float fTargetValue, float fTimeInSeconds)
{
	float fFrameTime = max(gEnv->pTimer->GetFrameTime(), g_fCamError);
	float fInterpolationWeight = max(2.0f, fTimeInSeconds / fFrameTime);
	return ((fInterpolationWeight - 1.0f) * fFromValue + fTargetValue) / fInterpolationWeight;
}

inline Vec3 InterpolateTo(const Vec3 &fFromValue, const Vec3 &fTargetValue, float fTimeInSeconds)
{
	Vec3 vResult(ZERO);
	vResult.x = InterpolateTo(fFromValue.x, fTargetValue.x, fTimeInSeconds);
	vResult.y = InterpolateTo(fFromValue.y, fTargetValue.y, fTimeInSeconds);
	vResult.z = InterpolateTo(fFromValue.z, fTargetValue.z, fTimeInSeconds);
	return vResult;
}

#endif