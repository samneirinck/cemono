////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IEntitySystem.h
//  Version:     v1.00
//  Created:     17/6/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(IEntitySystem.h)

#ifndef __IEntitySystem_h__
#define __IEntitySystem_h__
#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CRYENTITYDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CRYENTITYDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CRYENTITYDLL_EXPORTS
	#define CRYENTITYDLL_API DLL_EXPORT
#else
	#define CRYENTITYDLL_API DLL_IMPORT
#endif

#include <IEntity.h>
#include <IEntityClass.h>
#include <smartptr.h>
#include <Cry_Geo.h>
#include <IXml.h>

// Forward declarations.
struct ISystem;
struct IEntitySystem;
class  ICrySizer;
struct IEntity;
struct SpawnParams;
struct IPhysicalEntity;
struct IBreakEventListener;
struct SRenderNodeCloneLookup;
struct EventPhysRemoveEntityParts ;
class CEntity;

struct IEntityPoolManager;

// Summary:
//	 Area Manager Interface.
UNIQUE_IFACE struct IArea
{
	virtual ~IArea(){}
	virtual int GetEntityAmount() const = 0;
	virtual const EntityId GetEntityByIdx(int index) const = 0;
	virtual void GetMinMax(Vec3 **min, Vec3 **max) const = 0;
	virtual int GetPriority() const = 0;
};

// Summary:
//	 EventListener interface for the AreaManager.
UNIQUE_IFACE struct IAreaManagerEventListener
{
	virtual ~IAreaManagerEventListener(){}
	// Summary:
	//	 Callback event.
	virtual void OnAreaManagerEvent( EEntityEvent event, EntityId TriggerEntityID, IArea *pArea ) = 0;
};

// Summary:
//	 Structure for additional AreaManager queries.
struct SAreaManagerResult
{
public: 
	SAreaManagerResult()
	{
		pArea = NULL;
		fDistanceSq = 0.0f;
		vPosOnHull	= Vec3(0);
		bInside	= false;
		bNear	= false;
	}

	IArea* pArea;
	float fDistanceSq;
	Vec3 vPosOnHull;
	bool bInside;
	bool bNear;
};


UNIQUE_IFACE struct IAreaManager
{
	virtual ~IAreaManager(){}
	virtual int						GetAreaAmount() const = 0;
	virtual const IArea*	GetArea(int areaIndex) const = 0;

	// Description:
	//	 Additional Query based on position. Needs preallocated space to write nMaxResults to pResults.
	// Return Value:
	//	 True on success or false on error or if provided structure was too small.
	virtual bool QueryAreas( Vec3 vPos, SAreaManagerResult *pResults, int nMaxResults) = 0;

	virtual void DrawLinkedAreas(EntityId linkedId) const = 0;

	// Summary:
	//	 Registers EventListener to the AreaManager.
	virtual void AddEventListener( IAreaManagerEventListener *pListener ) = 0;
	virtual void RemoveEventListener( IAreaManagerEventListener *pListener ) = 0;

	// Description:
	// Invokes a re-compilation of the area grid
	virtual void	SetAreasDirty() = 0;
};


// Description:
//	 Entity iterator interface. This interface is used to traverse trough all the entities in an entity system. In a way, 
//	 this iterator works a lot like a stl iterator.

UNIQUE_IFACE struct IEntityIt
{
	virtual ~IEntityIt(){}

	virtual void AddRef() = 0;

	// Summary:
	//	 Deletes this iterator and frees any memory it might have allocated.
	virtual void Release() = 0;

	// Summary:
	//	 Checks whether current iterator position is the end position.
	// Return Value:
	//	 True if iterator at end position.
	virtual bool IsEnd() = 0;

	// Summary:
	//	 Retrieves next entity.
	// Return Value:
	//	 A pointer to the entity that the iterator points to before it goes to the next.
	virtual IEntity * Next() = 0;

	// Summary:
	//	 Retrieves current entity.
	// Return Value:
	//	 The entity that the iterator points to.
	virtual IEntity * This() = 0;

	// Summary:
	//	 Positions the iterator at the beginning of the entity list.
	virtual void MoveFirst() = 0;
};

typedef _smart_ptr<IEntityIt> IEntityItPtr;

// Description:
//	 A callback interface for a class that wants to be aware when new entities are being spawned or removed. A class that implements
//	 this interface will be called every time a new entity is spawned, removed, or when an entity container is to be spawned.
struct IEntitySystemSink
{
	virtual ~IEntitySystemSink(){}
	// Description:
	//	 This callback is called before this entity is spawned. The entity will be created and added to the list of entities,
	//	 if this function returns true. Returning false will abort the spawning process.
	// Arguments:
	//	 params - The parameters that will be used to spawn the entity.
	virtual bool OnBeforeSpawn( SEntitySpawnParams &params ) = 0;
	
	// Description:
	//	 This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities, 
	//	 but has not been initialized yet.
	// Arguments:
	//	 pEntity - The entity that was just spawned
	//	 params	 -
	virtual void OnSpawn( IEntity *pEntity,SEntitySpawnParams &params ) = 0;

	// Description:
	//	 Called when an entity is being removed.
	// Arguments:
	//	 pEntity - The entity that is being removed. This entity is still fully valid.
	// Return Value:
	//	 True to allow removal, false to deny.
	virtual bool OnRemove( IEntity *pEntity ) = 0;

	// Description:
	//	 Called when an entity has been reused. You should clean up when this is called.
	// Arguments:
	//	 pEntity - The entity that is being reused. This entity is still fully valid and with its new EntityId.
	//   params - The new params this entity is using.
	virtual void OnReused( IEntity *pEntity, SEntitySpawnParams &params ) = 0;

	// Description:
	//	 Called in response to an entity event.
	// Arguments:
	//	 pEntity - The entity that is being removed. This entity is still fully valid.
	//	 event	 -
	virtual void OnEvent( IEntity *pEntity, SEntityEvent &event ) = 0;

	// Description:
	//	 Collect memory informations
	// Arguments:
	//	 pSizer - The Sizer class used to collect the memory informations
	virtual void GetMemoryUsage(class ICrySizer *pSizer) const{};
};

// Summary:
//	 Interface to the entity archetype.
UNIQUE_IFACE struct IEntityArchetype
{
	virtual ~IEntityArchetype(){}
	// Retrieve entity class of the archetype.
	virtual IEntityClass* GetClass() const = 0;
	virtual const char* GetName() const = 0;
	virtual IScriptTable* GetProperties() = 0;
	virtual void LoadFromXML( XmlNodeRef &propertiesNode ) = 0;
};

//////////////////////////////////////////////////////////////////////////
struct IEntityEventListener
{
	virtual ~IEntityEventListener(){}
	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event ) = 0;
};

// Forward declaration from physics interface.
struct EventPhys;
struct EventPhysRemoveEntityParts;

//////////////////////////////////////////////////////////////////////////
UNIQUE_IFACE struct IBreakableManager
{
	virtual ~IBreakableManager(){}
	enum EBReakageType
	{
		BREAKAGE_TYPE_DESTROY = 0,
		BREAKAGE_TYPE_FREEZE_SHATTER,
	};
	struct SBrokenObjRec
	{
		EntityId idEnt;
		IRenderNode * pSrcRenderNode;
		_smart_ptr<IStatObj> pStatObjOrg;
	};
	struct BreakageParams
	{
		EBReakageType type;					// Type of the breakage.
		float	fParticleLifeTime;		// Average lifetime of particle pieces.
		int		nGenericCount;				// If not 0, force particle pieces to spawn generically, this many times.
		bool	bForceEntity;					// Force pieces to spawn as entities.
		bool	bMaterialEffects;			// Automatically create "destroy" and "breakage" material effects on pieces.
		bool	bOnlyHelperPieces;		// Only spawn helper pieces.

		// Impulse params.
		float	fExplodeImpulse;			// Outward impulse to apply.
		Vec3	vHitImpulse;					// Hit impulse and center to apply.
		Vec3	vHitPoint;

		BreakageParams()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	struct SCreateParams
	{
		int nSlotIndex;
		Matrix34 slotTM;
		Matrix34 worldTM;
		float fScale;
		IMaterial *pCustomMtl;
		int nMatLayers;
		int nEntityFlagsAdd;
		int nEntitySlotFlagsAdd;
		int nRenderNodeFlags;
		IRenderNode *pSrcStaticRenderNode;
		const char *pName;
		IEntityClass* overrideEntityClass;

		SCreateParams() : fScale(1.0f),pCustomMtl(0),nSlotIndex(0),nRenderNodeFlags(0),pName(0),
			nMatLayers(0),nEntityFlagsAdd(0),nEntitySlotFlagsAdd(0),pSrcStaticRenderNode(0), overrideEntityClass(NULL) { slotTM.SetIdentity(); worldTM.SetIdentity(); };
	};
	virtual void BreakIntoPieces( IEntity *pEntity, int nSlot, int nPiecesSlot, BreakageParams const& Breakage ) = 0;

	// Summary:
	//	 Attaches the effect & params specified by material of object in slot.
	virtual void AttachSurfaceEffect( IEntity* pEntity, int nSlot, const char* sType, SpawnParams const& paramsIn, uint uEmitterFlags = 0 ) = 0;

	// Summary:
	//	 Checks if static object can be shattered, by checking it`s surface types.
	virtual bool CanShatter( IStatObj *pStatObj ) = 0;

	// Summary:
	//	 Checks if entity can be shattered, by checking surface types of geometry or character.
	virtual bool CanShatterEntity( IEntity *pEntity,int nSlot=-1 ) = 0;

	virtual void FakePhysicsEvent( EventPhys * pEvent ) = 0;

	virtual IEntity* CreateObjectAsEntity( IStatObj *pStatObj,IPhysicalEntity *pPhysEnt, IPhysicalEntity *pSrcPhysEnt, IBreakableManager::SCreateParams &createParams, bool bCreateSubstProxy=false ) = 0;

	// Summary:
	//	 Adds a break event listener
	virtual void AddBreakEventListener(IBreakEventListener * pListener) = 0;

	// Summary:
	//	 Removes a break event listener
	virtual void RemoveBreakEventListener(IBreakEventListener * pListener) = 0;

	// Summary:
	//	 Replays a RemoveSubPartsEvent
	virtual void ReplayRemoveSubPartsEvent( const EventPhysRemoveEntityParts *pRemoveEvent ) = 0;

	// Summary:
	//	 Records that there has been a call to CEntity::DrawSlot() for later playback
	virtual void EntityDrawSlot(CEntity * pEntity, int32 slot, int32 flags) = 0;

	// Summary:
	//	 Resets broken objects.
	virtual void ResetBrokenObjects() = 0;

	// Summary:
	//		Returns a vector of broken object records
	virtual const IBreakableManager::SBrokenObjRec * GetPartBrokenObjects(int& brokenObjectCount) = 0;

	// Summary:
	//		
	virtual void GetBrokenObjectIndicesForCloning(int32 * pPartRemovalIndices, int32& iNumPartRemovalIndices,
																								int32 * pOutIndiciesForCloning, int32& iNumEntitiesForCloning,
																								const EventPhysRemoveEntityParts * BreakEvents) = 0;

	virtual void ClonePartRemovedEntitiesByIndex(	int32 * pBrokenObjectIndices, int32 iNumBrokenObjectIndices,
																								EntityId * pOutClonedEntities, int32& iNumClonedBrokenEntities,
																								const EntityId * pRecordingEntities, int32 iNumRecordingEntities,
																								SRenderNodeCloneLookup& nodeLookup) = 0;


	virtual void HideBrokenObjectsByIndex( const int32 * pBrokenObjectIndices, const int32 iNumBrokenObjectIndices) = 0;
	virtual void UnhidePartRemovedObjectsByIndex( const int32 * pPartRemovalIndices, const int32 iNumPartRemovalIndices, const EventPhysRemoveEntityParts * BreakEvents) = 0;
	virtual void ApplySinglePartRemovalFromEventIndex(int32 iPartRemovalEventIndex, const SRenderNodeCloneLookup& renderNodeLookup, const EventPhysRemoveEntityParts * pBreakEvents) = 0;
	virtual void ApplyPartRemovalsUntilIndexToObjectList( int32 iFirstEventIndex, const SRenderNodeCloneLookup& renderNodeLookup, const EventPhysRemoveEntityParts * pBreakEvents) = 0;

	virtual struct ISurfaceType* GetFirstSurfaceType( IStatObj *pStatObj ) = 0;
	virtual struct ISurfaceType* GetFirstSurfaceType( ICharacterInstance *pCharacter ) = 0;
};

// Summary:
//	 Structure used by proximity query in entity system.
struct SEntityProximityQuery
{
	AABB box; // Report entities within this bounding box.
	IEntityClass *pEntityClass;
	uint32 nEntityFlags;

	// Output.
	IEntity** pEntities;
	int nCount;

	SEntityProximityQuery()
	{
		nCount = 0;
		pEntities = 0;
		pEntityClass = 0;
		nEntityFlags = 0;
	}
};

// removed unused ifdef for devirtualization
// #ifndef _NO_IENTITY
// UNIQUE_IFACE struct IEntitySystem
// #else
// struct IEntitySystemBase
// #endif

// Description:
//	 Interface to the system that manages the entities in the game, their creation, deletion and upkeep. The entities are kept in a map
//	 indexed by their unique entity ID. The entity system updates only unbound entities every frame (bound entities are updated by their
//	 parent entities), and deletes the entities marked as garbage every frame before the update. The entity system also keeps track of entities
//	 that have to be drawn last and with more zbuffer resolution.
// Summary:
//	 Interface to the system that manages the entities in the game.
UNIQUE_IFACE struct IEntitySystem
{
	virtual ~IEntitySystem(){}

	// Summary:
	//	 Releases entity system.
	virtual void Release() = 0;

	// Description:
	//   Updates entity system and all entities before physics is called. (or as early as possible after input in the multithreaded physics case)
	//	 This function executes once per frame.
	virtual void PrePhysicsUpdate() = 0;

	// Summary:
	//	 Updates entity system and all entities. This function executes once a frame.
	virtual	void	Update() = 0;

	// Summary:
	//	 Resets whole entity system, and destroy all entities.
	virtual void	Reset() = 0;

	// Summary:
	//	 Unloads whole entity system - should be called when level is unloaded.
	virtual void Unload() = 0;

	// Description:
	//     Deletes any pending entities (which got marked for deletion).
	virtual void  DeletePendingEntities() = 0;


	// Description:
	//     Retrieves the entity class registry interface.
	// Return:
	//     Pointer to the valid entity class registry interface.
	virtual IEntityClassRegistry* GetClassRegistry() = 0;

	// Summary:
	//	 Spawns a new entity according to the data in the Entity Descriptor.
	// Arguments:
	//	 params		- Entity descriptor structure that describes what kind of entity needs to be spawned
	//	 bAutoInit	- If true automatically initialize entity.
	// Return Value:
	//	 The spawned entity if successful, NULL if not.
	// See Also: 
	//	 CEntityDesc
	virtual IEntity* SpawnEntity( SEntitySpawnParams &params,bool bAutoInit=true ) = 0;

	// Summary: 
	//	 Initializes entity.
	// Description:
	//	 Initialize entity if entity was spawned not initialized (with bAutoInit false in SpawnEntity).
	// Note:
	//	 Used only by Editor, to setup properties & other things before initializing entity,
	//	 do not use this directly.
	// Arguments:	
	//	 pEntity - Pointer to just spawned entity object.
	//	 params	 - Entity descriptor structure that describes what kind of entity needs to be spawned.
	// Return value:	
	//	 True if successfully initialized entity.
	virtual bool InitEntity( IEntity* pEntity,SEntitySpawnParams &params ) = 0;
	
	// Summary:
	//	 Retrieves entity from its unique id.
	// Arguments:
	//	 id - The unique ID of the entity required.
	// Return value:
	//	 The entity if one with such an ID exists, and NULL if no entity could be matched with the id
    virtual IEntity* GetEntity( EntityId id ) const = 0;

	// Summary:
	//	 Find first entity with given name.
	// Arguments:
	//	 sEntityName - The name to look for.
	// Return value:
	//	 The entity if found, 0 if failed.
	virtual IEntity* FindEntityByName( const char *sEntityName ) const = 0;

	// Note:
	//	 Might be needed to call before loading of entities to be sure we get the requested IDs.
	// Arguments:
	//   id - must not be 0.
	virtual void ReserveEntityId( const EntityId id ) = 0;
	
	// Summary:
	//	 Removes an entity by ID.
	// Arguments:
	//	 entity			 - The id of the entity to be removed.
	//	 bForceRemoveNow - If true forces immediately delete of entity, overwise will delete entity on next update.
	virtual void	RemoveEntity( EntityId entity,bool bForceRemoveNow=false ) = 0;

	// Summary:
	//	 Gets number of entities stored in entity system.
	// Return value:
	//	 The number of entities.
	virtual uint32 GetNumEntities() const = 0;

	// Summary:
	//	 Gets a entity iterator. 
	// Description: 
	//	 Gets a entity iterator. This iterator interface can be used to traverse all the entities in this entity system.
	// Return value:
	//	 An entityIterator.
	// See also:
	//	 IEntityIt
	virtual IEntityIt * GetEntityIterator() = 0;

	// Description:
	//    Sends the same event to all entities in Entity System.
	// Arguments:
	//    event - Event to send.
	virtual void SendEventToAll( SEntityEvent &event ) = 0;

	// Summary: 
	//	 Get all entities within proximity of the specified bounding box.
	// Note:
	//	 Query is not exact, entities reported can be a few meters away from the bounding box.
	virtual int QueryProximity( SEntityProximityQuery &query ) = 0;

	// Summary:
	//	 Resizes the proximity grid.
	// Note: 
	//	 Call this when you know dimensions of the level, before entities are created.
	virtual void ResizeProximityGrid( int nWidth,int nHeight ) = 0;

	
	// Summary:
	//	 Gets all entities in specified radius.
	// Arguments:
	//	 origin		-
	//	 radius		-
	//	 pList		-
	//	 physFlags	- is one or more of PhysicalEntityFlag.
	// See also:
	//	 PhysicalEntityFlag
	virtual int	GetPhysicalEntitiesInBox( const Vec3 &origin, float radius, IPhysicalEntity **&pList, int physFlags = (1<<1)|(1<<2)|(1<<3)|(1<<4) ) const = 0;

	// Description:
	//   Retrieves host entity from the physical entity.
	virtual IEntity* GetEntityFromPhysics( IPhysicalEntity *pPhysEntity ) const = 0;

	enum SinkEventSubscriptions
	{
		OnBeforeSpawn = BIT(0),
		OnSpawn				= BIT(1),
		OnRemove			= BIT(2),
		OnReused			= BIT(3),
		OnEvent				= BIT(4),

		AllSinkEvents	= ~0ul,
	};

	enum
	{
		SinkMaxEventSubscriptionCount = 5,
	};

	// Description:
	//	 Adds the sink of the entity system. The sink is a class which implements IEntitySystemSink.
	// Arguments:
	//	 sink - Pointer to the sink, must not be 0.
	//	 subscription - combination of SinkEventSubscriptions flags specifying which events to receive
	// See also:
	//	 IEntitySystemSink
	virtual void AddSink( IEntitySystemSink *sink, uint32 subscriptions, uint64 onEventSubscriptions ) = 0;

	// Description:
	//	 Removes listening sink from the entity system. The sink is a class which implements IEntitySystemSink.
	// Arguments:
	//	 sink - Pointer to the sink, must not be 0.
	// See also:
	//	 IEntitySystemSink
	virtual void RemoveSink( IEntitySystemSink *sink ) = 0;

	// Description:
	//    Pause all entity timers.
	// Arguments:
	//    bPause - true to pause timer, false to resume.
	virtual void	PauseTimers( bool bPause,bool bResume=false ) = 0;

	// Summary:
	//	 Checks whether a given entity ID is already used.
	virtual bool IsIDUsed( EntityId nID ) const = 0;

	// Description:
	//	 Puts the memory statistics of the entities into the given sizer object
	//	 according to the specifications in interface ICrySizer.
	virtual void GetMemoryStatistics(ICrySizer *pSizer) const = 0;

	// Summary:
	//	 Gets pointer to original ISystem.
	virtual ISystem* GetSystem() const = 0;

	// Description:
	//    Loads entities exported from Editor.
	virtual void LoadEntities( XmlNodeRef &objectsNode ) = 0;

	// Summary:
	//	  Registers Entity Event`s listeners.
	virtual void AddEntityEventListener( EntityId nEntity,EEntityEvent event,IEntityEventListener *pListener ) = 0;
	virtual void RemoveEntityEventListener( EntityId nEntity,EEntityEvent event,IEntityEventListener *pListener ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity GUIDs
	//////////////////////////////////////////////////////////////////////////

	// Summary:
	//	 Finds entity by Entity GUID.
	virtual EntityId FindEntityByGuid( const EntityGUID &guid ) const = 0;

	// Summary:
	//	 Gets a pointer to access to area manager.
	virtual IAreaManager* GetAreaManager() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//    Return the breakable manager interface.
	virtual IBreakableManager* GetBreakableManager() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//	 Returns the entity pool manager interface.
	virtual IEntityPoolManager* GetIEntityPoolManager() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity archetypes.
	//////////////////////////////////////////////////////////////////////////
	virtual IEntityArchetype* LoadEntityArchetype( XmlNodeRef oArchetype) = 0;
	virtual IEntityArchetype* LoadEntityArchetype( const char *sArchetype ) = 0;
	virtual IEntityArchetype* CreateEntityArchetype( IEntityClass *pClass,const char *sArchetype ) = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Serializes basic entity system members (timers etc. ) to/from a savegame;
	virtual void Serialize(TSerialize ser) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Makes sure the next SpawnEntity will use the id provided (if it's in use, the current entity is deleted).
	virtual void SetNextSpawnId(EntityId id) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Resets any area state for the specified entity.
	virtual void ResetAreas() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Unloads any area state for the specified entity.
	virtual void UnloadAreas() = 0;

  //////////////////////////////////////////////////////////////////////////
  // Description:
  //     Dumps entities in system.
  virtual void DumpEntities() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Do not spawn any entities unless forced to.
	virtual void LockSpawning(bool lock) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Handles entity-related loading of a level
	virtual bool OnLoadLevel(const char* szLevelPath) = 0;

	// Add entity layer
	virtual void AddLayer(const char* name, const char* parent, uint16 id, bool bHasPhysics, bool bDefaultLoaded) = 0;

	// Add entity to entity layer
	virtual void AddEntityToLayer(const char* layer, EntityId id) = 0;

	// Remove entity from all layers
	virtual void RemoveEntityFromLayers(EntityId id) = 0;

	// Clear list of entity layers
	virtual void ClearLayers() = 0;

	// Enable all the default layers
	virtual void EnableDefaultLayers(bool isSerialized = true) = 0;

	// Enable entity layer
	virtual void EnableLayer(const char* layer, bool isEnable, bool isSerialized = true) = 0;

	// Returns true if entity is not in a layer or the layer is enabled/serialized
	virtual bool ShouldSerializedEntity(IEntity* pEntity) = 0;

	// Register callbacks from Physics System
	virtual void RegisterPhysicCallbacks() = 0;
	virtual void UnregisterPhysicCallbacks() = 0;

	virtual void PurgeDeferredCollisionEvents( bool bForce = false ) =0;

	virtual void DebugDraw() = 0;
};

extern "C"
{
	CRYENTITYDLL_API struct IEntitySystem* CreateEntitySystem( ISystem *pISystem );
}

typedef struct IEntitySystem * (* PFNCREATEENTITYSYSTEM)( ISystem *pISystem );

#endif // __IEntitySystem_h__
