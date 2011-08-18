#pragma once
#include <IScriptSystem.h>
#include <IGameObject.h>
#include "Mono.h"

class CMonoEntity : public CGameObjectExtensionHelper<CMonoEntity, IGameObjectExtension>
{
public:
	CMonoEntity();
	CMonoEntity(MonoClass *pClass, MonoImage *pImage);
	virtual ~CMonoEntity();

	// IGameObjectExtension
	virtual bool Init(IGameObject *pGameObject);
	virtual void InitClient(int channelId);
	virtual void PostInit(IGameObject *pGameObject);
	virtual void PostInitClient(int channelId);
	virtual void Release();
	virtual void FullSerialize( TSerialize ser );
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags );
	virtual void PostSerialize();
	virtual void SerializeSpawnInfo( TSerialize ser );
	virtual ISerializableInfoPtr GetSpawnInfo();
	virtual void Update( SEntityUpdateContext &ctx, int updateSlot);
	virtual void PostUpdate(float frameTime );
	virtual void PostRemoteSpawn();
	virtual void HandleEvent( const SGameObjectEvent &);
	virtual void ProcessEvent(SEntityEvent &);
	virtual void SetChannelId(uint16 id);
	virtual void SetAuthority(bool auth);
	virtual void GetMemoryStatistics(ICrySizer * s);
	//~IGameObjectExtension

	ILINE MonoObject* GetMonoObject() { return m_pMonoObject; }
	void MonoEventFunction(const char* funcName, void** args);


	static CMonoEntity* GetById(EntityId id);

protected:
	MonoObject* m_pMonoObject;
	MonoClass* m_pClass;
	MonoImage* m_pImage;
	static std::vector<CMonoEntity*> m_monoEntities;


};

