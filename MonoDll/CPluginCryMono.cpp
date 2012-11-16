#include <StdAfx.h>
#include <CPluginCryMono.h>

namespace CryMonoPlugin
{
    CPluginCryMono* gPlugin = NULL;

    CPluginCryMono::CPluginCryMono()
    {
        gPlugin = this;
    }

    CPluginCryMono::~CPluginCryMono()
    {
        Release( true );

        gPlugin = NULL;
    }

    bool CPluginCryMono::Release( bool bForce )
    {
        bool bRet = true;
        bool bWasInitialized = m_bIsFullyInitialized; // Will be reset by base class so backup

        if ( !m_bCanUnload )
        {
            // Note: Type Unregistration will be automatically done by the Base class (Through RegisterTypes)
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBaseMinimal::Release( bForce );

            if ( bRet )
            {
                if ( bWasInitialized )
                {
                    SAFE_RELEASE(gEnv->pMonoScriptSystem);

                    // TODO: Cleanup stuff that can only be cleaned up if the plugin was initialized
                }

                // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
                gPluginManager->UnloadPlugin( GetName() );

                // Allow Plugin Manager garbage collector to unload this plugin
                AllowDllUnload();
            }
        }

        return bRet;
    };

    bool CPluginCryMono::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
    {
        gPluginManager = ( PluginManager::IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
        CPluginBaseMinimal::Init( env, startupParams, pPluginManager, sPluginDirectory );

        gEnv->pMonoScriptSystem = new CScriptSystem();

#pragma warning(push)
#pragma warning(disable : 4800 )
        return bool( gEnv->pMonoScriptSystem );
#pragma warning(pop)
    }

    bool CPluginCryMono::RegisterTypes( int nFactoryType, bool bUnregister )
    {
        bool bRet = CPluginBaseMinimal::RegisterTypes( nFactoryType, bUnregister );

        using namespace PluginManager;
        eFactoryType enFactoryType = eFactoryType( nFactoryType );

        if ( bRet )
        {
            if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
            {
                // Flownodes
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_Flownode ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVars here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVars here if you have some
                        // ...
                    }
                }

                // CVars
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVar ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVars here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVars here if you have some
                        // ...
                    }
                }

                // CVars Commands
                if ( gEnv->pConsole && ( enFactoryType == FT_All || enFactoryType == FT_CVarCommand ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register CVar Commands here if you have some
                        // ...
                    }

                    else
                    {
                        // TODO: Unregister CVar Commands here if you have some
                        // ...
                    }
                }

                // Game Objects
                if ( gEnv->pGameFramework && ( enFactoryType == FT_All || enFactoryType == FT_GameObjectExtension ) )
                {
                    if ( !bUnregister )
                    {
                        // TODO: Register Game Object Extensions here if you have some
                        // ...
                    }
                }
            }
        }

        return bRet;
    }
}