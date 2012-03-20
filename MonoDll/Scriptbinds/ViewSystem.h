///////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// CryENGINE particle system scriptbind
//////////////////////////////////////////////////////////////////////////
// 17/03/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_VIEW_SYSTEM__
#define __SCRIPTBIND_VIEW_SYSTEM__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

struct SViewParams;

class CScriptbind_ViewSystem : public IMonoScriptBind
{
public:
	CScriptbind_ViewSystem();
	~CScriptbind_ViewSystem() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "View"; }
	// ~IMonoScriptBind

	// Externals
	static EntityId CreateView();
	static void RemoveView(EntityId viewId);

	static EntityId GetActiveView();
	static void SetActiveView(EntityId viewId);

	static SViewParams GetViewParams(EntityId viewId);
	static void SetViewParams(EntityId viewId, SViewParams);
	// ~Externals
};

#endif //__SCRIPTBIND_VIEW_SYSTEM__