/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Script Binding for Item
  
 -------------------------------------------------------------------------
  History:
  - 27:10:2004   11:30 : Created by Márcio Martins

*************************************************************************/
#ifndef __SCRIPTBIND_ITEM_H__
#define __SCRIPTBIND_ITEM_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <IScriptSystem.h>
#include <ScriptHelpers.h>


struct IItemSystem;
struct IGameFramework;
class CItem;
class CActor;


class CScriptBind_Item :
	public CScriptableBase
{
public:
	CScriptBind_Item(ISystem *pSystem, IGameFramework *pGameFramework);
	virtual ~CScriptBind_Item();
	void AttachTo(CItem *pItem);

	// <title SetExtensionActivation>
	// Syntax: Item.SetExtensionActivation( const char *extension, bool activation )
	// Arguments:
	//		extension	- Extension name.
	//		activation	- True to activate, false to deactivate.
	// Description:
	//		Activates/deactivates an extension.
	int SetExtensionActivation(IFunctionHandler *pH, const char *extension, bool activation);
	// <title SetExtensionParams>
	// Syntax: Item.SetExtensionParams( const char *extension, SmartScriptTable params )
	// Arguments:
	//		extension - Extension name.
	//		params	  - Extension parameters.
	// Description:
	//		Sets extension parameters.
	int SetExtensionParams(IFunctionHandler* pH, const char *extension, SmartScriptTable params);
	// <title GetExtensionParams>
	// Syntax: Item.GetExtensionParams( const char *extension, SmartScriptTable params )
	// Arguments:
	//		extension - Extension name.
	//		params	  - Extension parameters.
	// Description:
	//		Gets extension parameters.
	int GetExtensionParams(IFunctionHandler* pH, const char *extension, SmartScriptTable params);

	// <title Select>
	// Syntax: Item.Select( bool select )
	// Arguments:
	//		select - True to select, false to deselect.
	// Description:
	//		Selects/deselects the item.
	int Select(IFunctionHandler *pH, bool select);
	// <title GetStats>
	// Syntax: Item.GetStats()
	// Description:
	//		Gets the item statistics.
	int GetStats(IFunctionHandler *pH);
	// <title GetParams>
	// Syntax: Item.GetParams()
	// Description:
	//		Gets the item parameters.
	int GetParams(IFunctionHandler *pH);

	// <title Reset>
	// Syntax: Item.Reset()
	// Description:
	//		Resets the item.
	int Reset(IFunctionHandler *pH);
	// <title Quiet>
	// Syntax: Item.Quiet()
	// Description:
	//		Quiets the item.
	int Quiet(IFunctionHandler *pH);

	// <title CanPickUp>
	// Syntax: Item.CanPickUp( ScriptHandle userId )
	// Arguments:
	//		userId - User identifier.
	// Description:
	//		Checks if the item can be picked up by the user.
	int CanPickUp(IFunctionHandler *pH, ScriptHandle userId);
	// <title CanUse>
	// Syntax: Item.CanUse( ScriptHandle userId )
	// Arguments:
	//		userId - User identifier.
	// Description:
	//		Checks if the item can be used by the user.
	int CanUse(IFunctionHandler *pH, ScriptHandle userId);
	// <title IsMounted>
	// Syntax: Item.IsMounted()
	// Description:
	//		Checks if the item is mounted.	
	int IsMounted(IFunctionHandler *pH);
	
	// <title PlayAction>
	// Syntax: Item.PlayAction( const char *actionName )
	// Arguments:
	//		actionName - Action name.
	// Description:
	//		Plays the specified action.
	int PlayAction(IFunctionHandler *pH, const char *actionName);

	// <title GetOwnerId>
	// Syntax: Item.GetOwnerId()
	// Description:
	//		Gets the identifier of the item's owner.
	int GetOwnerId(IFunctionHandler *pH);
	// <title StartUse>
	// Syntax: Item.StartUse( ScriptHandle userId )
	// Arguments:
	//		userId - User identifier.
	// Description:
	//		The specified user now will start to use the item.
	int StartUse(IFunctionHandler *pH, ScriptHandle userId);
	// <title StopUse>
	// Syntax: Item.StopUse( ScriptHandle userId )
	// Arguments:
	//		userId - User identifier.
	// Description:
	//		The specified user now will stop to use the item.
	int StopUse(IFunctionHandler *pH, ScriptHandle userId);
	// <title Use>
	// Syntax: Item.Use( ScriptHandle userId )
	// Arguments:
	//		userId - User identifier.
	// Description:
	//		The specified user now will use the item.
	int Use(IFunctionHandler *pH, ScriptHandle userId);
	// <title IsUsed>
	// Syntax: Item.IsUsed()
	// Description:
	//		Checks if the item is used.
	int IsUsed(IFunctionHandler *pH);
	// <title OnUsed>
	// Syntax: Item.OnUsed( ScriptHandle userId )
	// Arguments:
	//		userId - User identifier.
	// Description:
	//		Events that occurs when the item is used.
	int OnUsed(IFunctionHandler *pH, ScriptHandle userId);

	// <title GetMountedDir>
	// Syntax: Item.GetMountedDir()
	// Description:
	//		Gets the mounted direction for the item.
	int GetMountedDir(IFunctionHandler *pH);
	// <title GetMountedAngleLimits>
	// Syntax: Item.GetMountedAngleLimits()
	// Description:
	//		Gets the mounted angle limits for the item.
	int GetMountedAngleLimits(IFunctionHandler *pH);
	// <title SetMountedAngleLimits>
	// Syntax: Item.SetMountedAngleLimits( float min_pitch, float max_pitch, float yaw_range )
	// Arguments:
	//		min_pitch - Minimum pitch angle value.
	//		max_pitch - Maximum pitch angle value.
	//		yaw_range - Yaw range value.
	// Description:
	//		Sets the mounted angle limits (yaw-pitch).
	int SetMountedAngleLimits(IFunctionHandler *pH, float min_pitch, float max_pitch, float yaw_range);

	// <title OnHit>
	// Syntax: Item.OnHit( SmartScriptTable hitTable )
	// Arguments:
	//		hitTable - Table with the hit values.
	// Description:
	//		Events that occurs when the item is hit.
	int OnHit(IFunctionHandler *pH, SmartScriptTable hitTable);
	// <title IsDestroyed>
	// Syntax: Item.IsDestroyed()
	// Description:
	//		Event that occurs when the item is destroyed.
	int IsDestroyed(IFunctionHandler *pH);
	// <title GetHealth>
	// Syntax: Item.GetHealth()
	// Description:
	//		Gets item health.
	int GetHealth(IFunctionHandler *pH);
	// <title GetMaxHealth>
	// Syntax: Item.GetMaxHealth()
	// Description:
	//		Gets the maximum item health.
	int GetMaxHealth(IFunctionHandler *pH);

private:
	void RegisterGlobals();
	void RegisterMethods();

	CItem *GetItem(IFunctionHandler *pH);
	CActor *GetActor(EntityId actorId);

	ISystem						*m_pSystem;
	IScriptSystem			*m_pSS;
	IGameFramework		*m_pGameFW;

	SmartScriptTable	m_stats;
	SmartScriptTable	m_params;
};


#endif //__SCRIPTBIND_ITEM_H__