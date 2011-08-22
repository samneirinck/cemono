#include DEVIRTUALIZE_HEADER_FIX(ILog.h)

#ifndef _ILOG_H_
#define _ILOG_H_

#include "ProjectDefines.h"

#include <platform.h>



#include "IMiniLog.h"

// forward declarations
class ICrySizer;

// Summary:
//	 Callback interface to the ILog.
struct ILogCallback
{
	virtual ~ILogCallback(){}
	virtual void OnWriteToConsole( const char *sText,bool bNewLine ) = 0;
	virtual void OnWriteToFile( const char *sText,bool bNewLine ) = 0;
};

// Summary:
//	 Interface for logging operations based on IMiniLog.
// Notes:
//	 Logging in CryEngine should be done using the following global functions:
//		CryLog (eMessage)
//		CryLogAlways (eAlways)
//		CryError (eError)
//		CryWarning (eWarning)
//		CryComment (eComment) 
//	 ILog gives you more control on logging operations.
// See also:
//	 IMiniLog, CryLog, CryLogAlways, CryError, CryWarning
UNIQUE_IFACE struct ILog: public IMiniLog
{
	virtual void Release() = 0;

	// Summary:
	//	 Sets the file used to log to disk.
	virtual bool	SetFileName(const char *command = NULL) = 0;

	// Summary:
	//	 Gets the filename used to log to disk.
	virtual const char*	GetFileName() = 0;

	// Summary:
	//	 Gets the filename where the current log backup was copied to on disk
	virtual const char* GetBackupFileName() = 0;

	//all the following functions will be removed are here just to be able to compile the project ---------------------------

	// Summary:
	//	 Logs the text both to file and console.
	virtual void	Log(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	virtual void	LogAlways(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	virtual void	LogWarning(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	virtual void	LogError(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	// Summary:
	//	 Logs the text both to the end of file and console.
	virtual void	LogPlus(const char *command,...) PRINTF_PARAMS(2, 3) = 0;	

	// Summary:
	//	 Logs to the file specified in SetFileName.
	// See also:
	//	 SetFileName
	virtual void	LogToFile(const char *command,...) PRINTF_PARAMS(2, 3) = 0;	

	//
	virtual void	LogToFilePlus(const char *command,...) PRINTF_PARAMS(2, 3) = 0;

	// Summary:
	//	 Logs to console only.
	virtual void	LogToConsole(const char *command,...) PRINTF_PARAMS(2, 3) = 0;

	//
	virtual void	LogToConsolePlus(const char *command,...) PRINTF_PARAMS(2, 3) = 0;

	//
	virtual void	UpdateLoadingScreen(const char *command,...) PRINTF_PARAMS(2, 3) = 0;	

	//
	virtual void RegisterConsoleVariables() {}

	//
	virtual void UnregisterConsoleVariables() {}

	// Notes:
	//	 Full logging (to console and file) can be enabled with verbosity 4.
	//	 In the console 'log_Verbosity 4' command can be used.
	virtual void	SetVerbosity( int verbosity ) = 0;

	virtual int		GetVerbosityLevel()=0;

	virtual void  AddCallback( ILogCallback *pCallback ) = 0;
	virtual void  RemoveCallback( ILogCallback *pCallback ) = 0;

	// Notes:
	//	 The function called every frame by system.
	virtual void Update() = 0;

	virtual const char* GetModuleFilter() = 0;
	
	// Notes:
	//	 Collect memory statistics in CrySizer
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const = 0;

#if !defined(_RELEASE) && !defined(RESOURCE_COMPILER)
	virtual void Indent(class CLogIndenter * indenter) = 0;
	virtual void Unindent(class CLogIndenter * indenter) = 0;
#endif

	DEVIRTUALIZATION_VTABLE_FIX
};

#if defined(_RELEASE) || defined(RESOURCE_COMPILER)
#define INDENT_LOG_DURING_SCOPE(...) (void)(0)
#else
class CLogIndenter
{
	public:
	CLogIndenter(ILog * log) : m_log(log), m_enabled(false), m_nextIndenter(NULL)
	{
	}

	void Enable(bool enable = true)
	{
		enable &= (m_log != NULL);

		if (enable != m_enabled && m_log)
		{
			if (enable)
			{
				m_log->Indent(this);
			}
			else
			{
				m_log->Unindent(this);
			}
			m_enabled = enable;
		}
	}

	CLogIndenter * GetNextIndenter()
	{
		return m_nextIndenter;
	}

	void SetNextIndenter(CLogIndenter * indenter)
	{
		m_nextIndenter = indenter;
	}

	~CLogIndenter()
	{
		Enable(false);
	}

	private:
	bool m_enabled;
	ILog * m_log;
	CLogIndenter * m_nextIndenter;
};

#define ILOG_CONCAT_IMPL( x, y ) x##y
#define ILOG_CONCAT_MACRO( x, y ) ILOG_CONCAT_IMPL( x, y )
#define INDENT_LOG_DURING_SCOPE(...) CLogIndenter ILOG_CONCAT_MACRO(indentMe, __LINE__) ((CryGetCurrentThreadId() == gEnv->mMainThreadId) ? gEnv->pLog : NULL); ILOG_CONCAT_MACRO(indentMe, __LINE__).Enable(__VA_ARGS__)
#endif


#endif //_ILOG_H_



