#ifndef __GAME_H__
#define __GAME_H__

#pragma once

#include <IGame.h>
#include <IGameFramework.h>

class CGame : public IGame, public IGameFrameworkListener, public IInputEventListener
{
public:
	CGame();
	virtual ~CGame();

	// IGame
	virtual bool Init(IGameFramework *pFramework);
	virtual void GetMemoryStatistics(ICrySizer * s);
	virtual bool CompleteInit();
	virtual void Shutdown();
	virtual string InitMapReloading();
	virtual int Update(bool haveFocus, unsigned int updateFlags);
	virtual void ConfigureGameChannel(bool isServer, IProtocolBuilder *pBuilder);
	virtual void EditorResetGame(bool bStart);
	virtual void PlayerIdSet(EntityId playerId);
	virtual IGameFramework *GetIGameFramework();
	virtual const char *GetLongName();
	virtual const char *GetName();
	virtual void LoadActionMaps(const char* filename);
	virtual void OnClearPlayerIds();
	virtual bool IsReloading();
	virtual const char* CreateSaveGameName();
	virtual const char* GetMappedLevelName(const char *levelName) const;

	// IGameFrameworkListener
	virtual void OnPostUpdate(float fDeltaTime);
	virtual void OnSaveGame(ISaveGame* pSaveGame);
	virtual void OnLoadGame(ILoadGame* pLoadGame);
	virtual void OnLevelEnd(const char* nextLevel);
	virtual void OnActionEvent(const SActionEvent& event);

	// IInputEventListener
	virtual bool OnInputEvent( const SInputEvent &event );
	virtual bool OnInputEventUI( const SInputEvent &event );
protected:
	IGameFramework						*m_pFramework;
};

extern CGame* g_pGame;

#endif