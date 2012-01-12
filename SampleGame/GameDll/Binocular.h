/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Binocular Implementation

-------------------------------------------------------------------------
History:
- 18:12:2005   14:00 : Created by Márcio Martins
- 08:01:2011   11:11 : Sascha Hoba

*************************************************************************/
#ifndef __BINOCULAR_H__
#define __BINOCULAR_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <IItemSystem.h>
#include "Weapon.h"


class CBinocular :	public CWeapon
{
	struct EndRaiseWeaponAction;

public:

	CBinocular();

	virtual void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value);
	virtual void Select(bool select);
	virtual void GetMemoryUsage(ICrySizer * s) const { s->Add(*this); CWeapon::GetMemoryUsage(s); }
	virtual void OnDropped(EntityId actorId);
	virtual void UpdateFPView(float frameTime);

private:

	~CBinocular();

	static TActionHandler<CBinocular> s_actionHandler;
	bool OnActionZoom(EntityId actorId, const ActionId& actionId, int activationMode, float value);
	bool OnActionZoomIn(EntityId actorId, const ActionId& actionId, int activationMode, float value);
	bool OnActionZoomOut(EntityId actorId, const ActionId& actionId, int activationMode, float value);
	bool OnActionToggleNightVision(EntityId actorId, const ActionId& actionId, int activationMode, float value);

	void Zoom();
	void ResetState();

	bool m_bZoomed;
	bool m_bNightVisionEnabled;
	ICVar* m_pNightVisionCVar;
	ICVar* m_pDefaultNVMode;
	ICVar* m_pSpecCVar;
	IPhysicalWorld* m_pPhysWorld;
};

#endif // __BINOCULAR_H__