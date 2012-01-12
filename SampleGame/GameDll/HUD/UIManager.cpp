////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UIManager.cpp
//  Version:     v1.00
//  Created:     08/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UIManager.h"

#include "UIInput.h"
#include "UIObjectives.h"
#include "UISettings.h"
#include "UIMultiPlayer.h"

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Singleton ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CUIManager* CUIManager::m_pInstance = NULL;

void CUIManager::Init()
{ 
	assert( m_pInstance == NULL );
	if ( !m_pInstance )
		m_pInstance = new CUIManager();
}

/////////////////////////////////////////////////////////////////////////////////////
void CUIManager::Destroy()
{
	assert( m_pInstance != NULL );
	SAFE_DELETE( m_pInstance );
}

/////////////////////////////////////////////////////////////////////////////////////
CUIManager* CUIManager::GetInstance()
{
	if ( !m_pInstance )
		Init();
	return m_pInstance;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// CTor/DTor ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CUIManager::CUIManager()
	: m_bPickupMsgVisible(false)
{
	m_pUIInput = new CUIInput();
	m_pUIObjectives = new CUIObjectives();
	m_pUISettings = new CUISettings();
	m_pUIMultiPlayer = new CUIMultiPlayer();
}

/////////////////////////////////////////////////////////////////////////////////////
CUIManager::~CUIManager()
{
	SAFE_DELETE( m_pUIInput );
	SAFE_DELETE( m_pUIObjectives );
	SAFE_DELETE( m_pUISettings );
	SAFE_DELETE( m_pUIMultiPlayer );
}

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Ext Access  ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CUIObjectives* CUIManager::GetUIObjectives()
{
	return m_pUIObjectives;
}

CUIInput* CUIManager::GetUIInput()
{
	return m_pUIInput;
}

CUIMultiPlayer* CUIManager::GetUIMultiPlayer()
{
	return m_pUIMultiPlayer;
}


void CUIManager::UpdatePickupMessage(bool bShow)
{
	if (!gEnv->pFlashUI) return;

	if (m_bPickupMsgVisible != bShow)
	{
		m_bPickupMsgVisible = bShow;
		static IUIAction* pAction = gEnv->pFlashUI->GetUIAction("DisplayPickupText");
		if (pAction)
		{
			SUIArguments args;
			args.AddArgument(bShow ? string("@ui_pickup") : string(""));
			gEnv->pFlashUI->GetUIActionManager()->StartAction(pAction, args);
		}
	}
}

