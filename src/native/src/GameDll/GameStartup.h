#pragma once

#include <IGameFramework.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#endif


class CGameStartup : public IGameStartup
{
public:
	CGameStartup();
	virtual ~CGameStartup();

	// IGameStartup interface
	virtual IGameRef Init(SSystemInitParams &startupParams);
	virtual void Shutdown();
	virtual int Update(bool haveFocus, unsigned int updateFlags);
	virtual bool GetRestartLevel(char** levelName);
	virtual const char* GetPatch() const;
	virtual bool GetRestartMod(char* pModName, int nameLenMax);
	virtual int Run( const char * autoStartLevelName );
	// -IGameStartup

	virtual IGameRef Reset();
	static HMODULE GetFrameworkDll(const char* binariesDirectory);
	static void CleanupFrameworkDll();

private:
	static bool InitWindow(SSystemInitParams &startupParams);
	static void ShutdownWindow();
	static bool InitFramework(SSystemInitParams &startupParams);
	static void ShutdownFramework();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	static const char* FRAMEWORK_DLL_NAME;
	static const char* WINDOW_CLASS_NAME;
	static bool m_initWindow;
	static IGameFramework* m_pGameFramework;
	static HMODULE m_gameFrameworkDll;
	static HMODULE m_gameDll;
	static IGame* m_pGame;

};