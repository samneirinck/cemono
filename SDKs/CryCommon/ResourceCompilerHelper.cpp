// #include <CryModuleDefs.h>

#include "StdAfx.h"

#undef eCryModule
#define eCryModule eCryM_System

#undef RC_EXECUTABLE
#define RC_EXECUTABLE			"rc.exe"

#include "ResourceCompilerHelper.h"
#include "LineStreamBuffer.h"


#pragma warning (disable:4312)


#if defined(WIN32) || defined(WIN64)
#include <windows.h>		
#include <shellapi.h> //ShellExecute()

// pseudo-variable that represents the DOS header of the module
EXTERN_C IMAGE_DOS_HEADER __ImageBase;


//static bool g_bWindowQuit;
//static CResourceCompilerHelper *g_pThis=0;
/*
static const uint32 IDC_hWndRCPath	= 100;
static const uint32 IDC_hWndPickRCPath	=	101;
static const uint32 IDC_hWndTest	=	102;
static const uint32 IDC_hBtnShowWindow	=	103;
static const uint32 IDC_hBtnHideCustom	=	104;
static const uint32 IDC_hBtnPrefer32Bit	=	105;
static const uint32 IDC_hBtnStatic	=	106;
*/

static CEngineSettingsManager *g_pSettingsManager = 0;

//////////////////////////////////////////////////////////////////////////
CResourceCompilerHelper::CResourceCompilerHelper(const TCHAR* moduleName) : 
	m_bErrorFlag(false)
{
	if (!g_pSettingsManager)
	{
		// Can create memory leak here
		g_pSettingsManager = new CEngineSettingsManager(moduleName);
	}
}


//////////////////////////////////////////////////////////////////////////
CResourceCompilerHelper::~CResourceCompilerHelper()
{
}


//////////////////////////////////////////////////////////////////////////
tstring CResourceCompilerHelper::GetRootPath(bool pullFromRegistry)
{
	return pullFromRegistry ? g_pSettingsManager->GetRootPath() : g_pSettingsManager->GetValue<tstring>(_T("ENG_RootPath"));
}

//////////////////////////////////////////////////////////////////////////
void CResourceCompilerHelper::ResourceCompilerUI( void* hParent )
{
	g_pSettingsManager->CallSettingsDialog(hParent);
}


class ResourceCompilerLineHandler
{
public:
	ResourceCompilerLineHandler(IResourceCompilerListener* listener): m_listener(listener) {}
	void HandleLine(const char* line)
	{
		if (m_listener && line)
		{
			// Check the first character to see if it's a warning or error.
			IResourceCompilerListener::MessageSeverity severity = IResourceCompilerListener::MessageSeverity_Info;

			if ((line[0] == 'E') && (line[1]==':'))
			{
				line += 2;  // skip the prefix
				severity = IResourceCompilerListener::MessageSeverity_Error;
			}
			else if ((line[0] == 'W') && (line[1]==':'))
			{
				line += 2;  // skip the prefix
				severity = IResourceCompilerListener::MessageSeverity_Warning;
			}
			else if ((line[0] == ' ') && (line[1]==' '))
			{
				line += 2;  // skip the prefix
			}

			// skip thread prefix
			while(*line == ' ')
			{
				++line;
			}
			while(isdigit(*line))
			{
				++line;
			}
			if (*line == '>')
			{
				++line;
			}

			// skip time
			while(*line == ' ')
			{
				++line;
			}
			while(isdigit(*line))
			{
				++line;
			}
			if(*line == ':')
			{
				++line;
			}
			while(isdigit(*line))
			{
				++line;
			}
			if(*line == ' ')
			{
				++line;
			}

			m_listener->OnRCMessage(severity, line);
		}
	}

private:
	IResourceCompilerListener* m_listener;
};


//////////////////////////////////////////////////////////////////////////
CResourceCompilerHelper::ERcCallResult CResourceCompilerHelper::CallResourceCompiler(
	const TCHAR* szFileName, 
	const TCHAR* szAdditionalSettings, 
	IResourceCompilerListener* listener, 
	bool bMayShowWindow, 
	bool bUseQuota, 
	CResourceCompilerHelper::ERcExePath rcExePath, 
	bool bSilent,
	bool bNoUserDialog,
	const TCHAR *szWorkingDirectory)
{
	// make command for execution
	TCHAR szRemoteCmdLine[MAX_PATH*3];

	if (!szAdditionalSettings)
	{
		szAdditionalSettings = _T("");
	}

	tstring path;
	switch (rcExePath)
	{
	case eRcExePath_registry:
		path = GetRootPath(true);
		break;
	case eRcExePath_settingsManager:
		path = GetRootPath(false);
		break;
	case eRcExePath_currentFolder:
		path = _T(".");
		break;
	default:
		return eRcCallResult_notFound;
	}

	if (path.empty())
	{
		path = _T(".");
	}

	TCHAR szRemoteDirectory[512];
	_stprintf_s(szRemoteDirectory, _T("%s/Bin32/rc"), path.c_str());

	const TCHAR* const szHideCustom = ((g_pSettingsManager->GetValue<tstring>(_T("HideCustom"))==_T("true")) || bNoUserDialog) 
		? _T("") 
		: _T(" /userdialogcustom=0");

	if (!szFileName)
	{
		_stprintf_s(szRemoteCmdLine, _T("%s/rc.exe /userdialog=0 %s"), szRemoteDirectory, szAdditionalSettings);
	}
	else
	{
		const TCHAR* const szFormat = bUseQuota
			? _T("%s/rc.exe \"%s\" %s %s %s")
			: _T("%s/rc.exe %s %s %s %s");
		const TCHAR* const szUserDialog = bNoUserDialog
			? _T("/userdialog=0")
			: _T("/userdialog=1");
		_stprintf_s(szRemoteCmdLine, szFormat, szRemoteDirectory, szFileName, szUserDialog, szAdditionalSettings, szHideCustom);
	}

	// Create a pipe to read the stdout of the RC.
	SECURITY_ATTRIBUTES saAttr;
	std::memset(&saAttr, 0, sizeof(saAttr));
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = 0;
	HANDLE hChildStdOutRd, hChildStdOutWr;
	CreatePipe(&hChildStdOutRd, &hChildStdOutWr, &saAttr, 0);
	SetHandleInformation(hChildStdOutRd, HANDLE_FLAG_INHERIT, 0); // Need to do this according to MSDN
	HANDLE hChildStdInRd, hChildStdInWr;
	CreatePipe(&hChildStdInRd, &hChildStdInWr, &saAttr, 0);
	SetHandleInformation(hChildStdInWr, HANDLE_FLAG_INHERIT, 0); // Need to do this according to MSDN

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwX = 100;
	si.dwY = 100;
	si.hStdError = hChildStdOutWr;
	si.hStdOutput = hChildStdOutWr;
	si.hStdInput = hChildStdInRd;
	si.dwFlags = STARTF_USEPOSITION | STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(
		NULL,                   // No module name (use command line).
		szRemoteCmdLine,        // Command line.
		NULL,                   // Process handle not inheritable.
		NULL,                   // Thread handle not inheritable.
		TRUE,                   // Set handle inheritance to TRUE.
		bMayShowWindow && (g_pSettingsManager->GetValue<tstring>(_T("ShowWindow"))==_T("true"))?0:CREATE_NO_WINDOW, // creation flags.
		NULL,                   // Use parent's environment block.
		szWorkingDirectory?szWorkingDirectory:szRemoteDirectory,  // Set starting directory.
		&si,                    // Pointer to STARTUPINFO structure.
		&pi ))                  // Pointer to PROCESS_INFORMATION structure.
	{
		TCHAR szMessage[65535] = _T("");
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szMessage, 65354, NULL);
		GetCurrentDirectory(65534, szMessage);
		if (!bSilent)
		{
			MessageBox(0,_T("ResourceCompiler was not found.\n\nPlease verify CryENGINE RootPath."),_T("Error"),MB_ICONERROR|MB_OK);
		}
		return eRcCallResult_notFound;
	}

	// Close the pipe that writes to the child process, since we don't actually have any input for it.
	CloseHandle(hChildStdInWr);

	// Read all the output from the child process.
	CloseHandle(hChildStdOutWr);
	ResourceCompilerLineHandler lineHandler(listener);
	LineStreamBuffer lineBuffer(&lineHandler, &ResourceCompilerLineHandler::HandleLine);
	for (;;)
	{
		char buffer[2048];
		DWORD bytesRead;
		if (!ReadFile(hChildStdOutRd, buffer, sizeof(buffer), &bytesRead, NULL) || (bytesRead == 0))
		{
			break;
		}
		lineBuffer.HandleText(buffer, bytesRead);
	} 

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	bool ok = true;
	{
		DWORD exitCode = 1;
		if ((GetExitCodeProcess(pi.hProcess, &exitCode) == 0) || (exitCode != 0))
		{
			ok = false;
		}	
	}

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return ok ? eRcCallResult_success : eRcCallResult_error;
}


//////////////////////////////////////////////////////////////////////////
bool CResourceCompilerHelper::IsPrefer32Bit()
{ 
	return g_pSettingsManager->GetValue<tstring>(_T("EDT_Prefer32Bit"))==_T("true"); 
}


//////////////////////////////////////////////////////////////////////////
bool CResourceCompilerHelper::InvokeResourceCompiler( const TCHAR *szSrcFile, const TCHAR *szDestFile, const TCHAR *szDataFolder, const bool bWindow ) const
{
	bool bRet=true;

	// make command for execution
	TCHAR szRemoteCmdLine[512];
	TCHAR szMasterCDDir[512];
	TCHAR szDir[512];

	GetCurrentDirectory(512,szMasterCDDir);

	if (_tcslen(szDataFolder) > 0)
	{
		const bool bDataFolderIsAbsolute = 
			(szDataFolder[0] == _T('\\')) || 
			(szDataFolder[0] == _T('/')) || 
			((szDataFolder[0] != 0) && (szDataFolder[1] == _T(':')));

		if (bDataFolderIsAbsolute)
		{
			_stprintf_s(szRemoteCmdLine, _T("Bin32/rc/%s \"%s/%s\" /userdialog=0"), RC_EXECUTABLE, szDataFolder,szSrcFile);
		}
		else
		{
			_stprintf_s(szRemoteCmdLine, _T("Bin32/rc/%s \"%s/%s/%s\" /userdialog=0"), RC_EXECUTABLE, szMasterCDDir,szDataFolder,szSrcFile);
		}
	}
	else
	{
		_stprintf_s(szRemoteCmdLine, _T("Bin32/rc/%s \"%s\\%s\" /userdialog=0"), RC_EXECUTABLE, szMasterCDDir,szSrcFile);
	}

	_stprintf_s(szDir, _T("%s\\Bin32\\rc"), szMasterCDDir);

	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwX = 100;
	si.dwY = 100;
	si.dwFlags = STARTF_USEPOSITION;

	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );

	if( !CreateProcess( NULL, // No module name (use command line). 
		szRemoteCmdLine,				// Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		bWindow?0:CREATE_NO_WINDOW,	// creation flags. 
		NULL,             // Use parent's environment block. 
		szDir,					  // Set starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		bRet=false;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
tstring CResourceCompilerHelper::GetEditorExecutable()
{
	tstring editorExe = g_pSettingsManager->GetRootPath();

	if(editorExe.empty())
	{
		MessageBox(NULL, _T("Can't Find the Material Editor.\nPlease, setup correct CryENGINE root path in the engine settings dialog"), _T("Error"), MB_ICONERROR | MB_OK);
		ResourceCompilerUI(0);
		editorExe = g_pSettingsManager->GetRootPath();
		if(editorExe.empty())
			return editorExe;
	}

	if (g_pSettingsManager->GetValue<tstring>(_T("EDT_Prefer32Bit"))==_T("true"))
		editorExe += _T("/Bin32/Editor.exe");
	else
		editorExe += _T("/Bin64/Editor.exe");
	return editorExe;
}


//////////////////////////////////////////////////////////////////////////
bool CResourceCompilerHelper::IsError() const
{
	return m_bErrorFlag;
}


//////////////////////////////////////////////////////////////////////////
tstring CResourceCompilerHelper::GetInputFilename( const TCHAR *szFilePath, const unsigned int dwIndex ) const
{
	const TCHAR *ext = GetExtension(szFilePath);

	if(ext)
	{
		if(_tcsicmp(ext,_T("dds"))==0)
		{
			switch(dwIndex)
			{
				case 0: return ReplaceExtension(szFilePath,_T("tif"));	// index 0
//					case 1: return ReplaceExtension(szFilePath,".srf");	// index 1
				default: return _T("");	// last one
			}
		}
	}

	if(dwIndex)
		return _T("");				// last one

	return szFilePath;	// index 0
}


//////////////////////////////////////////////////////////////////////////
bool CResourceCompilerHelper::IsDestinationFormat( const TCHAR *szExtension ) const
{
	if(_tcsicmp(szExtension,_T("dds"))==0)		// DirectX surface format
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////
bool CResourceCompilerHelper::IsSourceFormat( const TCHAR *szExtension ) const
{
	if(_tcsicmp(szExtension,_T("tif"))==0)			// Crytek resource compiler image input format
//		|| stricmp(szExtension,"srf")==0)		// Crytek surface formats (e.g. normalmap)
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////
void* CResourceCompilerHelper::CallEditor( void* pParent, const TCHAR * pWndName, const TCHAR * pFlag )
{
	HWND hWnd = ::FindWindow(NULL, pWndName);
	if(hWnd)
		return hWnd;
	else
	{
		tstring editorExecutable = GetEditorExecutable();

		if (editorExecutable!=_T(""))		
		{
			INT_PTR hIns = (INT_PTR)ShellExecute(NULL, _T("open"), editorExecutable.c_str(), pFlag, NULL, SW_SHOWNORMAL);
			if(hIns<=32)
			{
				MessageBox(0,_T("Editor.exe was not found.\n\nPlease verify CryENGINE root path."),_T("Error"),MB_ICONERROR|MB_OK);
				ResourceCompilerUI(pParent);
				editorExecutable = GetEditorExecutable();
				ShellExecute(NULL, _T("open"), editorExecutable.c_str(), pFlag, NULL, SW_SHOWNORMAL);
			}
		}
	}
	return 0;
}

CEngineSettingsManager* CResourceCompilerHelper::GetSettingsManager()
{
	return g_pSettingsManager;
}
#endif //(WIN32) || (WIN64)
