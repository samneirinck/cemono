/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// The entity manager handles spawning, removing and storing of mono
// entities.
//////////////////////////////////////////////////////////////////////////
// 21/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "MonoCommon.h"
#include <IMonoArray.h>

#include <IAnimatedCharacter.h>
#include <IMonoEntityManager.h>
#include <IMonoScriptBind.h>

#include <mono\mini\jit.h>
#include <IEntitySystem.h>

struct IMonoScript;
class CMonoEntity;

struct MovementRequest
{
	ECharacterMoveType type;

	Vec3 velocity;
};

struct EntitySpawnParams
{
	mono::string sName;
	mono::string sClass;

	MonoObject *pos;
	MonoObject *rot;
	MonoObject *scale;

	EEntityFlags flags;

	SEntitySpawnParams Convert()
	{
		SEntitySpawnParams params;

		params.sName = ToCryString(sName);
		params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(ToCryString(sClass));

		//params.vPosition = (Vec3)mono_object_unbox(pos);
		//params.qRotation = Quat((Ang3)mono_object_unbox(rot));
		//params.vScale = (Vec3)mono_object_unbox(scale);

		params.nFlags = flags;

		return params;
	}
};

struct EntityRegisterParams
{
	mono::string Name;
	mono::string Category;

	mono::string EditorHelper;
	mono::string EditorIcon;

	EEntityClassFlags Flags;
};

struct SMonoEntityProperty
{
	mono::string name;
	mono::string description;
	mono::string editType;

	IEntityPropertyHandler::EPropertyType type;
	uint32 flags;

	IEntityPropertyHandler::SPropertyInfo::SLimits limits;
};

class CEntityManager 
	: public IMonoEntityManager
	, public IMonoScriptBind
	, public IEntitySystemSink
{

public:
	CEntityManager();
	~CEntityManager();

	// IEntitySystemSink
	virtual bool OnBeforeSpawn( SEntitySpawnParams &params );
	virtual void OnSpawn( IEntity *pEntity,SEntitySpawnParams &params );
	virtual bool OnRemove( IEntity *pEntity );
	virtual void OnReused( IEntity *pEntity, SEntitySpawnParams &params ) {}
	virtual void OnEvent( IEntity *pEntity, SEntityEvent &event ) {}
	// ~IEntitySystemSink

	CMonoEntity *GetEntity(EntityId entityId);
	int GetScriptId(EntityId entityId, bool returnBackIfInvalid = false) override;
	bool IsMonoEntity(const char *entityClassName);

	// IMonoScriptBind
	virtual void Release() override { if( 0 >= --m_refs) delete this; }
	virtual void AddRef() override { ++m_refs; }
protected:

	virtual const char* GetClassName() { return "EntitySystem"; }
	// ~IMonoScriptBind

	// ScriptBinds
	ExposedMonoMethod(EntityId, SpawnEntity, EntitySpawnParams &, bool);
	MonoMethod(bool, RegisterEntityClass, EntityRegisterParams, mono::array);

	MonoMethod(EntityId, FindEntity, mono::string);
	MonoMethod(mono::array, GetEntitiesByClass, mono::string);
	/// End direct entity calls

	// ~ScriptBinds
	
	typedef std::vector<CMonoEntity *> TMonoEntities;
	TMonoEntities m_monoEntities;

	static std::vector<const char *> m_monoEntityClasses;

	int m_refs;
};

#endif //__ENTITY_MANAGER_H__