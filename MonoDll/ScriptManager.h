/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// ScriptManager class used to handle all Mono scripts.
// Used as a game object to allow support for RMI's and proper updates.
//////////////////////////////////////////////////////////////////////////
// 25/02/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/


#include <IMonoScriptManager.h>
#include <MonoCommon.h>

struct IMonoClass;
struct IMonoArray;

class CScriptManager
	: public CGameObjectExtensionHelper <CScriptManager, IMonoScriptManager>
{

	typedef std::map<IMonoClass *, int> TScripts;
public:
	CScriptManager();
	~CScriptManager();

	void CompileScripts();

	// IGameObjectExtension
	virtual bool Init(IGameObject * pGameObject);
	virtual void PostInit( IGameObject * pGameObject ) {}
	virtual void InitClient(int channelId) {}
	virtual void PostInitClient(int channelId) {}
	
	virtual bool ReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) { return true; }
	virtual void PostReloadExtension( IGameObject * pGameObject, const SEntitySpawnParams &params ) {}

	virtual bool GetEntityPoolSignature( TSerialize signature ) { return false; }

	virtual void Release() { delete this; }

	virtual void FullSerialize( TSerialize ser ) {}
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) { return true; }
	virtual void PostSerialize() {}

	virtual void SerializeSpawnInfo( TSerialize ser ) {}
	virtual ISerializableInfoPtr GetSpawnInfo() { return 0; }

	virtual void Update( SEntityUpdateContext& ctx, int updateSlot );
	virtual void PostUpdate( float frameTime );

	virtual void HandleEvent( const SGameObjectEvent& event ) {}

	virtual void ProcessEvent( SEntityEvent& event ) {}	

	virtual void SetChannelId(uint16 id) {}
	virtual void SetAuthority( bool auth ) {}

	virtual void PostRemoteSpawn() {}

	virtual void GetMemoryUsage(ICrySizer *pSizer ) const
	{
		pSizer->Add(*this);
	}
	// ~IGameObjectExtension

	// IMonoScriptManager
	virtual int InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters = nullptr) override;
	virtual IMonoClass *GetScriptById(int id) override;
	virtual void RemoveScriptInstance(int id) override;
	// ~IMonoScriptManager

	IMonoClass *GetManagedManager() const { return m_pScriptManager; }

protected:
	static void CmdDumpMonoState(IConsoleCmdArgs *cmdArgs);

	// The app domain in which we load scripts into. Killed and reloaded on script reload.
	MonoDomain *m_pScriptDomain;

	IMonoClass *m_pScriptManager;
	IMonoClass *m_AppDomainSerializer;

	TScripts m_scripts;
};

