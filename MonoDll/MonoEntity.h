/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Entity class to forward events to C#
//////////////////////////////////////////////////////////////////////////
// ??/??/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ENTITY__
#define __MONO_ENTITY__

#include <IAnimatedCharacter.h>

#include <IEntityClass.h>

#include "EntityManager.h"

struct SQueuedProperty
{
	SQueuedProperty(IEntityPropertyHandler::SPropertyInfo propInfo, const char *val)
		: propertyInfo(propInfo)
		, value(ToMonoString(val)) {}

	mono::string value;
	IEntityPropertyHandler::SPropertyInfo propertyInfo;
};

class CEntity : public IEntityEventListener
{
public:
	CEntity(SEntitySpawnParams &spawnParams);
	~CEntity();

	// IEntityEventListener
	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event );
	// ~IEntityEventListener

	void SetPropertyValue(IEntityPropertyHandler::SPropertyInfo propertyInfo, const char *value);
	// Register game object after using IGameObjectSystem::CreateGameObjectForEntity.
	void RegisterGameObject(IGameObject *pGameObject) { m_pGameObject = pGameObject; }

	void OnSpawn(IEntity *pEntity, SEntitySpawnParams &spawnParams);

	IGameObject *GetGameObject() { return m_pGameObject; }

	EntityId GetEntityId() { return m_entityId; }
	EntityGUID GetEntityGUID() { return m_entityGUID; }

	IMonoClass *GetScript() { return m_pScriptClass; }

	bool IsSpawned() { return m_pScriptClass != NULL; }

protected:
	EntityId m_entityId;
	EntityGUID m_entityGUID;

	IMonoClass *m_pScriptClass;

	IGameObject *m_pGameObject;

	// If we haven't spawned yet, we have to store these and call them on spawn
	std::list<SQueuedProperty> m_propertyQueue;
};

#endif