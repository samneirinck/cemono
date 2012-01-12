////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UIObjectives.cpp
//  Version:     v1.00
//  Created:     20/1/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UIObjectives.h"
#include "GameRules.h"

SUIEventHelper<CUIObjectives> CUIObjectives::s_EventDispatcher;

//--------------------------------------------------------------------------------------------
CUIObjectives::CUIObjectives()
{
	if ( gEnv->pFlashUI )
	{
		// events that be fired to the UI
		m_pUIOEvt = gEnv->pFlashUI->CreateEventSystem( "UIObjectives", IUIEventSystem::eEST_SYSTEM_TO_UI );

		{
			SUIEventDesc evtDesc( "ObjectiveAdded", "ObjectiveAdded", "Mission objective added" );
			evtDesc.Params.push_back( SUIParameterDesc( "ObjectiveID", "MissionID", "ID of the mission" ) );
			evtDesc.Params.push_back( SUIParameterDesc( "Name", "Name", "Name of the mission" ) );
			evtDesc.Params.push_back( SUIParameterDesc( "Desc", "Desc", "Description of the mission" ) );
			evtDesc.Params.push_back( SUIParameterDesc( "State", "State", "State of the objective" ) );
			m_EventMap[ eUIOE_ObjectiveAdded ] = m_pUIOEvt->RegisterEvent( evtDesc );
		}

		{
			SUIEventDesc evtDesc( "ObjectiveRemoved", "ObjectiveRemoved", "Mission objective removed" );
			evtDesc.Params.push_back( SUIParameterDesc( "ObjectiveID", "MissionID", "ID of the mission" ) );
			m_EventMap[ eUIOE_ObjectiveRemoved ] = m_pUIOEvt->RegisterEvent( evtDesc );
		}

		{
			SUIEventDesc evtDesc( "ObjectivesReset", "ObjectivesReset", "All mission objectives reset" );
			m_EventMap[ eUIOE_ObjectivesReset ] = m_pUIOEvt->RegisterEvent( evtDesc );
		}

		{
			SUIEventDesc evtDesc( "ObjectiveStateChanged", "ObjectiveStateChanged", "Objective status changed" );
			evtDesc.Params.push_back( SUIParameterDesc( "ObjectiveID", "MissionID", "ID of the mission" ) );
			evtDesc.Params.push_back( SUIParameterDesc( "State", "State", "State of the objective" ) );
			m_EventMap[ eUIOE_ObjectiveStateChanged ] = m_pUIOEvt->RegisterEvent( evtDesc );
		}

		// event system to receive events from UI
		m_pUIOFct = gEnv->pFlashUI->CreateEventSystem( "UIObjectives", IUIEventSystem::eEST_UI_TO_SYSTEM );
		m_pUIOFct->RegisterListener( this );

		{
			SUIEventDesc evtDesc( "RequestObjectives", "RequestObjectives", "Request all mission objectives (force to call ObjectiveAdded for each objective)" );
			s_EventDispatcher.RegisterEvent( m_pUIOFct, evtDesc, &CUIObjectives::OnRequestMissionObjectives );
		}
	}
	UpdateObjectiveInfo();
}

CUIObjectives::~CUIObjectives()
{
	if ( m_pUIOFct )
		m_pUIOFct->UnregisterListener( this );
}

//--------------------------------------------------------------------------------------------
//---------------------- functions that generate events for the UI ---------------------------
//--------------------------------------------------------------------------------------------

void CUIObjectives::MissionObjectiveAdded( const string& objectiveID, int state )
{
	if ( gEnv->IsEditor() )
	{
		UpdateObjectiveInfo();
	}
	SMissionObjectiveInfo* pInfo = GetMissionObjectiveInfo( objectiveID );
	if ( pInfo )
	{
		SUIArguments args;
		args.AddArgument( objectiveID );
		args.AddArgument( pInfo->Name );
		args.AddArgument( pInfo->Desc );
		args.AddArgument( state );
		NotifyUI( eUIOE_ObjectiveAdded, args );
	}
}

//--------------------------------------------------------------------------------------------
void CUIObjectives::MissionObjectiveRemoved( const string& objectiveID )
{
	SMissionObjectiveInfo* pInfo = GetMissionObjectiveInfo( objectiveID );
	if ( pInfo )
	{
		SUIArguments args;
		args.AddArgument( objectiveID );
		NotifyUI( eUIOE_ObjectiveRemoved, args );
	}
}

//--------------------------------------------------------------------------------------------
void CUIObjectives::MissionObjectivesReset()
{
	NotifyUI( eUIOE_ObjectivesReset );
}

//--------------------------------------------------------------------------------------------
void CUIObjectives::MissionObjectiveStateChanged( const string& objectiveID, int state )
{
	SMissionObjectiveInfo* pInfo = GetMissionObjectiveInfo( objectiveID );
	if ( pInfo )
	{
		SUIArguments args;
		args.AddArgument( objectiveID );
		args.AddArgument( state );
		NotifyUI( eUIOE_ObjectiveStateChanged, args );
	}
}

//--------------------------------------------------------------------------------------------
//----------------------------- events that are fired by the UI ------------------------------
//--------------------------------------------------------------------------------------------
void CUIObjectives::OnEvent( const SUIEvent& event )
{
	s_EventDispatcher.Dispatch( this, event );
}

void CUIObjectives::OnRequestMissionObjectives( const SUIEvent& event )
{
	CGameRules* pGameRules = GetGameRules();
	if ( pGameRules && g_pGame->GetIGameFramework()->GetClientActor() )
	{
		CActor* pActor = pGameRules->GetActorByChannelId( g_pGame->GetIGameFramework()->GetClientActor()->GetChannelId() );
		if ( pActor )
		{
			std::map< string, int > tmpList;
			int teamID = pGameRules->GetTeam( pActor->GetEntityId() );
			for ( TObjectiveMap::iterator it = m_ObjectiveMap.begin(); it != m_ObjectiveMap.end(); ++it )
			{
				CGameRules::TObjective* pObjective = pGameRules->GetObjective( teamID, it->first.c_str() );
				if ( pObjective )
					tmpList[ it->first ] = pObjective->status;
			}
			for ( std::map< string, int >::iterator it = tmpList.begin(); it != tmpList.end(); ++it )
				MissionObjectiveAdded( it->first, it->second );
		}
	}

}

//--------------------------------------------------------------------------------------------
//------------------------------------- private functions ------------------------------------
//--------------------------------------------------------------------------------------------

void CUIObjectives::NotifyUI( EUIObjectiveEvent eventType, const SUIArguments& args )
{
	if( m_pUIOEvt )
		m_pUIOEvt->SendEvent( SUIEvent(m_EventMap[eventType], args) );
}

//--------------------------------------------------------------------------------------------
void CUIObjectives::UpdateObjectiveInfo()
{
	m_ObjectiveMap.clear();

	string path = "Libs/UI/Objectives_new.xml";
	XmlNodeRef missionObjectives = GetISystem()->LoadXmlFromFile( path.c_str() );
	if (missionObjectives == 0)
	{
		gEnv->pLog->LogError("Error while loading MissionObjective file '%s'", path.c_str() );
		return;
	}

	for(int tag = 0; tag < missionObjectives->getChildCount(); ++tag)
	{
		XmlNodeRef mission = missionObjectives->getChild(tag);
		const char* attrib;
		const char* objective;
		const char* text;
		for(int obj = 0; obj < mission->getChildCount(); ++obj)
		{
			XmlNodeRef objectiveNode = mission->getChild(obj);
			string id(mission->getTag());
			id += ".";
			id += objectiveNode->getTag();
			if(objectiveNode->getAttributeByIndex(0, &attrib, &objective) && objectiveNode->getAttributeByIndex(1, &attrib, &text))
			{
				m_ObjectiveMap[ id ].Name = objective;
				m_ObjectiveMap[ id ].Desc = text;
			}
			else
			{
				gEnv->pLog->LogError("Error while loading MissionObjective file '%s'", path.c_str() );
				return;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
CUIObjectives::SMissionObjectiveInfo* CUIObjectives::GetMissionObjectiveInfo( const string& objectiveID, bool bLogError )
{
	TObjectiveMap::iterator it = m_ObjectiveMap.find( objectiveID );
	if ( it != m_ObjectiveMap.end() )
	{
		return &it->second;
	}
	if ( bLogError )
		gEnv->pLog->LogError( "[UIObjectives] Mission Objective \"%s\" is not defined!", objectiveID.c_str() );
	return NULL;
}

//--------------------------------------------------------------------------------------------
CGameRules* CUIObjectives::GetGameRules()
{
	return static_cast<CGameRules *>( g_pGame->GetIGameFramework()->GetIGameRulesSystem()->GetCurrentGameRules() );
}