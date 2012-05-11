#include "StdAfx.h"
#include "MonoInput.h"

#include <IGameFramework.h>

#include <IMonoAssembly.h>
#include <IMonoClass.h>
#include <IMonoArray.h> 

TActionHandler<CInput>	CInput::s_actionHandler;

CInput::CInput()
	: m_pClass(NULL)
{
	REGISTER_METHOD(RegisterAction);

	gEnv->pGameFramework->GetIActionMapManager()->AddExtraActionListener(this);
	gEnv->pHardwareMouse->AddListener(this);
	gEnv->pInput->AddEventListener(this);
}

CInput::~CInput()
{
	gEnv->pGameFramework->GetIActionMapManager()->RemoveExtraActionListener(this);
	gEnv->pHardwareMouse->RemoveListener(this);
}

void CInput::Reset()
{
	m_pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass(GetClassName());
}

void CInput::OnHardwareMouseEvent(int iX,int iY,EHARDWAREMOUSEEVENT eHardwareMouseEvent, int wheelDelta)
{
	IMonoArray *pParams = CreateMonoArray(4);
	pParams->Insert(iX);
	pParams->Insert(iY);
	pParams->Insert(eHardwareMouseEvent);
	pParams->Insert(wheelDelta);

	m_pClass->CallMethod("OnMouseEvent", pParams, true);
	SAFE_RELEASE(pParams);
}

bool CInput::OnInputEvent(const SInputEvent &event)
{
	IMonoArray *pParams = CreateMonoArray(2);
	pParams->Insert(event.keyName.c_str());
	pParams->Insert(event.value);

	m_pClass->CallMethod("OnKeyEvent", pParams, true);
	SAFE_RELEASE(pParams);

	return false;
}

void CInput::OnAction(const ActionId& actionId, int activationMode, float value)
{
	s_actionHandler.Dispatch(this, 0, actionId, activationMode, value);
}

bool CInput::OnActionTriggered(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	IMonoArray *pParams = CreateMonoArray(3);
	pParams->Insert(actionId.c_str());
	pParams->Insert(activationMode);
	pParams->Insert(value);

	m_pClass->CallMethod("OnActionTriggered", pParams, true);
	SAFE_RELEASE(pParams);

	return false;
}

// Scriptbinds
void CInput::RegisterAction(mono::string actionName)
{
	if(!s_actionHandler.GetHandler(ToCryString(actionName)))
		s_actionHandler.AddHandler(ToCryString(actionName), &CInput::OnActionTriggered);
}