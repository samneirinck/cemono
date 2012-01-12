// handles turning actions into CMovementRequests and setting player state
// for the local player

#ifndef __PLAYERINPUT_H__
#define __PLAYERINPUT_H__

#pragma once

#include "IActionMapManager.h"
#include "IPlayerInput.h"
#include "Player.h"

class CPlayer;
struct SPlayerStats;

class CPlayerInput : public IPlayerInput, public IActionListener, public IPlayerEventListener
{
public:

	friend class CCameraInputHelper;

	enum EMoveButtonMask
	{
		eMBM_Forward	= (1 << 0),
		eMBM_Back			= (1 << 1),
		eMBM_Left			= (1 << 2),
		eMBM_Right		= (1 << 3)
	};

	CPlayerInput( CPlayer * pPlayer );
	~CPlayerInput();

	// IPlayerInput
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	// ~IPlayerInput

	// IActionListener
	virtual void OnAction( const ActionId& action, int activationMode, float value );
	// ~IActionListener
	
	// IPlayerEventListener
	virtual void OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded);
	// ~IPlayerEventListener

	virtual void SetState( const SSerializedPlayerInput& input );
	virtual void GetState( SSerializedPlayerInput& input );

	virtual void Reset();
	virtual void DisableXI(bool disabled);

	virtual void GetMemoryUsage(ICrySizer * s) const {s->Add(*this);}

	virtual EInputType GetType() const
	{
		return PLAYER_INPUT;
	};

	ILINE virtual uint32 GetMoveButtonsState() const { return m_moveButtonState; }
	ILINE virtual uint32 GetActions() const { return m_actions; }

	//this helper computes mode transitions and camera pitch
	virtual CCameraInputHelper *GetCameraInputHelper() const { return m_pCameraInputHelper; }

	ILINE Ang3& GetRawControllerInput() { return m_xi_deltaRotationRaw; }
	ILINE Ang3& GetRawMouseInput() { return m_lastMouseRawInput; }

	// ~IPlayerInput

	virtual void SerializeSaveGame( TSerialize ser );

private:

	EStance FigureOutStance();
	void AdjustMoveButtonState( EMoveButtonMask buttonMask, int activationMode );
	bool CheckMoveButtonStateChanged( EMoveButtonMask buttonMask, int activationMode );
	float MapControllerValue(float value, float scale, float curve, bool inverse);

	void ApplyMovement(Vec3 delta);
	const Vec3 &FilterMovement(const Vec3 &desired);

	bool CanMove() const;

	// Normalise stick input and apply a sensitivity curve
	float AdjustStickInputHorz(float value);
	// Normalise stick input and apply a sensitivity curve
	float AdjustStickInputVert(float value);

	bool OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionVRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value); // needed so player can shake unfreeze while in a vehicle
	bool OnActionVRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionToggleStance(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionProne(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	//bool OnActionZeroGBrake(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionGyroscope(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionGBoots(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionLeanLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionLeanRight(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	//bool OnActionHolsterItem(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionUse(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	
	// Cheats
	bool OnActionThirdPerson(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionFlyMode(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionGodMode(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionAIDebugDraw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionPDrawHelpers(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionXIMoveX(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionXIMoveY(EntityId entityId, const ActionId& actionId, int activationMode, float value);
	bool OnActionXIDisconnect(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	bool OnActionInvertMouse(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	void SmoothControllerInput(Ang3 xiDeltaRot);	//fake some acceleration through input smoothing
	void ControlCameraMode();						//control whether to display 1st or 3rd person cam, depending on cvars

	void StopSprint();
	void StartSprint();

private:
	Vec3 m_lastPos;

	CPlayer* m_pPlayer;
	SPlayerStats* m_pStats;
	uint32 m_actions;
	uint32 m_lastActions;
	Vec3 m_deltaMovement;
	Vec3 m_xi_deltaMovement;
  Vec3 m_deltaMovementPrev;
	Ang3 m_deltaRotation;
	Ang3 m_lastMouseRawInput;
	Ang3 m_xi_deltaRotation;
	Ang3 m_xi_deltaRotationRaw;
	float m_speedLean;
	float	m_buttonPressure;
	bool m_bDisabledXIRot;
	float	m_fCrouchPressedTime;
	bool m_bUseXIInput;
	uint32 m_moveButtonState;
	Vec3 m_filteredDeltaMovement;
	bool m_checkZoom;
	int m_iCarryingObject;
	int m_lastSerializeFrameID;

	bool m_doubleJumped;

	float m_fSprintTime;
	float m_fRestingTime;

	//camera helpers
	bool m_mouseInput;
	float m_deltaPitch;
	float m_deltaPitchOnce;
	float m_cameraStickLR;
	float m_cameraStickUD;
	float m_moveStickLR;
	float m_moveStickUD;
	CCameraInputHelper *m_pCameraInputHelper;

	static TActionHandler<CPlayerInput>	s_actionHandler;
};




class CAIInput : public IPlayerInput
{
public:
	enum EMoveButtonMask
	{
		eMBM_Forward	= (1 << 0),
		eMBM_Back			= (1 << 1),
		eMBM_Left			= (1 << 2),
		eMBM_Right		= (1 << 3)
	};

	CAIInput( CPlayer * pPlayer );
	~CAIInput();

	// IPlayerInput
	virtual void PreUpdate() {};
	virtual void Update() {};
	virtual void PostUpdate() {};

	virtual void OnAction( const ActionId& action, int activationMode, float value ) {};

	virtual void SetState( const SSerializedPlayerInput& input );
	virtual void GetState( SSerializedPlayerInput& input );

	virtual void Reset() {};
	virtual void DisableXI(bool disabled) {};

	ILINE virtual uint32 GetMoveButtonsState() const { return 0; }
	ILINE virtual uint32 GetActions() const { return 0; }

	virtual void GetMemoryUsage(ICrySizer * s) const {s->Add(*this);}

	virtual EInputType GetType() const
	{
		return AI_INPUT;
	};

	// ~IPlayerInput

private:
	CPlayer* m_pPlayer;
	SPlayerStats* m_pStats;
	Vec3 m_deltaMovement;
};

#endif
