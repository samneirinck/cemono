/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Script Binding for Weapon
  
 -------------------------------------------------------------------------
  History:
  - 25:11:2004   11:30 : Created by Márcio Martins

*************************************************************************/
#ifndef __SCRIPTBIND_WEAPON_H__
#define __SCRIPTBIND_WEAPON_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <IScriptSystem.h>
#include <ScriptHelpers.h>


struct IItemSystem;
struct IGameFramework;
class CItem;
class CWeapon;


class CScriptBind_Weapon :
	public CScriptableBase
{
public:
	CScriptBind_Weapon(ISystem *pSystem, IGameFramework *pGameFramework);
	virtual ~CScriptBind_Weapon();

	void AttachTo(CWeapon *pWeapon);

	// <title SetAmmoCount>
	// Syntax: Weapon.SetAmmoCount(const char *ammoName, int ammo)
	// Arguments:
	//		ammoName - Ammunition name.
	//		ammo	 - Ammunition amount.
	// Description:
	//		Sets the amount of the specified ammunition.
	int SetAmmoCount(IFunctionHandler *pH);
	// <title GetAmmoCount>
	// Syntax: Weapon.GetAmmoCount()
	// Description:
	//		Gets the amount of ammunition for the current fire mode.
	int GetAmmoCount(IFunctionHandler *pH);
	// <title GetClipSize>
	// Syntax: Weapon.GetClipSize()
	// Description:
	//		Get the clip size for the current fire mode.
	int GetClipSize(IFunctionHandler *pH);

	// <title IsZoomed>
	// Syntax: Weapon.IsZoomed()
	// Description:
	//		Checks if the current view is zoomed.
	int IsZoomed(IFunctionHandler *pH);
	// <title IsZooming>
	// Syntax: Weapon.IsZooming()
	// Description:
	//		Checks if the player is zooming with this weapon.
	int IsZooming(IFunctionHandler *pH);
	// <title GetDamage>
	// Syntax: Weapon.GetDamage()
	// Description:
	//		Gets the damage caused by the current fire mode.
	int GetDamage(IFunctionHandler *pH);
	// <title GetAmmoType>
	// Syntax: Weapon.GetAmmoType()
	// Description:
	//		Gets the ammunition type used by the current fire mode.
	int GetAmmoType(IFunctionHandler *pH);

	// <title GetRecoil>
	// Syntax: Weapon.GetRecoil()
	// Description:
	//		Gets the recoil of the current fire mode.
	int GetRecoil(IFunctionHandler *pH);
	// <title GetSpread>
	// Syntax: Weapon.GetSpread()
	// Description:
	//		Gets the spread of the current fire mode.
	int GetSpread(IFunctionHandler *pH);
	// <title GetCrosshair>
	// Syntax: Weapon.GetCrosshair()
	// Description:
	//		Gets the crosshair of the current fire mode.
	int GetCrosshair(IFunctionHandler *pH);
	// <title GetCrosshairOpacity>
	// Syntax: Weapon.GetCrosshairOpacity()
	// Description:
	//		Gets the crosshair opacity of the current fire mode.
	int GetCrosshairOpacity(IFunctionHandler *pH);
	// <title GetCrosshairVisibility>
	// Syntax: Weapon.GetCrosshairVisibility()
	// Description:
	//		Gets the crosshair visibility of the current fire mode.
	int GetCrosshairVisibility(IFunctionHandler *pH);
	// <title ModifyCommit>
	// Syntax: Weapon.ModifyCommit()
	// Description:
	//		Modifies the commit.
	int ModifyCommit(IFunctionHandler *pH);
	// <title SupportsAccessory>
	// Syntax: Weapon.SupportsAccessory( const char *accessoryName )
	// Description:
	//		Checks if the weapon supports the specified accessory
	int SupportsAccessory(IFunctionHandler *pH, const char *accessoryName);
	// <title GetAccessory>
	// Syntax: Weapon.GetAccessory( const char *accessoryName )
	// Description:
	//		Gets the specified accessory.
	int GetAccessory(IFunctionHandler *pH, const char *accessoryName);
	// <title AttachAccessoryPlaceHolder>
	// Syntax: Weapon.AttachAccessoryPlaceHolder(SmartScriptTable accessory, bool attach)
	// Arguments:
	//		accessory - Accessory identifier.
	//		attach	  - True to attach the accessory to the weapon, false to detach it.
	// Description:
	//		Attaches/detaches the accessory to the weapon.
	int AttachAccessoryPlaceHolder(IFunctionHandler *pH, SmartScriptTable accessory, bool attach);
	// <title GetAttachmentHelperPos>
	// Syntax: Weapon.GetAttachmentHelperPos( const char *helperName )
	// Arguments:
	//		helperName - Helper name.
	// Description:
	//		Gets the attachment helper position.
	int GetAttachmentHelperPos(IFunctionHandler *pH, const char *helperName);
	// <title GetShooter>
	// Syntax: Weapon.GetShooter()
	// Description:
	//		Gets the shooter identifier.
	int GetShooter(IFunctionHandler *pH);
	// <title ScheduleAttach>
	// Syntax: Weapon.ScheduleAttach( const char *className, bool attach )
	// Arguments:
	//		className	- Attach class name.
	//		attach		- True to schedule the attach, false to unschedule.
	// Description:
	//		Schedules/unschedules the specified attach.
	int ScheduleAttach(IFunctionHandler *pH, const char *className, bool attach);
	// <title AttachAccessory>
	// Syntax: Weapon.AttachAccessory( const char *className, bool attach, bool force )
	// Arguments:
	//		className	- Attach class name.
	//		attach		- True to attach, false to detach.
	//		force		- True to force the attach, false otherwise.
	// Description:
	//		Attaches/detaches the specified accessory.
	int AttachAccessory(IFunctionHandler *pH, const char *className, bool attach, bool force);
	// <title SwitchAccessory>
	// Syntax: Weapon.SwitchAccessory( const char *className )
	// Arguments:
	//		className - Accessory name.
	// Description:
	//		Switch with another accessory.
	int SwitchAccessory(IFunctionHandler *pH, const char *className);

	// <title AttachTo>
	// Syntax: Weapon.AttachTo()
	// Description:
	//		Checks if the weapon is firing.
	int IsFiring(IFunctionHandler *pH);

	// <title EnableFireMode>
	// Syntax: Weapon.EnableFireMode(const char *name, bool enable)
	// Arguments:
	//      name - Fire mode name.
	//      enable - enable (true) / disable (false) the fire mode
	// Description:
	//      Sets the current fire mode.
	int EnableFireMode(IFunctionHandler *pH, const char* name, bool enable);

	// <title IsFireModeEnabled>
	// Syntax: Weapon.IsFireModeEnabled(const char *name)
	// Arguments:
	//      name - Fire mode name.
	// Description:
	//      Returns true if specified fire mode is enabled, false otherwise.
	int IsFireModeEnabled(IFunctionHandler *pH, const char* name);

	// <title SetCurrentFireMode>
	// Syntax: Weapon.SetCurrentFireMode(const char *name)
	// Arguments:
	//		name - Fire mode name.
	// Description:
	//		Sets the current fire mode.
	int SetCurrentFireMode(IFunctionHandler *pH, const char *name);

	// <title GetCurrentFireMode>
	// Syntax: Weapon.GetCurrentFireMode()
	// Description:
	//      Returns the name of the current fire mode, nil if none.
	int GetCurrentFireMode(IFunctionHandler *pH);

	// <title GetFireMode>
	// Syntax: Weapon.GetFireMode(int idx)
	// Arguments:
	//      idx - Zero-based index of requested fire mode.
	// Description:
	//      Returns the fire mode name at the specified index, nil if invalid index.
	int GetFireMode(IFunctionHandler *pH, int idx);

	// <title GetFireModeIdx>
	// Syntax: Weapon.GetFireModeIdx(const char* name)
	// Arguments:
	//      name - Fire mode name.
	// Description:
	//      Returns the zero-based index of the specified fire mode, nil if none invalid name.
	int GetFireModeIdx(IFunctionHandler *pH, const char* name);

	// <title GetNumOfFireModes>
	// Syntax: Weapon.GetNumOfFireModes()
	// Description:
	//      Returns the number of fire modes defined in this weapon
	int GetNumOfFireModes(IFunctionHandler *pH);

	// <title SetCurrentZoomMode>
	// Syntax: Weapon.SetCurrentZoomMode(const char *name)
	// Arguments:
	//		name - Zoom mode name.
	// Description:
	//		Sets the current zoom mode.
	int SetCurrentZoomMode(IFunctionHandler *pH, const char *name);

	// <title AutoShoot>
	// Syntax: Weapon.AutoShoot(int nshots, bool autoReload)
	// Arguments:
	//		nshots - Number of shots before reloading.
	//		autoReload - True to auto reload, false otherwise.
	// Description:
	//		Sets the auto shoot mode.
	int AutoShoot(IFunctionHandler *pH, int nshots, bool autoReload);

	// <title Reload>
	// Syntax: Weapon.Reload()
	// Description:
	//		Reloads the weapon.
	int Reload(IFunctionHandler *pH);

	// <title ActivateLamLaser>
	// Syntax: Weapon.ActivateLamLaser( bool activate )
	// Arguments:
	//		activate - True to activate  the lam laser, false otherwise.
	// Description:
	//		Activates/deactivates the lam laser.
	int ActivateLamLaser(IFunctionHandler *pH, bool activate);
	// <title ActivateLamLight>
	// Syntax: Weapon.ActivateLamLight( bool activate )
	// Arguments:
	//		activate - True to activate  the lam light, false otherwise.
	// Description:
	//		Activates/deactivates the lam light.
	int ActivateLamLight(IFunctionHandler *pH, bool activate);

private:
	void RegisterGlobals();
	void RegisterMethods();

	CItem *GetItem(IFunctionHandler *pH);
	CWeapon *GetWeapon(IFunctionHandler *pH);

	ISystem						*m_pSystem;
	IScriptSystem			*m_pSS;
	IGameFramework		*m_pGameFW;
};


#endif //__SCRIPTBIND_ITEM_H__