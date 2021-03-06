#pragma once

#include <IGame.h>
#include <IGameFramework.h>
#include <CryFixedString.h>
#include <ICemono.h>


class CGame : public IGame, public IGameFrameworkListener, public IInputEventListener
{
public:
	CGame();
	virtual ~CGame();

	// IGame
	virtual bool Init(IGameFramework *pFramework);
	virtual void GetMemoryStatistics(ICrySizer * s) const;
	virtual bool CompleteInit();
	virtual void Shutdown();
	virtual int Update(bool haveFocus, unsigned int updateFlags);
	virtual void ConfigureGameChannel(bool isServer, IProtocolBuilder *pBuilder);
	virtual void EditorResetGame(bool bStart);
	virtual void PlayerIdSet(EntityId playerId);
	virtual IGameFramework *GetIGameFramework();
	virtual const char *GetLongName();
	virtual const char *GetName();
	virtual void LoadActionMaps(const char* filename);
	virtual void OnClearPlayerIds();
	virtual IGame::TSaveGameName CreateSaveGameName();
	virtual const char* GetMappedLevelName(const char *levelName) const;
	virtual IGameStateRecorder* CreateGameStateRecorder(IGameplayListener* pL);
	virtual const bool DoInitialSavegame() const;
	virtual uint32 AddGameWarning(const char* stringId, const char* paramMessage, IGameWarningsListener* pListener = NULL);
	virtual void RenderGameWarnings();
	virtual void RemoveGameWarning(const char* stringId);
	virtual bool GameEndLevel(const char* stringId);
	virtual void SetUserProfileChanged(bool yesNo);
	virtual void OnRenderScene();
	virtual const uint8* GetDRMKey();
	virtual const char* GetDRMFileList();

	// IGameFrameworkListener
	virtual void OnPostUpdate(float fDeltaTime);
	virtual void OnSaveGame(ISaveGame* pSaveGame);
	virtual void OnLoadGame(ILoadGame* pLoadGame);
	virtual void OnLevelEnd(const char* nextLevel);
	virtual void OnActionEvent(const SActionEvent& event);

	// IInputEventListener
	virtual bool OnInputEvent( const SInputEvent &event );
	virtual bool OnInputEventUI( const SInputEvent &event );

	// Cemono 
//	virtual void SetCemonoGameObject(CCemonoObject pGameObject);



protected:
	IGameFramework*						m_pFramework;
//	CCemonoObject						m_gameObject;

};

extern CGame* g_pGame;
extern ICemonoPtr g_pCemono;