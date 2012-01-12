////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UIObjectives.h
//  Version:     v1.00
//  Created:     20/1/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIObjectives_H__
#define __UIObjectives_H__

#include <IFlashUI.h>

class CGameRules;

class CUIObjectives	: public IUIEventListener
{
public:
	CUIObjectives();
	~CUIObjectives();

	// these functions are called by CGameRules ( mission objective related functions )
	void MissionObjectiveAdded( const string& objectiveID, int state );
	void MissionObjectiveRemoved( const string& objectiveID );
	void MissionObjectivesReset();
	void MissionObjectiveStateChanged( const string& objectiveID, int state );

	// IUIEventListener
	virtual void OnEvent( const SUIEvent& event );
	// ~IUIEventListener

private:
	IUIEventSystem* m_pUIOEvt;
	IUIEventSystem* m_pUIOFct;

	static SUIEventHelper<CUIObjectives> s_EventDispatcher;

	enum EUIObjectiveEvent
	{
		eUIOE_ObjectiveAdded,
		eUIOE_ObjectiveRemoved,
		eUIOE_ObjectivesReset,
		eUIOE_ObjectiveStateChanged,
	};
	std::map< EUIObjectiveEvent, uint > m_EventMap;

	// mission objectives info
	struct SMissionObjectiveInfo
	{
		string Name;
		string Desc;
	};
	typedef std::map< string, SMissionObjectiveInfo > TObjectiveMap;
	TObjectiveMap m_ObjectiveMap;

	void NotifyUI( EUIObjectiveEvent eventType, const SUIArguments& args = SUIArguments() );

	void UpdateObjectiveInfo();
	SMissionObjectiveInfo* GetMissionObjectiveInfo( const string& objectiveID, bool bLogError = true );
	CGameRules* GetGameRules();

	void OnRequestMissionObjectives( const SUIEvent& event );
};

#endif
