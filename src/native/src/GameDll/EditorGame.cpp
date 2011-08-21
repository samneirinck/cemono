#include "StdAfx.h"

#include "EditorGame.h"

CEditorGame::CEditorGame()
{
}

CEditorGame::~CEditorGame()
{
}

bool CEditorGame::Init( ISystem *pSystem,IGameToEditorInterface *pEditorInterface )
{
	return false;
}

int CEditorGame::Update(bool haveFocus, unsigned int updateFlags)
{
	return 0;
}

void CEditorGame::Shutdown()
{

}

bool CEditorGame::SetGameMode(bool bGameMode)
{
	return false;
}

IEntity* CEditorGame::GetPlayer()
{
	return NULL;
}

void CEditorGame::SetPlayerPosAng(Vec3 pos,Vec3 viewDir)
{

}

void CEditorGame::HidePlayer(bool bHide)
{

}

void CEditorGame::OnBeforeLevelLoad()
{

}

void CEditorGame::OnAfterLevelLoad(const char *levelName, const char *levelFolder)
{

}

void CEditorGame::OnCloseLevel()
{

}

bool CEditorGame::BuildEntitySerializationList(XmlNodeRef output)
{
	return false;
}

bool CEditorGame::GetAdditionalMinimapData(XmlNodeRef output)
{
	return false;
}

IFlowSystem* CEditorGame::GetIFlowSystem()
{
	return NULL;
}

IGameTokenSystem* CEditorGame::GetIGameTokenSystem()
{
	return NULL;
}

IEquipmentSystemInterface* CEditorGame::GetIEquipmentSystemInterface()
{
	return NULL;
}

bool CEditorGame::SupportsMultiplayerGameRules()
{
	return false;
}
void CEditorGame::ToggleMultiplayerGameRules()
{

}