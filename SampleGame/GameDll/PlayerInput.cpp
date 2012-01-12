#include "StdAfx.h"
#include "PlayerInput.h"
#include "Player.h"
#include "Game.h"
#include "GameCVars.h"
#include "GameActions.h"
#include "Weapon.h"
#include "WeaponSystem.h"
#include "IVehicleSystem.h"
#include "VehicleClient.h"
#include "OffHand.h"
#include "Fists.h"
#include "GameRules.h"
#include "Camera/CameraInputHelper.h"

#include <IWorldQuery.h>
#include <IInteractor.h>
#include "IAIActor.h"
TActionHandler<CPlayerInput>	CPlayerInput::s_actionHandler;

CPlayerInput::CPlayerInput( CPlayer * pPlayer ) : 
	m_pPlayer(pPlayer), 
	m_pStats(&pPlayer->m_stats),
	m_actions(ACTION_GYROSCOPE), 
	m_deltaRotation(0,0,0), 
	m_lastMouseRawInput(0,0,0),
	m_deltaMovement(0,0,0), 
	m_xi_deltaMovement(0,0,0),
	m_xi_deltaRotation(0,0,0),
	m_xi_deltaRotationRaw(0.0f, 0.0f, 0.0f),
	m_filteredDeltaMovement(0,0,0),
	m_buttonPressure(0.0f),
	m_deltaPitch(0),
	m_deltaPitchOnce(0),
	m_cameraStickLR(0),
	m_cameraStickUD(0),
	m_moveStickLR(0),
	m_moveStickUD(0),
	m_speedLean(0.0f),
	m_bDisabledXIRot(false),
	m_fCrouchPressedTime(-1.0f),
	m_moveButtonState(0),
	m_bUseXIInput(false),
	m_checkZoom(false),
	m_lastPos(0,0,0),
	m_iCarryingObject(0),
	m_lastSerializeFrameID(0),
	m_fSprintTime(0),
	m_fRestingTime(0)
{
	m_pPlayer->RegisterPlayerEventListener(this);
	m_pPlayer->GetGameObject()->CaptureActions(this);

	// Create the input helper class for the new third person camera
	m_pCameraInputHelper = new CCameraInputHelper(m_pPlayer, this);

	// set up the handlers
	if (s_actionHandler.GetNumHandlers() == 0)
	{
	#define ADD_HANDLER(action, func) s_actionHandler.AddHandler(actions.action, &CPlayerInput::func)
		const CGameActions& actions = g_pGame->Actions();

		ADD_HANDLER(moveforward, OnActionMoveForward);	// Keyboard
		ADD_HANDLER(moveback, OnActionMoveBack);				// Keyboard
		ADD_HANDLER(moveleft, OnActionMoveLeft);				// Keyboard
		ADD_HANDLER(moveright, OnActionMoveRight);			// Keyboard
		ADD_HANDLER(rotateyaw, OnActionRotateYaw);			// Mouse
		ADD_HANDLER(rotatepitch, OnActionRotatePitch);	// Mouse
		ADD_HANDLER(jump, OnActionJump);
		ADD_HANDLER(crouch, OnActionCrouch);
		ADD_HANDLER(sprint, OnActionSprint);
		ADD_HANDLER(togglestance, OnActionToggleStance);
		ADD_HANDLER(prone, OnActionProne);
		//ADD_HANDLER(zerogbrake, OnActionZeroGBrake);
		ADD_HANDLER(gyroscope, OnActionGyroscope);
		ADD_HANDLER(gboots, OnActionGBoots);
		ADD_HANDLER(leanleft, OnActionLeanLeft);
		ADD_HANDLER(leanright, OnActionLeanRight);
		//ADD_HANDLER(holsteritem, OnActionHolsterItem);
		ADD_HANDLER(use, OnActionUse);

		ADD_HANDLER(thirdperson, OnActionThirdPerson);
		ADD_HANDLER(flymode, OnActionFlyMode);
		ADD_HANDLER(godmode, OnActionGodMode);
		ADD_HANDLER(toggleaidebugdraw, OnActionAIDebugDraw);
		ADD_HANDLER(togglepdrawhelpers, OnActionPDrawHelpers);

		ADD_HANDLER(v_rotateyaw, OnActionVRotateYaw); // needed so player can shake unfreeze while in a vehicle
		ADD_HANDLER(v_rotatepitch, OnActionVRotatePitch);

		ADD_HANDLER(xi_v_rotateyaw, OnActionXIRotateYaw);
		ADD_HANDLER(xi_rotateyaw, OnActionXIRotateYaw);
		ADD_HANDLER(xi_rotatepitch, OnActionXIRotatePitch);

		ADD_HANDLER(xi_v_rotatepitch, OnActionXIRotatePitch);
		ADD_HANDLER(xi_movex, OnActionXIMoveX);
		ADD_HANDLER(xi_movey, OnActionXIMoveY);
		ADD_HANDLER(xi_disconnect, OnActionXIDisconnect);
		ADD_HANDLER(xi_use, OnActionUse);

		ADD_HANDLER(invert_mouse, OnActionInvertMouse);

	#undef ADD_HANDLER
	}
}

CPlayerInput::~CPlayerInput()
{
	m_pPlayer->UnregisterPlayerEventListener(this);
	SAFE_DELETE(m_pCameraInputHelper);
	m_pPlayer->GetGameObject()->ReleaseActions(this);
}

void CPlayerInput::Reset()
{
	m_actions = ACTION_GYROSCOPE;//gyroscope is on by default
	m_lastActions = m_actions;

	m_deltaMovement.zero();
	m_xi_deltaMovement.zero();
	m_filteredDeltaMovement.zero();
	m_deltaRotation.Set(0,0,0);
	m_lastMouseRawInput.Set(0,0,0);
	m_xi_deltaRotation.Set(0,0,0);
	m_xi_deltaRotationRaw(0.0f, 0.0f, 0.0f),
	m_bDisabledXIRot = false;
	m_moveButtonState = 0;
	m_lastSerializeFrameID = 0;

	// camera resetting stuff
	m_deltaPitch		= 0;
	m_deltaPitchOnce= 0;
	m_cameraStickLR = 0;
	m_cameraStickUD = 0;
	m_moveStickLR		= 0;
	m_moveStickUD		= 0;
	m_mouseInput = false;
}

// stick normalisation could also be moved into the joystick device.
// Normalise the input, correcting for the dead zone
static inline float NormaliseStick(float value)
{
	// Assuming a 0.24 dead zone in the driver, but will still function if this is false.
	if(value>=0.24f)
	{
		value=(value-0.24f)/(1-0.24f);
		if (value<0) value=0;
	}
	else if (value<=-0.24f)
	{
		value=(value+0.24f)/(1-0.24f);
		if (value>0) value=0;
	}
	else
		value=0;

	return (value<-1) ? -1 : (value>1) ? 1 : value;
}

// Normalize the input, correcting for the dead zone, then apply a gain/power sensitivity curve
// the higher gamma is, the more sensitive the stick is at the centre, and the less sensitive it is at the edge.
// the gain speeds up the stick over its entire centre to edge range.
static inline float AdjustStickInput(float value,float gain,float gamma,float clamp)
{
	value=NormaliseStick(value);

	if(value<0)
		gain=-gain;
	value=gain*pow(abs(value),gamma);

	if(clamp>0.01f)
		value = value<-clamp ? -clamp : value>clamp ? clamp : value;
	return value;
}

void CPlayerInput::DisableXI(bool disabled)
{
	m_bDisabledXIRot = disabled;
}

void CPlayerInput::ApplyMovement(Vec3 delta)
{
	//m_deltaMovement += delta;
	m_deltaMovement.x = clamp_tpl(m_deltaMovement.x+delta.x,-1.0f,1.0f);
	m_deltaMovement.y = clamp_tpl(m_deltaMovement.y+delta.y,-1.0f,1.0f);
	m_deltaMovement.z = 0;

	//static float color[] = {1,1,1,1};
	//gEnv->pRenderer->Draw2dLabel(100,50,1.5,color,false,"m_deltaMovement:%f,%f (requested:%f,%f", m_deltaMovement.x, m_deltaMovement.y,delta.x,delta.y);
}

void CPlayerInput::OnAction( const ActionId& actionId, int activationMode, float value )
{
	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);

	m_pPlayer->GetGameObject()->ChangedNetworkState( INPUT_ASPECT );

	m_lastActions=m_actions;

	//this tell if OnAction have to be forwarded to scripts, now its true by default, only high framerate actions are ignored
	bool filterOut = true;
	m_checkZoom = false;
	const CGameActions& actions = g_pGame->Actions();
	IVehicle* pVehicle = m_pPlayer->GetLinkedVehicle();

	bool canMove = CanMove();

	// disable movement while standing up
	if (!canMove)
		m_deltaMovement.zero();

	// try to dispatch action to OnActionHandlers
	bool handled;

	{
		FRAME_PROFILER("New Action Processing", GetISystem(), PROFILE_GAME);
		handled = s_actionHandler.Dispatch(this, m_pPlayer->GetEntityId(), actionId, activationMode, value, filterOut);
	}

	{
		FRAME_PROFILER("Regular Action Processing", GetISystem(), PROFILE_GAME);
		if (!handled)
		{
			filterOut = true;
			if (!m_pPlayer->m_stats.spectatorMode)
			{
				if (actions.ulammo==actionId && m_pPlayer->m_pGameFramework->CanCheat() && gEnv->pSystem->IsDevMode())
				{
					g_pGameCVars->i_unlimitedammo = 1;
				}
				else if (actions.debug_ag_step == actionId)
				{
					gEnv->pConsole->ExecuteString("ag_step");
				}
				else if(actions.voice_chat_talk == actionId)
				{
					if(gEnv->bMultiplayer)
					{
						if(activationMode == eAAM_OnPress)
							g_pGame->GetIGameFramework()->EnableVoiceRecording(true);
						else if(activationMode == eAAM_OnRelease)
							g_pGame->GetIGameFramework()->EnableVoiceRecording(false);
					}
				}
			}
		}

		if (!m_pPlayer->m_stats.spectatorMode)
		{
			IInventory* pInventory = m_pPlayer->GetInventory();
			if (!pInventory)
				return;

			bool scope = false;
			EntityId itemId = pInventory->GetCurrentItem();
			CWeapon *pWeapon = 0;
			if (itemId)
			{
				pWeapon = m_pPlayer->GetWeapon(itemId);
				if (pWeapon)
				{
					scope = (pWeapon->IsZoomed() && pWeapon->GetMaxZoomSteps()>1);
				}
			}

			if (pVehicle)
			{
				if (m_pPlayer->m_pVehicleClient && !m_pPlayer->IsFrozen())
					m_pPlayer->m_pVehicleClient->OnAction(pVehicle, m_pPlayer->GetEntityId(), actionId, activationMode, value);

				//FIXME:not really good
				m_actions = 0;
				m_deltaMovement.Set(0,0,0);
			}
			else if (m_pPlayer->GetHealth() > 0 && !m_pPlayer->m_stats.isFrozen.Value() && !m_pPlayer->m_stats.inFreefall.Value() && !m_pPlayer->m_stats.isOnLadder 
				&& !m_pPlayer->m_stats.isStandingUp && m_pPlayer->GetGameObject()->GetAspectProfile(eEA_Physics)!=eAP_Sleep)
			{
				m_pPlayer->CActor::OnAction(actionId, activationMode, value);

				if ((!scope || actionId == actions.use))
				{
					COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
					if (pOffHand)
					{
						pOffHand->OnAction(m_pPlayer->GetEntityId(), actionId, activationMode, value);
					}

					if ((!pWeapon || !pWeapon->IsMounted()))
					{
						if ((actions.drop==actionId) && itemId)
						{
							float impulseScale=1.0f;
							if (activationMode==eAAM_OnPress)
								m_buttonPressure=2.5f;
							if (activationMode==eAAM_OnRelease)
							{
								m_buttonPressure=CLAMP(m_buttonPressure, 0.0f, 2.5f);
								impulseScale=1.0f+(1.0f-m_buttonPressure/2.5f)*15.0f;
								if (m_pPlayer->DropItem(itemId, impulseScale, true) && pOffHand && pOffHand->IsSelected())
								{							
									if (EntityId fistsId = pInventory->GetItemByClass(CItem::sFistsClass))
									{
										m_pPlayer->SelectItem(fistsId, false);
									}
									pOffHand->PreExecuteAction(eOHA_REINIT_WEAPON, eAAM_OnPress);
									CItem* pItem = static_cast<CItem*>(m_pPlayer->GetCurrentItem());
									if (pItem)
									{
										pItem->SetActionSuffix("akimbo_");
										pItem->PlayAction(g_pItemStrings->idle);
									}
								}
							}
						}
						else if (actions.nextitem==actionId)
							m_pPlayer->SelectNextItem(1, true, 0);
						else if (actions.previtem==actionId)
							m_pPlayer->SelectNextItem(-1, true, 0);
						else if (actions.handgrenade==actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.explosive==actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.utility==actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.small==actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.medium==actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.heavy==actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.debug==actionId)
						{
							if (g_pGame)
							{							
								if (!m_pPlayer->GetInventory()->GetItemByClass(CItem::sDebugGunClass))
									g_pGame->GetWeaponSystem()->DebugGun(0);				
								if (!m_pPlayer->GetInventory()->GetItemByClass(CItem::sRefWeaponClass))
									g_pGame->GetWeaponSystem()->RefGun(0);
							}

							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						}
					}
				}
				else 
				{
					if (actions.handgrenade==actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.explosive==actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.utility==actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.small==actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.medium==actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.heavy==actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.drop==actionId && activationMode == eAAM_OnRelease && itemId)
						m_pPlayer->DropItem(itemId, 1.0f, true);
				}
			}

			if (m_checkZoom)
			{
				IItem* pCurItem = m_pPlayer->GetCurrentItem();
				IWeapon* pWeapon = 0;
				if(pCurItem)
					pWeapon = pCurItem->GetIWeapon();
				if (pWeapon)
				{
					IZoomMode *zm = pWeapon->GetZoomMode(pWeapon->GetCurrentZoomMode());
					CScreenEffects* pScreenEffects = m_pPlayer->GetScreenEffects();
					if (zm && !zm->IsZoomingInOrOut() && !zm->IsZoomed() && pScreenEffects != 0)
					{
						if (!m_moveButtonState && m_pPlayer->IsClient())
						{
							IBlendedEffect *fovEffect	= CBlendedEffect<CFOVEffect>::Create(CFOVEffect(m_pPlayer->GetEntityId(),1.0f));
							IBlendType   *blend				= CBlendType<CLinearBlend>::Create(CLinearBlend(1.0f));
							pScreenEffects->ResetBlendGroup(CScreenEffects::eSFX_GID_ZoomIn, false);
							pScreenEffects->ResetBlendGroup(CScreenEffects::eSFX_GID_ZoomOut, false);
							pScreenEffects->StartBlend(fovEffect, blend, 1.0f/.25f, CScreenEffects::eSFX_GID_ZoomIn);
						}
						else
						{
							pScreenEffects->EnableBlends(true, CScreenEffects::eSFX_GID_ZoomIn);
							pScreenEffects->EnableBlends(true, CScreenEffects::eSFX_GID_ZoomOut);
							pScreenEffects->EnableBlends(true, CScreenEffects::eSFX_GID_HitReaction);
						}
					}
				}
			}
		}
	}


	bool hudFilterOut = true;

	// FIXME: temporary method to dispatch Actions to HUD (it's not yet possible to register)
	hudFilterOut = true;

	//Filter must take into account offHand too
	COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
	if(pOffHand && pOffHand->IsSelected())
		filterOut = false;

	//send the onAction to scripts, after filter the range of actions. for now just use and hold
	if (filterOut && hudFilterOut)
	{
		FRAME_PROFILER("Script Processing", GetISystem(), PROFILE_GAME);
		HSCRIPTFUNCTION scriptOnAction(NULL);

		IScriptTable *scriptTbl = m_pPlayer->GetEntity()->GetScriptTable();

		if (scriptTbl)
		{
			scriptTbl->GetValue("OnAction", scriptOnAction);

			if (scriptOnAction)
			{
				char *activation = 0;

				switch(activationMode)
				{
				case eAAM_OnHold:
					activation = "hold";
					break;
				case eAAM_OnPress:
					activation = "press";
					break;
				case eAAM_OnRelease:
					activation = "release";
					break;
				default:
					activation = "";
					break;
				}

				Script::Call(gEnv->pScriptSystem,scriptOnAction,scriptTbl,actionId.c_str(),activation, value);
			}
		}

		gEnv->pScriptSystem->ReleaseFunc(scriptOnAction);
	}	
}

void CPlayerInput::OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded)
{
	if(m_pPlayer == pActor)
	{
		m_iCarryingObject = bIsGrab ? (bIsTwoHanded ? 2 : 1) : 0;
	}
}

//this function basically returns a smoothed movement vector, for better movement responsivness in small spaces
const Vec3 &CPlayerInput::FilterMovement(const Vec3 &desired)
{
	float frameTimeCap(min(gEnv->pTimer->GetFrameTime(),0.033f));
	float inputAccel(g_pGameCVars->pl_inputAccel);

	Vec3 oldFilteredMovement = m_filteredDeltaMovement;

	if (desired.len2()<0.01f)
	{
		m_filteredDeltaMovement.zero();
	}
	else if (inputAccel<=0.0f)
	{
		m_filteredDeltaMovement = desired;
	}
	else
	{
		Vec3 delta(desired - m_filteredDeltaMovement);

		float len(delta.len());
		if (len<=1.0f)
			delta = delta * (1.0f - len*0.55f);

		m_filteredDeltaMovement += delta * min(frameTimeCap * inputAccel,1.0f);
	}

	if (oldFilteredMovement.GetDistance(m_filteredDeltaMovement) > 0.001f)
		m_pPlayer->GetGameObject()->ChangedNetworkState( INPUT_ASPECT );

	return m_filteredDeltaMovement;
}

bool CPlayerInput::CanMove() const
{
	bool canMove = !m_pPlayer->m_stats.spectatorMode || m_pPlayer->m_stats.spectatorMode==CActor::eASM_Fixed;
	canMove &=!m_pPlayer->m_stats.isStandingUp;
	return canMove;
}

void CPlayerInput::PreUpdate()
{
	CMovementRequest request;
	
	// get rotation into a manageable form
	float mouseSensitivity;
	if (m_pPlayer->InZeroG())
		mouseSensitivity = 0.00333f*MAX(0.01f, g_pGameCVars->cl_sensitivityZeroG);
	else
		mouseSensitivity = 0.00333f*MAX(0.01f, g_pGameCVars->cl_sensitivity);

	mouseSensitivity *= gf_PI / 180.0f;//doesnt make much sense, but after all helps to keep reasonable values for the sensitivity cvars
	//these 2 could be moved to CPlayerRotation
	mouseSensitivity *= m_pPlayer->m_params.viewSensitivity;
	mouseSensitivity *= m_pPlayer->GetMassFactor();
	COffHand * pOffHand=static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
	if(pOffHand && (pOffHand->GetOffHandState()&eOHS_HOLDING_NPC))
		mouseSensitivity *= pOffHand->GetObjectMassScale();

	// When carrying object/enemy, adapt mouse sensitiviy to feel the weight
	// Designers requested we ignore single-handed objects (1 == m_iCarryingObject)
	if(2 == m_iCarryingObject)
	{
		mouseSensitivity /= 2.0f;
	}

	if(m_fCrouchPressedTime>0.0f)
	{
		float fNow = gEnv->pTimer->GetAsyncTime().GetMilliSeconds();
		if((fNow - m_fCrouchPressedTime) > 300.0f)
		{
			if(m_actions & ACTION_CROUCH)
			{
				m_actions &= ~ACTION_CROUCH;
				m_actions |= ACTION_PRONE;
			}
			m_fCrouchPressedTime = -1.0f;
		}
	}

	Ang3 deltaRotation(m_deltaRotation * mouseSensitivity);

	if (m_pStats->isFrozen.Value() && m_pPlayer->IsPlayer() && m_pPlayer->GetHealth()>0)
	{
		float sMin = g_pGameCVars->cl_frozenSensMin;
		float sMax = g_pGameCVars->cl_frozenSensMax;

		float mult = sMin + (sMax-sMin)*(1.f-m_pPlayer->GetFrozenAmount(true));    
		deltaRotation *= mult;

		m_pPlayer->UpdateUnfreezeInput(m_deltaRotation, m_deltaMovement-m_deltaMovementPrev, mult);
	}

	// apply rotation from xinput controller
	if(!m_bDisabledXIRot)
	{
		// Controller framerate compensation needs frame time! 
		// The constant is to counter for small frame time values.
		// adjust some too small values, should be handled differently later on
		Ang3 xiDeltaRot=m_xi_deltaRotation*gEnv->pTimer->GetFrameTime() * mouseSensitivity * 50.0f;
		SmoothControllerInput(xiDeltaRot);
		ControlCameraMode();

		// Applying aspect modifiers
		if (g_pGameCVars->ctrl_aspectCorrection > 0)
		{
			int vx, vy, vw, vh;
			gEnv->pRenderer->GetViewport(&vx, &vy, &vw, &vh);
			float med=((float)vw+vh)/2.0f;
			float crW=((float)vw)/med;
			float crH=((float)vh)/med;
			xiDeltaRot.x*=g_pGameCVars->ctrl_aspectCorrection == 2 ? crW : crH;
			xiDeltaRot.z*=g_pGameCVars->ctrl_aspectCorrection == 2 ? crH : crW;
		}

		if(g_pGameCVars->cl_invertController)
			xiDeltaRot.x*=-1;

		deltaRotation+=xiDeltaRot;

		IVehicle *pVehicle = m_pPlayer->GetLinkedVehicle();
		if (pVehicle)
		{
			if (m_pPlayer->m_pVehicleClient)
			{
				m_pPlayer->m_pVehicleClient->PreUpdate(pVehicle, m_pPlayer->GetEntityId(), gEnv->pTimer->GetFrameTime());
			}

			//FIXME:not really good
			m_actions = 0;
			m_deltaMovement.Set(0,0,0);
			m_deltaRotation.Set(0,0,0);
		}
	}

	if(m_bUseXIInput)
	{
		m_deltaMovement.x = m_xi_deltaMovement.x;
		m_deltaMovement.y = m_xi_deltaMovement.y;
		m_deltaMovement.z = 0;

		if (m_xi_deltaMovement.len2()>0.0f)
			m_actions |= ACTION_MOVE;
		else
			m_actions &= ~ACTION_MOVE;
	}

	bool animControlled(m_pPlayer->m_stats.animationControlled);

	// If there was a recent serialization, ignore the delta rotation, since it's accumulated over several frames.
	if ((m_lastSerializeFrameID + 2) > gEnv->pRenderer->GetFrameID())
		deltaRotation.Set(0,0,0);

	//if(m_pPlayer->m_stats.isOnLadder)
		//deltaRotation.z = 0.0f;

	const SCVars* pGameCVars = g_pGameCVars;
	if(pGameCVars->cl_cam_orbit != 0 && m_pPlayer->IsClient() && m_pPlayer->IsThirdPerson())
	{
		static bool IsInit = false;
		if (!IsInit)
		{
			m_pPlayer->m_camViewMtxFinal = Matrix33(gEnv->pRenderer->GetCamera().GetViewMatrix());
			IsInit = true;
		}

		float frameTime=gEnv->pTimer->GetFrameTime();
		float frameTimeNormalised=(frameTime>1 ? 1 : frameTime<0.0001f ? 0.0001f : frameTime)*30;	// 1/30th => 1 1/60th =>0.5 etc
		float frameTimeClamped=(frameTime>1 ? 1 : frameTime<0.0001f ? 0.0001f : frameTime);
		m_pCameraInputHelper->UpdateCameraInput(deltaRotation, frameTimeClamped,frameTimeNormalised);	// also modifies deltaRotation.
	}

	if (!animControlled)
		request.AddDeltaRotation( deltaRotation );

	// add some movement...
	if (!m_pStats->isFrozen.Value() && !animControlled)  
		request.AddDeltaMovement( FilterMovement(m_deltaMovement) );

  m_deltaMovementPrev = m_deltaMovement;

	// handle actions
	if (m_actions & ACTION_JUMP)
	{
		if (m_pPlayer->GetStance() != STANCE_PRONE)
			request.SetJump();
		else
			m_actions &= ~ACTION_JUMP;

		//m_actions &= ~ACTION_PRONE;

		/*if (m_pPlayer->GetStance() != STANCE_PRONE)
		{
			if(m_pPlayer->GetStance() == STANCE_STAND || m_pPlayer->TrySetStance(STANCE_STAND))
 				request.SetJump();
		}
		else if(!m_pPlayer->TrySetStance(STANCE_STAND))
			m_actions &= ~ACTION_JUMP;
		else
			m_actions &= ~ACTION_PRONE;*/
	}

	if (m_pPlayer->m_stats.isOnLadder)
	{
		m_actions &= ~ACTION_PRONE;
		m_actions &= ~ACTION_CROUCH;
	}
	
	request.SetStance(FigureOutStance());

	float pseudoSpeed = 0.0f;
	if (m_deltaMovement.len2() > 0.0f)
	{
		pseudoSpeed = m_pPlayer->CalculatePseudoSpeed(m_pPlayer->m_stats.bSprinting);
	}
	/* design changed: sprinting with controller is removed from full stick up to Left Bumper
	if(m_bUseXIInput && m_xi_deltaMovement.len2() > 0.999f)
	{
		m_actions |= ACTION_SPRINT;
	}
	else if(m_bUseXIInput)
	{
		m_actions &= ~ACTION_SPRINT;
	}*/
	request.SetPseudoSpeed(pseudoSpeed);

	if (m_deltaMovement.GetLength() > 0.1f)
	{
		float moveAngle = (float)RAD2DEG(fabs_tpl(cry_atan2f(-m_deltaMovement.x, fabsf(m_deltaMovement.y)<0.01f?0.01f:m_deltaMovement.y)));
		request.SetAllowStrafing(moveAngle > 20.0f);
	}
	else
	{
		request.SetAllowStrafing(true);
	}

	// send the movement request to the appropriate spot!
	m_pPlayer->m_pMovementController->RequestMovement( request );
	m_pPlayer->m_actions = m_actions;

	// reset things for next frame that need to be
	m_lastMouseRawInput = m_deltaRotation;
	m_deltaRotation = Ang3(0,0,0);

	//static float color[] = {1,1,1,1};    
  //gEnv->pRenderer->Draw2dLabel(100,50,1.5,color,false,"deltaMovement:%f,%f", m_deltaMovement.x,m_deltaMovement.y);

  // PLAYERPREDICTION
  m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);
  // ~PLAYERPREDICTION

}

EStance CPlayerInput::FigureOutStance()
{
	if (m_actions & ACTION_CROUCH)
		return STANCE_CROUCH;
	else if (m_actions & ACTION_PRONE)
		return STANCE_PRONE;
	else if (m_actions & ACTION_RELAXED)
		return STANCE_RELAXED;
	else if (m_actions & ACTION_STEALTH)
		return STANCE_STEALTH;
	else if (m_pPlayer->GetStance() == STANCE_NULL)
		return STANCE_STAND;
	return STANCE_STAND;
}

void CPlayerInput::Update()
{
	if (m_buttonPressure>0.0f)
	{
		m_buttonPressure-=gEnv->pTimer->GetFrameTime();
		if (m_buttonPressure<0.0f)
			m_buttonPressure=0.0f;
	}
}

void CPlayerInput::PostUpdate()
{
	if (m_actions!=m_lastActions)
		m_pPlayer->GetGameObject()->ChangedNetworkState( INPUT_ASPECT );

	m_actions &= ~(ACTION_LEANLEFT | ACTION_LEANRIGHT);

	float frameTime(gEnv->pTimer->GetFrameTime());
	//bool bFlipPitch=true;
	float deltaPitch= m_deltaPitch;
	deltaPitch += m_deltaPitchOnce;
	m_deltaPitchOnce = 0;
	m_pCameraInputHelper->PostUpdate(frameTime, deltaPitch);

	if (m_mouseInput)
	{
		m_cameraStickLR = 0;
		m_cameraStickUD = 0;
		m_mouseInput = false;
	}

	if ( m_actions & ACTION_SPRINT
		 && (  ( g_pGameCVars->cl_sprintTime != 0 && g_pGameCVars->cl_sprintRestingTime != 0 
				&& gEnv->pTimer->GetCurrTime() - m_fSprintTime > g_pGameCVars->cl_sprintTime )
			|| !( m_actions & ACTION_MOVE ) 
			) 
		)
		StopSprint();
}

void CPlayerInput::GetState( SSerializedPlayerInput& input )
{
	SMovementState movementState;
	m_pPlayer->GetMovementController()->GetMovementState( movementState );

	Quat worldRot = m_pPlayer->GetBaseQuat();
	input.stance = FigureOutStance();
	
	// PLAYERPREDICTION
  	if (g_pGameCVars->pl_serialisePhysVel)
  	{
    	//--- Serialise the physics vel instead, velocity over the NET_SERIALISE_PLAYER_MAX_SPEED will be clamped by the network so no guards here
    	IPhysicalEntity* pEnt = m_pPlayer->GetEntity()->GetPhysics();
    	if (pEnt)
    	{
      		pe_status_dynamics dynStat;
      		pEnt->GetStatus(&dynStat);

      		input.deltaMovement = dynStat.v / g_pGameCVars->pl_netSerialiseMaxSpeed;
      		input.deltaMovement.z = 0.0f;
    	}
  	}
  	else
  	{
	  	input.deltaMovement = worldRot.GetNormalized() * m_filteredDeltaMovement;
	  	// ensure deltaMovement has the right length
	  	input.deltaMovement = input.deltaMovement.GetNormalizedSafe(ZERO) * m_filteredDeltaMovement.GetLength();
  	}
	// ~PLAYERPREDICTION
	
	input.sprint = (((m_actions & ACTION_SPRINT) != 0) && !m_pPlayer->m_stats.bIgnoreSprinting);
	input.usinglookik = true;
	input.aiming = true;
	input.leanl = (m_actions & ACTION_LEANLEFT) != 0;
	input.leanr = (m_actions & ACTION_LEANRIGHT) != 0;
	input.lookDirection = movementState.eyeDirection;
	
	// PLAYERPREDICTION
	input.bodyDirection = movementState.entityDirection;
	// ~PLAYERPREDICTION

	m_lastPos = movementState.pos;
}

void CPlayerInput::SetState( const SSerializedPlayerInput& input )
{
	GameWarning("CPlayerInput::SetState called: should never happen");
}

void CPlayerInput::SerializeSaveGame( TSerialize ser )
{
	if(ser.GetSerializationTarget() != eST_Network)
	{
		// Store the frame we serialize, to avoid accumulated input during serialization.
		m_lastSerializeFrameID = gEnv->pRenderer->GetFrameID();

		bool proning = (m_actions & ACTION_PRONE)?true:false;
		ser.Value("ProningAction", proning);

		if(ser.IsReading())
		{
			Reset();
			if(proning)
				OnAction(g_pGame->Actions().prone, 1, 1.0f);
		}

		//ser.Value("Actions", m_actions); //don't serialize the actions - this will only lead to repeating movement (no key-release)
	}
}

bool CPlayerInput::OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if(activationMode == 2)
		{
			if(!(m_moveButtonState&eMBM_Back))
			{
				// for camera
				m_moveStickUD = 0;
				m_deltaMovement.y = 0;

				if (!(m_moveButtonState&eMBM_Left) && !(m_moveButtonState&eMBM_Right))
					m_actions &= ~ACTION_MOVE;
			}
		}
		else 
		{
			m_actions |= ACTION_MOVE;
		}

		if(CheckMoveButtonStateChanged(eMBM_Forward, activationMode))
		{
			if(activationMode != 2)
			{
				ApplyMovement(Vec3(0,value * 2.0f - 1.0f,0));

				// for camera
				m_moveStickUD = value * 2.0f - 1.0f;
				m_deltaMovement.y = value;
			}

			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Forward, activationMode);
		}
	}

	return false;
}

bool CPlayerInput::OnActionMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if(activationMode == 2)
		{
			if(!(m_moveButtonState&eMBM_Forward))
			{
				// for camera
				m_moveStickUD = 0;
				m_deltaMovement.y = 0;

				if (!(m_moveButtonState&eMBM_Left) && !(m_moveButtonState&eMBM_Right))
					m_actions &= ~ACTION_MOVE;
			}
		}
		else
			m_actions |= ACTION_MOVE;

		if(CheckMoveButtonStateChanged(eMBM_Back, activationMode))
		{
			if(activationMode != 2)
			{
				ApplyMovement(Vec3(0,-(value*2.0f - 1.0f),0));

				// for camera
				m_moveStickUD = -(value * 2.0f - 1.0f);
				m_deltaMovement.y = -value;
			}

			if(m_pPlayer->GetActorStats()->inZeroG)
			{
				if(activationMode == 2)
					m_actions &= ~ACTION_ZEROGBACK;
				else
					m_actions |= ACTION_ZEROGBACK;
			}

			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Back, activationMode);
		}
	}

	return false;
}

bool CPlayerInput::OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if(activationMode == 2)
		{
			if(!(m_moveButtonState&eMBM_Right))
			{
				// for camera
				m_deltaMovement.x = 0;
				m_moveStickLR = 0;

				if (!(m_moveButtonState&eMBM_Forward) && !(m_moveButtonState&eMBM_Back))
					m_actions &= ~ACTION_MOVE;
			}
		}
		else
			m_actions |= ACTION_MOVE;

		if(CheckMoveButtonStateChanged(eMBM_Left, activationMode))
		{
			if(activationMode != 2)
			{
				ApplyMovement(Vec3(-(value*2.0f - 1.0f),0,0));

				// for camera
				m_deltaMovement.x = -value;
				m_moveStickLR = -value;
			}
			
			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Left, activationMode);
			if(m_pPlayer->m_stats.isOnLadder)
				m_pPlayer->m_stats.ladderAction = CPlayer::eLAT_StrafeLeft;
		}
	}

	return false;
}

bool CPlayerInput::OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if(activationMode == 2)
		{
			if(!(m_moveButtonState&eMBM_Left))
			{
				// for camera
				m_deltaMovement.x = 0;
				m_moveStickLR = 0;

				if (!(m_moveButtonState&eMBM_Back) && !(m_moveButtonState&eMBM_Forward))
					m_actions &= ~ACTION_MOVE;
			}
		}
		else
			m_actions |= ACTION_MOVE;

		if(CheckMoveButtonStateChanged(eMBM_Right, activationMode))
		{
			if(activationMode != 2)
			{
				ApplyMovement(Vec3(value*2.0f - 1.0f,0,0));

				// for camera
				m_deltaMovement.x = value;
				m_moveStickLR = value;
			}

			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Right, activationMode);
			if(m_pPlayer->m_stats.isOnLadder)
				m_pPlayer->m_stats.ladderAction = CPlayer::eLAT_StrafeRight;
		}
	}

	return false;
}

bool CPlayerInput::OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_deltaRotation.z -= value;
	m_cameraStickLR = value * g_pGameCVars->cl_cam_mouseYawScale;
	m_mouseInput = true;

	if((m_actions & ACTION_SPRINT) && g_pGameCVars->g_enableSpeedLean)
	{	
		if(value < 0 && m_speedLean > 0)
			m_speedLean = 0.0f;
		else if(value > 0 && m_speedLean < 0)
			m_speedLean = 0.0f;

		m_pPlayer->SetSpeedLean(m_speedLean);
	}

	return false;
}

bool CPlayerInput::OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	/*if(m_pPlayer->GetActorStats()->inZeroG)	//check for flip over in zeroG .. this makes no sense
	{
	SPlayerStats *stats = static_cast<SPlayerStats*> (m_pPlayer->GetActorStats());
	float absAngle = fabsf(acos_tpl(stats->upVector.Dot(stats->zeroGUp)));
	if(absAngle > 1.57f) //90°
	{
	if(value > 0)
	m_deltaRotation.x -= value;
	}
	else
	m_deltaRotation.x -= value;
	}
	else*/
	m_deltaRotation.x -= value;
	if(g_pGameCVars->cl_invertMouse)
		m_deltaRotation.x*=-1.0f;

	m_mouseInput = true;
	m_cameraStickUD = -value * g_pGameCVars->cl_cam_mousePitchScale;

	return false;
}

bool CPlayerInput::OnActionVRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_deltaRotation.z -= value;
	m_cameraStickLR = -value;

	return false;
}

bool CPlayerInput::OnActionVRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_deltaRotation.x -= value;
	if(g_pGameCVars->cl_invertMouse)
		m_deltaRotation.x*=-1.0f;

	m_cameraStickUD = -value;

	return false;
}

bool CPlayerInput::OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	bool canJump = ((m_pPlayer->GetStance() == STANCE_ZEROG) || 
									(m_pPlayer->GetStance() == STANCE_SWIM) ||
									 m_pPlayer->TrySetStance(STANCE_STAND));

	if (CanMove() && canJump && m_pPlayer->GetSpectatorMode() != CActor::eASM_Free && m_pPlayer->GetSpectatorMode() != CActor::eASM_Fixed)
	{
		if (value > 0.0f)
		{
			if(m_actions & ACTION_PRONE || m_actions & ACTION_CROUCH)
			{
				m_actions &= ~ACTION_PRONE;
				m_actions &= ~ACTION_CROUCH;
				return false;
			}

			//if (m_pPlayer->m_params.speedMultiplier > 0.99f)
			m_actions |= ACTION_JUMP;
			if(m_speedLean)
				m_speedLean = 0.0f;
			return true;
		}
	}

	// Moved this outside, since if the top condition is false the JUMP flag might not be cleared, 
	// and the player continues jumping as if the jump key was held.
	m_actions &= ~ACTION_JUMP;

	m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);

	return false;
}

bool CPlayerInput::OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove() && m_pPlayer->GetSpectatorMode() != CActor::eASM_Free && m_pPlayer->GetSpectatorMode() != CActor::eASM_Fixed)
	{
		if (g_pGameCVars->cl_crouchToggle)
		{
			if (value > 0.0f)
			{
				if (!(m_actions & ACTION_CROUCH))
					m_actions |= ACTION_CROUCH;
				else
					m_actions &= ~ACTION_CROUCH;
			}
		}
		else
		{
			if (value > 0.0f)
			{
				//if (m_pPlayer->m_params.speedMultiplier > 0.99f)
				m_actions |= ACTION_CROUCH;
			}
			else
			{
				m_actions &= ~ACTION_CROUCH;
			}
		}
	}

	return false;
}

bool CPlayerInput::OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (g_pGameCVars->cl_sprintToggle)
		{
			if (activationMode == eAAM_OnPress)
			{
				if (value > 0.0f)
				{
					if (!(m_actions & ACTION_SPRINT))
						StartSprint();
					else
						StopSprint();
				}
			}
		}
		else
		{
			if (value > 0.0f)
				StartSprint();
			else
				StopSprint();
		}
	}

	return false;
}



bool CPlayerInput::OnActionToggleStance(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if(CanMove())
	{
		if(activationMode == eAAM_OnPress)
		{
			if(m_actions & ACTION_PRONE || m_actions & ACTION_CROUCH)
			{
				m_fCrouchPressedTime = gEnv->pTimer->GetAsyncTime().GetMilliSeconds();
			}
			else
			{
				m_actions |= ACTION_CROUCH;
				m_fCrouchPressedTime = gEnv->pTimer->GetAsyncTime().GetMilliSeconds();
			}
		}
		else if(activationMode == eAAM_OnRelease)
		{
			if(m_fCrouchPressedTime > 0.0f)
			{
				if(m_actions & ACTION_PRONE)
				{
					m_actions &= ~ACTION_PRONE;
					m_actions |= ACTION_CROUCH;
				}
			}
			m_fCrouchPressedTime = -1.0f;
		}
	}
	return false;
}

bool CPlayerInput::OnActionProne(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	//No prone if holding something
	COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
	if(pOffHand && pOffHand->IsHoldingEntity())
		return false;

	if (!m_pPlayer->m_stats.spectatorMode)
	{		
		if(!m_pPlayer->GetActorStats()->inZeroG)
		{
			if(activationMode == eAAM_OnPress)
			{
				CItem *curItem = static_cast<CItem*>(gEnv->pGame->GetIGameFramework()->GetIItemSystem()->GetItem(m_pPlayer->GetInventory()->GetCurrentItem()));
				if(curItem && curItem->GetParams().prone_not_usable)
				{
					// go crouched instead.
					// Nope, actually do nothing
					// 				if (!(m_actions & ACTION_CROUCH))
					// 					m_actions |= ACTION_CROUCH;
					// 				else
					// 					m_actions &= ~ACTION_CROUCH;
				}
				else
				{
					if (!(m_actions & ACTION_PRONE))
					{
						if(!m_pPlayer->GetActorStats()->inAir)
							m_actions |= ACTION_PRONE;
					}
					else
						m_actions &= ~ACTION_PRONE;
				}
			}
		}
	}
	
	return false;
}

bool CPlayerInput::OnActionGyroscope(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	//FIXME:makes more sense a ExosuitActive()
	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->InZeroG())
	{
		if (m_actions & ACTION_GYROSCOPE)
			if(g_pGameCVars->pl_zeroGSwitchableGyro)
			{
				m_actions &= ~ACTION_GYROSCOPE;
				m_pPlayer->CreateScriptEvent("gyroscope",(m_actions & ACTION_GYROSCOPE)?1.0f:0.0f);
			}
			else
			{
				m_actions |= ACTION_GYROSCOPE;
				m_pPlayer->CreateScriptEvent("gyroscope",(m_actions & ACTION_GYROSCOPE)?1.0f:0.0f);
			}
	}
	return false;
}

bool CPlayerInput::OnActionGBoots(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	return false;
}

bool CPlayerInput::OnActionLeanLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode && !m_pPlayer->m_stats.inFreefall.Value())
	{
		m_actions |= ACTION_LEANLEFT;
		//not sure about this, its for zeroG
		m_deltaRotation.y -= 30.0f;
	}
	return false;
}

bool CPlayerInput::OnActionLeanRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode && !m_pPlayer->m_stats.inFreefall.Value())
	{
		m_actions |= ACTION_LEANRIGHT;
		//not sure about this, its for zeroG
		m_deltaRotation.y += 30.0f;
	}
	return false;
}

/************************NO HOLSTER****************
bool CPlayerInput::OnActionHolsterItem(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode)
	{
		//Don't holster mounted weapons
		if(CItem *pItem = static_cast<CItem*>(m_pPlayer->GetCurrentItem()))
			if(pItem->IsMounted())
				return false;

		COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
		if(pOffHand && (pOffHand->GetOffHandState()==eOHS_INIT_STATE))
		{
			//If offHand was doing something don't holster/unholster item
			bool holster = (m_pPlayer->GetInventory()->GetHolsteredItem())?false:true;
			m_pPlayer->HolsterItem(holster);
		}
	}
	return false;
}
*********************************************************/

bool CPlayerInput::OnActionUse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	bool filterOut = true;
	IVehicle* pVehicle = m_pPlayer->GetLinkedVehicle();

	//FIXME:on vehicles use cannot be used
	if (pVehicle)
	{
		filterOut = false;
	}
	
	if (activationMode==eAAM_OnPress)
	{
		COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
		IEntity *pEntity=gEnv->pEntitySystem->GetEntity(m_pPlayer->GetGameObject()->GetWorldQuery()->GetLookAtEntityId());

		//Drop objects/npc before enter a vehicle
		if(pOffHand)
		{
			if(pOffHand->GetOffHandState()&(eOHS_HOLDING_OBJECT|eOHS_HOLDING_NPC))
			{
				pOffHand->OnAction(m_pPlayer->GetEntityId(), actionId, activationMode, 0);
				return false;
			}
		}

		//--------------------------LADDERS-----------------------------------------------		
		if(m_pPlayer->m_stats.isOnLadder)
		{
			m_pPlayer->RequestLeaveLadder(CPlayer::eLAT_Use);
			return false;
		}
		else
		{
			if(m_pPlayer->IsLadderUsable())
			{
				m_pPlayer->RequestGrabOnLadder(CPlayer::eLAT_Use);
				return false;
			}
		}
	}

	return filterOut;
}

bool CPlayerInput::OnActionThirdPerson(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	// Comment in the two lines below if you don't want to allow 1st/3rd person toggling
	// outside of the editor
	//if (!gEnv->pSystem->IsDevMode())
		//return false;

	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->m_pGameFramework->CanCheat())
	{
		if (!m_pPlayer->GetLinkedVehicle())
		{
			bool isInThirdPerson = m_pPlayer->IsThirdPerson();
			if (!isInThirdPerson || isInThirdPerson && g_pGameCVars->cl_cam_orbit == 1)
			{
				if (g_pGameCVars->g_tpview_control == 1)
				{
					g_pGameCVars->g_tpview_enable = !isInThirdPerson;
				}
				g_pGameCVars->cl_cam_orbit = 0;
				m_pPlayer->ToggleThirdPerson();
			}
			else
			{
				g_pGameCVars->cl_cam_orbit = 1;
			}
		}
	}
	return false;
}

bool CPlayerInput::OnActionFlyMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!gEnv->pSystem->IsDevMode())
		return false;

	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->m_pGameFramework->CanCheat())
	{
		uint8 flyMode=m_pPlayer->GetFlyMode()+1;
		if (flyMode>2)
			flyMode=0;
		m_pPlayer->SetFlyMode(flyMode);

		switch(m_pPlayer->m_stats.flyMode)
		{
		case 0:m_pPlayer->CreateScriptEvent("printhud",0,"FlyMode/NoClip OFF");break;
		case 1:m_pPlayer->CreateScriptEvent("printhud",0,"FlyMode ON");break;
		case 2:m_pPlayer->CreateScriptEvent("printhud",0,"NoClip ON");break;
		}
	}
	return false;
}

bool CPlayerInput::OnActionGodMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!gEnv->pSystem->IsDevMode())
		return false;

	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->m_pGameFramework->CanCheat())
	{
		int godMode(g_pGameCVars->g_godMode);

		godMode = (godMode+1)%4;

		if(godMode && m_pPlayer->GetHealth() <= 0)
		{
			m_pPlayer->StandUp();
			m_pPlayer->Revive(false);
			m_pPlayer->SetHealth(100);
		}

		g_pGameCVars->g_godMode = godMode;
	}
	return false;
}

bool CPlayerInput::OnActionAIDebugDraw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!gEnv->pSystem->IsDevMode())
		return false;

	if(ICVar* pCVar = gEnv->pConsole->GetCVar("ai_DebugDraw"))
	{
		pCVar->Set(pCVar->GetIVal()==0 ? 1 : 0);
		return true;
	}
	return false;
}

bool CPlayerInput::OnActionPDrawHelpers(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!gEnv->pSystem->IsDevMode())
		return false;

	if(ICVar* pCVar = gEnv->pConsole->GetCVar("p_draw_helpers"))
	{
		pCVar->Set(pCVar->GetIVal()==0 ? 1 : 0);
		return true;
	}
	return false;
}


bool CPlayerInput::OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_xi_deltaRotationRaw.z = value;

	m_xi_deltaRotation.z = MapControllerValue(value, g_pGameCVars->ctrl_Coeff_Z, g_pGameCVars->ctrl_Curve_Z, true);




	if(fabs(m_xi_deltaRotation.z) < 0.003f)


		m_xi_deltaRotation.z = 0.f;//some dead point

	m_cameraStickLR = NormaliseStick(value);

	return false;
}

bool CPlayerInput::OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_xi_deltaRotationRaw.z = value;

	m_xi_deltaRotation.x = MapControllerValue(value, g_pGameCVars->ctrl_Coeff_X, g_pGameCVars->ctrl_Curve_X, false);




	if(fabs(m_xi_deltaRotation.x) < 0.003f)


		m_xi_deltaRotation.x = 0.f;//some dead point

	// For the new camera
	// Invert control
	if (g_pGameCVars->cl_invertController)
	{
		value *= -1.0f;
	}
	value = AdjustStickInputVert(value);
	m_cameraStickUD = NormaliseStick(value);

	// camera pitch rotation
	m_deltaPitch = DEG2RAD(value);

	// This was the original new camera's calculation 
	// For integration we stick to the old one above
	//const float valueScaled = value * 20.0f * damping;
	//m_xi_deltaRotation.x = valueScaled;

	return false;
}

bool CPlayerInput::OnActionXIMoveX(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		m_moveStickLR = value;        // for camera
		m_xi_deltaMovement.x = value;
		if(fabsf(value)>0.001f && !m_bUseXIInput)
		{
			m_bUseXIInput = true;
		}
		else if(fabsf(value)<=0.001f && m_bUseXIInput && fabsf(m_xi_deltaMovement.y)<=0.001f)
		{
			m_bUseXIInput = false;
			if (!GetMoveButtonsState())
				m_actions &= ~ACTION_MOVE;

			m_deltaMovement.zero();
		}
	}

	return false;
}

bool CPlayerInput::OnActionXIMoveY(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		m_moveStickUD = value;
		m_xi_deltaMovement.y = value;
		if(fabsf(value)>0.001f && !m_bUseXIInput)
		{
			m_bUseXIInput = true;
		}
		else if(fabsf(value)<=0.001f && m_bUseXIInput && fabsf(m_xi_deltaMovement.x)<=0.001f)
		{
			m_bUseXIInput = false;
			if (!GetMoveButtonsState())
				m_actions &= ~ACTION_MOVE;

			m_deltaMovement.zero();
		}
	}

	return false;
}

bool CPlayerInput::OnActionXIDisconnect(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_xi_deltaRotation.Set(0,0,0);
	m_xi_deltaMovement.zero();
	m_bUseXIInput = false;
	if (!GetMoveButtonsState())
		m_actions &= ~ACTION_MOVE;
	m_deltaMovement.zero();

	return false;
}

bool CPlayerInput::OnActionInvertMouse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	g_pGameCVars->cl_invertMouse = !g_pGameCVars->cl_invertMouse;

	return false;
}


void CPlayerInput::AdjustMoveButtonState(EMoveButtonMask buttonMask, int activationMode )
{
	if (activationMode == eAAM_OnPress)
	{
		m_moveButtonState |= buttonMask;
	}
	else if (activationMode == eAAM_OnRelease)
	{
		m_moveButtonState &= ~buttonMask;
	}
}

bool CPlayerInput::CheckMoveButtonStateChanged(EMoveButtonMask buttonMask, int activationMode)
{
	bool current = (m_moveButtonState & buttonMask) != 0;

	if(activationMode == eAAM_OnRelease)
	{
		return current;
	}
	else if(activationMode == eAAM_OnPress)
	{
		return !current;
	}
	return true;
}

float CPlayerInput::MapControllerValue(float value, float scale, float curve, bool inverse)
{
	// Any attempts to create an advanced analog stick value mapping function could be put here

	// After several experiments a simple pow(x, n) function seemed to work best.
	float res=scale * powf(fabs(value), curve);
	return (value >= 0.0f ? (inverse ? -1.0f : 1.0f) : (inverse ? 1.0f : -1.0f))*res;
}

//////////////////////////////////////////////////////////////////////////
//added for CE3 release

//fake some acceleration though input smoothing
void CPlayerInput::SmoothControllerInput(Ang3 xiDeltaRot)	
{
	static Ang3 g_vDeltaInput = Ang3(0,0,0);
	float fIntensity = max(4.0f, g_pGameCVars->ctrl_input_smoothing);
	if(fabsf(xiDeltaRot.z) < fabsf(g_vDeltaInput.z))
		fIntensity = fIntensity * 0.3f;
	g_vDeltaInput = (g_vDeltaInput * (fIntensity - 1.0f) + xiDeltaRot) / fIntensity;
	xiDeltaRot = g_vDeltaInput;
}


//control tpview by cvar
void CPlayerInput::ControlCameraMode()
{
	if(g_pGameCVars->g_tpview_control)
	{
		if(	( g_pGameCVars->g_tpview_enable && !m_pPlayer->IsThirdPerson() )
			||(!g_pGameCVars->g_tpview_enable && m_pPlayer->IsThirdPerson() )
			)
		{	
			m_pPlayer->ToggleThirdPerson();	
		}
	}
	
	//force GOC if third person
	if(g_pGameCVars->g_tpview_force_goc)
	{
		g_pGameCVars->goc_enable = m_pPlayer->IsThirdPerson();
	}//disable goc yourself when not forcing it anymore
}

// Normalise the input, correcting for the dead zone, then apply a gain/power curve
// the higher gamma is, the more sensitive the stick is at the centre, and the less sensitive it is at the edge.
// the gain speeds up the stick over its entire centre to edge range.
float CPlayerInput::AdjustStickInputHorz(float value)
{
	float gain=1,gamma=1,clamp=1;

	const SCVars* pGameCVars = g_pGameCVars;

	// nav mode
	{
		gain	=	pGameCVars->cl_joy_nav_speedH;
		gamma =	pGameCVars->cl_joy_nav_sensitivityH;
		clamp =	pGameCVars->cl_joy_nav_maxH;
	}

	return AdjustStickInput(value,gain,gamma,clamp);
}

float CPlayerInput::AdjustStickInputVert(float value)
{
	const SCVars* pGameCVars = g_pGameCVars;

	int iUseHForV=0;

	{
		iUseHForV=pGameCVars->cl_joy_nav_useHforV;
	}

	if(0!=iUseHForV)
	{
		value=AdjustStickInputHorz(value);
	}
	else
	{
		float gain=1,gamma=1,clamp=1;

		// nav mode
		{
			gain=pGameCVars->cl_joy_nav_speedV;
			gamma=pGameCVars->cl_joy_nav_sensitivityV;
			clamp=pGameCVars->cl_joy_nav_maxV;
		}

		value = AdjustStickInput(value,gain,gamma,clamp);
	}
	return value;
}

void CPlayerInput::StartSprint()
{
	if (  !( m_actions & ACTION_SPRINT )
		&& (   gEnv->pTimer->GetCurrTime() - m_fRestingTime > g_pGameCVars->cl_sprintRestingTime
			|| gEnv->pTimer->GetCurrTime() - m_fSprintTime < g_pGameCVars->cl_sprintTime ) )
	{
		if ( m_pPlayer->m_params.speedMultiplier*m_pPlayer->GetZoomSpeedMultiplier() > 0.99f )
		{
			m_actions |= ACTION_SPRINT;
			m_pPlayer->m_stats.bIgnoreSprinting = false;
			if ( gEnv->pTimer->GetCurrTime() - m_fRestingTime > g_pGameCVars->cl_sprintRestingTime )
				m_fSprintTime = gEnv->pTimer->GetCurrTime();
			m_fRestingTime = 0;
		}
	}
}

void CPlayerInput::StopSprint()
{
	if ( m_actions & ACTION_SPRINT )
	{
		m_speedLean = 0.0f;
		m_pPlayer->SetSpeedLean(0.0f);
		CItem* pItem = static_cast<CItem*>( m_pPlayer->GetCurrentItem() );
		if( pItem )
			pItem->ForcePendingActions();

		if ( m_fRestingTime == 0 )
			m_fRestingTime = gEnv->pTimer->GetCurrTime();
	}
	m_actions &= ~ACTION_SPRINT;
}
//////////////////////////////////////////////////////////////////////////

CAIInput::CAIInput( CPlayer * pPlayer ) : 
m_pPlayer(pPlayer), 
m_pStats(&pPlayer->m_stats)
{
}

CAIInput::~CAIInput()
{
}

void CAIInput::GetState( SSerializedPlayerInput& input )
{
	SMovementState movementState;
	m_pPlayer->GetMovementController()->GetMovementState( movementState );

	Quat worldRot = m_pPlayer->GetBaseQuat();
	input.stance = movementState.stance;
	input.bodystate = 0;

	IAIActor* pAIActor = CastToIAIActorSafe(m_pPlayer->GetEntity()->GetAI());
	if (pAIActor)
	{
		input.bodystate=pAIActor->GetState().bodystate;
		input.allowStrafing = pAIActor->GetState().allowStrafing;
	}

	float maxSpeed = m_pPlayer->GetStanceMaxSpeed(m_pPlayer->GetStance());
	if(maxSpeed == 0.0f)
		maxSpeed = 1.0f;
	input.deltaMovement = movementState.movementDirection.GetNormalizedSafe()*movementState.desiredSpeed / maxSpeed;;
	input.lookDirection = movementState.eyeDirection;
	
	// PLAYERPREDICTION
	input.bodyDirection = movementState.entityDirection;
	// ~PLAYERPREDICTION
	
	input.sprint = false;
	input.leanl = false;
	input.leanr = false;
	

	IAnimationGraphState *pState=0;
	if (m_pPlayer->GetAnimatedCharacter())
		pState=m_pPlayer->GetAnimatedCharacter()->GetAnimationGraphState();

	if (pState)
	{
		input.aiming = pState->GetInputAsFloat(m_pPlayer->m_inputAiming)!=0.0f;
		input.usinglookik = pState->GetInputAsFloat(m_pPlayer->m_inputUsingLookIK)!=0.0f;
		input.pseudoSpeed=pState->GetInputAsFloat(pState->GetInputId("PseudoSpeed"));
	}
}

void CAIInput::SetState( const SSerializedPlayerInput& input )
{
	GameWarning("CAIInput::SetState called: should never happen");
}
