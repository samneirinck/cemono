#ifndef __MONO_ENTITY__
#define __MONO_ENTITY__

#include <IAnimatedCharacter.h>

#include <IEntitySystem.h>

#include "EntityManager.h"

class CMonoEntity : public IEntityEventListener
{
public:
	CMonoEntity(int scriptId);
	~CMonoEntity();

	void OnSpawn(EntityId id);

	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event );

	// Register game object after using IGameObjectSystem::CreateGameObjectForEntity.
	void RegisterGameObject(IGameObject *pGameObject) { m_pGameObject = pGameObject; }

	void AddMovement(const MovementRequest &request);

	IGameObject *GetGameObject() { return m_pGameObject; }

	EntityId GetEntityId() { return m_entityId; }
	int GetScriptId() { return m_scriptId; }

protected:
	EntityId m_entityId;
	int m_scriptId;

	IGameObject *m_pGameObject;
	IAnimatedCharacter *m_pAnimatedCharacter;
};

#endif