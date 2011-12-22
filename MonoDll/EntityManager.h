#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "MonoString.h"

#include <IMonoEntityManager.h>
#include <IMonoScriptBind.h>

#include <mono\mini\jit.h>
#include <IEntitySystem.h>

struct IMonoScript;

struct SMonoEntitySpawnParams
{
	MonoString *sName;
	MonoString *sClass;

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

struct SMonoEntityRegisterParams
{
	MonoString *Name;
	MonoString *Category;

	MonoString *EditorHelper;
	MonoString *EditorIcon;

	EEntityClassFlags Flags;
};

struct SMonoEntityProperty
{
	MonoString *name;
	MonoString *description;
	MonoString *editType;

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
	virtual bool OnBeforeSpawn( SEntitySpawnParams &params ) { return true; }
	virtual void OnSpawn( IEntity *pEntity,SEntitySpawnParams &params );
	virtual bool OnRemove( IEntity *pEntity );
	virtual void OnReused( IEntity *pEntity, SEntitySpawnParams &params ) {}
	virtual void OnEvent( IEntity *pEntity, SEntityEvent &event ) {}
	// ~IEntitySystemSink

	int GetScriptId(EntityId entityId);
	bool IsMonoEntity(const char *entityClassName);

protected:
	// IMonoScriptBind
	virtual const char* GetClassName() { return "EntitySystem"; }
	// ~IMonoScriptBind

	// ScriptBinds
	static EntityId SpawnEntity(SMonoEntitySpawnParams &params, bool bAutoInit = true);
	static bool RegisterEntityClass(SMonoEntityRegisterParams params, MonoArray *properties);

	static MonoString *GetPropertyValue(EntityId entityId, MonoString *propertyName);
	static void SetPropertyValue(EntityId entityId, MonoString *propertyName, MonoString *value);

	static EntityId FindEntity(MonoString *name);
	static MonoArray *GetEntitiesByClass(MonoString *_class);

	/// The scriptbinds below are called directly from Entity scripts.
	static void SetWorldPos(EntityId id, Vec3 newPos);
	static Vec3 GetWorldPos(EntityId id);
	static void SetWorldAngles(EntityId id, Vec3 newAngles);
	static Vec3 GetWorldAngles(EntityId id);
	/// End direct entity calls

	// ~ScriptBinds
	
	typedef std::map<int, EntityId> TMonoEntityScripts;
	TMonoEntityScripts m_entityScripts;

	static std::vector<const char *> m_monoEntityClasses;
};

#endif //__ENTITY_MANAGER_H__