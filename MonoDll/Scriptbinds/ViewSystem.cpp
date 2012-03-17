#include "StdAfx.h"
#include "ViewSystem.h"

#include <IViewSystem.h>
#include <IGameFramework.h>

CScriptbind_ViewSystem::CScriptbind_ViewSystem()
{
	REGISTER_METHOD(CreateView);
	REGISTER_METHOD(RemoveView);

	REGISTER_METHOD(GetActiveView);
	REGISTER_METHOD(SetActiveView);

	REGISTER_METHOD(GetViewParams);
	REGISTER_METHOD(SetViewParams);
}

EntityId CScriptbind_ViewSystem::CreateView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetViewId(pViewSystem->CreateView());

	return 0;
}

void CScriptbind_ViewSystem::RemoveView(EntityId viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetViewByEntityId(viewId))
			pViewSystem->RemoveView(viewId);
	}
}

EntityId CScriptbind_ViewSystem::GetActiveView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetActiveViewId();

	return 0;
}

void CScriptbind_ViewSystem::SetActiveView(EntityId viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetViewByEntityId(viewId))
			pViewSystem->SetActiveView(viewId);
	}
}

SViewParams CScriptbind_ViewSystem::GetViewParams(EntityId viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetView(viewId))
			return *pView->GetCurrentParams();
	}

	return SViewParams();
}

void CScriptbind_ViewSystem::SetViewParams(EntityId viewId, SViewParams viewParams)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetView(viewId))
		{
			viewParams.SaveLast();

			pView->SetCurrentParams(viewParams);
		}
	}
}