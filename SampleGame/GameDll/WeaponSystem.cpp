/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 18:10:2005   18:00 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "Game.h"
#include <IEntitySystem.h>
#include <ICryPak.h>
#include <IScriptSystem.h>
#include "IGameObject.h"
#include "Actor.h"
#include "ItemParams.h"
#include "WeaponSystem.h"

#include "Projectile.h"
#include "Bullet.h"
#include "Rock.h"
#include "Rocket.h"
#include "HomingMissile.h"
#include "TacBullet.h"
#include "TagBullet.h"
#include "AVMine.h"
#include "Claymore.h"
//#include "EMPField.h"
#include "C4Projectile.h"

#include "Single.h"
#include "Automatic.h"
#include "Burst.h"
#include "Rapid.h"
#include "Throw.h"
#include "Plant.h"
#include "Detonate.h"
#include "FreezingBeam.h"
#include "Charge.h"
#include "Shotgun.h"
#include "Melee.h"
#include "WorkOnTarget.h"
#include "Scan.h"
#include "SingleTG.h"


#include "IronSight.h"
#include "Scope.h"

template <typename T, typename R> R *CreateIt() { return new T(); };



#define REGISTER_PROJECTILE(name, T)	\
struct C##name##Creator : public IGameObjectExtensionCreatorBase	\
{ \
	T *Create() \
	{ \
		return new T(); \
	} \
	void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount ) \
	{ \
		T::GetGameObjectExtensionRMIData( ppRMI, nCount ); \
	} \
}; \
static C##name##Creator _##name##Creator; \
RegisterProjectile(#name, &_##name##Creator);

//------------------------------------------------------------------------
CWeaponSystem::CWeaponSystem(CGame *pGame, ISystem *pSystem)
: m_pGame(pGame),
	m_pSystem(pSystem),
	m_pItemSystem(pGame->GetIGameFramework()->GetIItemSystem()),
	m_pPrecache(0),
	m_reloading(false),
	m_recursing(false),
	m_frozenEnvironment(false),
	m_wetEnvironment(false),
	m_tokensUpdated(false)
{
	// register fire modes here
	RegisterFireMode("Single", &CreateIt<CSingle, IFireMode>);
	RegisterFireMode("Automatic", &CreateIt<CAutomatic, IFireMode>);
	RegisterFireMode("Burst", &CreateIt<CBurst, IFireMode>);
	RegisterFireMode("Rapid", &CreateIt<CRapid, IFireMode>);
	RegisterFireMode("Throw", &CreateIt<CThrow, IFireMode>);
	RegisterFireMode("Plant", &CreateIt<CPlant, IFireMode>);
	RegisterFireMode("Detonate", &CreateIt<CDetonate, IFireMode>);
	RegisterFireMode("FreezingBeam", &CreateIt<CFreezingBeam, IFireMode>);
	RegisterFireMode("Charge", &CreateIt<CCharge, IFireMode>);
	RegisterFireMode("Shotgun", &CreateIt<CShotgun, IFireMode>);	
	RegisterFireMode("Melee", &CreateIt<CMelee, IFireMode>);
	RegisterFireMode("WorkOnTarget", &CreateIt<CWorkOnTarget, IFireMode>);
	RegisterFireMode("Scan", &CreateIt<CScan, IFireMode>);
	RegisterFireMode("SingleTG", &CreateIt<CSingleTG, IFireMode>);
  
	// register zoom modes here
	RegisterZoomMode("IronSight", &CreateIt<CIronSight, IZoomMode>);
	RegisterZoomMode("Scope", &CreateIt<CScope, IZoomMode>);

	// register zoom mode data/params that might be shared between different instances of the same weapon class
	RegisterZoomModeData("IronSightData", &CreateIt<CIronSightSharedData, IWeaponSharedData>);
	RegisterZoomModeData("ScopeData",	&CreateIt<CScopeSharedData, IWeaponSharedData>);

	// registyer fire mode data/params that might be shared between different instances of the same weapon class
	RegisterFireModeData("SingleData", &CreateIt<CSingleSharedData, IWeaponSharedData>);
	RegisterFireModeData("BurstData", &CreateIt<CBurstSharedData, IWeaponSharedData>);
	RegisterFireModeData("AutomaticData", &CreateIt<CAutomaticSharedData, IWeaponSharedData>);
	RegisterFireModeData("ChargeData", &CreateIt<CChargeSharedData, IWeaponSharedData>);
	RegisterFireModeData("BeamData", &CreateIt<CBeamSharedData, IWeaponSharedData>);
	RegisterFireModeData("FreezingBeamData", &CreateIt<CFreezingBeamSharedData, IWeaponSharedData>);
	RegisterFireModeData("RapidData", &CreateIt<CRapidSharedData, IWeaponSharedData>);
	RegisterFireModeData("ShotgunData", &CreateIt<CShotgunSharedData, IWeaponSharedData>);
	RegisterFireModeData("ThrowData", &CreateIt<CThrowSharedData, IWeaponSharedData>);
	RegisterFireModeData("MeleeData", &CreateIt<CMeleeSharedData, IWeaponSharedData>);


	// register projectile classes here
	REGISTER_PROJECTILE(Projectile, CProjectile);
	REGISTER_PROJECTILE(Bullet, CBullet);
	REGISTER_PROJECTILE(Rock, CRock);
	REGISTER_PROJECTILE(Rocket, CRocket);
  REGISTER_PROJECTILE(HomingMissile, CHomingMissile);
	REGISTER_PROJECTILE(TacBullet, CTacBullet);
	REGISTER_PROJECTILE(TagBullet, CTagBullet);
	REGISTER_PROJECTILE(AVExplosive, CAVMine);
	REGISTER_PROJECTILE(ClaymoreExplosive, CClaymore);
	//REGISTER_PROJECTILE(EMPField, CEMPField);
	REGISTER_PROJECTILE(C4Projectile, CC4Projectile); 

	m_pPrecache = gEnv->pConsole->GetCVar("i_precache");

	CBullet::EntityClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("Bullet");
	CBullet::SetWaterMaterialId();

	m_pGame->GetIGameFramework()->GetILevelSystem()->AddListener(this);
}

//------------------------------------------------------------------------
CWeaponSystem::~CWeaponSystem()
{
	DumpPoolSizes();
	while (!m_pools.empty())
		FreePool(m_pools.begin()->first);

	// cleanup current projectiles
	for (TProjectileMap::iterator pit = m_projectiles.begin(); pit != m_projectiles.end(); ++pit)
		gEnv->pEntitySystem->RemoveEntity(pit->first, true);

	for (TAmmoTypeParams::iterator it = m_ammoparams.begin(); it != m_ammoparams.end(); ++it)
	{
		SAmmoTypeDesc &desc=it->second;
		delete desc.params;

		if (!desc.configurations.empty())
		{
			for (std::map<string, const SAmmoParams *>::iterator ait=desc.configurations.begin(); ait!=desc.configurations.end(); ait++)
				delete ait->second;
		}
	}

	m_pGame->GetIGameFramework()->GetILevelSystem()->RemoveListener(this);
}

//------------------------------------------------------------------------
void CWeaponSystem::Update(float frameTime)
{
	m_tracerManager.Update(frameTime);
	CheckEnvironmentChanges();
}

//------------------------------------------------------------------------
void CWeaponSystem::Release()
{
	delete this;
}

//------------------------------------------------------------------------
void CWeaponSystem::Reload()
{
	m_reloading = true;

	// cleanup current projectiles
	for (TProjectileMap::iterator pit = m_projectiles.begin(); pit != m_projectiles.end();)
	{
		//Bugfix: RemoveEntity removes projectile from map, thus invalidating iterator
		TProjectileMap::iterator next = pit;        
		next++;
		gEnv->pEntitySystem->RemoveEntity(pit->first, true);
		pit = next;
	}
	m_projectiles.clear();

	for (TAmmoTypeParams::iterator it = m_ammoparams.begin(); it != m_ammoparams.end(); ++it)
	{
		SAmmoTypeDesc &desc=it->second;
		delete desc.params;
		if (!desc.configurations.empty())
		{
			for (std::map<string, const SAmmoParams *>::iterator ait=desc.configurations.begin(); ait!=desc.configurations.end(); ait++)
				delete ait->second;
		}
	}

	m_ammoparams.clear();

	m_tracerManager.Reset();

	for (TFolderList::iterator it=m_folders.begin(); it!=m_folders.end(); ++it)
		Scan(it->c_str());

	m_reloading = false;
}

//------------------------------------------------------------------------
void CWeaponSystem::OnLoadingStart(ILevelInfo *pLevel)
{
	if (gEnv->bMultiplayer)
		SetConfiguration("mp");
	else
		SetConfiguration("");

	//Reset cache geometry flags, since cache geometry is reset on loading start in ItemSytem
	for (TAmmoTypeParams::iterator it = m_ammoparams.begin(); it != m_ammoparams.end(); ++it)
	{
		SAmmoTypeDesc &desc=it->second;
		desc.params->ResetCacheGeometry();
		if (!desc.configurations.empty())
		{
			for (std::map<string, const SAmmoParams *>::iterator ait=desc.configurations.begin(); ait!=desc.configurations.end(); ait++)
			{
				ait->second->ResetCacheGeometry();
			}
		}
	}
}

//------------------------------------------------------------------------
void CWeaponSystem::OnLoadingComplete(ILevel *pLevel)
{
	// marcio: precaching of items enabled by default for now
//	ICVar *sys_preload=gEnv->pConsole->GetCVar("sys_preload");
//	if ((!sys_preload || sys_preload->GetIVal()) && m_pPrecache->GetIVal())
	{
		for (TAmmoTypeParams::iterator it=m_ammoparams.begin(); it!=m_ammoparams.end(); ++it)
		{
			const SAmmoParams *pParams=GetAmmoParams(it->first);
			const IItemParamsNode *params = pParams->pItemParams;

			//Beni - Projectile Geometry is cached on demand, only if it's spawn at least once
			//const IItemParamsNode *geometry = params?params->GetChild("geometry"):0;

			//m_pItemSystem->CacheGeometry(geometry);

			// Preload particle assets.
			for (int ch = params->GetChildCount()-1; ch >= 0; ch--)
			{
				const IItemParamsNode *child = params->GetChild(ch);
				if (child)
				{
					CItemParamReader reader(child);
					const char *effect = 0;
					reader.Read("effect", effect);
					if (effect && *effect)
						gEnv->pParticleManager->FindEffect(effect, "WeaponSystem");
				}
			}
		}
	}	

	if(!m_tokensUpdated)
	{
		m_wetEnvironment = m_frozenEnvironment = false;
		ApplyEnvironmentChanges(); //Reset on loading new level
		CreateEnvironmentGameTokens(m_frozenEnvironment,m_wetEnvironment); //Do not force set/creation if exit
	}
	m_tokensUpdated = false;
	
}

//------------------------------------------------------------------------
IFireMode *CWeaponSystem::CreateFireMode(const char *name)
{
	TFireModeRegistry::iterator it = m_fmregistry.find(CONST_TEMP_STRING(name));
	if (it != m_fmregistry.end())
		return it->second();
	return 0;
}

//------------------------------------------------------------------------
void CWeaponSystem::RegisterFireMode(const char *name, IFireMode *(*CreateProc)())
{
	m_fmregistry.insert(TFireModeRegistry::value_type(name, CreateProc));
}

//------------------------------------------------------------------------
IZoomMode *CWeaponSystem::CreateZoomMode(const char *name)
{
	TZoomModeRegistry::iterator it = m_zmregistry.find(CONST_TEMP_STRING(name));
	if (it != m_zmregistry.end())
		return it->second();
	return 0;
}

//------------------------------------------------------------------------
void CWeaponSystem::RegisterZoomMode(const char *name, IZoomMode *(*CreateProc)())
{
	m_zmregistry.insert(TZoomModeRegistry::value_type(name, CreateProc));
}

//-------------------------------------------------------------------------
IWeaponSharedData* CWeaponSystem::CreateZoomModeData(const char *name)
{
	TWeaponSharedDataRegistry::iterator it = m_zmDataRegistry.find(CONST_TEMP_STRING(name));
	if(it != m_zmDataRegistry.end())
		return it->second();
	return 0;
}

//--------------------------------------------------------------------------
void CWeaponSystem::RegisterZoomModeData(const char* name, IWeaponSharedData *(*CreatorProc)())
{
	m_zmDataRegistry.insert(TWeaponSharedDataRegistry::value_type(name, CreatorProc));
}

//-------------------------------------------------------------------------
IWeaponSharedData* CWeaponSystem::CreateFireModeData(const char *name)
{
	TWeaponSharedDataRegistry::iterator it = m_fmDataRegistry.find(CONST_TEMP_STRING(name));
	if(it != m_fmDataRegistry.end())
		return it->second();
	return 0;
}

//--------------------------------------------------------------------------
void CWeaponSystem::RegisterFireModeData(const char* name, IWeaponSharedData *(*CreatorProc)())
{
	m_fmDataRegistry.insert(TWeaponSharedDataRegistry::value_type(name, CreatorProc));
}

//------------------------------------------------------------------------
CProjectile *CWeaponSystem::SpawnAmmo(IEntityClass* pAmmoType, bool isRemote)
{
	TAmmoTypeParams::const_iterator it = m_ammoparams.find(pAmmoType);
	if (it == m_ammoparams.end())
	{
		GameWarning("Failed to spawn ammo '%s'! Unknown class or entity class not registered...", pAmmoType?pAmmoType->GetName():"");
		return 0;
	}

	const SAmmoParams* pAmmoParams = it->second.params;
	if (!m_config.empty())
	{
		std::map<string, const SAmmoParams *>::const_iterator cit=it->second.configurations.find(m_config);
		if (cit != it->second.configurations.end())
			pAmmoParams=cit->second;
		else
			pAmmoParams=it->second.params;
	}

	if (pAmmoParams->reusable)
	{
		if (isRemote || (!pAmmoParams->serverSpawn &&
			(pAmmoParams->flags&(ENTITY_FLAG_CLIENT_ONLY|ENTITY_FLAG_SERVER_ONLY))))
			return UseFromPool(pAmmoType, pAmmoParams);
	}

	return DoSpawnAmmo(pAmmoType, isRemote, pAmmoParams);
}


//------------------------------------------------------------------------
CProjectile *CWeaponSystem::DoSpawnAmmo(IEntityClass* pAmmoType, bool isRemote, const SAmmoParams *pAmmoParams)
{
	bool isServer=gEnv->bServer;
	bool isClient=gEnv->IsClient();

	if ( pAmmoParams->serverSpawn && (!isServer || IsDemoPlayback()) )
	{
		if (!pAmmoParams->predictSpawn || isRemote)
			return 0;
	}

	SEntitySpawnParams spawnParams;
	spawnParams.pClass = pAmmoType;
	spawnParams.sName = "ammo";
	spawnParams.nFlags = pAmmoParams->flags | ENTITY_FLAG_NO_PROXIMITY; // No proximity for this entity.

	IEntity *pEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams);
	if (!pEntity)
	{
		GameWarning("Failed to spawn ammo '%s'! Entity creation failed...", pAmmoType->GetName());
		return 0;
	}

	CProjectile *pProjectile = GetProjectile(pEntity->GetId());

	if (pProjectile && !isServer && !isRemote && pAmmoParams->predictSpawn)
		pProjectile->GetGameObject()->RegisterAsPredicted();

	return pProjectile;
}

//------------------------------------------------------------------------
bool CWeaponSystem::IsServerSpawn(IEntityClass* pAmmoType) const
{
	if (!pAmmoType)
		return false;

	if (const SAmmoParams *pAmmoParams=GetAmmoParams(pAmmoType))
		return pAmmoParams->serverSpawn!=0;
	return false;
}

//-------------------------------------------	-----------------------------
void CWeaponSystem::RegisterProjectile(const char *name, IGameObjectExtensionCreatorBase *pCreator)
{
	m_projectileregistry.insert(TProjectileRegistry::value_type(name, pCreator));
}

//------------------------------------------------------------------------
const SAmmoParams* CWeaponSystem::GetAmmoParams(IEntityClass* pAmmoType) const
{
	TAmmoTypeParams::const_iterator it=m_ammoparams.find(pAmmoType);
	if (it==m_ammoparams.end())
		return 0;

	if (m_config.empty())
		return it->second.params;

	std::map<string, const SAmmoParams *>::const_iterator cit=it->second.configurations.find(m_config);
	if (cit != it->second.configurations.end())
		return cit->second;

	return it->second.params;
}

//------------------------------------------------------------------------
void CWeaponSystem::AddProjectile(IEntity *pEntity, CProjectile *pProjectile)
{
	m_projectiles.insert(TProjectileMap::value_type(pEntity->GetId(), pProjectile));
}

//------------------------------------------------------------------------
void CWeaponSystem::RemoveProjectile(CProjectile *pProjectile)
{
	m_projectiles.erase(pProjectile->GetEntity()->GetId());

	RemoveFromPool(pProjectile);
}

//------------------------------------------------------------------------
CProjectile *CWeaponSystem::GetProjectile(EntityId entityId)
{
	TProjectileMap::iterator it = m_projectiles.find(entityId);
	if (it != m_projectiles.end())
		return it->second;
	return 0;
}

//------------------------------------------------------------------------
int  CWeaponSystem::QueryProjectiles(SProjectileQuery& q)
{
    IEntityClass* pClass = q.ammoName?gEnv->pEntitySystem->GetClassRegistry()->FindClass(q.ammoName):0;
    m_queryResults.resize(0);
    if(q.box.IsEmpty())
    {
        for(TProjectileMap::iterator it = m_projectiles.begin();it!=m_projectiles.end();++it)
        {
            IEntity *pEntity = it->second->GetEntity();
            if(pClass == 0 || pEntity->GetClass() == pClass)
            m_queryResults.push_back(pEntity);
        }
    }
    else
    {
        for(TProjectileMap::iterator it = m_projectiles.begin();it!=m_projectiles.end();++it)
        {
            IEntity *pEntity = it->second->GetEntity();
            if(q.box.IsContainPoint(pEntity->GetWorldPos()))
            {
                m_queryResults.push_back(pEntity);
            }
        }
    }
    
    q.nCount = int(m_queryResults.size());
    if(q.nCount)
        q.pResults = &m_queryResults[0];
    return q.nCount;
}

//------------------------------------------------------------------------
void CWeaponSystem::Scan(const char *folderName)
{
	stack_string folder = folderName;
	stack_string search = folder;
	stack_string subName;
	stack_string xmlFile;
	search += "/*.*";

	ICryPak *pPak = gEnv->pCryPak;

	_finddata_t fd;
	intptr_t handle = pPak->FindFirst(search.c_str(), &fd);

	if (!m_recursing)
		CryLog("Loading ammo XML definitions from '%s'!", folderName);

	if (handle > -1)
	{
		do
		{
			if (!strcmp(fd.name, ".") || !strcmp(fd.name, ".."))
				continue;

			if (fd.attrib & _A_SUBDIR)
			{
				subName = folder+"/"+fd.name;
				if (m_recursing)
					Scan(subName.c_str());
				else
				{
					m_recursing=true;
					Scan(subName.c_str());
					m_recursing=false;
				}
				continue;
			}

			if (stricmp(PathUtil::GetExt(fd.name), "xml"))
				continue;

			xmlFile = folder + "/" + fd.name;
			XmlNodeRef rootNode = m_pSystem->LoadXmlFromFile(xmlFile.c_str());

			if (!rootNode)
			{
				GameWarning("Invalid XML file '%s'! Skipping...", xmlFile.c_str());
				continue;
			}

			if (!ScanXML(rootNode, xmlFile.c_str()))
				continue;

		} while (pPak->FindNext(handle, &fd) >= 0);
	}

	if (!m_recursing)
		CryLog("Finished loading ammo XML definitions from '%s'!", folderName);

	if (!m_reloading && !m_recursing)
		m_folders.push_back(folderName);
}

//------------------------------------------------------------------------
bool CWeaponSystem::ScanXML(XmlNodeRef &root, const char *xmlFile)
{
	MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_Other, 0, "Weapon xml (%s)", xmlFile);

	if (strcmpi(root->getTag(), "ammo"))
		return false;

	const char *name = root->getAttr("name");
	if (!name)
	{
		GameWarning("Missing ammo name in XML '%s'! Skipping...", xmlFile);
		return false;
	}

	const char *className = root->getAttr("class");

	if (!className)
	{
		GameWarning("Missing ammo class in XML '%s'! Skipping...", xmlFile);
		return false;
	}

	TProjectileRegistry::iterator it = m_projectileregistry.find(CONST_TEMP_STRING(className));
	if (it == m_projectileregistry.end())
	{
		GameWarning("Unknown ammo class '%s' specified in XML '%s'! Skipping...", className, xmlFile);
		return false;
	}

	const char *scriptName = root->getAttr("script");
	IEntityClassRegistry::SEntityClassDesc classDesc;
	classDesc.sName = name;
	classDesc.sScriptFile = scriptName?scriptName:"";
	//classDesc.pUserProxyData = (void *)it->second;
	//classDesc.pUserProxyCreateFunc = &CreateProxy<CProjectile>;
	classDesc.flags |= ECLF_INVISIBLE;

	IEntityClass* pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name);

	if (!m_reloading && !pClass)
	{
		m_pGame->GetIGameFramework()->GetIGameObjectSystem()->RegisterExtension(name, it->second, &classDesc);
		pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name);
		assert(pClass);
	}


	TAmmoTypeParams::iterator ait=m_ammoparams.find(pClass);
	if (ait==m_ammoparams.end())
	{
		std::pair<TAmmoTypeParams::iterator, bool> result = m_ammoparams.insert(TAmmoTypeParams::value_type(pClass, SAmmoTypeDesc()));
		ait=result.first;
	}

	const char *configName = root->getAttr("configuration");

	IItemParamsNode *params = m_pItemSystem->CreateParams();
	params->ConvertFromXML(root);

	SAmmoParams *pAmmoParams=new SAmmoParams(params, pClass);

	SAmmoTypeDesc &desc=ait->second;

	if (!configName || !configName[0])
	{
		if (desc.params)
			delete desc.params;
		desc.params=pAmmoParams;
	}
	else
		desc.configurations.insert(std::make_pair<string, const SAmmoParams*>(configName, pAmmoParams));

	return true;
}

//------------------------------------------------------------------------
void CWeaponSystem::DebugGun(IConsoleCmdArgs *args)
{
  IGameFramework* pGF = gEnv->pGame->GetIGameFramework();  
  IItemSystem* pItemSystem = pGF->GetIItemSystem();
 
  IActor* pActor = pGF->GetClientActor();
  if (!pActor || !pActor->IsPlayer())
    return;

  IInventory *pInventory = pActor->GetInventory();
  if (!pInventory)
    return;  
  
  // give & select the debuggun 
	EntityId itemId = pInventory->GetItemByClass(CItem::sDebugGunClass);
  if (0 == itemId)        
  {
    // if actor doesn't have it, only give it in editor
    if (!gEnv->IsEditor())
      return;

		itemId = pItemSystem->GiveItem(pActor, CItem::sDebugGunClass->GetName(), false, true, true);
  }
  pItemSystem->SetActorItem(pActor, itemId, true);      
}

//------------------------------------------------------------------------
void CWeaponSystem::RefGun(IConsoleCmdArgs *args)
{
	IGameFramework* pGF = gEnv->pGame->GetIGameFramework();  
	IItemSystem* pItemSystem = pGF->GetIItemSystem();

	IActor* pActor = pGF->GetClientActor();
	if (!pActor || !pActor->IsPlayer())
		return;

	IInventory *pInventory = pActor->GetInventory();
	if (!pInventory)
		return;

	// give & select the refgun 
	EntityId itemId = pInventory->GetItemByClass(CItem::sRefWeaponClass);
	if (0 == itemId)        
	{
		// if actor doesn't have it, only give it in editor
		if (!gEnv->IsEditor())
			return;

		itemId = pItemSystem->GiveItem(pActor, CItem::sRefWeaponClass->GetName(), false, true, true);
	}
	pItemSystem->SetActorItem(pActor, itemId, true);   

}

//---------------------------------------------------------------
void CWeaponSystem::CreateEnvironmentGameTokens(bool frozenEnvironment, bool wetEnvironment)
{
	if(gEnv->bMultiplayer)
		return;

	IGameTokenSystem *pGameTokenSystem = gEnv->pGame->GetIGameFramework()->GetIGameTokenSystem();

	// create the game tokens if not present
	if(pGameTokenSystem)
	{
		pGameTokenSystem->SetOrCreateToken("weapon.effects.ice", TFlowInputData(frozenEnvironment));
		pGameTokenSystem->SetOrCreateToken("weapon.effects.wet", TFlowInputData(wetEnvironment));
	}
}

//--------------------------------------------------------------
void CWeaponSystem::ApplyEnvironmentChanges()
{	
}

//--------------------------------------------------------------
void CWeaponSystem::CheckEnvironmentChanges()	
{
	if(gEnv->bMultiplayer)
		return;

	IGameTokenSystem *pGameTokenSystem = gEnv->pGame->GetIGameFramework()->GetIGameTokenSystem();

	bool frozenEnvironment = m_frozenEnvironment;
	bool wetEnvironment = m_wetEnvironment;
	m_tokensUpdated = false;

	if(pGameTokenSystem)
	{
		// next call will leave value unchanged if not found
		pGameTokenSystem->GetTokenValueAs("weapon.effects.ice", frozenEnvironment);
		pGameTokenSystem->GetTokenValueAs("weapon.effects.wet", wetEnvironment);
	}

	if(m_frozenEnvironment!=frozenEnvironment)
	{
		m_frozenEnvironment = frozenEnvironment;
		ApplyEnvironmentChanges();
	}
	if(m_wetEnvironment!=wetEnvironment)
	{
		m_wetEnvironment = wetEnvironment;
		ApplyEnvironmentChanges();
	}

}

//------------------------------------------------------------------------
void CWeaponSystem::CreatePool(IEntityClass *pClass)
{
	TAmmoPoolMap::iterator it=m_pools.find(pClass);

	if (it!=m_pools.end())
		return;

	m_pools.insert(TAmmoPoolMap::value_type(pClass, SAmmoPoolDesc()));
}

//------------------------------------------------------------------------
void CWeaponSystem::FreePool(IEntityClass *pClass)
{
	TAmmoPoolMap::iterator it=m_pools.find(pClass);

	if (it==m_pools.end())
		return;

	SAmmoPoolDesc &desc=it->second;

	while(!desc.frees.empty())
	{
		CProjectile *pFree=desc.frees.front();
		desc.frees.pop_front();

		gEnv->pEntitySystem->RemoveEntity(pFree->GetEntityId(), true);
		--desc.size;
	}

	m_pools.erase(it);
}

//------------------------------------------------------------------------
uint16 CWeaponSystem::GetPoolSize(IEntityClass *pClass)
{
	TAmmoPoolMap::iterator it=m_pools.find(pClass);

	if (it==m_pools.end())
		return 0;

	return it->second.size;
}

//------------------------------------------------------------------------
CProjectile *CWeaponSystem::UseFromPool(IEntityClass *pClass, const SAmmoParams *pAmmoParams)
{
	TAmmoPoolMap::iterator it=m_pools.find(pClass);

	if (it==m_pools.end())
	{
		CreatePool(pClass);
		it=m_pools.find(pClass);
	}

	SAmmoPoolDesc &desc=it->second;

	if (!desc.frees.empty())
	{
		CProjectile *pProjectile=desc.frees.front();
		desc.frees.pop_front();

		pProjectile->GetEntity()->Hide(false);
		pProjectile->ReInitFromPool();
		return pProjectile;
	}
	else
	{
		CProjectile *pProjectile=DoSpawnAmmo(pClass, false, pAmmoParams);
		++desc.size;
		
		return pProjectile;
	}
}

//------------------------------------------------------------------------
bool CWeaponSystem::ReturnToPool(CProjectile *pProjectile)
{
	TAmmoPoolMap::iterator it=m_pools.find(pProjectile->GetEntity()->GetClass());
	assert(it!=m_pools.end());

	// It should not happen, but looks like it can some how while load/saving under certain circumstances...
	if(it == m_pools.end())
	{
		// Log trace, and return false (projectile will handle it)
		GameWarning("CWeaponSystem::ReturnToPool(): Trying to return projectile to a pool that doesn't exist (Class: %s)", pProjectile->GetEntity()->GetClass()->GetName());
		return false;
	}

	it->second.frees.push_back(pProjectile);

	pProjectile->GetEntity()->Hide(true);
	pProjectile->GetEntity()->SetWorldTM(IDENTITY);

	return true;
}

//------------------------------------------------------------------------
void CWeaponSystem::RemoveFromPool(CProjectile *pProjectile)
{
	TAmmoPoolMap::iterator it=m_pools.find(pProjectile->GetEntity()->GetClass());
	if (it==m_pools.end())
		return;

	if (stl::find_and_erase(it->second.frees, pProjectile))
		--it->second.size;
}

//------------------------------------------------------------------------
void CWeaponSystem::DumpPoolSizes()
{
	CryLogAlways("Ammo Pool Statistics:");
	for (TAmmoPoolMap::iterator it=m_pools.begin(); it!=m_pools.end(); it++)
	{
		int size=it->second.size;
		CryLogAlways("%s: %d", it->first->GetName(), size);
	}
}

//----------------------------------------
void CWeaponSystem::Serialize(TSerialize ser)
{
	ser.Value("m_wetEnvironment",m_wetEnvironment);
	ser.Value("m_frozenEnvironment",m_frozenEnvironment);
	
	if(ser.IsReading())
	{
		CreateEnvironmentGameTokens(m_frozenEnvironment,m_wetEnvironment);
		m_tokensUpdated = true;
	}
}

void CWeaponSystem::GetMemoryUsage(ICrySizer * s) const
{
	SIZER_SUBCOMPONENT_NAME(s, "WeaponSystem");
	int nSize = sizeof(*this);
	s->AddObject(this,nSize);

	m_tracerManager.GetMemoryUsage(s);
	s->AddContainer(m_fmregistry);
	s->AddContainer(m_zmregistry);
	s->AddContainer(m_projectileregistry);
	s->AddContainer(m_folders);
	s->AddContainer(m_queryResults);
	s->AddContainer(m_config);

	{
		SIZER_SUBCOMPONENT_NAME(s, "AmmoParams");
		int nSize = m_ammoparams.size() * sizeof(TAmmoTypeParams::value_type);
		for (TAmmoTypeParams::const_iterator iter = m_ammoparams.begin(); iter != m_ammoparams.end(); ++iter)
		{
			nSize += iter->second.params->GetMemorySize();
			nSize += iter->second.configurations.size()*sizeof(std::map<string, SAmmoTypeDesc>::value_type);

			for (std::map<string, const SAmmoParams *>::const_iterator it=iter->second.configurations.begin(); it!=iter->second.configurations.end(); it++)
			{
				nSize+=it->first.size();
				nSize+=it->second->GetMemorySize();
			}
		}
		s->AddObject(&m_ammoparams,nSize);
	}
	
	{
		SIZER_SUBCOMPONENT_NAME(s, "Projectiles");
		int nSize = m_projectiles.size() * sizeof(TProjectileMap::value_type);
		for (TProjectileMap::const_iterator iter = m_projectiles.begin(); iter != m_projectiles.end(); ++iter)
		{
			nSize += iter->second->GetMemorySize();
		}
		s->AddObject(&m_projectiles,nSize);
	}

	{
		SIZER_SUBCOMPONENT_NAME(s, "Pools");
		int nSize = m_pools.size() * sizeof(TAmmoPoolMap::value_type);
		for (TAmmoPoolMap::const_iterator piter = m_pools.begin(); piter != m_pools.end(); ++piter)
		{
			nSize += piter->second.frees.size()*sizeof(CProjectile *);
		}
		s->AddObject(&m_pools,nSize);
	}
}
