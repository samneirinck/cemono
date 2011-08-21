#include "StdAfx.h"

#include <CryLibrary.h>
#include <platform_impl.h>
#include <IHardwareMouse.h>
#include <ICemono.h>
#include <CryExtension/CryCreateClassInstance.h>
#include <CryExtension/ICryFactoryRegistry.h>

#include "GameStartup.h"
#include "Game.h"
#include "resource.h"


// Set up statics
const char* CGameStartup::FRAMEWORK_DLL_NAME = "cryaction.dll";
const char* CGameStartup::WINDOW_CLASS_NAME = "CryENGINE";
IGameFramework* CGameStartup::m_pGameFramework = NULL;
IGame* CGameStartup::m_pGame = NULL;
ICemonoPtr CGameStartup::m_pCemono;
HMODULE CGameStartup::m_gameFrameworkDll;
HMODULE CGameStartup::m_gameDll;
bool CGameStartup::m_initWindow = false;


CGameStartup::CGameStartup()
{

}

CGameStartup::~CGameStartup()
{
	if (m_pGame)
	{
		m_pGame->Shutdown();
		m_pGame = NULL;
	}

	if (m_gameDll)
	{
		CryFreeLibrary(m_gameDll);
		m_gameDll = NULL;
	}

	ShutdownFramework();
}

IGameRef CGameStartup::Init(SSystemInitParams &startupParams)
{
	if (!InitFramework(startupParams))
	{
		// No need for logging, InitFramework already describes the error
		return NULL;
	}

	ISystem* pSystem = m_pGameFramework->GetISystem();
	IConsole* pConsole = gEnv->pConsole;
	startupParams.pSystem = pSystem;

	IGameRef pOut = Reset();

	if (!m_pGameFramework->CompleteInit())
	{
		pOut->Shutdown();
		return NULL;
	}

	if (startupParams.bExecuteCommandLine)
		pSystem->ExecuteCommandLine();

	if (!InitCemono())
	{
		return NULL;
	}


	return pOut;
}

IGameRef CGameStartup::Reset()
{
	if (m_pGame)
	{
		m_pGame->Shutdown();

		if (m_gameDll)
		{
			CryFreeLibrary(m_gameDll);
			m_gameDll = NULL;
		}
	}

	ModuleInitISystem(m_pGameFramework->GetISystem(),"CryGame");
	static char pGameBuffer[sizeof(CGame)];
	m_pGame = new ((void*)pGameBuffer) CGame();

	if (m_pGame && m_pGame->Init(m_pGameFramework))
	{
		return &m_pGame;
	}
	return NULL;
}

void CGameStartup::Shutdown()
{
	this->~CGameStartup();
}

int CGameStartup::Update(bool haveFocus, unsigned int updateFlags)
{
	int returnCode = 0;

	if (gEnv && gEnv->pSystem && gEnv->pConsole)
	{
		if (gEnv->pRenderer && gEnv->pRenderer->GetHWND())
		{
			bool focus = (::GetFocus() == gEnv->pRenderer->GetHWND());
			static bool focused = focus;
			if (focus != focused)
			{
				if(gEnv->pSystem->GetISystemEventDispatcher())
				{
					gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, focus, 0);
				}
				focused = focus;
			}
		}
	}

	// Update game
	if (m_pGame)
	{
		returnCode = m_pGame->Update(haveFocus, updateFlags);
	}
		// ghetto fullscreen detection, because renderer does not provide any kind of listener
	if (gEnv && gEnv->pSystem && gEnv->pConsole)
	{
		ICVar *pVar = gEnv->pConsole->GetCVar("r_Fullscreen");
		if (pVar)
		{
			static int fullscreen = pVar->GetIVal();
			if (fullscreen != pVar->GetIVal())
			{
				if(gEnv->pSystem->GetISystemEventDispatcher())
				{
					gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_TOGGLE_FULLSCREEN, pVar->GetIVal(), 0);
				}
				fullscreen = pVar->GetIVal();
			}
		}
	}

	return returnCode;
}

bool CGameStartup::GetRestartLevel(char** levelName)
{
	if(GetISystem()->IsRelaunch())
		*levelName = (char*)(gEnv->pGame->GetIGameFramework()->GetLevelName());
	return GetISystem()->IsRelaunch();
}

const char* CGameStartup::GetPatch() const
{
	// NOT SUPPORTED
	return NULL; 
}

bool CGameStartup::GetRestartMod(char* pModName, int nameLenMax)
{
	return true;
}

int CGameStartup::Run( const char * autoStartLevelName )
{
	return 0;
}



bool CGameStartup::InitWindow(SSystemInitParams &startupParams)
{
	WNDCLASS wc;

	memset(&wc, 0, sizeof(WNDCLASS));

	wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = (WNDPROC)CGameStartup::WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(0);
	wc.hIcon         = LoadIcon((HINSTANCE)startupParams.hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor       = LoadCursor((HINSTANCE)startupParams.hInstance, MAKEINTRESOURCE(IDC_CURSOR));
	wc.hbrBackground =(HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&wc))
	{
		return false;
	}

	if (startupParams.pSystem == NULL || (!startupParams.bEditor && !gEnv->IsDedicated()))
		::ShowCursor(FALSE);

	return true;
}

void CGameStartup::ShutdownWindow()
{
	if (m_initWindow)
	{
		UnregisterClass(WINDOW_CLASS_NAME, GetModuleHandle(0));
	}
}
bool CGameStartup::InitFramework(SSystemInitParams &startupParams)
{
	m_gameFrameworkDll = GetFrameworkDll(startupParams.szBinariesDir);
	if (!m_gameFrameworkDll)
	{
		CryFatalError("Failed to open the GameFramework dll");
		return false;
	}

	IGameFramework::TEntryFunction CreateGameFramework = (IGameFramework::TEntryFunction)CryGetProcAddress(m_gameFrameworkDll, "CreateGameFramework" );
	if (!CreateGameFramework)
	{
		CryFatalError("Specified GameFramework DLL is not valid!");
		return false;
	}

	m_pGameFramework = CreateGameFramework();
	if (!m_pGameFramework)
	{
		CryFatalError("Failed to create the GameFramework interface");
		return false;
	}

	if (!startupParams.hWnd)
	{
		m_initWindow = true;
		if (!InitWindow(startupParams))
		{
			CryFatalError("Failed to register CryENGINE window class!");
			return false;
		}
	}

	// Init engine
	if (!m_pGameFramework->Init(startupParams))
	{
		CryFatalError("Failed to initialize CryENGINE!");
		return false;
	}
	ModuleInitISystem(m_pGameFramework->GetISystem(),"CryGame");

	return true;
}

LRESULT CALLBACK CGameStartup::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		if (gEnv && gEnv->pSystem)
			gEnv->pSystem->Quit();
		return 0;
	case WM_MOUSEACTIVATE:
		return MA_ACTIVATEANDEAT;
	case WM_ENTERSIZEMOVE:
		if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIHardwareMouse())
		{
			gEnv->pSystem->GetIHardwareMouse()->IncrementCounter();
		}
		return  0;
	case WM_EXITSIZEMOVE:
		if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIHardwareMouse())
		{
			gEnv->pSystem->GetIHardwareMouse()->DecrementCounter();
		}
		return  0;
	case WM_ENTERMENULOOP:
		if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIHardwareMouse())
		{
			gEnv->pSystem->GetIHardwareMouse()->IncrementCounter();
		}
		return  0;
	case WM_EXITMENULOOP:
		if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIHardwareMouse())
		{
			gEnv->pSystem->GetIHardwareMouse()->DecrementCounter();
		}
		return  0;
	case WM_HOTKEY:
	case WM_SYSCHAR:	// prevent ALT + key combinations from creating 'ding' sounds
		return  0;
	case WM_CHAR:
		{
			if (gEnv && gEnv->pInput)
			{
				SInputEvent event;
				event.modifiers = gEnv->pInput->GetModifiers();
				event.deviceId = eDI_Keyboard;
				event.state = eIS_UI;
				event.value = 1.0f;
				event.pSymbol = 0;//m_rawKeyboard->GetSymbol((lParam>>16)&0xff);
				if (event.pSymbol)
					event.keyId = event.pSymbol->keyId;

				wchar_t tmp[2] = { 0 };
				MultiByteToWideChar(CP_ACP, 0, (char*)&wParam, 1, tmp, 2);
				event.timestamp = tmp[0];

				char szKeyName[4] = {0};
				if (wctomb(szKeyName, (WCHAR)wParam) != -1)
				{
					if (szKeyName[1]==0 && ((unsigned char)szKeyName[0])>=32)
					{
						event.keyName = szKeyName;
						gEnv->pInput->PostInputEvent(event);
					}
				}
			}
		}
		break;
	case WM_SYSKEYDOWN:	// prevent ALT-key entering menu loop
			if (wParam != VK_RETURN && wParam != VK_F4)
			{
				return 0;
			}
			else
			{
				if (wParam == VK_RETURN)	// toggle fullscreen
				{
					if (gEnv && gEnv->pRenderer && gEnv->pRenderer->GetRenderType() != eRT_DX11)
					{
						ICVar *pVar = gEnv->pConsole->GetCVar("r_Fullscreen");
						if (pVar)
						{
							int fullscreen = pVar->GetIVal();
							pVar->Set((int)(fullscreen == 0));
						}
					}
				}
				// let the F4 pass through to default handler (it will send an WM_CLOSE)
			}
		break;
	case WM_SETCURSOR:
		if(g_pGame)
		{
			HCURSOR hCursor = LoadCursor(GetModuleHandle(0),MAKEINTRESOURCE(IDC_CURSOR));
			::SetCursor(hCursor);
		}
		return 0;
	case WM_MOUSEMOVE:
		if(gEnv && gEnv->pHardwareMouse)
		{
			gEnv->pHardwareMouse->Event(LOWORD(lParam),HIWORD(lParam),HARDWAREMOUSEEVENT_MOVE);
		}
		return 0;
	case WM_LBUTTONDOWN:
		if(gEnv && gEnv->pHardwareMouse)
		{
			gEnv->pHardwareMouse->Event(LOWORD(lParam),HIWORD(lParam),HARDWAREMOUSEEVENT_LBUTTONDOWN);
		}
		return 0;
	case WM_LBUTTONUP:
		if(gEnv && gEnv->pHardwareMouse)
		{
			gEnv->pHardwareMouse->Event(LOWORD(lParam),HIWORD(lParam),HARDWAREMOUSEEVENT_LBUTTONUP);
		}
		return 0;
	case WM_LBUTTONDBLCLK:
		if(gEnv && gEnv->pHardwareMouse)
		{
			gEnv->pHardwareMouse->Event(LOWORD(lParam),HIWORD(lParam),HARDWAREMOUSEEVENT_LBUTTONDOUBLECLICK);
		}
		return 0;
	case WM_MOVE:
		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_MOVE,LOWORD(lParam), HIWORD(lParam));
		}
	return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_SIZE:
		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RESIZE,LOWORD(lParam), HIWORD(lParam));
		}
	return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_ACTIVATE:

		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			//gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, LOWORD(wParam) != WA_INACTIVE, 0);
		}
		break;
	case WM_SETFOCUS:
		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			//gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, 1, 0);
		}
		break;
	case WM_KILLFOCUS:
		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			//gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, 0, 0);
		}
		break;
  case WM_WINDOWPOSCHANGED:
		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			//gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, 1, 0);
		}
		break;
  case WM_STYLECHANGED:
	if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
	{
	  gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_CHANGE_FOCUS, 1, 0);
	}
		break;
	case WM_INPUTLANGCHANGE:
		if(gEnv && gEnv->pSystem && gEnv->pSystem->GetISystemEventDispatcher())
		{
			gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LANGUAGE_CHANGE, wParam, lParam);
		}
		break;
  }

	return DefWindowProc(hWnd, msg, wParam, lParam);
}




void CGameStartup::ShutdownFramework()
{
	if (m_pGameFramework)
	{
		m_pGameFramework->Shutdown();
		m_pGameFramework = NULL;
	}

	ShutdownWindow();
}

HMODULE CGameStartup::GetFrameworkDll(const char* binariesDirectory)
{
	if (m_gameFrameworkDll)
		return m_gameFrameworkDll;

	HMODULE frameworkDll = NULL;

	string dllName;
	if (binariesDirectory && binariesDirectory[0])
	{
		dllName = PathUtil::Make(binariesDirectory, FRAMEWORK_DLL_NAME);
	} else {
		dllName = FRAMEWORK_DLL_NAME;
	}
	frameworkDll = CryLoadLibrary(dllName.c_str());

	return frameworkDll;
}


void CGameStartup::CleanupFrameworkDll()
{
	CryFreeLibrary( m_gameFrameworkDll );
	m_gameFrameworkDll = NULL;

}

bool CGameStartup::InitCemono()
{
	bool result = false;

	auto cemonoDll = CryLoadLibrary("cemono.dll");
	
	ICemono::TEntryFunction InitCemono = (ICemono::TEntryFunction)CryGetProcAddress(cemonoDll, "InitCemono");
	if (!InitCemono)
	{
		CryFatalError("Specified Cemono DLL is not valid!");
		return false;
	}

	InitCemono(gEnv->pSystem);
	if (!CryCreateClassInstance("Cemono", m_pCemono))
	{
		CryFatalError("Failed to intialize Cemono");
		return false;
	}

	if (m_pCemono)
	{
		result = m_pCemono->Init();
	}

	return result;
}