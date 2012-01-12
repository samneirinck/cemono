/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.

-------------------------------------------------------------------------
History:
- 31:05:2010  Created by Jens Schöbel

*************************************************************************/

#ifndef STEREOFRAMEWORK_H
#define STEREOFRAMEWORK_H

#include "Stereo3D/StereoZoom.h"
#include "RayCastQueue.h"

namespace Stereo3D
{	
  void Update(float deltaTime);

  namespace Zoom
  {
    void SetFinalPlaneDist(float planeDist, float transitionTime);
    void SetFinalEyeDist(float eyeDist, float transitionTime);
    void ReturnToNormalSetting(float);
  } // namespace zoom

  namespace Weapon{
    const int MAX_RAY_IDS = 5;
    const QueuedRayID INVALID_RAY_ID = 0;

    class CWeaponCheck
    {
    public:
      CWeaponCheck() : m_closestCastDist(1000.f) {}
      ~CWeaponCheck();
      void Update(float deltaTime);
      void OnRayCastResult(const QueuedRayID &rayID, const RayCastResult &result);
      float GetCurrentPlaneDist();

    private:
      void CastRays();

      float  m_closestDist;
      float  m_closestCastDist;
      QueuedRayID m_rayIDs[MAX_RAY_IDS];
      uint8  m_numFramesWaiting;
      uint8  m_numResults;
    };
  } // 
} // namespace Stereo3D

#endif
