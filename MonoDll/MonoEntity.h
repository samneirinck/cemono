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

#include <IGameObject.h>
#include <IMonoClass.h>

struct SQueuedProperty
{
	SQueuedProperty() {}

	SQueuedProperty(IEntityPropertyHandler::SPropertyInfo propInfo, const char *val)
		: propertyInfo(propInfo)
		, value(string(val)) {}

	string value;
	IEntityPropertyHandler::SPropertyInfo propertyInfo;
};

class CEntity
	: public CGameObjectExtensionHelper<CEntity, IGameObjectExtension>
	, public IEntityProxy
{
public:
	CEntity();
	virtual ~CEntity();

	// IGameObjectExtension
	virtual bool Init(IGameObject *pGameObject);
	virtual void InitClient( int channelId ) {}
	virtual void PostInit(IGameObject *pGameObject) {}
	virtual void PostInitClient( int channelId ) {}
	virtual bool ReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) { return false; }
	virtual void PostReloadExtension( IGameObject* pGameObject, const SEntitySpawnParams& params ) {}
	virtual bool GetEntityPoolSignature( TSerialize signature ) { return false; }
	virtual void Release() { delete this; }
	virtual void FullSerialize( TSerialize ser ) {}
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags ) { return true; }
	virtual void PostSerialize() {}
	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() { return 0; }
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot ) {}
	virtual void PostUpdate( float frameTime ) {}
	virtual void PostRemoteSpawn() {}
	virtual void HandleEvent( const SGameObjectEvent& event ) {}
	virtual void ProcessEvent( SEntityEvent& event );
	virtual void SetChannelId( uint16 id ) {}
	virtual void SetAuthority( bool auth ) {}
	virtual void GetMemoryUsage( ICrySizer* s ) const { s->Add( *this ); }
	// ~IGameObjectExtension

	// IEntityProxy
	virtual EEntityProxy GetType() { return ENTITY_PROXY_USER; };
	virtual bool Init(IEntity *pEntity, SEntitySpawnParams &spawnParams) { return true; }
	virtual void Reload(IEntity *pEntity, SEntitySpawnParams &params) {}
	virtual void Done() {}
	virtual	void Update(SEntityUpdateContext &ctx) {}
	virtual void SerializeXML(XmlNodeRef &entityNode, bool loading) {}
	virtual void Serialize(TSerialize ser) {}
	virtual bool NeedSerialize() { return true; }
	virtual bool GetSignature(TSerialize signature) { return true; }
	// ~IEntityProxy

	IMonoClass *GetScript() { return m_pScriptClass; }

	void SetPropertyValue(IEntityPropertyHandler::SPropertyInfo propertyInfo, const char *value);

	bool IsInitialized() { return m_bInitialized; }

protected:
	IMonoClass *m_pScriptClass;

	bool m_bInitialized;
};

#endif