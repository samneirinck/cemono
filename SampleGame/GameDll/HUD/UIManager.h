////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UIManager.h
//  Version:     v1.00
//  Created:     08/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __UIManager_H__
#define __UIManager_H__

class CUIInput;
class CUIObjectives;
class CUISettings;
class CUIMultiPlayer;

class CUIManager
{
public:
	static void Init();
	static void Destroy();
	static CUIManager* GetInstance();

	CUIObjectives* GetUIObjectives();
	CUIInput* GetUIInput();
	CUIMultiPlayer* GetUIMultiPlayer();

	void UpdatePickupMessage(bool bShow);

private:
	CUIManager();
	CUIManager(const CUIManager& ) {}
	CUIManager operator=(const CUIManager& ) {}
	~CUIManager();

	static CUIManager* m_pInstance;

private:
	CUIInput* m_pUIInput;
	CUIObjectives* m_pUIObjectives;
	CUISettings* m_pUISettings;
	CUIMultiPlayer* m_pUIMultiPlayer;

	bool m_bPickupMsgVisible;
};

#define NOTIFY_UI_OBJECTIVES( fct ) { CUIManager::GetInstance()->GetUIObjectives()->fct; }
#define NOTIFY_UI_MP( fct ) { CUIManager::GetInstance()->GetUIMultiPlayer()->fct; }

#endif

