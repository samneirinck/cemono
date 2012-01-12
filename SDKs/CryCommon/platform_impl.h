////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   platform_impl.h
//  Version:     v1.00
//  Created:     23/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: This file should only be included Once in DLL module.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __platform_impl_h__
#define __platform_impl_h__
#pragma once

#include <platform.h>
#include <ISystem.h>
//#include <CryUnitTest.h>
#include <ITestSystem.h>
#include <CryExtension/Impl/RegFactoryNode.h>
#include <CryExtension/Impl/ICryFactoryRegistryImpl.h>
































#if defined(_LIB) && !defined(_LAUNCHER)
	extern CRndGen g_random_generator;



	extern SSystemGlobalEnvironment* gEnv;


#else //_LIB

//////////////////////////////////////////////////////////////////////////
// If not in static library.
#include <CryThreadImpl.h>
#include <CryCommon.cpp>



// this global environment variable must be initialized in each module!
#if !defined(PS3)
	SSystemGlobalEnvironment* gEnv = NULL;
#endif
/*
#ifdef CRY_STRING
int sEmptyStringBuffer[] = { -1, 0, 0, 0 };
template <>
string::StrHeader* string::m_emptyStringData = (string::StrHeader*)&sEmptyStringBuffer;
template <>
wstring::StrHeader* wstring::m_emptyStringData = (wstring::StrHeader*)&sEmptyStringBuffer;
#endif //CRY_STRING
*/

// Define UnitTest static variables
CryUnitTest::Test* CryUnitTest::Test::m_pFirst = 0;
CryUnitTest::Test* CryUnitTest::Test::m_pLast = 0;

#if !defined(PS3) && !(defined(XENON) && defined(_LIB))
	struct SRegFactoryNode* g_pHeadToRegFactories = 0;
#endif

#if defined(WIN32) || defined(WIN64) || defined(XENON)
void CryPureCallHandler()
{
	CryFatalError("Pure function call");
}

void CryInvalidParameterHandler(
	const wchar_t * expression,
	const wchar_t * function, 
	const wchar_t * file, 
	unsigned int line,
	uintptr_t pReserved
	)
{
	//size_t i;
	//char sFunc[128];
	//char sExpression[128];
	//char sFile[128];
	//wcstombs_s( &i,sFunc,sizeof(sFunc),function,_TRUNCATE );
	//wcstombs_s( &i,sExpression,sizeof(sExpression),expression,_TRUNCATE );
	//wcstombs_s( &i,sFile,sizeof(sFile),file,_TRUNCATE );
	//CryFatalError( "Invalid parameter detected in function %s. File: %s Line: %d, Expression: %s",sFunc,sFile,line,sExpression );
	CryFatalError( "Invalid parameter detected in CRT function" );
}

void InitCRTHandlers()
{
	_set_purecall_handler(CryPureCallHandler);
	_set_invalid_parameter_handler(CryInvalidParameterHandler);
}
#else
void InitCRTHandlers() {}
#endif

//////////////////////////////////////////////////////////////////////////
// This is an entry to DLL initialization function that must be called for each loaded module
//////////////////////////////////////////////////////////////////////////
extern "C" DLL_EXPORT void ModuleInitISystem( ISystem *pSystem,const char *moduleName )
{  
	if (gEnv) // Already registered.
		return;

	InitCRTHandlers();

#if !defined(PS3)
	if (pSystem) // DONT REMOVE THIS. ITS FOR RESOURCE COMPILER!!!!
	gEnv = pSystem->GetGlobalEnvironment();
#endif
#if !defined(PS3) && !(defined(XENON) && defined(_LIB))
	if (pSystem)
	{
		ICryFactoryRegistryImpl* pCryFactoryImpl = static_cast<ICryFactoryRegistryImpl*>(pSystem->GetCryFactoryRegistry());
		pCryFactoryImpl->RegisterFactories(g_pHeadToRegFactories);
	}
#endif
#ifdef CRY_UNIT_TESTING
	// Register All unit tests of this module.
	if(pSystem)
	{
		CryUnitTest::Test *pTest = CryUnitTest::Test::m_pFirst; 
		for (; pTest != 0; pTest = pTest->m_pNext)
		{
			CryUnitTest::IUnitTestManager *pTestManager = pSystem->GetITestSystem()->GetIUnitTestManager();
			if (pTestManager)
			{
				pTest->m_unitTestInfo.module = moduleName;
				pTestManager->CreateTest( pTest->m_unitTestInfo );
			}
		}
	}
#endif //CRY_UNIT_TESTING
}

bool g_bProfilerEnabled = false;
int g_iTraceAllocations = 0;

//////////////////////////////////////////////////////////////////////////
extern "C" {
	CRYSYSTEM_API unsigned int CryRandom(); // Exported by CrySystem
}

CRndGen g_random_generator;
uint32 cry_rand32()
{
	return g_random_generator.Generate();
}
unsigned int cry_rand()
{
	//return CryRandom(); // Return in range from 0 to RAND_MAX
	return g_random_generator.Generate() & RAND_MAX;
}
float cry_frand()
{
	return g_random_generator.GenerateFloat();
}

//////////////////////////////////////////////////////////////////////////

// when using STL Port _STLP_DEBUG and _STLP_DEBUG_TERMINATE - avoid actually 
// crashing (default terminator seems to kill the thread, which isn't nice).
#ifdef _STLP_DEBUG_TERMINATE

# ifdef __stl_debug_terminate
#  undef __stl_debug_terminate
# endif

void __stl_debug_terminate(void)
{
	assert( 0 && "STL Debug Error" );
}
#endif

#ifdef _STLP_DEBUG_MESSAGE

# ifdef __stl_debug_message
#  undef __stl_debug_message
# endif

void __stl_debug_message(const char * format_str, ...)
{	
	va_list __args;
	va_start( __args, format_str );
#ifdef WIN32
	char __buffer [4096];
	vsnprintf(__buffer, sizeof(__buffer) / sizeof(char), format_str, __args);
	OutputDebugStringA(__buffer);
#endif
	if(gEnv && gEnv->pLog) gEnv->pLog->LogV( ILog::eErrorAlways,format_str,__args );
	va_end(__args);
}
#endif //_STLP_DEBUG_MESSAGE


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#endif

#if defined(WIN32) || defined(WIN64)
#include <intrin.h>
#endif

// If we use cry memory manager this should be also included in every module.
#if defined(USING_CRY_MEMORY_MANAGER) && !defined(__SPU__)
	#include <CryMemoryManager_impl.h>
#endif

#if defined (_WIN32) || defined (XENON)

#include "CryAssert_impl.h"

//////////////////////////////////////////////////////////////////////////
void CryDebugBreak()
{
	DebugBreak();
}

//////////////////////////////////////////////////////////////////////////
void CrySleep( unsigned int dwMilliseconds )
{
	Sleep( dwMilliseconds );
}

//////////////////////////////////////////////////////////////////////////
int CryMessageBox( const char *lpText,const char *lpCaption,unsigned int uType)
{
#ifdef WIN32
	return MessageBox( NULL,lpText,lpCaption,uType );
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
int CryCreateDirectory( const char *lpPathName,void *lpSecurityAttributes )
{
	return CreateDirectory( lpPathName,(LPSECURITY_ATTRIBUTES)lpSecurityAttributes );
}

//////////////////////////////////////////////////////////////////////////
int CryGetCurrentDirectory( unsigned int nBufferLength,char *lpBuffer )
{
#ifdef WIN32
	return GetCurrentDirectory(nBufferLength,lpBuffer);
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
short CryGetAsyncKeyState( int vKey )
{
#ifdef WIN32
	return GetAsyncKeyState(vKey);
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
long  CryInterlockedIncrement( int volatile *lpAddend )
{
	return InterlockedIncrement((long*)lpAddend);
}

//////////////////////////////////////////////////////////////////////////
long  CryInterlockedDecrement( int volatile *lpAddend )
{
	return InterlockedDecrement((long*)lpAddend);
}

//////////////////////////////////////////////////////////////////////////
long	CryInterlockedExchangeAdd(long volatile * lpAddend, long Value)
{
	return InterlockedExchangeAdd(lpAddend, Value);
}

long	CryInterlockedCompareExchange(long volatile * dst, long exchange, long comperand)
{
	return InterlockedCompareExchange(dst, exchange, comperand);
}

void*	CryInterlockedCompareExchangePointer(void* volatile * dst, void* exchange, void* comperand)
{
	return InterlockedCompareExchangePointer(dst, exchange, comperand);
}


//////////////////////////////////////////////////////////////////////////
void* CryCreateCriticalSection()
{
	CRITICAL_SECTION *pCS = new CRITICAL_SECTION;
	InitializeCriticalSection(pCS);
	return pCS;
}

void  CryCreateCriticalSectionInplace(void* pCS)
{
  InitializeCriticalSection((CRITICAL_SECTION *)pCS);
}
//////////////////////////////////////////////////////////////////////////
void  CryDeleteCriticalSection( void *cs )
{
	CRITICAL_SECTION *pCS = (CRITICAL_SECTION*)cs;
	if (pCS->LockCount >= 0)
		CryFatalError("Critical Section hanging lock" );
	DeleteCriticalSection(pCS);
	delete pCS;
}

//////////////////////////////////////////////////////////////////////////
void  CryDeleteCriticalSectionInplace( void *cs )
{
  CRITICAL_SECTION *pCS = (CRITICAL_SECTION*)cs;
  if (pCS->LockCount >= 0)
    CryFatalError("Critical Section hanging lock" );
  DeleteCriticalSection(pCS);
}

//////////////////////////////////////////////////////////////////////////
void  CryEnterCriticalSection( void *cs )
{
	EnterCriticalSection((CRITICAL_SECTION*)cs);
}

//////////////////////////////////////////////////////////////////////////
bool  CryTryCriticalSection( void *cs )
{
	return TryEnterCriticalSection((CRITICAL_SECTION*)cs) != 0;
}

//////////////////////////////////////////////////////////////////////////
void  CryLeaveCriticalSection( void *cs )
{
	LeaveCriticalSection((CRITICAL_SECTION*)cs);
}

//////////////////////////////////////////////////////////////////////////
uint32 CryGetFileAttributes( const char *lpFileName )
{
	return GetFileAttributes( lpFileName );
}

//////////////////////////////////////////////////////////////////////////
bool CrySetFileAttributes( const char *lpFileName,uint32 dwFileAttributes )
{
	return SetFileAttributes( lpFileName,dwFileAttributes ) != 0;
}

//////////////////////////////////////////////////////////////////////////
unsigned int CryGetCurrentThreadId()
{
	return GetCurrentThreadId();
}

#else // WIN32

// These are implemented in WinBase.cpp























#endif // _WIN32

#endif //_LIB


#ifndef _LIB











































#endif

#if defined(WIN32) || defined(WIN64) || defined(XENON)
#if !defined(_LIB) || defined(_LAUNCHER)
int64 CryQueryPerformanceCounter()
{
#if !defined (USE_MULTICORE_SAVE_TIMING) && !defined(XENON)
	return __rdtsc();
#else
	LARGE_INTEGER li;
	QueryPerformanceCounter( &li );
	return li.QuadPart;	
#endif
}
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Threads implementation. For static linking it must be declared inline otherwise creating multiple symbols
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) && !defined(_LAUNCHER)
	#define THR_INLINE inline
#else
	#define THR_INLINE 
#endif

//////////////////////////////////////////////////////////////////////////
inline void CryDebugStr( const char *format,... )
{
	/*
#ifdef CRYSYSTEM_EXPORTS
	va_list	ArgList;
	char		szBuffer[65535];
	va_start(ArgList, format);
	_vsnprintf_c(szBuffer,sizeof(szBuffer)-1, format, ArgList);
	va_end(ArgList);
	strcat(szBuffer,"\n");
	OutputDebugString(szBuffer);
#endif
	*/
}










#endif // __platform_impl_h__
