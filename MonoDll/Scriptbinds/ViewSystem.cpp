#include "StdAfx.h"
#include "ViewSystem.h"

#include <IViewSystem.h>
#include <IGameFramework.h>

#include <IEntitySystem.h>

CScriptbind_ViewSystem::CScriptbind_ViewSystem()
{
	REGISTER_METHOD(GetView);
	REGISTER_METHOD(RemoveView);

	REGISTER_METHOD(GetActiveView);
	REGISTER_METHOD(SetActiveView);

	REGISTER_METHOD(SetViewParams);
	REGISTER_METHOD(GetViewParams);
}

unsigned int CScriptbind_ViewSystem::GetView(EntityId id, bool forceCreate)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetViewByEntityId(id, forceCreate))
			return pViewSystem->GetViewId(pView);
	}

	return 0;
}

void CScriptbind_ViewSystem::RemoveView(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetView(viewId))
			pViewSystem->RemoveView(viewId);
	}
}

unsigned int CScriptbind_ViewSystem::GetActiveView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetActiveViewId();

	return 0;
}

void CScriptbind_ViewSystem::SetActiveView(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetView(viewId))
			pViewSystem->SetActiveView(viewId);
	}
}

SViewParams CScriptbind_ViewSystem::GetViewParams(unsigned int viewId)
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