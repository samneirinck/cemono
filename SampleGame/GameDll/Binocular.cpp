/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 18:12:2005   14:01 : Created by Márcio Martins
- 08:01:2011   11:11 : Sascha Hoba

*************************************************************************/
#include "StdAfx.h"
#include "Binocular.h"
#include "GameActions.h"
#include "Audio/SoundMoods.h"
#include "Actor.h"

TActionHandler<CBinocular> CBinocular::s_actionHandler;

struct CBinocular::EndRaiseWeaponAction
{
	EndRaiseWeaponAction(CBinocular *_binocs): binocs(_binocs){}
	CBinocular *binocs;

	void execute(CItem *_this)
	{
		binocs->Zoom();
	}
};

CBinocular::CBinocular():
m_bZoomed(false),
m_bNightVisionEnabled(false),
m_pNightVisionCVar(NULL),
m_pSpecCVar(NULL),
m_pPhysWorld(NULL)
{
	if(s_actionHandler.GetNumHandlers() == 0)
	{
#define ADD_HANDLER(action, func) s_actionHandler.AddHandler(actions.action, &CBinocular::func)
		const CGameActions& actions = g_pGame->Actions();

		ADD_HANDLER(zoom,OnActionZoom);
		ADD_HANDLER(firemode,OnActionToggleNightVision);
		ADD_HANDLER(zoom_in,OnActionZoomIn);
		ADD_HANDLER(v_zoom_in,OnActionZoomIn);
		ADD_HANDLER(zoom_out,OnActionZoomOut);
		ADD_HANDLER(v_zoom_out,OnActionZoomOut);
#undef ADD_HANDLER
	}

	m_pPhysWorld = gEnv->pPhysicalWorld;

	if(gEnv->pConsole == NULL)
		return;

	m_pNightVisionCVar	= gEnv->pConsole->GetCVar("r_NightVision");
	m_pSpecCVar			= gEnv->pConsole->GetCVar("sys_spec");
	m_pDefaultNVMode	= gEnv->pConsole->GetCVar("pl_nightvisionModeBinocular");
	
}
//------------------------------------------------------------------------
CBinocular::~CBinocular()
{
}

//------------------------------------------------------------------------
void CBinocular::OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value)
{
	s_actionHandler.Dispatch(this,actorId,actionId,activationMode,value);
}

//------------------------------------------------------------------------
void CBinocular::Select(bool select)
{
	if(select == false)
	{
		PlayAction(g_pItemStrings->lower);
		ResetState();
	}
	else
	{
		PlayAction(g_pItemStrings->select);
	}
		
	CWeapon::Select(select);
}

//-----------------------------------------------------------------------
bool CBinocular::OnActionZoom(EntityId actorId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == eAAM_OnPress)
	{
		if(m_bZoomed == false)
		{
			PlayAction(g_pItemStrings->raise);
			SetWeaponRaised(true);
			GetScheduler()->TimerAction(GetCurrentAnimationTime(eIGS_FirstPerson), CSchedulerAction<EndRaiseWeaponAction>::Create(EndRaiseWeaponAction(this)), true);
		}
		else
		{
			if(m_zm && m_zm->IsZoomed())
			{
				PlayAction(g_pItemStrings->lower);
				m_zm->ExitZoom();
			}

			ResetState();
		}
	}

	return true;
}

//-------------------------------------------------------------------------
bool CBinocular::OnActionZoomIn(EntityId actorId, const ActionId& actionId, int activationMode, float value)
{
	if (m_zm->GetCurrentStep() < m_zm->GetMaxZoomSteps())
	{
		PlayAction(g_pItemStrings->zoom_in);
		m_zm->StartZoom(false, true);
	}

	return true;
}

//--------------------------------------------------------------------------
bool CBinocular::OnActionZoomOut(EntityId actorId, const ActionId& actionId, int activationMode, float value)
{
	if (m_zm)
	{
		m_zm->ZoomOut();
	}

	return true;
}

bool CBinocular::OnActionToggleNightVision( EntityId actorId, const ActionId& actionId, int activationMode, float value )
{
	if(m_bZoomed == false)
		return true;

	if(m_pNightVisionCVar == NULL)
		return true;

	if(m_pDefaultNVMode == NULL)
		return true;

	if (m_bNightVisionEnabled == true)
	{
		PlayAction(g_pItemStrings->nightvision_off);
		m_pNightVisionCVar->ForceSet("0");
	}
	else
	{
		PlayAction(g_pItemStrings->nightvision_on);
		m_pNightVisionCVar->ForceSet(m_pDefaultNVMode->GetString());
	}

	m_bNightVisionEnabled = !m_bNightVisionEnabled;

	return true;
}

void CBinocular::Zoom()
{
	if (m_zm)
	{
		gEnv->p3DEngine->SetPostEffectParam("Dof_UseMask", 0);
		gEnv->p3DEngine->SetPostEffectParam("Dof_Active", 1);
		gEnv->p3DEngine->SetPostEffectParam("Dof_BlurAmount", 1.0f);
		gEnv->p3DEngine->SetPostEffectParamString("Dof_MaskTexName", "");

		PlayAction(g_pItemStrings->zoom_in);
		m_zm->StartZoom();
		m_bZoomed = true;
		SetDefaultIdleAnimation( eIGS_FirstPerson,g_pItemStrings->idle_raised);
	}
}

void CBinocular::OnDropped( EntityId actorId )
{
	ResetState();

	if(GetOwnerActor() == NULL)
		return;

	GetOwnerActor()->SelectLastItem(true, true);
}

void CBinocular::ResetState()
{
	SetDefaultIdleAnimation( eIGS_FirstPerson,g_pItemStrings->idle_relaxed);
	SetWeaponRaised(false);

	if(m_pNightVisionCVar)
		m_pNightVisionCVar->ForceSet("0");

	m_bNightVisionEnabled = false;
	m_bZoomed = false;

	gEnv->p3DEngine->SetPostEffectParam("Dof_Active", 0);
}

void CBinocular::UpdateFPView( float frameTime )
{
	CWeapon::UpdateFPView(frameTime);

	if(m_bZoomed == false)
		return;

	if (m_pSpecCVar == NULL)
		return;

	if(m_pPhysWorld == NULL)
		return;

	if (m_pSpecCVar->GetIVal() > 2)
	{
		ray_hit hit;
		const CCamera& cam = GetISystem()->GetViewCamera();
		const Vec3 pos = cam.GetPosition()+cam.GetViewdir();
		const Vec3 direction = cam.GetViewdir();
		
		int numHits = m_pPhysWorld->RayWorldIntersection( pos,direction * 1000.0f,
			ent_all,
			rwi_stop_at_pierceable|rwi_colltype_any,
			&hit, 1 );

		if(numHits > 0)
		{
			gEnv->p3DEngine->SetPostEffectParam("Dof_UseMask", 0);
			gEnv->p3DEngine->SetPostEffectParam("Dof_FocusDistance", hit.dist);
			gEnv->p3DEngine->SetPostEffectParam("Dof_FocusRange", hit.dist * 5.0f);
		}
	}
}