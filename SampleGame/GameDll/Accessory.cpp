/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------

Description:  Item/weapon accessories

-------------------------------------------------------------------------
History:
- 28:1:2008   Created by Benito G.R. 

*************************************************************************/

#include "StdAfx.h"
#include "Accessory.h"
#include "Player.h"
#include "GameRules.h"
#include "ItemSharedParams.h"

void CAccessory::PickUp(EntityId pickerId, bool sound, bool select, bool keepHistory, const char *setup)
{
	CActor *pActor=GetActor(pickerId);
	if (!pActor)
		return;

	if(!CheckAmmoRestrictions(pickerId))
	{
		if (IsServer())
			g_pGame->GetGameRules()->SendTextMessage(eTextMessageCenter, "@ammo_maxed_out", eRMI_ToClientChannel, pActor->GetChannelId(), (string("@")+GetEntity()->GetClass()->GetName()).c_str());
		return;
	}

	TriggerRespawn();

	GetEntity()->EnablePhysics(false);
	Physicalize(false, false);

	bool soundEnabled = IsSoundEnabled();
	EnableSound(sound);

	SetViewMode(0);		
	SetOwnerId(pickerId);

	CopyRenderFlags(GetOwner());

	Hide(true);
	m_stats.dropped = false;
	m_stats.brandnew = false;


	IInventory *pInventory = pActor->GetInventory();
	if (!pInventory)
	{
		GameWarning("Actor '%s' has no inventory, when trying to pickup '%s'!",pActor->GetEntity()->GetName(),GetEntity()->GetName());
		return;
	}

	if(!pInventory->HasAccessory(GetEntity()->GetClass()))
	{
		pInventory->AddAccessory(GetEntity()->GetClass());
	}

	OnPickedUp(pickerId, m_sharedparams->params.unique);	

	PlayAction(g_pItemStrings->pickedup);

	EnableSound(soundEnabled);

	if (IsServer() && !IsDemoPlayback())
	{
		if(!gEnv->bMultiplayer)
			RemoveEntity();
		else if(g_pGame->GetGameRules())
			g_pGame->GetGameRules()->ScheduleEntityRemoval(GetEntityId(),10.0f,false); //Give some time to the clients to pick the msg
	}

	if (IsServer())
	{
		GetGameObject()->SetNetworkParent(pickerId);
		if ((GetEntity()->GetFlags()&(ENTITY_FLAG_CLIENT_ONLY|ENTITY_FLAG_SERVER_ONLY)) == 0)
		{
			pActor->GetGameObject()->InvokeRMIWithDependentObject(CActor::ClPickUp(), CActor::PickItemParams(GetEntityId(), m_stats.selected, sound), eRMI_ToAllClients|eRMI_NoLocalCalls, GetEntityId());

			const char *displayName=GetDisplayName();
}
	}
}
