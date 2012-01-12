/*************************************************************************
	Crytek Source File.
	Copyright (C), Crytek Studios, 2001-2004.
	-------------------------------------------------------------------------
	$Id$
	$DateTime$

	-------------------------------------------------------------------------
	History:
	- 15:2:2006   12:50 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "Weapon.h"
#include "Actor.h"
#include "Game.h"
#include "GameRules.h"

//------------------------------------------------------------------------
int CWeapon::NetGetCurrentAmmoCount() const
{
	if (!m_fm)
		return 0;

	return GetAmmoCount(m_fm->GetAmmoType());
}

//------------------------------------------------------------------------
void CWeapon::NetSetCurrentAmmoCount(int count)
{
	if (!m_fm)
		return;

	SetAmmoCount(m_fm->GetAmmoType(), count);
}

int CWeapon::GetReloadState() const
{
	return m_reloadState;
}

bool CWeapon::NetGetWeaponRaised() const
{
	return IsWeaponRaised();
}

void CWeapon::NetSetWeaponRaised(bool raise)
{
	IActor *pActor = GetOwnerActor();

	if(pActor && !pActor->IsClient())
		RaiseWeapon(raise);
}

void CWeapon::NetUpdateFireMode(SEntityUpdateContext& ctx)
{
	// CGunTurret and CVehicleWeapon overide NetAllowUpdate to perform their own checks.
	if(NetAllowUpdate(true))
	{
		m_netNextShot -= ctx.fFrameTime;

		if(IsReloading())
			return;	// reloading, bail

		if((!m_isFiringStarted) && (m_isFiring || m_shootCounter > 0)) 
		{
			m_isFiringStarted = true;
			m_netNextShot = 0.f;
			NetStartFire();
			EnableUpdate(true, eIUS_FireMode);
		}

		if(m_fm)
		{
			if(m_shootCounter > 0 && m_netNextShot <= 0.0f)
			{
				// Aside from the prediction handle, needed for the server, NetShoot/Ex parameters
				// are no longer used, these will need removing when the client->server RMI's are tided up
				m_fm->NetShoot(Vec3(0.f, 0.f, 0.f), 0);
				m_shootCounter--;

				//if fireRate == 0.0f, set m_netNextShot to 0.0f, otherwise increment by 60.f / fireRate.
				//	fres used to avoid microcoded instructions, fsel to avoid branching - Rich S
				const float fRawFireRate		= m_fm->GetFireRate();
				const float fFireRateSelect = -fabsf(fRawFireRate);
				const float fFireRateForDiv = (float)__fsel(fFireRateSelect, 1.0f, fRawFireRate);
				const float fNextShot				= (float)__fsel(fFireRateSelect, 0.0f, m_netNextShot + (60.f * __fres(fFireRateForDiv)));
				m_netNextShot = fNextShot;
			}
		}

		if(m_isFiringStarted && !m_isFiring && m_shootCounter <= 0)
		{
			m_isFiringStarted = false;
			NetStopFire();
			EnableUpdate(false, eIUS_FireMode);
		}

		// this needs to happen here, or NetStopFire interrupts the animation
		if(m_doMelee && m_melee)
		{
			m_melee->NetStartFire();
			m_doMelee= false;
		}
	}

}

bool CWeapon::NetAllowUpdate(bool requireActor)
{
	if (!gEnv->bServer)
	{
		IActor *pActor = GetOwnerActor();

		if(!pActor && requireActor)
		{
			return false;
		}

		if (!pActor || !pActor->IsClient())
		{
			return true;
		}
	}

	return false;
}

void CWeapon::ClSetReloadState(int state)
{
	assert(!gEnv->bServer);

	if(m_fm)
	{
		IActor *pActor = GetOwnerActor();
		bool ownerIsLocal = pActor && pActor->IsClient();

		switch(state)
		{
		case eNRS_NoReload:
			{
				if(IsReloading())
					m_fm->NetEndReload();
				m_reloadState = state;
				break;
			}

		case eNRS_StartReload:
			{
				m_fm->Reload(m_zm ? m_zm->GetCurrentStep() : 0);
				m_reloadState = eNRS_StartReload;
				break;
			}

		case eNRS_EndReload:
			{
				if(ownerIsLocal)
				{
					m_fm->NetEndReload();
				}
				m_reloadState = eNRS_NoReload;
				break;
			}

		case eNRS_CancelReload:
			{
				if(!ownerIsLocal)
				{
					m_fm->CancelReload();
				}

				m_reloadState = eNRS_NoReload;
				break;
			}

		default:
			{
				break;
			}
		}
	}
}

void CWeapon::SvSetReloadState(int state)
{
	m_reloadState = state;
	CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
}

void CWeapon::SvCancelReload()
{
	if(m_fm)
	{
		m_fm->CancelReload();
		SvSetReloadState(eNRS_CancelReload);
	}
}

void CWeapon::NetStateSent()
{
	if(m_reloadState == eNRS_EndReload || m_reloadState == eNRS_CancelReload)
	{
		m_reloadState = eNRS_NoReload;
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestReload()
{
	CActor *pActor=GetOwnerActor();

	if (!gEnv->bServer && pActor && pActor->IsClient())
	{
		GetGameObject()->InvokeRMI(SvRequestReload(), DefaultParams(), eRMI_ToServer);
	}
	else if (IsServer())
	{
		SvSetReloadState(eNRS_StartReload);
	}
}

//-----------------------------------------------------------------------
void CWeapon::RequestCancelReload()
{
	CActor *pActor=GetOwnerActor();

	if (!gEnv->bServer && pActor && pActor->IsClient())
	{
		if(m_fm)
		{
			m_fm->CancelReload();
		}
		GetGameObject()->InvokeRMI(SvRequestCancelReload(), DefaultParams(), eRMI_ToServer);
	}
	else if (IsServer())
	{
		SvCancelReload();
	}
}

//------------------------------------------------------------------------
void CWeapon::SendEndReload()
{
	SvSetReloadState(eNRS_EndReload);
}

//------------------------------------------------------------------------
void CWeapon::NetShoot(const Vec3 &hit, int predictionHandle)
{
	if (m_fm)
		m_fm->NetShoot(hit, predictionHandle);
}

//------------------------------------------------------------------------
void CWeapon::NetShootEx(const Vec3 &pos, const Vec3 &dir, const Vec3 &vel, const Vec3 &hit, float extra, int predictionHandle)
{
	if (m_fm)
		m_fm->NetShootEx(pos, dir, vel, hit, extra, predictionHandle);
}

//------------------------------------------------------------------------
void CWeapon::NetStartFire()
{
	if (m_fm)
		m_fm->NetStartFire();
}

//------------------------------------------------------------------------
void CWeapon::NetStopFire()
{
	if (m_fm)
		m_fm->NetStopFire();
}

//------------------------------------------------------------------------
void CWeapon::NetStartMeleeAttack(bool weaponMelee)
{
	if (weaponMelee && m_melee)
		m_melee->NetStartFire();
	else if (m_fm)
		m_fm->NetStartFire();
}

//------------------------------------------------------------------------
void CWeapon::NetMeleeAttack(bool weaponMelee, const Vec3 &pos, const Vec3 &dir)
{
	if (weaponMelee && m_melee)
	{
		m_melee->NetShootEx(pos, dir, ZERO, ZERO, 1.0f, 0);
		if (IsServer())
			m_pGameplayRecorder->Event(GetOwner(), GameplayEvent(eGE_WeaponMelee, 0, 0, (void *)GetEntityId()));
	}
	else if (m_fm)
	{
		m_fm->NetShootEx(pos, dir, ZERO, ZERO, 1.0f, 0);
		if (IsServer())
			m_pGameplayRecorder->Event(GetOwner(), GameplayEvent(eGE_WeaponMelee, 0, 0, (void *)GetEntityId()));
	}
}

//------------------------------------------------------------------------
void CWeapon::NetZoom(float fov)
{
	if (CActor *pOwner=GetOwnerActor())
	{
		if (pOwner->IsClient())
			return;

		SActorParams *pActorParams = pOwner->GetActorParams();
		if (!pActorParams)
			return;

		pActorParams->viewFoVScale = fov;
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestShoot(IEntityClass* pAmmoType, const Vec3 &pos, const Vec3 &dir, const Vec3 &vel, const Vec3 &hit, float extra, int predictionHandle, bool forceExtended)
{
	CActor *pActor=GetOwnerActor();

	if (!gEnv->bServer && pActor && pActor->IsClient())
	{
		if (IsServerSpawn(pAmmoType) || forceExtended)
			GetGameObject()->InvokeRMI(CWeapon::SvRequestShootEx(), SvRequestShootExParams(pos, dir, vel, hit, extra, predictionHandle), eRMI_ToServer);
		else
			GetGameObject()->InvokeRMI(CWeapon::SvRequestShoot(), SvRequestShootParams(hit, predictionHandle), eRMI_ToServer);

		m_expended_ammo++;
	}
	else if (IsServer())
	{
		m_fireCounter++;
		CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestStartFire()
{
	CActor *pActor=GetOwnerActor();

	if (!gEnv->bServer && pActor && pActor->IsClient())
	{
		GetGameObject()->InvokeRMI(CWeapon::SvRequestStartFire(), DefaultParams(), eRMI_ToServer);
	}
	else if (IsServer())
	{
		NetSetIsFiring(true);
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestZoom(float fov)
{
	GetGameObject()->Pulse('bang');
	CActor *pActor=GetOwnerActor();
	if (pActor)
		pActor->GetGameObject()->Pulse('bang');

	if (gEnv->IsClient() && pActor && pActor->IsClient())
		GetGameObject()->InvokeRMI(CWeapon::SvRequestZoom(), ZoomParams(fov), eRMI_ToServer);
	else
	{
		int to=gEnv->IsClient()?eRMI_ToRemoteClients:eRMI_ToAllClients;
		GetGameObject()->InvokeRMI(CWeapon::ClZoom(), ZoomParams(fov), to);
		if (!gEnv->IsClient())
			NetZoom(fov);
	}
}

void CWeapon::NetSetIsFiring(bool isFiring)
{
	m_isFiring = isFiring;
	if(gEnv->bServer)
		CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
}

//------------------------------------------------------------------------
void CWeapon::RequestStartMeleeAttack(bool weaponMelee, bool boostedAttack)
{
	CActor *pActor=GetOwnerActor();

	if (!gEnv->bServer && pActor && pActor->IsClient())
	{
		GetGameObject()->InvokeRMI(CWeapon::SvRequestStartMeleeAttack(), RequestStartMeleeAttackParams(weaponMelee), eRMI_ToServer);
	}
	else if (IsServer())
	{
		m_meleeCounter++;
		CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestStopFire()
{
	CActor *pActor=GetOwnerActor();

	if (!gEnv->bServer && pActor && pActor->IsClient())
	{
		GetGameObject()->InvokeRMI(CWeapon::SvRequestStopFire(), DefaultParams(), eRMI_ToServer);
	}
	else if (IsServer())
	{
		NetSetIsFiring(false);
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestFireMode(int fmId)
{
	CActor *pActor=GetOwnerActor();

	if (gEnv->IsClient() && pActor && pActor->IsClient())
	{
		float animationTime = GetCurrentAnimationTime(eIGS_Owner)/1000.0f;
		m_switchFireModeTimeStap = gEnv->pTimer->GetCurrTime() + animationTime;

		if(gEnv->bServer)
		{
			SetCurrentFireMode(fmId);
		}
		else
		{
			if(m_fm)
			{
				m_fm->Activate(false);
			}
			GetGameObject()->InvokeRMI(SvRequestFireMode(), SvRequestFireModeParams(fmId), eRMI_ToServer);
		}
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestLock(EntityId id, int partId)
{
	IActor *pActor=m_pGameFramework->GetClientActor();
	if (!pActor || pActor->IsClient())
	{
		if (gEnv->bServer)
		{
			if (m_fm)
				m_fm->Lock(id, partId);

			GetGameObject()->InvokeRMI(CWeapon::ClLock(), LockParams(id, partId), eRMI_ToRemoteClients);
		}
		else
			GetGameObject()->InvokeRMI(SvRequestLock(), LockParams(id, partId), eRMI_ToServer);
	}
}

//------------------------------------------------------------------------
void CWeapon::RequestUnlock()
{
	IActor *pActor=m_pGameFramework->GetClientActor();
	if (!pActor || pActor->IsClient())
		GetGameObject()->InvokeRMI(SvRequestUnlock(), EmptyParams(), eRMI_ToServer);
}

//------------------------------------------------------------------------
void CWeapon::RequestWeaponRaised(bool raise)
{
	if(gEnv->bMultiplayer)
	{
		CActor* pActor = GetOwnerActor();
		if(pActor && pActor->IsClient())
		{
			if (gEnv->bServer)
				CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
			else
				GetGameObject()->InvokeRMI(SvRequestWeaponRaised(), WeaponRaiseParams(raise), eRMI_ToServer);
		}
	}
}

void CWeapon::RequestSetZoomState(bool zoomed)
{
	if(gEnv->bMultiplayer)
	{
		CActor* pActor = GetOwnerActor();
		if(pActor && pActor->IsClient())
		{
			if (gEnv->bServer)
				CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
			else
				GetGameObject()->InvokeRMI(SvRequestSetZoomState(), ZoomStateParams(zoomed), eRMI_ToServer);
		}
	}
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestStartFire)
{
	CHECK_OWNER_REQUEST();

	CActor *pActor=GetActorByNetChannel(pNetChannel);
	IActor *pLocalActor=m_pGameFramework->GetClientActor();
	bool isLocal = pLocalActor && pActor && (pLocalActor->GetChannelId() == pActor->GetChannelId());

	if (!isLocal)
	{
		NetSetIsFiring(true);
		NetStartFire();
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestStopFire)
{
	CHECK_OWNER_REQUEST();

	CActor *pActor=GetActorByNetChannel(pNetChannel);
	IActor *pLocalActor=m_pGameFramework->GetClientActor();
	bool isLocal = pLocalActor && pActor && (pLocalActor->GetChannelId() == pActor->GetChannelId());

	if (!isLocal)
	{
		NetSetIsFiring(false);
		NetStopFire();
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestShoot)
{
	CHECK_OWNER_REQUEST();

	bool ok=true;
	CActor *pActor=GetActorByNetChannel(pNetChannel);
	if (!pActor || pActor->GetHealth()<=0)
		ok=false;

	if (ok)
	{
		m_fireCounter++;
		m_expended_ammo++;

		IActor *pLocalActor=m_pGameFramework->GetClientActor();
		const bool isLocal = pLocalActor && (pLocalActor->GetChannelId() == pActor->GetChannelId());

		if (!isLocal)
		{
			NetShoot(params.hit, params.predictionHandle);
		}

		CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestShootEx)
{
	CHECK_OWNER_REQUEST();

	bool ok=true;
	CActor *pActor=GetActorByNetChannel(pNetChannel);
	if (!pActor || pActor->GetHealth()<=0)
		ok=false;

	if (ok)
	{
		m_fireCounter++;
		m_expended_ammo++;

		IActor *pLocalActor=m_pGameFramework->GetClientActor();
		bool isLocal = pLocalActor && (pLocalActor->GetChannelId() == pActor->GetChannelId());

		if (!isLocal)
		{
			NetShootEx(params.pos, params.dir, params.vel, params.hit, params.extra, params.predictionHandle);
		}

		CHANGED_NETWORK_STATE(this, ASPECT_STREAM);
	}

	return true;
}


//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestStartMeleeAttack)
{
	CHECK_OWNER_REQUEST();

	CActor *pActor=GetActorByNetChannel(pNetChannel);
	IActor *pLocalActor=m_pGameFramework->GetClientActor();
	bool isLocal = pLocalActor && pActor && (pLocalActor->GetChannelId() == pActor->GetChannelId());

	if (!isLocal)
	{
		NetStartMeleeAttack(params.wmelee);
	}

	m_meleeCounter++;
	CHANGED_NETWORK_STATE(this, ASPECT_STREAM);

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestFireMode)
{
	CHECK_OWNER_REQUEST();

	SetCurrentFireMode(params.id);

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestReload)
{
	CHECK_OWNER_REQUEST();

	bool ok=true;
	CActor *pActor=GetActorByNetChannel(pNetChannel);
	if (!pActor || pActor->IsDead())
		ok=false;

	if (ok)
	{
		SvSetReloadState(eNRS_StartReload);

		if(m_fm)
			m_fm->Reload(0);
	}

	return true;
}


//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestCancelReload)
{
	CHECK_OWNER_REQUEST();

	SvCancelReload();

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, ClLock)
{
	if (m_fm)
		m_fm->Lock(params.entityId, params.partId);

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, ClUnlock)
{
	if (m_fm)
		m_fm->Unlock();

	return true;
}


//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestLock)
{
	CHECK_OWNER_REQUEST();

	if (m_fm)
		m_fm->Lock(params.entityId, params.partId);

	GetGameObject()->InvokeRMI(CWeapon::ClLock(), params, eRMI_ToRemoteClients);

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestUnlock)
{
	CHECK_OWNER_REQUEST();

	if (m_fm)
		m_fm->Unlock();

	GetGameObject()->InvokeRMI(CWeapon::ClUnlock(), params, eRMI_ToRemoteClients);

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, SvRequestWeaponRaised)
{
	CHECK_OWNER_REQUEST();

	CHANGED_NETWORK_STATE(this, ASPECT_STREAM);

	return true;
}

IMPLEMENT_RMI(CWeapon, SvRequestSetZoomState)
{
	CHECK_OWNER_REQUEST();

	if (params.zoomed)
		StartZoom(GetOwnerId(), 1);
	else
		StopZoom(GetOwnerId());

	CHANGED_NETWORK_STATE(this, ASPECT_STREAM);

	return true;
}

IMPLEMENT_RMI(CWeapon, SvRequestZoom)
{
	CHECK_OWNER_REQUEST();

	bool ok=true;
	CActor *pActor=GetActorByNetChannel(pNetChannel);
	if (!pActor || pActor->GetHealth()<=0)
		ok=false;

	if (ok)
	{
		GetGameObject()->InvokeRMI(CWeapon::ClZoom(), params,
			eRMI_ToOtherClients|eRMI_NoLocalCalls, m_pGameFramework->GetGameChannelId(pNetChannel));

		IActor *pLocalActor=m_pGameFramework->GetClientActor();
		bool isLocal = pLocalActor && (pLocalActor->GetChannelId() == pActor->GetChannelId());

		if (!isLocal)
			NetZoom(params.fov);

		int event=eGE_ZoomedOut;
		if (params.fov<0.99f)
			event=eGE_ZoomedIn;
		m_pGameplayRecorder->Event(GetOwner(), GameplayEvent(event, 0, 0, (void *)GetEntityId()));
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CWeapon, ClZoom)
{
	NetZoom(params.fov);

	return true;
}