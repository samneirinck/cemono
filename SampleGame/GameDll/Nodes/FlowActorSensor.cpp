#include "StdAfx.h"
#include "Game.h"

#include "Nodes/G2FlowBaseNode.h"
#include "Player.h"
#include "Weapon.h"
#include "GameCVars.h"

#include <IVehicleSystem.h>
#include <StringUtils.h>

CPlayer* RetrPlayer(EntityId id)
{
	if (id == 0)
		return NULL;
	CActor* pActor = g_pGame ? static_cast<CActor*> (g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(id)) : NULL;
	if (pActor != NULL && pActor->GetActorClass() == CPlayer::GetActorClassType())
		return static_cast<CPlayer*> (pActor);
	return NULL;
}

IVehicle* GetVehicle(EntityId id)
{
	if (id == 0)
		return NULL;
	return g_pGame ? g_pGame->GetIGameFramework()->GetIVehicleSystem()->GetVehicle(id) : NULL;
}

class CFlowNode_ActorSensor : public CFlowBaseNode, public IPlayerEventListener, public IVehicleEventListener
{
public:
	CFlowNode_ActorSensor( SActivationInfo * pActInfo ) : m_entityId(0), m_vehicleId(0), m_bEnabled(false)
	{
	}

	~CFlowNode_ActorSensor()
	{
		UnRegisterActor();
	}

	IFlowNodePtr Clone(SActivationInfo* pActInfo)
	{
		return new CFlowNode_ActorSensor(pActInfo);
	}

	void Serialize(SActivationInfo* pActivationInfo, TSerialize ser)
	{
		if (ser.IsReading())
		{
			UnRegisterActor();
		}
		ser.Value("m_entityId", m_entityId, 'eid');
		ser.Value("m_vehicleId", m_vehicleId, 'eid');
		if (ser.IsReading())
		{
			if (m_entityId != 0)
			{
				RegisterActor();
			}
			if (m_vehicleId != 0)
			{
				IVehicle* pVehicle = g_pGame->GetIGameFramework()->GetIVehicleSystem()->GetVehicle(m_vehicleId);
				if (pVehicle)
				{
					pVehicle->RegisterVehicleEventListener(this, "CFlowNode_ActorSensor");
				}
				else
					m_vehicleId = 0;
			}
		}
	}
	enum INS
	{
		EIP_TRIGGER = 0,
		EIP_ENABLE,
		EIP_DISABLE,
	};

	enum OUTS
	{
		EOP_ENTER = 0,
		EOP_EXIT,
		EOP_SEAT,
		EOP_ITEMPICKEDUP,
		EOP_ITEMDROPPED,
		EOP_ITEMUSED,
		EOP_NPCGRABBED,
		EOP_NPCTHROWN,
		EOP_OBJECTGRABBED,
		EOP_OBJECTTHROWN,
		EOP_STANCECHANGED,
		EOP_SPECIALMOVE,
		EOP_ONDEATH,
		EOP_ONREVIVE,
		EOP_ONENTERSPECATOR,
		EOP_ONLEAVESPECATOR,
		EOP_ONHEALTHCHANGE,
		EOP_ONTOGGLETHIRDPERSON,
	};

	void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig in_ports[] = 
		{
			InputPortConfig_Void( "Get", _HELP("Trigger \"On*\" outputs according to current state.")),
			InputPortConfig_Void( "Enable", _HELP("Trigger to enable")),
			InputPortConfig_Void( "Disable", _HELP("Trigger to enable")),
			{0}
		};
		static const SOutputPortConfig out_ports[] = 
		{
			OutputPortConfig<EntityId> ( "EnterVehicle", _HELP("Triggered when entering a vehicle")),
			OutputPortConfig<EntityId> ( "ExitVehicle", _HELP("Triggered when exiting a vehicle")),
			OutputPortConfig<int>      ( "SeatChange", _HELP("Triggered when seat has changed")),
			OutputPortConfig<EntityId> ( "ItemPickedUp", _HELP("Triggered when an item is picked up")),
			OutputPortConfig<EntityId> ( "ItemDropped", _HELP("Triggered when an item is dropped")),
			OutputPortConfig<EntityId> ( "ItemUsed", _HELP("Triggered when an item is used")),
			OutputPortConfig<EntityId> ( "NPCGrabbed", _HELP("Triggered when an NPC is grabbed")),
			OutputPortConfig<EntityId> ( "NPCThrown", _HELP("Triggered when an NPC is thrown")),
			OutputPortConfig<EntityId> ( "ObjectGrabbed", _HELP("Triggered when an object is grabbed")),
			OutputPortConfig<EntityId> ( "ObjectThrown", _HELP("Triggered when an object is thrown")),
			OutputPortConfig<int>      ( "StanceChanged", _HELP("Triggered when Stance changed. 0=Stand,1=Crouch,2=Prone,3=Relaxed,4=Stealth,5=Swim,6=ZeroG")),
			OutputPortConfig<int>      ( "SpecialMove", _HELP("Triggered On SpecialMove. 0=Jump,1=SpeedSprint,2=StrengthJump")),
			OutputPortConfig<int>      ( "OnDeath", _HELP("Triggered when Actor dies. Outputs 0 if not god. 1 if god.")),
			OutputPortConfig<int>      ( "OnRevive", _HELP("Triggered when Actor revives. Outputs 0 if not god. 1 if god.")),
			OutputPortConfig<int>      ( "OnEnterSpecator", _HELP("Triggered when Actor enter specator mode. Outputs spectator mode uint")),
			OutputPortConfig<bool>     ( "OnLeaveSpecator", _HELP("Triggered when Actor leaves specator mode.")),
			OutputPortConfig<int>    ( "OnHealthChange", _HELP("Triggered when Actors health changed. Outputs current health.")),
			OutputPortConfig<bool>     ( "OnToggleThirdPerson", _HELP("Triggered when Actors view mode changed. Outputs true for third person otherwise false.")),
			{0}
		};
		config.nFlags |= EFLN_TARGET_ENTITY;
		config.pInputPorts = in_ports;
		config.pOutputPorts = out_ports;
		config.sDescription = _HELP("Tracks the attached Entity and its Vehicle-related actions");
		config.SetCategory(EFLN_APPROVED);
	}

	void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			m_actInfo = *pActInfo;
			break;
		case eFE_SetEntityId:
			if (m_bEnabled)
			{
				UnRegisterActor();
				m_entityId = pActInfo->pEntity != 0 ? pActInfo->pEntity->GetId() : 0;
				RegisterActor();
			}
			break;
		case eFE_Activate:
			if (IsPortActive(pActInfo, EIP_TRIGGER))
				TriggerPorts(pActInfo);

			if (IsPortActive(pActInfo, EIP_DISABLE))
				UnRegisterActor();

			if (IsPortActive(pActInfo, EIP_ENABLE))
			{
				UnRegisterActor();
				m_entityId = pActInfo->pEntity != 0 ? pActInfo->pEntity->GetId() : 0;
				RegisterActor();
			}
			break;
		}
	}

	
	void RegisterActor()
	{
		m_bEnabled = true;

		if (m_entityId == 0)
			return;
		CPlayer* pPlayer = RetrPlayer(m_entityId);
		if (pPlayer != 0)
		{
			pPlayer->RegisterPlayerEventListener(this);
			return;
		}
		m_entityId = 0;
	}

	void UnRegisterActor()
	{
		m_bEnabled = false;

		if (m_entityId == 0)
			return;

		CPlayer* pPlayer = RetrPlayer(m_entityId);
		if (pPlayer != 0)
			pPlayer->UnregisterPlayerEventListener(this);
		m_entityId = 0;

		IVehicle* pVehicle = GetVehicle(m_vehicleId);
		if (pVehicle != 0)
			pVehicle->UnregisterVehicleEventListener(this);
		m_vehicleId = 0;
	}

	void TriggerPorts(SActivationInfo *pActInfo)
	{
		EntityId eid = pActInfo->pEntity != 0 ? pActInfo->pEntity->GetId() : 0;
		CPlayer* pPlayer = RetrPlayer(eid);
		if (pPlayer)
		{
			ActivateOutput(pActInfo, EOP_STANCECHANGED, static_cast<int> (pPlayer->GetStance()));

			if (pPlayer->IsDead())
				ActivateOutput(pActInfo, EOP_ONDEATH, pPlayer->IsGod());
			else
				ActivateOutput(pActInfo, EOP_ONREVIVE, pPlayer->IsGod());

			ActivateOutput(pActInfo, EOP_ONTOGGLETHIRDPERSON, pPlayer->IsThirdPerson());

			if (pPlayer->GetSpectatorMode() == CActor::eASM_None)
				ActivateOutput(pActInfo, EOP_ONLEAVESPECATOR, true);
			else
				ActivateOutput(pActInfo, EOP_ONENTERSPECATOR, static_cast<int> (pPlayer->GetSpectatorMode()));

			ActivateOutput(pActInfo, EOP_ONHEALTHCHANGE, (int) pPlayer->GetHealth());
		}
	}

	// IPlayerEventListener
	virtual void OnEnterVehicle(IActor *pActor,const char *strVehicleClassName,const char *strSeatName,bool bThirdPerson)
	{
		if (pActor->GetEntityId() != m_entityId)
			return;
		CPlayer* pPlayer = static_cast<CPlayer*> (pActor);
		if(m_vehicleId)
		{
			if(IVehicle *pVehicle = GetVehicle(m_vehicleId))
				pVehicle->UnregisterVehicleEventListener(this);
		}
		IVehicle* pVehicle = pPlayer->GetLinkedVehicle();
		if (pVehicle == 0)
			return;
		pVehicle->RegisterVehicleEventListener(this, "CFlowNode_ActorSensor");
		m_vehicleId = pVehicle->GetEntityId();
		ActivateOutput(&m_actInfo, EOP_ENTER, m_vehicleId);
		IVehicleSeat* pSeat = pVehicle->GetSeatForPassenger(m_entityId);
		if (pSeat)
			ActivateOutput(&m_actInfo, EOP_SEAT, pSeat->GetSeatId());
	}

	virtual void OnExitVehicle(IActor *pActor)
	{
		if (pActor->GetEntityId() != m_entityId)
			return;
		IVehicle* pVehicle = GetVehicle(m_vehicleId);
		if (pVehicle == 0)
		{
			m_vehicleId = 0;
			return;
		}
		ActivateOutput(&m_actInfo, EOP_EXIT, m_vehicleId);
		pVehicle->UnregisterVehicleEventListener(this);
		m_vehicleId = 0;
	}

	virtual void OnToggleThirdPerson(IActor *pActor,bool bThirdPerson)
	{
		ActivateOutput(&m_actInfo, EOP_ONTOGGLETHIRDPERSON, bThirdPerson);
	}

	virtual void OnItemDropped(IActor* pActor, EntityId itemId)
	{
		ActivateOutput(&m_actInfo, EOP_ITEMDROPPED, itemId);
	}
	virtual void OnItemUsed(IActor* pActor, EntityId itemId)
	{
		ActivateOutput(&m_actInfo, EOP_ITEMUSED, itemId);
	}
	virtual void OnItemPickedUp(IActor* pActor, EntityId itemId)
	{
		ActivateOutput(&m_actInfo, EOP_ITEMPICKEDUP, itemId);
	}
	virtual void OnStanceChanged(IActor* pActor, EStance stance)
	{
		ActivateOutput(&m_actInfo, EOP_STANCECHANGED, static_cast<int> (stance));
	}
	virtual void OnSpecialMove(IActor* pActor, ESpecialMove move)
	{
		ActivateOutput(&m_actInfo, EOP_SPECIALMOVE, static_cast<int> (move));
	}
	virtual void OnDeath(IActor* pActor, bool bIsGod)
	{
		ActivateOutput(&m_actInfo, EOP_ONDEATH, bIsGod ? 1 : 0);
	}
	virtual void OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded)
	{
		if (bIsGrab)
			ActivateOutput(&m_actInfo, bIsNPC ? EOP_NPCGRABBED : EOP_OBJECTGRABBED, objectId);
		else
			ActivateOutput(&m_actInfo, bIsNPC ? EOP_NPCTHROWN : EOP_OBJECTTHROWN, objectId);
	}
	virtual void OnRevive(IActor* pActor, bool bIsGod)
	{
		ActivateOutput(&m_actInfo, EOP_ONREVIVE, bIsGod);
	}
	virtual void OnSpectatorModeChanged(IActor* pActor, uint8 mode)
	{
		if (mode == CActor::eASM_None)
			ActivateOutput(&m_actInfo, EOP_ONLEAVESPECATOR, true);
		else
			ActivateOutput(&m_actInfo, EOP_ONENTERSPECATOR, static_cast<int> (mode));
	}
	virtual void OnHealthChange(IActor* pActor, float fHealth)
	{
		ActivateOutput(&m_actInfo, EOP_ONHEALTHCHANGE, (int) fHealth);
	}


	// ~IPlayerEventListener

	// IVehicleEventListener
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
	{
		if (event == eVE_VehicleDeleted)
		{
			IVehicle* pVehicle = GetVehicle(m_vehicleId);
			if (pVehicle == 0)
			{
				m_vehicleId = 0;
				return;
			}
			pVehicle->UnregisterVehicleEventListener(this);
			m_vehicleId = 0;
		}
		else if (event == eVE_PassengerChangeSeat)
		{
			if (params.entityId == m_entityId)
			{
				ActivateOutput(&m_actInfo, EOP_SEAT, params.iParam); // seat id
			}
		}
	}
	// ~IVehicleEventListener

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

protected:
	EntityId m_entityId;
	EntityId m_vehicleId;
	SActivationInfo m_actInfo;
	bool m_bEnabled;
};


class CFlowNode_WeaponSensor 
	: public CFlowBaseNode
	, public IWeaponEventListener
	, public IItemSystemListener
	, public ISystemEventListener

{
public:
	CFlowNode_WeaponSensor( SActivationInfo * pActInfo ) 
		: m_entityId(0)
		, m_bEnabled(false)
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener( this );
	}

	~CFlowNode_WeaponSensor()
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener( this );
		Disable();
	}


	enum INS
	{
		EIP_TRIGGER = 0,
		EIP_ENABLE,
		EIP_DISABLE,
	};

	enum OUTS
	{
		EOP_ONWEAPONCHANGE = 0,
		EOP_ONFIREMODECHANGE,
		EOP_ONSHOOT,
		EOP_ONZOOM,
		EOP_ONRELOAD,
		EOP_ONOUTOFAMMO,

		EOP_WEAPONID,
		EOP_WEAPONNAME,
		EOP_ISMELEE,

		EOP_AMMO,
		EOP_AMMOTYPE,
		EOP_AMMONAME,
		EOP_MAXAMMO,

		EOP_ZOOMED,
		EOP_ZOOMMODE,
		EOP_ZOOMNAME,
		EOP_ZOOMSTEP,
		EOP_ZOOMMAXSTEP,
	};

	void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig inputs[] = {
			InputPortConfig_Void( "Get", _HELP("Force node to trigger current state") ),
			InputPortConfig_Void( "Enable", _HELP("Trigger to enable")),
			InputPortConfig_Void( "Disable", _HELP("Trigger to enable")),
			{0}
		};
		static const SOutputPortConfig outputs[] = {
			OutputPortConfig_Void( "OnWeaponChange",	_HELP( "Triggers if weapon changed" ) ),
			OutputPortConfig_Void( "OnFiremodeChange",	_HELP( "Triggers if weapon changed" ) ),
			OutputPortConfig_Void( "OnShoot",	_HELP( "Triggers if weapon shoot" ) ),
			OutputPortConfig_Void( "OnZoom",	_HELP( "Triggers if weapon zoom changed" ) ),
			OutputPortConfig_Void( "OnReloaded",	_HELP( "Triggers if weapon was reloaded" ) ),
			OutputPortConfig_Void( "OnOutOfAmmo",	_HELP( "Triggers if out of ammo" ) ),

			OutputPortConfig<EntityId>( "WeaponId",	_HELP( "Weapon id" ) ),
			OutputPortConfig<string>  ( "WeaponName",	_HELP( "Weapon name" ) ),
			OutputPortConfig<bool>    ( "IsMelee",	_HELP( "Is melee weapon" ) ),

			OutputPortConfig<int>     ( "Ammo",	_HELP( "Current ammo count" ) ),
			OutputPortConfig<int>     ( "AmmoType",	_HELP( "Ammo type" ) ),
			OutputPortConfig<string>  ( "AmmoName",	_HELP( "Ammo name" ) ),
			OutputPortConfig<int>     ( "MaxAmmo",	_HELP( "Max ammo of current weapon" ) ),

			OutputPortConfig<bool>    ( "Zoomed",	_HELP( "Is weapon zoomed" ) ),
			OutputPortConfig<int>     ( "ZoomMode",	_HELP( "Weapons zoom mode" ) ),
			OutputPortConfig<string>  ( "ZoomName",	_HELP( "Weapons zoom name" ) ),
			OutputPortConfig<int>     ( "CurrZoomStep",	_HELP( "ZoomMode zoom name" ) ),
			OutputPortConfig<int>     ( "MaxZoomStep",	_HELP( "Weapons zoom name" ) ),
			{0}
		};

		config.nFlags |= EFLN_TARGET_ENTITY;
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Tracks the attached Entity and its Vehicle-related actions");
		config.SetCategory(EFLN_APPROVED);
	}

	void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			m_actInfo = *pActInfo;
			UnRegisterWeapon();
			UnRegisterItemSystem();
			break;
		case eFE_SetEntityId:
			if (m_bEnabled)
			{
				Enable(pActInfo);
			}
			break;
		case eFE_Activate:
			if (IsPortActive(pActInfo, EIP_TRIGGER))
			{
				CPlayer* pPlayer = RetrPlayer(pActInfo->pEntity != 0 ? pActInfo->pEntity->GetId() : 0);
				UpdateWeaponChange(pPlayer ? pPlayer->GetWeapon(pPlayer->GetCurrentItemId()) : NULL);
				ActivateOutput(&m_actInfo, EOP_ONWEAPONCHANGE, true);
				ActivateOutput(&m_actInfo,EOP_ONFIREMODECHANGE, true);
			}

			if (IsPortActive(pActInfo, EIP_DISABLE))
			{
				Disable();
			}

			if (IsPortActive(pActInfo, EIP_ENABLE))
			{
				Enable(pActInfo);
			}
			break;
		}
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	// ISystemEventListener
	virtual void OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam )
	{
		if (event == ESYSTEM_EVENT_LEVEL_UNLOAD)
			Disable();
	}
	// ~ISystemEventListener

	// IItemSystemListener
	virtual void OnSetActorItem(IActor *pActor, IItem *pItem )
	{
		CPlayer* pPlayer = RetrPlayer(m_entityId);
		if (pPlayer == pActor)
		{
			UnRegisterWeapon();
			if (pItem)
				RegisterWeapon(pItem->GetEntityId());

			CWeapon* pWeapon = GetCurrentWeapon();
			UpdateWeaponChange(pWeapon);
			ActivateOutput(&m_actInfo, EOP_ONWEAPONCHANGE, true);
		}
	}

	virtual void OnDropActorItem(IActor *pActor, IItem *pItem ) {}
	virtual void OnSetActorAccessory(IActor *pActor, IItem *pItem ) {}
	virtual void OnDropActorAccessory(IActor *pActor, IItem *pItem ) {}
	// ~IItemSystemListener

	// IWeaponEventListener
	virtual void OnStartFire(IWeapon *pWeapon, EntityId shooterId) {}
	virtual void OnStopFire(IWeapon *pWeapon, EntityId shooterId)  {}
	virtual void OnStartReload(IWeapon *pWeapon, EntityId shooterId, IEntityClass* pAmmoType) {}
	virtual void OnSetAmmoCount(IWeapon *pWeapon, EntityId shooterId) {}
	virtual void OnReadyToFire(IWeapon *pWeapon) {}
	virtual void OnPickedUp(IWeapon *pWeapon, EntityId actorId, bool destroyed) {}
	virtual void OnDropped(IWeapon *pWeapon, EntityId actorId) {}
	virtual void OnMelee(IWeapon* pWeapon, EntityId shooterId)  {}
	virtual void OnSelected(IWeapon *pWeapon, bool selected) {}
	virtual void OnStartTargetting(IWeapon *pWeapon) {}
	virtual void OnStopTargetting(IWeapon *pWeapon) {} 

	virtual void OnEndReload(IWeapon *pWeapon, EntityId shooterId, IEntityClass* pAmmoType)
	{
		IFireMode* pFireMode = pWeapon ? pWeapon->GetFireMode(pWeapon->GetCurrentFireMode()) : NULL;
		ActivateOutput(&m_actInfo,EOP_AMMO, pFireMode ? pFireMode->GetAmmoCount() : 0);
		ActivateOutput(&m_actInfo, EOP_ONRELOAD, true);
	}

	virtual void OnShoot(IWeapon *pWeapon, EntityId shooterId, EntityId ammoId, IEntityClass* pAmmoType,
		const Vec3 &pos, const Vec3 &dir, const Vec3 &vel) 
	{
		IFireMode* pFireMode = pWeapon ? pWeapon->GetFireMode(pWeapon->GetCurrentFireMode()) : NULL;
		ActivateOutput(&m_actInfo,EOP_AMMO, pFireMode ? pFireMode->GetAmmoCount() - 1 : 0);
		ActivateOutput(&m_actInfo, EOP_ONSHOOT, true);
	}

	virtual void OnFireModeChanged(IWeapon *pWeapon, int currentFireMode)
	{
		UpdateWeaponChange(GetCurrentWeapon());
		ActivateOutput(&m_actInfo,EOP_ONFIREMODECHANGE, true);
	}

	virtual void OnOutOfAmmo(IWeapon *pWeapon, IEntityClass* pAmmoType)
	{
		ActivateOutput(&m_actInfo,EOP_ONOUTOFAMMO, true);
	}

	virtual void OnZoomChanged(IWeapon* pWeapon, bool zoomed, int idx)
	{
		UpdateWeaponChange(GetCurrentWeapon());
		ActivateOutput(&m_actInfo,EOP_ONZOOM, true);
	}

	// ~IWeaponEventListener

private:
	void UpdateWeaponChange(CWeapon* pWeapon)
	{
		int iFireMode = pWeapon ? pWeapon->GetCurrentFireMode() : -1;
		IFireMode* pFireMode = pWeapon ? pWeapon->GetFireMode(iFireMode) : NULL;
		int iZoomMode = pWeapon ? pWeapon->GetCurrentZoomMode() : -1;
		IZoomMode* pZoomMode = pWeapon ? pWeapon->GetZoomMode(iZoomMode) : NULL;
		IEntity* pWeaponEntity = pWeapon ? pWeapon->GetEntity() : NULL;

		ActivateOutput(&m_actInfo,EOP_WEAPONID, pWeaponEntity ? pWeaponEntity->GetId() : 0);
		ActivateOutput(&m_actInfo,EOP_WEAPONNAME, pWeaponEntity ? string(pWeaponEntity->GetClass()->GetName()) : string(""));
		ActivateOutput(&m_actInfo,EOP_ISMELEE,  pFireMode ? pFireMode->GetClipSize() == 0 ? true : false : true);

		ActivateOutput(&m_actInfo,EOP_AMMOTYPE, iFireMode);
		ActivateOutput(&m_actInfo,EOP_AMMONAME, pFireMode ? string(pFireMode->GetName()) : string(""));
		ActivateOutput(&m_actInfo,EOP_AMMO, pFireMode ? pFireMode->GetAmmoCount() : 0);
		ActivateOutput(&m_actInfo,EOP_MAXAMMO, pFireMode ? pFireMode->GetClipSize() : 0);

		ActivateOutput(&m_actInfo,EOP_ZOOMED, pWeapon ? pWeapon->IsZoomed() : false);
		ActivateOutput(&m_actInfo,EOP_ZOOMMODE, iZoomMode);
		ActivateOutput(&m_actInfo,EOP_ZOOMNAME, pWeapon ? string(pWeapon->GetZoomModeName(iZoomMode)) : string(""));
		ActivateOutput(&m_actInfo,EOP_ZOOMSTEP, pZoomMode ? pZoomMode->GetCurrentStep() : 0);
		ActivateOutput(&m_actInfo,EOP_ZOOMMAXSTEP, pZoomMode ? pZoomMode->GetMaxZoomSteps(): 0);
	}

	void Enable(SActivationInfo* pActInfo)
	{
		Disable();
		m_entityId = pActInfo->pEntity != 0 ? pActInfo->pEntity->GetId() : 0;
		CPlayer* pPlayer = RetrPlayer(m_entityId);
		EntityId currItemId = pPlayer ? pPlayer->GetCurrentItemId() : 0;
		RegisterItemSystem();
		RegisterWeapon(currItemId);
		m_bEnabled = true;

	}

	void Disable()
	{
		UnRegisterWeapon();
		UnRegisterItemSystem();
		m_entityId = 0;
		m_bEnabled = false;
	}

	void RegisterWeapon(EntityId itemId)
	{
		m_currentWeaponId = 0;
		CPlayer* pPlayer = RetrPlayer(m_entityId);
		if (pPlayer)
		{
			CWeapon* pCurrentWeapon = pPlayer->GetWeapon(itemId);
				
			if ( pCurrentWeapon )
			{
				m_currentWeaponId = pCurrentWeapon->GetEntityId();
				pCurrentWeapon->AddEventListener( this, "CFlowNode_WeaponSensor" );
			}
		}
	}

	void UnRegisterWeapon()
	{
		CPlayer* pPlayer = RetrPlayer(m_entityId);
		if (pPlayer)
		{
			CWeapon* pOldWeapon = m_currentWeaponId != 0 ? pPlayer->GetWeapon( m_currentWeaponId ) : NULL;
			if ( pOldWeapon )
			{
				pOldWeapon->RemoveEventListener( this );
			}
		}
		m_currentWeaponId = 0;
	}

	void RegisterItemSystem()
	{
		IItemSystem* pItemSys = g_pGame ? g_pGame->GetIGameFramework()->GetIItemSystem() : NULL;
		if ( pItemSys )
			pItemSys->RegisterListener(this);
	}

	void UnRegisterItemSystem()
	{
		IItemSystem* pItemSys = g_pGame ? g_pGame->GetIGameFramework()->GetIItemSystem() : NULL;
		if ( pItemSys )
			pItemSys->UnregisterListener(this);
	}

	CWeapon* GetCurrentWeapon()
	{
		CPlayer* pPlayer = RetrPlayer(m_entityId);
		return pPlayer ? pPlayer->GetWeapon(m_currentWeaponId) : NULL;
	}

private:
	EntityId m_entityId;
	EntityId m_currentWeaponId;
	SActivationInfo m_actInfo;
	bool m_bEnabled;
};


class CFlowNode_DifficultyLevel : public CFlowBaseNode 
{
	enum INPUTS
	{
		EIP_Trigger = 0,
	};

	enum OUTPUTS
	{
		EOP_Easy = 0,
		EOP_Medium,
		EOP_Hard,
		EOP_Delta
	};

public:
	CFlowNode_DifficultyLevel( SActivationInfo * pActInfo ) { }

	void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig in_ports[] = 
		{
			InputPortConfig_Void  ( "Trigger", _HELP("Trigger to get difficulty level." )),
			{0}
		};
		static const SOutputPortConfig out_ports[] = 
		{
			OutputPortConfig_Void  ( "Easy", _HELP("Easy") ),
			OutputPortConfig_Void  ( "Normal", _HELP("Normal") ),
			OutputPortConfig_Void  ( "Hard", _HELP("Hard") ),
			OutputPortConfig_Void  ( "Delta", _HELP("Delta") ),
			{0}
		};
		config.pInputPorts = in_ports;
		config.pOutputPorts = out_ports;
		config.sDescription = _HELP("Get difficulty level.");
		config.SetCategory(EFLN_APPROVED);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		if(eFE_Activate == event && IsPortActive(pActInfo, EIP_Trigger))
		{
			const int level = g_pGameCVars->g_difficultyLevel; // [0] or 1-4
			if (level > EOP_Easy && level <= EOP_Delta)
			{
				ActivateOutput(pActInfo, level-1, true);
			}
		}
	}
};

// THIS FLOWNODE IS A SINGLETON, although it has member variables!
// THIS IS INTENDED!!!!
class CFlowNode_OverrideFOV : public CFlowBaseNode 
{
	enum INPUTS
	{
		EIP_SetFOV = 0,
		EIP_GetFOV,
		EIP_ResetFOV
	};

	enum OUTPUTS
	{
		EOP_CurFOV = 0,
		EOP_ResetDone,
	};

public:
	CFlowNode_OverrideFOV( SActivationInfo * pActInfo ) { m_storedFOV = 0.0f; }

	~CFlowNode_OverrideFOV()
	{
		if (m_storedFOV > 0.0f)
		{
			ICVar* pCVar = (gEnv && gEnv->pConsole) ? gEnv->pConsole->GetCVar("cl_fov") : 0;
			if (pCVar)
				pCVar->Set(m_storedFOV);
		}
	}

	void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig in_ports[] = 
		{
			InputPortConfig<float> ( "SetFOV", _HELP("Trigger to override Camera's FieldOfView." )),
			InputPortConfig_Void   ( "GetFOV", _HELP("Trigger to get current Camera's FieldOfView." )),
			InputPortConfig_Void   ( "ResetFOV", _HELP("Trigger to reset FieldOfView to default value." )),
			{0}
		};
		static const SOutputPortConfig out_ports[] = 
		{
			OutputPortConfig<float>  ( "CurFOV", _HELP("Current FieldOfView") ),
			OutputPortConfig_Void    ( "ResetDone", _HELP("Triggered after Reset") ),
			{0}
		};
		config.pInputPorts = in_ports;
		config.pOutputPorts = out_ports;
		config.sDescription = _HELP("Override Camera's FieldOfView. Cutscene ONLY!");
		config.SetCategory(EFLN_ADVANCED);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	void Serialize(SActivationInfo* pActInfo, TSerialize ser)
	{
		ICVar* pCVar = gEnv->pConsole->GetCVar("cl_fov");

		if (ser.IsWriting())
		{
			float curFOV = 0.0f;

			// in case we're currently active, store current value as well
			if (m_storedFOV > 0.0f && pCVar)
				curFOV = pCVar->GetFVal();

			ser.Value("m_storedFOV", m_storedFOV);
			ser.Value("curFOV", curFOV);
		}
		else
		{
			float storedFOV = 0.0f;
			float curFOV = 0.0f;
			ser.Value("m_storedFOV", storedFOV);
			ser.Value("curFOV", curFOV);

			// if we're currently active, restore first
			if(m_storedFOV > 0.0f)
			{
				if (pCVar)
					pCVar->Set(m_storedFOV);
				m_storedFOV = 0.0f;
			}

			m_storedFOV = storedFOV;
			if (m_storedFOV > 0.0f && curFOV > 0.0f && pCVar)
				pCVar->Set(curFOV);
		}
	}

	void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		ICVar* pCVar = gEnv->pConsole->GetCVar("cl_fov");
		if (pCVar == 0)
			return;

		switch (event)
		{
		case eFE_Initialize:
			if (m_storedFOV > 0.0f)
			{
				pCVar->Set(m_storedFOV);
				m_storedFOV = 0.0f;
			}
			break;

		case eFE_Activate:
			// get fov
			if (IsPortActive(pActInfo, EIP_GetFOV))
				ActivateOutput(pActInfo, EOP_CurFOV, pCVar->GetFVal());

			// set fov (store backup if not already done)
			if (IsPortActive(pActInfo, EIP_SetFOV))
			{
				if (m_storedFOV <= 0.0f)
					m_storedFOV = pCVar->GetFVal();
				pCVar->Set(GetPortFloat(pActInfo, EIP_SetFOV));
			}
			if (IsPortActive(pActInfo, EIP_ResetFOV))
			{
				if (m_storedFOV > 0.0f)
				{
					pCVar->Set(m_storedFOV);
					m_storedFOV = 0.0f;
				}
			}
			break;
		}
	}

	float m_storedFOV;
};


class CFlowNode_EntityToScreenPos : public CFlowBaseNode, public IGameFrameworkListener
{
	enum INPUTS
	{
		EIP_ENABLE = 0,
		EIP_DISABLE,
	};

	enum OUTPUTS
	{
		EOP_PX = 0,
		EOP_PY,
		EOP_FALLOFFX,
		EOP_FALLOFFY,
	};

public:
	CFlowNode_EntityToScreenPos( SActivationInfo * pActInfo )
		: m_pEntity(NULL)
	{
	}

	~CFlowNode_EntityToScreenPos()
	{
		if (gEnv && gEnv->pGame)
		gEnv->pGame->GetIGameFramework()->UnregisterListener(this);
	}

	void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig in_ports[] = 
		{
			InputPortConfig_Void   ( "Enable", _HELP("Trigger to enable this node" )),
			InputPortConfig_Void   ( "Disable", _HELP("Trigger to disable this node" )),
			{0}
		};
		static const SOutputPortConfig out_ports[] = 
		{
			OutputPortConfig<float>  ( "Px", _HELP("X pos on screen (0-1)") ),
			OutputPortConfig<float>  ( "Py", _HELP("Y pos on screen (0-1)") ),
			OutputPortConfig<int>  ( "FalloffX", _HELP("-1 if entity is left from viewfrustrum, 0 if inside, 1 if right") ),
			OutputPortConfig<int>  ( "FalloffY", _HELP("-1 if entity is above the viewfrustrum, 0 if inside, 1 if beneath") ),
			{0}
		};
		config.nFlags |= EFLN_TARGET_ENTITY;
		config.pInputPorts = in_ports;
		config.pOutputPorts = out_ports;
		config.sDescription = _HELP("Get Screen pos for current entity");
		config.SetCategory(EFLN_ADVANCED);
	}

	virtual void GetMemoryUsage(ICrySizer * s) const
	{
		s->Add(*this);
	}

	void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch (event)
		{
		case eFE_Initialize:
			m_actInfo = *pActInfo;
			m_pEntity = NULL;
			gEnv->pGame->GetIGameFramework()->UnregisterListener(this);
			break;
		case eFE_SetEntityId:
			m_pEntity = pActInfo->pEntity;
			break;
		case eFE_Activate:
			if (IsPortActive(pActInfo, EIP_ENABLE))
			{
				gEnv->pGame->GetIGameFramework()->RegisterListener(this, "CFlowNode_EntityToScreenPos", FRAMEWORKLISTENERPRIORITY_HUD);
				m_pEntity = pActInfo->pEntity;
			}
			if (IsPortActive(pActInfo, EIP_DISABLE))
			{
				gEnv->pGame->GetIGameFramework()->UnregisterListener(this);
			}
			break;
		}
	}

	// IGameFrameworkListener
	virtual void OnSaveGame(ISaveGame* pSaveGame) {}
	virtual void OnLoadGame(ILoadGame* pLoadGame) {}
	virtual void OnLevelEnd(const char* nextLevel) {}
	virtual void OnActionEvent(const SActionEvent& event) {}
	virtual void OnPostUpdate(float fDelta)
	{
		if ( m_pEntity )
		{
			AABB box;
			m_pEntity->GetWorldBounds(box);
			Vec3 entityPos = box.GetCenter();

			Vec3 screenPos;
			gEnv->pRenderer->ProjectToScreen(entityPos.x,entityPos.y,entityPos.z,&screenPos.x,&screenPos.y,&screenPos.z);
			screenPos.x *= 0.01f;
			screenPos.y *= 0.01f;
			int falloffx = screenPos.x < 0 ? -1 : screenPos.x > 1 ? 1 : screenPos.z < 1 ? 0 : -1;
			int falloffy = screenPos.y < 0 ? -1 : screenPos.y > 1 ? 1 : screenPos.z < 1 ? 0 : -1;
			screenPos.x = clamp(screenPos.x,0,1.f);
			screenPos.y = clamp(screenPos.y,0,1.f);
			ActivateOutput(&m_actInfo, EOP_PX, screenPos.x);
			ActivateOutput(&m_actInfo, EOP_PY, screenPos.y);
			ActivateOutput(&m_actInfo, EOP_FALLOFFX, falloffx);
			ActivateOutput(&m_actInfo, EOP_FALLOFFY, falloffy);
		}
	}
	// ~IGameFrameworkListener

private:
	IEntity* m_pEntity;
	SActivationInfo m_actInfo;
};





REGISTER_FLOW_NODE("Game:ActorSensor",	CFlowNode_ActorSensor);
REGISTER_FLOW_NODE("Game:WeaponSensor",	CFlowNode_WeaponSensor);
REGISTER_FLOW_NODE("Entity:EntityScreenPos", CFlowNode_EntityToScreenPos);
REGISTER_FLOW_NODE_SINGLETON("Game:DifficultyLevel",	CFlowNode_DifficultyLevel);
REGISTER_FLOW_NODE_SINGLETON("Camera:OverrideFOV",	CFlowNode_OverrideFOV); // Intended: Singleton although contains members!

