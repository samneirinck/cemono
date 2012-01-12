/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Definitions for typedefs and enumerated types specific to items

-------------------------------------------------------------------------
History:
- 22:10:2009   10:15 : Created by Claire Allan

*************************************************************************/
#ifndef __ITEMDEFINITIONS_H__
#define __ITEMDEFINITIONS_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "ItemString.h"

struct SAttachmentHelper;
struct SAccessoryParams;
struct SItemAction;
struct SCachedItemResourceString;
struct SLayer;
struct SParentFireModeParams;
struct SParentZoomModeParams;
struct SZoomModeParams;

typedef CryFixedStringT<256>										TempResourceName;
typedef std::vector<SAttachmentHelper>					THelperVector;
typedef std::vector<ItemString>									TInitialSetup;
typedef std::map<ItemString, bool>							TDualWieldSupportMap;
typedef std::map<IEntityClass*, int>						TAccessoryAmmoMap;
typedef std::map<ItemString, SAccessoryParams>	TAccessoryParamsMap;
typedef std::map<ItemString, SItemAction>				TActionMap;
typedef	std::vector<SCachedItemResourceString>	TResourceStringCache;
typedef std::map<ItemString, SLayer>						TLayerMap;
typedef std::map<IEntityClass*, int>						TAmmoMap;
typedef std::vector<SParentFireModeParams>			TParentFireModeParamsVector;
typedef std::vector<SParentZoomModeParams>			TParentZoomModeParamsVector;

enum eGeometrySlot
{
	eIGS_FirstPerson = 0,
	eIGS_ThirdPerson,
	eIGS_Arms,
	eIGS_Aux0,
	eIGS_Owner,
	eIGS_OwnerLooped,
	eIGS_OffHand,
	eIGS_Destroyed,
	eIGS_Aux1,
	eIGS_ThirdPersonAux,
	eIGS_Last,
};

enum ELTAGGrenadeType
{
	ELTAGGrenadeType_RICOCHET,
	ELTAGGrenadeType_STICKY,
	ELTAGGrenadeType_LAST
};

struct SWeaponAmmo
{
	SWeaponAmmo()
		: pAmmoClass(NULL)
		, count(0)
	{

	}

	SWeaponAmmo(IEntityClass* pClass, int ammoCount)
		: pAmmoClass(pClass)
		, count(ammoCount)
	{

	}

	void GetMemoryUsage(ICrySizer * s) const {}

	IEntityClass* pAmmoClass;
	int						count;
};

typedef std::vector<SWeaponAmmo>						TAmmoVector;

struct SWeaponAmmoUtils
{
	static const SWeaponAmmo* FindAmmoConst(const TAmmoVector& ammoVector, IEntityClass* pAmmoType)
	{
		const int ammoCount = ammoVector.size();
		for (int i = 0; i < ammoCount; ++i)
		{
			if (pAmmoType == ammoVector[i].pAmmoClass)
			{
				return &(ammoVector[i]); 
			}
		}

		return NULL;
	}

	static SWeaponAmmo* FindAmmo(TAmmoVector& ammoVector, IEntityClass* pAmmoType)
	{
		const int ammoCount = ammoVector.size();
		for (int i = 0; i < ammoCount; ++i)
		{
			if (pAmmoType == ammoVector[i].pAmmoClass)
			{
				return &(ammoVector[i]); 
			}
		}

		return NULL;
	}

	// returns true if the the ammo type was already found in the ammoVector
	static bool SetAmmo(TAmmoVector& ammoVector, IEntityClass* pClass, int count)
	{
		SWeaponAmmo* pAmmo = SWeaponAmmoUtils::FindAmmo(ammoVector, pClass);
		if (pAmmo != NULL)
		{
			pAmmo->count = count;
			return true;
		}
		else
		{
			ammoVector.push_back(SWeaponAmmo(pClass, count));
			return false;
		}
	}

	static int GetAmmoCount(const TAmmoVector& ammoVector, IEntityClass* pClass)
	{
		const SWeaponAmmo* pAmmo = SWeaponAmmoUtils::FindAmmoConst(ammoVector, pClass);

		return pAmmo ? pAmmo->count : 0;
	}
};

struct SItemStrings
{
	SItemStrings();
	~SItemStrings();

	// here they are
	ItemString activate;						// "activate";
	ItemString begin_reload;				// "begin_reload";
	ItemString cannon;							// "cannon";
	ItemString change_firemode;		// "change_firemode";
	ItemString change_firemode_zoomed; // "change_firemode_zoomed";
	ItemString crawl;							// "crawl";
	ItemString deactivate;					// "deactivate";
	ItemString deselect;						// "deselect";
	ItemString destroy;						// "destroy";
	ItemString enter_modify;				// "enter_modify";
	ItemString exit_reload_nopump; // "exit_reload_nopump";
	ItemString exit_reload_pump;		// "exit_reload_pump";
	ItemString fire;								// "fire";
	ItemString idle;								// "idle";
	ItemString idle_relaxed;				// "idle_relaxed";
	ItemString idle_raised;				// "idle_raised";
	ItemString jump_end;						// "jump_end";
	ItemString jump_idle;					// "jump_idle";
	ItemString jump_start;					// "jump_start";
	ItemString leave_modify;				// "leave_modify";
	ItemString left_item_attachment; // "left_item_attachment";
	ItemString lock;								// "lock";
	ItemString lower;							// "lower";
	ItemString modify_layer;       // "modify_layer";
	ItemString nw;									// "nw";
	ItemString left_hand;							// "left"
	ItemString right_hand;							// "right"
	ItemString offhand_on;					// "offhand_on";
	ItemString offhand_off;				// "offhand_off";
	ItemString pickedup;						// "pickedup";
	ItemString pickup_weapon_left; // "pickup_weapon_left";
	ItemString raise;							// "raise";
	ItemString reload_shell;				// "reload_shell";
	ItemString right_item_attachment;// "right_item_attachment";
	ItemString run_forward;				// "run_forward";
	ItemString SCARSleepAmmo;			// "SCARSleepAmmo";
	ItemString SCARTagAmmo;				// "SCARTagAmmo";
	ItemString select;							// "select";
	ItemString first_select;				// "first_select"; (For the LAW)
	ItemString select_grenade;			// "select_grenade";
	ItemString swim_idle;					// "swim_idle";
	ItemString swim_forward;				// "swim_forward";
	ItemString swim_forward_2;			// "swim_forward_2";
	ItemString swim_backward;			// "swim_backward";
	ItemString speed_swim;					// "speed_swim";
	ItemString turret;							// "turret";
	ItemString enable_light;        
	ItemString disable_light;
	ItemString use_light;
	ItemString LAM;
	ItemString LAMRifle;
	ItemString LAMFlashLight;
	ItemString LAMRifleFlashLight;
	ItemString Silencer;
	ItemString SOCOMSilencer;
	ItemString nightvision_on;
	ItemString nightvision_off;
	ItemString zoom_in;
	ItemString zoom_out;

	ItemString lever_layer_1;
	ItemString lever_layer_2;
};

extern struct SItemStrings* g_pItemStrings;

#endif //__ITEMDEFINITIONS_H__
