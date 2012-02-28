#include "StdAfx.h"
#include "MonoInput.h"

#include <IGameFramework.h>

#include <IMonoAssembly.h>
#include <IMonoClass.h>
#include <IMonoArray.h> 

TActionHandler<CInput>	CInput::s_actionHandler;

CInput::CInput()
{
	REGISTER_METHOD(RegisterAction);

	gEnv->pGameFramework->GetIActionMapManager()->AddExtraActionListener(this);
}

CInput::~CInput()
{
	gEnv->pGameFramework->GetIActionMapManager()->RemoveExtraActionListener(this);
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

	gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("InputSystem")->CallMethod("OnActionTriggered", pParams, true);
	SAFE_RELEASE(pParams);

	return false;
}

// Scriptbinds
void CInput::RegisterAction(mono::string actionName)
{
	s_actionHandler.AddHandler(ToCryString(actionName), &CInput::OnActionTriggered);
}