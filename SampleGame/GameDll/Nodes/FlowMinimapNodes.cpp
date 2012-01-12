#include "StdAfx.h"
#include "Nodes/G2FlowBaseNode.h"

#include <IGameFramework.h>
#include <ILevelSystem.h>
#include <IEntitySystem.h>


//---------------------------------------------------------------------------------------
//------------------------------------- MinimapInfo -------------------------------------
//---------------------------------------------------------------------------------------
class CMiniMapInfo
{
public:
	static CMiniMapInfo* GetInstance()
	{
		static CMiniMapInfo inst;
		return &inst;
	}

	struct LevelMapInfo {
		LevelMapInfo() : fStartX(0), fStartY(0), fEndX(1), fEndY(1), fDimX(1), fDimY(1), iWidth(1024), iHeight(1024) {}
		string sMinimapName;
		int iWidth;
		int iHeight;
		float fStartX;
		float fStartY;
		float fEndX;
		float fEndY;
		float fDimX;
		float fDimY;
	};

	const LevelMapInfo& GetLevelInfo() { UpdateLevelInfo(); return m_LevelMapInfo; }

	void GetPlayerData( IEntity* pEntity, float& px, float &py, int& rot ) const
	{
		if (pEntity)
		{
			px = clamp( ( pEntity->GetWorldPos().x - m_LevelMapInfo.fStartX ) / m_LevelMapInfo.fDimX, 0, 1 );
			py = clamp( ( pEntity->GetWorldPos().y - m_LevelMapInfo.fStartY ) / m_LevelMapInfo.fDimY, 0, 1 );
			rot = (int) ( pEntity->GetWorldAngles().z * 180.0f/gf_PI - 90.0f );
		}
	}

	void UpdateLevelInfo()
	{
		ILevel* pLevel = gEnv->pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel();
		if ( pLevel != NULL && pLevel->GetLevelInfo() != NULL )
		{
			IXmlParser*	pxml = gEnv->pGame->GetIGameFramework()->GetISystem()->GetXmlUtils()->CreateXmlParser();
			if( !pxml ) return;

			char tmp[255];
			sprintf( tmp,"%s/%s.xml", pLevel->GetLevelInfo()->GetPath(), pLevel->GetLevelInfo()->GetName() );
			XmlNodeRef node = GetISystem()->LoadXmlFromFile( tmp );
			if( !node ) return;

			node = node->findChild( "Minimap" );
			if ( node ) 
			{
				const char* minimap_dds;
				node->getAttr( "Filename", &minimap_dds );
				sprintf( tmp,"%s/%s", pLevel->GetLevelInfo()->GetPath(), minimap_dds );

				node->getAttr( "startX", m_LevelMapInfo.fStartX );
				node->getAttr( "startY", m_LevelMapInfo.fStartY );
				node->getAttr( "endX", m_LevelMapInfo.fEndX );
				node->getAttr( "endY", m_LevelMapInfo.fEndY );
				node->getAttr( "width", m_LevelMapInfo.iWidth );
				node->getAttr( "height", m_LevelMapInfo.iHeight );
				m_LevelMapInfo.fDimX = m_LevelMapInfo.fEndX - m_LevelMapInfo.fStartX;
				m_LevelMapInfo.fDimY = m_LevelMapInfo.fEndY - m_LevelMapInfo.fStartY;
				m_LevelMapInfo.fDimX = m_LevelMapInfo.fDimX > 0 ? m_LevelMapInfo.fDimX : 1;
				m_LevelMapInfo.fDimY = m_LevelMapInfo.fDimY > 0 ? m_LevelMapInfo.fDimY : 1;
				m_LevelMapInfo.sMinimapName = tmp;
			}
		}
	}

private:
	CMiniMapInfo() {}
	CMiniMapInfo( const CMiniMapInfo& ) {}
	CMiniMapInfo& operator=( const CMiniMapInfo& ) {}
	~CMiniMapInfo() {}

	LevelMapInfo m_LevelMapInfo;
};

//----------------------------------- MiniMap info node ---------------------------------
class CFlowMiniMapInfoNode 
	: public CFlowBaseNode
{
public:
	CFlowMiniMapInfoNode( SActivationInfo * pActInfo )
	{
	}

	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig inputs[] = {
			InputPortConfig_Void  ( "Get", _HELP("Get minimap info") ),
			{0}
		};
		static const SOutputPortConfig outputs[] = {
			OutputPortConfig_Void		( "OnGet",	_HELP( "Tirggers of port <Get> is activeated" ) ),
			OutputPortConfig<string>( "MapFile",	_HELP( "Name of minimap dds file" ) ),
			OutputPortConfig<int>		( "Width",		_HELP( "Minimap width" ) ),
			OutputPortConfig<int>		( "Height",		_HELP( "Minimap height" ) ),
			{0}
		};
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP( "Info about minimap" );
		config.SetCategory( EFLN_ADVANCED );
	}

	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		if (event == eFE_Activate && IsPortActive( pActInfo, eI_Get ))
		{
			const CMiniMapInfo::LevelMapInfo mapInfo = CMiniMapInfo::GetInstance()->GetLevelInfo();
			ActivateOutput( pActInfo, eO_OnGet,   true );
			ActivateOutput( pActInfo, eO_MapName,   mapInfo.sMinimapName );
			ActivateOutput( pActInfo, eO_MapWidth,  mapInfo.iWidth );
			ActivateOutput( pActInfo, eO_MapHeight, mapInfo.iHeight );
		}
	}

	virtual void GetMemoryUsage( ICrySizer * s ) const
	{
		s->Add( *this );
	}

private:
	enum EInputPorts
	{
		eI_Get,
	};

	enum EOutputPorts
	{
		eO_OnGet = 0,
		eO_MapName,
		eO_MapWidth,
		eO_MapHeight,
	};

};



//----------------------------------- Entity pos info --------------------------------------
class CFlowMiniMapEntityPosInfo
	: public CFlowBaseNode
	, public IEntityEventListener
{
public:
	CFlowMiniMapEntityPosInfo( SActivationInfo * pActInfo )
		: m_entityId(0)
	{
	}

	virtual ~CFlowMiniMapEntityPosInfo()
	{
	}

	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		static const SInputPortConfig inputs[] = 
		{
			InputPortConfig_Void("Get", _HELP("Trigger outputs.")),
			InputPortConfig_Void("Enable", _HELP("Trigger to enable")),
			InputPortConfig_Void("Disable", _HELP("Trigger to enable")),
			{0}
		};
		static const SOutputPortConfig outputs[] = {
			OutputPortConfig_Void	( "OnPosChange",_HELP( "Triggers if position has changed" ) ),
			OutputPortConfig<float>	("PosX",		_HELP( "X pos on minimap" ) ),
			OutputPortConfig<float>	("PosY",		_HELP( "Y pos on minimap" ) ),
			OutputPortConfig<int>	  ("Rotation",	_HELP( "Minimap rotation" ) ),
			{0}
		};

		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.SetCategory(EFLN_ADVANCED);

		SetFlagsAndDescriton(config);
	}

	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo )
	{
		switch(event)
		{
		case eFE_Initialize:
			m_actInfo = *pActInfo;
			CMiniMapInfo::GetInstance()->UpdateLevelInfo();
			m_entityId = GetEntityId(pActInfo);
			break;
		case eFE_Activate:
			if (IsPortActive(pActInfo, eI_Trigger))
				TriggerPorts(pActInfo);

			if (IsPortActive(pActInfo, eI_Disable))
				UnRegisterEntity();

			if (IsPortActive(pActInfo, eI_Enable))
			{
				UnRegisterEntity();
				m_entityId = GetEntityId(pActInfo);
				RegisterEntity();
			}
			break;
		}
	}

	virtual void GetMemoryUsage( ICrySizer * s ) const
	{
		s->Add( *this );
	}

	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event )
	{
		assert(pEntity->GetId() == m_entityId);
		if (event.event == ENTITY_EVENT_XFORM)
		{
			float px,py;
			int r;
			CMiniMapInfo::GetInstance()->GetPlayerData(pEntity, px, py, r);
			ActivateOutput( &m_actInfo, eO_OnPosChange, true);
			ActivateOutput( &m_actInfo, eO_PosX, px );
			ActivateOutput( &m_actInfo, eO_PosY, py );
			ActivateOutput( &m_actInfo, eO_Rotation, r );
		}
	}

protected:
	virtual EntityId GetEntityId( SActivationInfo *pActInfo )
	{
		return pActInfo->pEntity != 0 ? pActInfo->pEntity->GetId() : 0;
	}

	virtual void SetFlagsAndDescriton( SFlowNodeConfig& config )
	{
		config.nFlags |= EFLN_TARGET_ENTITY;
		config.sDescription = _HELP( "Info about entity position on minimap" );
	}

private:
	void RegisterEntity()
	{
		if (m_entityId == 0)
			return;
		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(m_entityId);
		if (pEntity != 0)
		{
			gEnv->pEntitySystem->AddEntityEventListener(m_entityId, ENTITY_EVENT_XFORM, this);
			return;
		}
		m_entityId = 0;
	}

	void UnRegisterEntity()
	{
		if (m_entityId == 0)
			return;

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(m_entityId);
		if (pEntity != 0)
		{
			gEnv->pEntitySystem->RemoveEntityEventListener(m_entityId, ENTITY_EVENT_XFORM, this);
			return;
		}
		m_entityId = 0;
	}

	void TriggerPorts( SActivationInfo *pActInfo )
	{
		if (m_entityId == 0)
			return;

		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(m_entityId);
		if (pEntity)
		{
			float px,py;
			int r;
			CMiniMapInfo::GetInstance()->GetPlayerData(pEntity, px, py, r);
			ActivateOutput( &m_actInfo, eO_OnPosChange, true);
			ActivateOutput( &m_actInfo, eO_PosX, px );
			ActivateOutput( &m_actInfo, eO_PosY, py );
			ActivateOutput( &m_actInfo, eO_Rotation, r );
		}
	}


private:
	EntityId m_entityId;
	SActivationInfo m_actInfo;

	enum EInputPorts
	{
		eI_Trigger = 0,
		eI_Enable,
		eI_Disable,
	};

	enum EOutputPorts
	{
		eO_OnPosChange,
		eO_PosX,
		eO_PosY,
		eO_Rotation,
	};
};

//----------------------------------- Player pos info --------------------------------------
class CFlowMiniMapPlayerPosInfo
	: public CFlowMiniMapEntityPosInfo
{
public:
	CFlowMiniMapPlayerPosInfo( SActivationInfo * pActInfo )
		: CFlowMiniMapEntityPosInfo(pActInfo)
	{
	}

	virtual ~CFlowMiniMapPlayerPosInfo()
	{
	}

protected:
	virtual EntityId GetEntityId(SActivationInfo *pActInfo)
	{
		return gEnv->pGame->GetIGameFramework()->GetClientActorId();
	}

	virtual void SetFlagsAndDescriton( SFlowNodeConfig& config )
	{
		config.sDescription = _HELP( "Info about player position on minimap" );
	}

};

REGISTER_FLOW_NODE_SINGLETON("Minimap:MapInfo", CFlowMiniMapInfoNode);
REGISTER_FLOW_NODE_SINGLETON("Minimap:PlayerPos", CFlowMiniMapPlayerPosInfo);
REGISTER_FLOW_NODE("Minimap:EntityPos", CFlowMiniMapEntityPosInfo);
