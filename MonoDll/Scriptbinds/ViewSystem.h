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
	static unsigned int GetView(unsigned int linkedId, bool forceCreate = false);
	static void RemoveView(unsigned int viewId);

	static EntityId GetActiveView();
	static void SetActiveView(unsigned int viewId);

	static SViewParams GetViewParams(unsigned int viewId);
	static void SetViewParams(unsigned int viewId, SViewParams);
	// ~Externals
};

#endif //__SCRIPTBIND_VIEW_SYSTEM__