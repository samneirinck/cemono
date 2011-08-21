#pragma once

#include <IGameRef.h>
#include <IEditorGame.h>

struct IGameStartup;

class CEditorGame : public IEditorGame
{
public:
	CEditorGame();
	virtual ~CEditorGame();

	// IEditorGame interface
	virtual bool Init( ISystem *pSystem,IGameToEditorInterface *pEditorInterface );
	virtual int Update(bool haveFocus, unsigned int updateFlags);
	virtual void Shutdown();
	virtual bool SetGameMode(bool bGameMode);
	virtual IEntity * GetPlayer();
	virtual void SetPlayerPosAng(Vec3 pos,Vec3 viewDir);
	virtual void HidePlayer(bool bHide);
	virtual void OnBeforeLevelLoad();
	virtual void OnAfterLevelLoad(const char *levelName, const char *levelFolder);
	virtual void OnCloseLevel();
	virtual bool BuildEntitySerializationList(XmlNodeRef output);
	virtual bool GetAdditionalMinimapData(XmlNodeRef output);
	virtual IFlowSystem * GetIFlowSystem();
	virtual IGameTokenSystem* GetIGameTokenSystem();
	virtual IEquipmentSystemInterface* GetIEquipmentSystemInterface();
	virtual bool SupportsMultiplayerGameRules();
	virtual void ToggleMultiplayerGameRules();
	// -IEditorGame

private:
	bool ConfigureNetContext(bool on);
	void EnablePlayer(bool player);
	const char* GetGameRulesName();

	bool m_bEnabled;
	bool m_bPlayer;
	bool m_bGameMode;
	IGameRef m_pGame;
	IGameStartup* m_pGameStartup;
	static const uint16 EDITOR_SERVER_PORT;
};