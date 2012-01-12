#ifndef __Bitmap_Ui__h__
#define __Bitmap_Ui__h__

#include <ILevelSystem.h>
#include <IGameFramework.h>

struct IGameFramework;
struct IUIDraw;

class CBitmapUi
	: public ILevelSystemListener
	, public IGameFrameworkListener
	, public ISystemEventListener
{
public:
	typedef enum
	{
		eSS_StartScreen,
		eSS_LoadingScreen,
		eSS_PausedScreen,
		eSS_InGameScreen,
	} EScreenState;

	CBitmapUi();
	virtual ~CBitmapUi();

	void Draw();

	// ILevelSystemListener
	VIRTUAL void OnLevelNotFound( const char* levelName ) {}
	VIRTUAL void OnLoadingStart( ILevelInfo* pLevel );
	VIRTUAL void OnLoadingComplete( ILevel* pLevel ) {}
	VIRTUAL void OnLoadingError( ILevelInfo* pLevel, const char* error ) {}
	VIRTUAL void OnLoadingProgress( ILevelInfo* pLevel, int progressAmount );
	VIRTUAL void OnUnloadComplete( ILevel* pLevel ) {}
	// ~ILevelSystemListener

	// IGameFrameworkListener
	VIRTUAL void OnPostUpdate( float fDeltaTime );
	VIRTUAL void OnSaveGame( ISaveGame* pSaveGame ) {}
	VIRTUAL void OnLoadGame( ILoadGame* pLoadGame );
	VIRTUAL void OnLevelEnd( const char* nextLevel ) {}
	VIRTUAL void OnActionEvent( const SActionEvent& event );
	// ~IGameFrameworkListener

	// ISystemEventListener
	VIRTUAL void OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam );
	// ~ISystemEventListener

	void SetScreenState( const EScreenState state );
	EScreenState GetScreenState() const;

protected:
	void DrawCurrentScreenState() const;

	bool CanDrawCrosshair() const;
	void DrawCrosshair() const;
	
	void DrawFullscreenQuad( const int textureId ) const;
	void DrawCenteredQuad( const int textureId, const float textureWidth, const float textureHeight ) const;	

	bool IsEnabled() const;

	void UpdateCurrentState();

	void DrawFullFrame();

private:	
	IGameFramework* m_pGameFramework;

	IUIDraw* m_pUiDraw;

	int m_iTexCrosshair;
	int m_iTexLoadingScreen;
	int m_iTexStartScreen;	
	int m_iTexPauseScreen;

	EScreenState m_currentScreenState;
};

#endif