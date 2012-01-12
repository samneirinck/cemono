/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera Manager.
-------------------------------------------------------------------------
History:
- 24:04:2008: Created by Nick Hesketh
- 08:2008: Continued by Jan Müller

*************************************************************************/


#include "StdAfx.h"
#include "CameraManager.h"
#include "GameCVars.h"
#include "CameraOverrides.h"
#include "Player.h"
#include "CameraView.h"

bool CCameraManager::g_bChangedCamera = false;

void SCamModeSettings::InterpolateTo(SCamModeSettings &targetSettings, float interpolationTime)
{
	float frameTime = max(0.00001f, gEnv->pTimer->GetFrameTime());
	float interpolationWeight = max(2.0f, interpolationTime / frameTime);

	if(dist != targetSettings.dist)
		dist = ((interpolationWeight - 1.0f) * dist + targetSettings.dist) / interpolationWeight;

	if(maxDist != targetSettings.maxDist)
		maxDist = ((interpolationWeight - 1.0f) * maxDist + targetSettings.maxDist) / interpolationWeight;

	if(hOff != targetSettings.hOff)
		hOff = ((interpolationWeight - 1.0f) * hOff + targetSettings.hOff) / interpolationWeight;

	if(vOff != targetSettings.vOff)
		vOff = ((interpolationWeight - 1.0f) * vOff + targetSettings.vOff) / interpolationWeight;

	if(FOV != targetSettings.FOV)
		FOV = ((interpolationWeight - 1.0f) * FOV + targetSettings.FOV) / interpolationWeight;

	if(pitchOff != targetSettings.pitchOff)
		pitchOff = ((interpolationWeight - 1.0f) * pitchOff + targetSettings.pitchOff) / interpolationWeight;
}

CCameraManager::CCameraManager()
{
	//init cam overrides
	m_pCamOverrides = new CCameraOverrides();
	//init view
	m_pCameraView = new CCameraView(NULL);

	m_camNodes.reserve(32);
	AddNullCam();
	m_idPrev = 0;
	m_idActive = 0;
	SetActiveCameraId(0);
}

CCameraManager::~CCameraManager()
{
	SAFE_DELETE(m_pCamOverrides);
	SAFE_DELETE(m_pCameraView);
}

void CCameraManager::Reset()
{
	ResetCameraState();

	//select a default cam type (follow cam or NULL)
	SetActiveCameraId(FindCameraByType(ECT_CamFollow));
}

void CCameraManager::Update()
{
	m_pCamOverrides->Update();
}

void CCameraManager::AddNullCam()
{
	SCamModeSettings nullCam;

	nullCam.camType=ECT_CamOrbit;	// ECT_CamOrbit, ECT_CamRear
	nullCam.dist=5.0f;
	nullCam.maxDist=3.0f;	// arbitrary
	nullCam.hOff=-0.0f;
	nullCam.vOff=-1.0f;
	nullCam.FOV=50.0f;
	nullCam.pitchOff=0.0f;
	nullCam.pitchLookDownPullBack=0.0f;
	nullCam.pitchLookUpPushIn=0.0f;
	nullCam.maxPitch=DEG2RAD(65.0f);
	nullCam.minPitch=-DEG2RAD(65.0f);
	nullCam.maxYaw=30.0f;

	// damping + lookahead
	const SCVars* pGameCVars = g_pGameCVars;

	nullCam.kVel=pGameCVars->cl_tpvDeltaVelNav;		// lookahead
	nullCam.kDist=pGameCVars->cl_tpvDeltaDistNav;	// lookahead
	nullCam.posDamping=400.0f;
	nullCam.angDamping=pGameCVars->cl_tpvAngDampingNav;
	nullCam.fovDamping=0.1f;

	// collision
	nullCam.collisionType=ECCT_CollisionTrackOrCut;	// ECCT_CollisionCut, ECCT_CollisionTrackOrCut

	//this tweaks the camera rotation speed
	nullCam.m_fYawApplyRate = 0.3f;
	nullCam.m_fPitchApplyRate = 0.3f;

	AddCamera("NullCam",nullCam);

	//initialize override with nullCam settings
	m_pCamOverrides->SetCameraOverrideSettings(nullCam);
}


bool CCameraManager::IsValidCameraId(CameraID idCamera) const
{
	bool bRet=false;
	if(idCamera>=0 && idCamera<m_camNodes.size())
		bRet=true;
	return bRet;
}

const char *CCameraManager::GetCameraNameForId(CameraID idCamera) const
{
	const char *psRet="InvalidCameraId";

	if(IsValidCameraId(idCamera))
	{
		psRet=m_camNodes[idCamera]->sName.c_str();
	}

	return psRet;
}

CameraID CCameraManager::AddCamera(const char *psCamName, const SCamModeSettings &settings)
{
	CameraID idRet;

	idRet=GetCameraIdForName(psCamName);
	if(IsValidCameraId(idRet))
	{
		gEnv->pLog->Log("CCameraManager:AddCamera: Camera '%s' already exists!",psCamName);
		SetCameraSettings(idRet,settings);
	}
	else
	{
		idRet=m_camNodes.size();
		if(idRet>=m_camNodes.capacity())
		{
			m_camNodes.reserve(idRet+32);
		}
		SCamNode *pNode=new SCamNode();
		pNode->sName=psCamName;
		pNode->settings=settings;
		m_camNodes.push_back(pNode);
	}

	//reset manager
	Reset();

	return idRet;
}

bool CCameraManager::GetCameraSettings(CameraID idCamera,SCamModeSettings &settings,bool bIgnoreOverride/*=false*/) const
{
	bool bRet=false;
	if(IsValidCameraId(idCamera))
	{
		settings=m_camNodes[idCamera]->settings;
		bRet=true;

		//if cam override is active, the camera settings are replaced
		if(!bIgnoreOverride && m_pCamOverrides->GetCameraOverride())
		{
			m_pCamOverrides->GetCameraOverrideSettings(settings);
		}
	}
	return bRet;
}

bool CCameraManager::SetCameraSettings(CameraID idCamera,const SCamModeSettings &settings)
{
	bool bRet=false;
	if(IsValidCameraId(idCamera))
	{
		m_camNodes[idCamera]->settings=settings;
		bRet=true;
	}
	return bRet;
}

void CCameraManager::GetCameraState(SCamRetainedState &state) const
{
	state=m_camState;
}

void CCameraManager::SetCameraState(SCamRetainedState &state)
{
	m_camState=state;
	g_bChangedCamera = false;
}

void CCameraManager::LogActiveCameraSettings()
{
	if(IsValidCameraId(m_idActive))
	{
		ILog *pLog=gEnv->pLog;
		
		SCamModeSettings s;
		GetCameraSettings(m_idActive,s);

		const char *psLog="";
		string sLog;

		switch(s.camType)
		{
		case 	ECT_CamOrbit:
			psLog="CamOrbit";
			break;
		case ECT_CamRear:
			psLog="CamRear";
			break;
		case ECT_CamRefDir:
			psLog="CamRefDir";
			break;
		case ECT_CamFollow:
			psLog="CamFollow";
			break;
		default:
			psLog="unknown";
			break;
		}

		if(g_pGameCVars->cl_cam_debug)
		{
			pLog->Log("Camera: CamType '%s'",psLog );

			pLog->Log("Camera: Dist %0.3f, maxDist %0.3f",s.dist,s.maxDist );
			pLog->Log("Camera: hOff %0.3f, vOff:%0.3f",s.hOff,s.vOff );
			pLog->Log("Camera: FOV %0.3f",s.FOV );
			pLog->Log("Camera: pitchOff %0.3f",s.pitchOff );
			pLog->Log("Camera: pitchLookDownPullBack %0.3f, lookUpPushIn %0.3f",s.pitchLookDownPullBack,s.pitchLookUpPushIn );
			pLog->Log("Camera: maxPitch %0.3f, minPitch %0.3f",s.maxPitch,s.minPitch);
			pLog->Log("Camera: maxYaw %0.3f",s.maxYaw);
			pLog->Log("Camera: vRefDir (%0.3f,%0.3f,%0.3f)",s.vRefDir.x,s.vRefDir.y,s.vRefDir.z);
			pLog->Log("Camera: kVel %0.3f, kDist %0.3f",s.kVel,s.kDist);
			pLog->Log("Camera: posDamping %0.3f",s.posDamping);
			pLog->Log("Camera: angDamping %0.3f",s.angDamping);
			pLog->Log("Camera: fovDamping %0.3f",s.fovDamping);

			psLog="";
			switch(s.collisionType)
			{
			case ECCT_CollisionCut:						// Cam cut pops cam in front of occluder.
				psLog="ECCT_CollisionCut";
				break;
			case ECCT_CollisionTrackOrCut:			// Cam tracks round occluder, or cuts if needed.
				psLog="ECCT_CollisionTrackOrCut";
				break;
			case ECCT_CollisionTrack:
				psLog="ECCT_CollisionTrack";
				break;
			case ECCT_CollisionNone:
				psLog="ECCT_CollisionNone";
				break;
			default:
				psLog="unknown";
				break;
			}
			pLog->Log("Camera: CollisionType '%s'",psLog );

			pLog->Log("Camera: yawDriverP4Rate %0.3f pitchDriverP4Rate: %0.3f",s.m_fYawApplyRate,s.m_fPitchApplyRate);

			pLog->Log("Camera: running in override mode, settings may differ.");
		}
	}
}

bool CCameraManager::SetActiveCameraId(CameraID idCamera)
{
	//CryLogAlways("Switching camera mode");
	bool bRet=false;
	if(IsValidCameraId(idCamera))
	{
		if(m_idActive!=idCamera)
		{
			if(g_pGameCVars->cl_cam_debug)
				gEnv->pLog->Log("Camera: Setting active camera to '%s' from '%s'",
					GetCameraNameForId(idCamera), GetCameraNameForId(m_idActive) );

			m_idPrev=m_idActive;
			m_idActive=idCamera;
			if(!m_pCamOverrides->GetCameraOverride()) //in cam override the new camera settings don't affect the game immediately
				g_bChangedCamera = true;

			LogActiveCameraSettings();
		}
		bRet=true;
	}
	return bRet;
}

CameraID CCameraManager::FindCameraByType(ECamTypes eType) const
{
	int numCamNodes = m_camNodes.size();
	for(int i = 0; i < numCamNodes; ++i)
	{
		if(m_camNodes[i]->settings.camType == eType)
			return i;
	}

	return 0;
}

CameraID CCameraManager::GetCameraIdForName(const char *psName) const
{
	int iCam,nCam=m_camNodes.size(),iRet;
	for(iCam=0,iRet=-1;iCam<nCam;++iCam)
	{
		if(0==strcmp(m_camNodes[iCam]->sName.c_str(),psName))
		{
			iRet=iCam;
			break;
		}
	}

	return iRet;
}