////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ResourceCompilerHelper.h
//  Version:     v1.00
//  Created:     12/07/2004 by MartinM.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef NOT_USE_CRY_MEMORY_MANAGER
	#include "CryModuleDefs.h"												// ECryModule
#endif

#ifndef __RESOURCECOMPILERHELPER_H__
#define __RESOURCECOMPILERHELPER_H__
#pragma once

//#ifndef NOT_USE_CRY_STRING
//	#include <platform.h>														// string
//#endif

#include <map>


#if defined(WIN32) || defined(WIN64)
//#include <windows.h>														// HWND
#endif

#include "EngineSettingsManager.h"


class IResourceCompilerListener
{
public:
	enum MessageSeverity
	{
		MessageSeverity_Debug,
		MessageSeverity_Info,
		MessageSeverity_Warning,
		MessageSeverity_Error
	};
	virtual void OnRCMessage(MessageSeverity severity, const char* text) = 0;
	virtual ~IResourceCompilerListener(){}
};

enum ERcExitCode
{
	eRcExitCode_Success = 0,   // must be 0
	eRcExitCode_Error = 1,
	eRcExitCode_FatalError = 100,
	eRcExitCode_UserFixing = 200,
};

//////////////////////////////////////////////////////////////////////////
// Provides settings and functions to make calls to RC.
class CResourceCompilerHelper
{
public:
	enum ERcCallResult
	{
		eRcCallResult_success,
		eRcCallResult_notFound,
		eRcCallResult_error
	};

	enum ERcExePath
	{
		eRcExePath_currentFolder,
		eRcExePath_registry,
		eRcExePath_settingsManager
	};

#if (defined(WIN32) || defined(WIN64))				// run compiler only on developer platform

	// Loads EngineSettingsManager to get settings information
	CResourceCompilerHelper(const TCHAR* moduleName=NULL);

	~CResourceCompilerHelper();

#if defined(_ICRY_PAK_HDR_)

	// checks file date and existence
	// Return:
	//   destination filename that should be loaded
	string ProcessIfNeeded( const TCHAR *szFilePath )
	{
		string sFileToLoad=szFilePath;

		string ext = GetExt(sFileToLoad);

		string sDestFile = GetOutputFilename(sFileToLoad.c_str());

		for(uint32 dwIndex=0;;++dwIndex)		// check for all input files
		{
			string sSrcFile = GetInputFilename(sFileToLoad.c_str(),dwIndex);

			if(sSrcFile.empty())
				break;					// last input file

			// compile if there is no destination
			// compare date of destination and source , recompile if needed
			// load dds header, check hash-value of the compile settings in the dds file, recompile if needed (not done yet)

			CDebugAllowFileAccess dafa;
			FILE* pDestFile = gEnv->pCryPak->FOpen(sDestFile.c_str(),_T("rb"));
			FILE* pSrcFile = gEnv->pCryPak->FOpen(sSrcFile.c_str(),_T("rb"));
			dafa.End();

			// files from the pak file do not count as date comparison do not seem to work there
			if(pDestFile)
			if(gEnv->pCryPak->IsInPak(pDestFile))
			{
				gEnv->pCryPak->FClose(pDestFile);pDestFile=0;
			}

			bool bInvokeResourceCompiler=false;

			// is there no destination file?
			if(pSrcFile && !pDestFile)
					bInvokeResourceCompiler=true;

			// if both files exist, is the source file newer?
			if(pDestFile && pSrcFile)
			{
				bInvokeResourceCompiler=true;

				ICryPak::FileTime timeSrc = gEnv->pCryPak->GetModificationTime(pSrcFile);
				ICryPak::FileTime timeDest = gEnv->pCryPak->GetModificationTime(pDestFile);

				if(timeDest>=timeSrc)
					bInvokeResourceCompiler=false;
			}

			if(pDestFile)
			{
				gEnv->pCryPak->FClose(pDestFile);pDestFile=0;
			}
			if(pSrcFile)
			{
				gEnv->pCryPak->FClose(pSrcFile);pSrcFile=0;
			}

			if(bInvokeResourceCompiler)
			{
				// Adjust filename so that they are global.
				char sFullSrcFilename[MAX_PATH];
				char sFullTrgFilename[MAX_PATH];
				gEnv->pCryPak->AdjustFileName( sSrcFile.c_str(),sFullSrcFilename,0 );
				gEnv->pCryPak->AdjustFileName( sDestFile.c_str(),sFullTrgFilename,0 );
				// call rc.exe
				if(!InvokeResourceCompiler(sFullSrcFilename,sFullTrgFilename,"",false))		// false=no window
				{
					m_bErrorFlag=true;
					assert(!pSrcFile);assert(!pDestFile);		// internal error
					return szFilePath;	// rc failed
				}
			}

			assert(!pSrcFile);assert(!pDestFile);		// internal error
		}
		return sDestFile;	// load without using RC (e.g. TGA)
	}

#endif // _ICRY_PAK_HDR_

	tstring GetRootPath(bool pullFromRegistry);

	void ResourceCompilerUI( void* hParent );

	// Arguments:
	//   szExtension e.g. "tif"
	bool IsSourceFormat( const TCHAR *szExtension ) const;

	// Arguments:
	//   szExtension e.g. "dds"
	bool IsDestinationFormat( const TCHAR *szExtension ) const;

	bool IsPrefer32Bit();

	CEngineSettingsManager* GetSettingsManager();

	tstring GetEditorExecutable();

	bool IsError() const;

	//
	// Arguments:
	//   szFileName null terminated ABSOLUTE file path or 0 can be used to test for rc.exe existance, relative path needs to be relative to bin32/rc directory
	//   szAdditionalSettings - 0 or e.g. "/refresh" or "/refresh /xyz=56"
	//
	ERcCallResult CallResourceCompiler(
		const TCHAR* szFileName=NULL, 
		const TCHAR* szAdditionalSettings=NULL, 
		IResourceCompilerListener* listener=NULL, 
		bool bMayShowWindow=true, 
		bool bUseQuota=true, 
		ERcExePath rcExePath=eRcExePath_registry, 
		bool bSilent=false,
		bool bNoUserDialog=false,
		const TCHAR* szWorkingDirectory=NULL);

	void* CallEditor( void* hParent, const TCHAR * pWndName, const TCHAR * pFlag );

private:
	bool m_bErrorFlag;

private:
	// Arguments:
	//   szFilePath - could be source or destination filename
	//   dwIndex - used to iterator through all input filenames, start with 0 and increment by 1
	// Return:
	//   "" if that was the last input format, a valid filename otherwise
	tstring GetInputFilename( const TCHAR *szFilePath, const unsigned int dwIndex ) const;	

	// Arguments:
	//   szDataFolder usually DATA_FOLDER = "Game"
	bool InvokeResourceCompiler( const TCHAR *szSrcFile, const TCHAR *szDestFile, const TCHAR *szDataFolder, const bool bWindow ) const; 

	// parses a file and stores all flags in a private key-value-map
	bool LoadValuesFromConfigFile(const TCHAR* szFileName);

#endif // defined(WIN32) && !defined(WIN64)

public:
	// little helper function (to stay independent)
	static const TCHAR* GetExtension( const TCHAR *in )
	{
		const size_t len = _tcsclen(in);
		for(const TCHAR* p = in + len-1; p >= in; --p)
		{
			switch(*p)
			{
			case ':':
			case '/':
			case '\\':
				// we've reached a path separator - it means there's no extension in this name
				return 0;
			case '.':
				// there's an extension in this file name
				return p+1;
			}
		}
		return 0;
	}

	// little helper function (to stay independent)
	static tstring ReplaceExtension( const TCHAR *path, const TCHAR *new_ext )
	{
		const TCHAR* const ext = GetExtension(path);

		if(ext)
			return tstring(path,ext)+new_ext;
		else
			return tstring(path)+_T(".")+new_ext;
	}

	// Arguments:
	//   szFilePath - could be source or destination filename
	static tstring GetOutputFilename( const TCHAR *szFilePath )
	{
		const TCHAR* const ext = GetExtension(szFilePath);

		if(ext)
		{
			if(_tcsicmp(ext,_T("tif"))==0)
				return ReplaceExtension(szFilePath,_T("dds"));
			if(_tcsicmp(ext,_T("srf"))==0)
				return ReplaceExtension(szFilePath,_T("dds"));
		}

		return szFilePath;
	}

	// only for image formats supported by the resource compiler
	// Arguments:
	//   szExtension - e.g. ".tif", can be 0
	static bool IsImageFormat( const TCHAR *szExtension )
	{
		if(szExtension)
		{
			if(   _tcsicmp(szExtension,_T("dds"))==0    // DirectX surface format
			   || _tcsicmp(szExtension,_T("tif"))==0)   // Crytek resource compiler image input format	
			{
				return true;
			}
		}

		return false;
	}

	//! Extract extension from full specified file path (copy to keep ResourceCompilerHelper independent from other code - needed as it is used in many projects)
	inline static tstring GetExt( const tstring &filepath )
	{
		const TCHAR* const ext = GetExtension(filepath.c_str());

		return ext ? tstring(ext) : tstring();
	}
};


#endif // __RESOURCECOMPILERHELPER_H__
