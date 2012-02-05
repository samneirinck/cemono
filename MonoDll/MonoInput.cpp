#include "StdAfx.h"
#include "MonoInput.h"

#include <IGameFramework.h>

#include <IMonoAssembly.h>
#include <IMonoClass.h>

TActionHandler<CMonoInput>	CMonoInput::s_actionHandler;

CMonoInput::CMonoInput()
{
	REGISTER_METHOD(RegisterAction);

	gEnv->pGameFramework->GetIActionMapManager()->AddExtraActionListener(this);

	m_pClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("InputSystem");
}

CMonoInput::~CMonoInput()
{
	gEnv->pGameFramework->GetIActionMapManager()->RemoveExtraActionListener(this);
}

void CMonoInput::OnAction(const ActionId& actionId, int activationMode, float value)
{
	s_actionHandler.Dispatch(this, 0, actionId, activationMode, value);
}

bool CMonoInput::OnActionTriggered(EntityId entityId, const ActionId& actionId, int activationMode, float value)
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
void CMonoInput::RegisterAction(mono::string actionName)
{
	s_actionHandler.AddHandler(ToCryString(actionName), &CMonoInput::OnActionTriggered);
}