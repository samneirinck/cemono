#include "StdAfx.h"
#include "BitmapUi.h"

#include "IGameFramework.h"
#include "IRenderer.h"
#include "UIDraw/UIDraw.h"
#include "GameCVars.h"
#include "Actor.h"
#include "Weapon.h"


static const char* CROSSHAIR_TEXTURE = "Textures\\Gui\\Crosshair.tif";
static const float CROSSHAIR_TEXTURE_WIDTH  = 64.0f;
static const float CROSSHAIR_TEXTURE_HEIGHT = CROSSHAIR_TEXTURE_WIDTH;


#if defined ( WIN32 ) || defined ( WIN64 )
	static const char* LOADINGSCREEN_TEXTURE		= "Textures\\Gui\\loading_screen.tif";
	static const char* STARTSCREEN_TEXTURE			= "Textures\\Gui\\loading_screen.tif";
	static const char* PAUSESCREEN_TEXTURE      = "Textures\\Gui\\loading_screen.tif";
#else
	// Console optimized textures: Save on memory
	static const char* LOADINGSCREEN_TEXTURE		= "Textures\\Gui\\loading_screen_con.tif";
	static const char* STARTSCREEN_TEXTURE			= "Textures\\Gui\\loading_screen_con.tif";
	static const char* PAUSESCREEN_TEXTURE      = "Textures\\Gui\\loading_screen_con.tif";
#endif


//////////////////////////////////////////////////////////////////////////
CBitmapUi::CBitmapUi() 
: m_pGameFramework( NULL )
, m_pUiDraw( NULL )
, m_iTexCrosshair( 0 )
, m_iTexLoadingScreen( 0 )
, m_iTexStartScreen( 0 )
, m_iTexPauseScreen( 0 )
, m_currentScreenState( eSS_StartScreen )
{
	assert( g_pGame != NULL );

	m_pGameFramework = g_pGame->GetIGameFramework();

	assert( m_pGameFramework != NULL );
	if ( m_pGameFramework == NULL )
	{
		return;
	}

	m_pUiDraw = m_pGameFramework->GetIUIDraw();

	assert( m_pUiDraw != NULL );
	if ( m_pUiDraw == NULL )
	{
		return;
	}

	m_iTexCrosshair = m_pUiDraw->CreateTexture( CROSSHAIR_TEXTURE, false );
	m_iTexLoadingScreen = m_pUiDraw->CreateTexture( LOADINGSCREEN_TEXTURE, false );
	m_iTexStartScreen = m_pUiDraw->CreateTexture( STARTSCREEN_TEXTURE, false );
	m_iTexPauseScreen = m_pUiDraw->CreateTexture( PAUSESCREEN_TEXTURE, false );

	if ( gEnv->IsEditor() )
	{
		// In editor mode we always want to show the game screen.
		m_currentScreenState = eSS_InGameScreen;
	}

	m_pGameFramework->RegisterListener( this, "BitmapUI", FRAMEWORKLISTENERPRIORITY_HUD );

	ILevelSystem* pLevelSystem = m_pGameFramework->GetILevelSystem();
	assert( pLevelSystem != NULL );
	if ( pLevelSystem != NULL )
	{
		pLevelSystem->AddListener( this );
	}

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener( this );
}


//////////////////////////////////////////////////////////////////////////
CBitmapUi::~CBitmapUi()
{
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener( this );

	if ( m_pGameFramework != NULL )
	{
		m_pGameFramework->GetILevelSystem()->RemoveListener( this );
		m_pGameFramework->UnregisterListener( this );
	}

	if ( m_pUiDraw != NULL )
	{
		m_pUiDraw->DeleteTexture( m_iTexCrosshair );
		m_pUiDraw->DeleteTexture( m_iTexLoadingScreen );
		m_pUiDraw->DeleteTexture( m_iTexStartScreen );
		m_pUiDraw->DeleteTexture( m_iTexPauseScreen );
	}
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::SetScreenState( const EScreenState state )
{
	if ( gEnv->IsEditor() )
	{
		return;
	}

	m_currentScreenState = state;

	// TODO: Fix console drawing during CSystem::UpdateLoadingScreen to properly remove one of
	// the issues that can cause flickering during level load and when the console is visible.
	if ( state == eSS_LoadingScreen )
	{
		IConsole* pConsole = gEnv->pConsole;
		if ( pConsole != NULL )
		{
			pConsole->ShowConsole( false );
		}
	}	
}


//////////////////////////////////////////////////////////////////////////
CBitmapUi::EScreenState CBitmapUi::GetScreenState() const
{
	return m_currentScreenState;
}


//////////////////////////////////////////////////////////////////////////
bool CBitmapUi::IsEnabled() const
{
	bool showBitmapUi = ( g_pGameCVars->g_showBitmapUi == 1 );
	return showBitmapUi;
}


//////////////////////////////////////////////////////////////////////////
bool CBitmapUi::CanDrawCrosshair() const
{
	assert( m_pGameFramework != NULL );

	if ( ! g_pGameCVars->g_show_crosshair )
	{
		return false;
	}

	IActor* pPlayer = m_pGameFramework->GetClientActor();
	if ( pPlayer == NULL )
	{
		return false;
	}

	bool isPlayerDead = pPlayer->IsDead();
	if ( isPlayerDead )
	{
		return false;
	}

	bool thirdPersonMode = pPlayer->IsThirdPerson();
	bool crosshairEnabledInThirdPerson = ( g_pGameCVars->g_show_crosshair_tp != 0 );
	if ( thirdPersonMode && ! crosshairEnabledInThirdPerson )
	{
		return false;
	}

	IItem* pItem = pPlayer->GetCurrentItem();
	if ( pItem == NULL )
	{
		return false;
	}

	IWeapon* pWeapon = pItem->GetIWeapon();
	if ( pWeapon == NULL )
	{
		return false;
	}

	bool carryingMeleeWeapon = pWeapon->CanMeleeAttack();
	if ( carryingMeleeWeapon )
	{
		return false;
	}

	bool isWeaponZoomed = pWeapon->IsZoomed();
	bool usingWeaponSightForAiming = ( ! thirdPersonMode && isWeaponZoomed );		
	if ( usingWeaponSightForAiming )
	{
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::DrawCrosshair() const
{
	bool canDrawCrosshair = CanDrawCrosshair();
	if ( ! canDrawCrosshair )
	{
		return;
	}

	DrawCenteredQuad( m_iTexCrosshair, CROSSHAIR_TEXTURE_WIDTH, CROSSHAIR_TEXTURE_HEIGHT );
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::Draw()
{
	if ( ! IsEnabled() )
	{
		return;
	}

	assert( m_pGameFramework != NULL );
	if ( m_pGameFramework == NULL )
	{
		return;
	}

	assert( m_pUiDraw != NULL );
	if ( m_pUiDraw == NULL )
	{
		return;
	}

	m_pUiDraw->PreRender();

	UpdateCurrentState();
	DrawCurrentScreenState();

	m_pUiDraw->PostRender();	
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::DrawFullFrame()
{
	IRenderer* pRenderer = gEnv->pRenderer;
	assert( pRenderer != NULL );

	bool isStandAlone = ( pRenderer->EF_Query( EFQ_RecurseLevel ) <= 0 );
	if ( isStandAlone )
	{
		pRenderer->BeginFrame();
	}

	Draw();

	if ( isStandAlone )
	{
		pRenderer->EndFrame();
	}
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::UpdateCurrentState()
{
	// TODO: Remove this function once we add a listener/event for game pause.
	assert( m_pGameFramework != NULL );

	EScreenState currentScreenState = GetScreenState();

	if ( currentScreenState == eSS_InGameScreen )
	{
		bool isGamePaused = m_pGameFramework->IsGamePaused();
		if ( isGamePaused )
		{
			SetScreenState( eSS_PausedScreen );
		}

		return;
	}

	if ( currentScreenState == eSS_PausedScreen )
	{
		bool isGamePaused = m_pGameFramework->IsGamePaused();
		if ( ! isGamePaused )
		{
			SetScreenState( eSS_InGameScreen );
		}

		return;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBitmapUi::DrawCurrentScreenState() const
{
	EScreenState currentScreenState = GetScreenState();
	switch ( currentScreenState )
	{
	case eSS_InGameScreen:
		DrawCrosshair();
		break;

	case eSS_LoadingScreen:
		DrawFullscreenQuad( m_iTexLoadingScreen );
		break;

	case eSS_StartScreen:
		DrawFullscreenQuad( m_iTexStartScreen );
		break;

	case eSS_PausedScreen:
		DrawFullscreenQuad( m_iTexPauseScreen );
		break;

	default:
		assert( false );
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::DrawFullscreenQuad( const int textureId ) const
{
	assert( m_pUiDraw != NULL );

	IRenderer* pRenderer = gEnv->pRenderer;
	assert( pRenderer != NULL );
	if ( pRenderer == NULL )
	{
		return;
	}

	float screenWidth = static_cast< float >( pRenderer->GetWidth() );
	float screenHeight = static_cast< float >( pRenderer->GetHeight() );
	
	m_pUiDraw->DrawQuadSimple( 0, 0, screenWidth, screenHeight, 0xFFFFFFFF, textureId );
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::DrawCenteredQuad( const int textureId, const float textureWidth, const float textureHeight ) const
{
	assert( m_pUiDraw != NULL );

	IRenderer* pRenderer = gEnv->pRenderer;
	assert( pRenderer != NULL );
	if ( pRenderer == NULL )
	{
		return;
	}

	float screenCentreX = pRenderer->GetWidth()	* 0.5f;
	float screenCentreY = pRenderer->GetHeight()* 0.5f;

	float offsetX = screenCentreX - textureWidth * 0.5f;
	float offsetY = screenCentreY - textureHeight * 0.5f;

	m_pUiDraw->DrawQuadSimple( offsetX, offsetY, textureWidth, textureHeight, 0xFFFFFFFF, textureId );
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::OnActionEvent(const SActionEvent& event)
{
	if ( event.m_event == eAE_inGame )
	{
		SetScreenState( eSS_InGameScreen );
	}
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::OnLoadingProgress( ILevelInfo* pLevel, int progressAmount )
{
	EScreenState currentScreenState = GetScreenState();
	if ( currentScreenState == eSS_LoadingScreen )
	{
		// 3d Engine notifies of loading progress too when already in game ( eg. texture startup streaming finish ),
		// so we'll only draw a full frame when we're sure we're in loading state.
		DrawFullFrame();
	}
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::OnSystemEvent( ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam )
{
	switch ( event )
	{
	case ESYSTEM_EVENT_LEVEL_UNLOAD:
		SetScreenState( eSS_LoadingScreen );
		DrawFullFrame();
		break;

	case ESYSTEM_EVENT_LEVEL_LOAD_PREPARE:
		SetScreenState( eSS_LoadingScreen );
		gEnv->pGame->GetIGameFramework()->SetLevelPrecachingDone( false );
		break;

	case ESYSTEM_EVENT_LEVEL_LOAD_END:
		SetScreenState(eSS_InGameScreen);
		gEnv->pGame->GetIGameFramework()->SetLevelPrecachingDone( true );
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::OnLoadingStart( ILevelInfo* pLevel )
{
	SetScreenState( eSS_LoadingScreen );
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::OnPostUpdate( float fDeltaTime )
{
	Draw();
}


//////////////////////////////////////////////////////////////////////////
void CBitmapUi::OnLoadGame( ILoadGame* pLoadGame )
{
	SetScreenState( eSS_LoadingScreen );
}
