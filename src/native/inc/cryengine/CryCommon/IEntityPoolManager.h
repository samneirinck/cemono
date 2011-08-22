/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2010.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Interface for Entity pool manager
  
 -------------------------------------------------------------------------
  History:
  - 15:03:2010: Created by Kevin Kirst

*************************************************************************/

#ifndef __IENTITYPOOLMANAGER_H__
#define __IENTITYPOOLMANAGER_H__

#include <ISerialize.h>

struct SEntitySpawnParams;

struct IEntityPoolListener
{
	enum ESubscriptions
	{
		PoolBookmarkCreated			= BIT(0),
		EntityPreparedFromPool	= BIT(1),
		EntityReturnedToPool		= BIT(2),
		PoolDefinitionsLoaded		= BIT(3),
		BookmarkEntitySerialize = BIT(4),
	};
	enum { COUNT_SUBSCRIPTIONS = 5 };

	virtual ~IEntityPoolListener() {}

	//! Called when an entity bookmark is created on level load
	virtual void OnPoolBookmarkCreated(EntityId entityId, const SEntitySpawnParams& params, XmlNodeRef entityNode) {}

	//! Called when a bookmarked entity is prepared from the pool
	virtual void OnEntityPreparedFromPool(EntityId entityId, IEntity *pEntity) {}

	//! Called when a bookmarked entity is returned to the pool
	virtual void OnEntityReturnedToPool(EntityId entityId, IEntity *pEntity) {}

	//! Called on loading pool definitions
	virtual void OnPoolDefinitionsLoaded(size_t numAI) {}

	//! Called when serializing a particular entity, either to or from a bookmark
	virtual void OnBookmarkEntitySerialize(TSerialize serialize, void *pVEntity) {}
};

struct IEntityPoolManager
{
	struct SPreparingParams
	{
		EntityId entityId;
		tAIObjectID aiObjectId;
	};

	virtual ~IEntityPoolManager() {}

	virtual void Serialize(TSerialize ser) = 0;

	//! Request entity pools be enabled
	virtual void Enable(bool bEnable) = 0;

	//! Resets all entity pools and returns all active entities
	virtual void ResetPools(bool bSaveState = true) = 0;

	//! Listener setup
	virtual void AddListener(IEntityPoolListener *pListener, const char* szWho, uint32 uSubscriptions) = 0;
	virtual void RemoveListener(IEntityPoolListener *pListener) = 0;

	//! Controls to bring a bookmarked entity into/out of existence via its assigned pool
	virtual bool PrepareFromPool(EntityId entityId, bool bPrepareNow = false) = 0;
	virtual bool ReturnToPool(EntityId entityId, bool bSaveState = true) = 0;
	virtual void ResetBookmark(EntityId entityId) = 0;

	//! Returns true if the given entity class name is set to be bookmarked by default
	virtual bool IsClassDefaultBookmarked(const char* szClassName) const = 0;

	//! Returns true if the given entityId has a bookmark (is set to be created through the pool)
	virtual bool IsEntityBookmarked(EntityId entityId) const = 0;
	virtual const char* GetBookmarkedEntityName(EntityId entityId) const = 0;

	//! Returns true if the system is currently preparing an entity from the pool (and fills outParams)
	virtual bool IsPreparingEntity(SPreparingParams& outParams) const = 0;
};

#endif //__IENTITYPOOLMANAGER_H__
