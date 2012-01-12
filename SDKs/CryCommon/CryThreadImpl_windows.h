#ifndef __CRYTHREADIMPL_WINDOWS_H__
#define __CRYTHREADIMPL_WINDOWS_H__
#pragma once

//#include <IThreadTask.h>

#define WIN32_LEAN_AND_MEAN
#ifndef XENON
#include <windows.h>
#endif
#include <process.h>

struct SThreadNameDesc
{
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;
};

THREADLOCAL CrySimpleThreadSelf* CrySimpleThreadSelf::m_Self = NULL;

//////////////////////////////////////////////////////////////////////////
CryEvent::CryEvent()
{
	m_handle = (void*)CreateEvent(NULL, FALSE, FALSE, NULL);
}

//////////////////////////////////////////////////////////////////////////
CryEvent::~CryEvent()
{
	CloseHandle(m_handle);
}

//////////////////////////////////////////////////////////////////////////
void CryEvent::Reset()
{
	ResetEvent(m_handle);
}

//////////////////////////////////////////////////////////////////////////
void CryEvent::Set()
{
	SetEvent(m_handle);
}

//////////////////////////////////////////////////////////////////////////
void CryEvent::Wait() const
{
	WaitForSingleObject(m_handle, INFINITE);
}

//////////////////////////////////////////////////////////////////////////
bool CryEvent::Wait( const uint32 timeoutMillis ) const
{
	if (WaitForSingleObject(m_handle, timeoutMillis) == WAIT_TIMEOUT)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CryLock_WinMutex
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CryLock_WinMutex::CryLock_WinMutex() : m_hdl(CreateMutex(NULL, FALSE, NULL)) {}
CryLock_WinMutex::~CryLock_WinMutex()
{
	CloseHandle(m_hdl);
}

//////////////////////////////////////////////////////////////////////////
void CryLock_WinMutex::Lock()
{
	WaitForSingleObject(m_hdl, INFINITE);
}

//////////////////////////////////////////////////////////////////////////
void CryLock_WinMutex::Unlock()
{
	ReleaseMutex(m_hdl);
}

//////////////////////////////////////////////////////////////////////////
bool CryLock_WinMutex::TryLock()
{
	return WaitForSingleObject(m_hdl, 0) != WAIT_TIMEOUT;
}

//////////////////////////////////////////////////////////////////////////
// CryLock_CritSection
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CryLock_CritSection::CryLock_CritSection()
{
	InitializeCriticalSection((CRITICAL_SECTION*)&m_cs);
}

//////////////////////////////////////////////////////////////////////////
CryLock_CritSection::~CryLock_CritSection()
{
	DeleteCriticalSection((CRITICAL_SECTION*)&m_cs);
}

//////////////////////////////////////////////////////////////////////////
void CryLock_CritSection::Lock()
{
	EnterCriticalSection((CRITICAL_SECTION*)&m_cs);
}

//////////////////////////////////////////////////////////////////////////
void CryLock_CritSection::Unlock()
{
	LeaveCriticalSection((CRITICAL_SECTION*)&m_cs);
}

//////////////////////////////////////////////////////////////////////////
bool CryLock_CritSection::TryLock()
{
	return TryEnterCriticalSection((CRITICAL_SECTION*)&m_cs) != FALSE;
}

//////////////////////////////////////////////////////////////////////////
// most of this is taken from http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
//////////////////////////////////////////////////////////////////////////
CryConditionVariable::CryConditionVariable()
{
	m_waitersCount = 0;
	m_wasBroadcast = 0;
	m_sema = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
	InitializeCriticalSection((CRITICAL_SECTION*)&m_waitersCountLock);
	m_waitersDone = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//////////////////////////////////////////////////////////////////////////
CryConditionVariable::~CryConditionVariable()
{
	CloseHandle(m_sema);
	DeleteCriticalSection((CRITICAL_SECTION*)&m_waitersCountLock);
	CloseHandle(m_waitersDone);
}

//////////////////////////////////////////////////////////////////////////
void CryConditionVariable::Wait( LockType& lock )
{
	EnterCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
	m_waitersCount ++;
	LeaveCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );

	SignalObjectAndWait( lock._get_win32_handle(), m_sema, INFINITE, FALSE );

	EnterCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
	m_waitersCount --;
	bool lastWaiter = m_wasBroadcast && m_waitersCount == 0;
	LeaveCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );

	if (lastWaiter)
		SignalObjectAndWait( m_waitersDone, lock._get_win32_handle(), INFINITE, FALSE );
	else
		WaitForSingleObject( lock._get_win32_handle(), INFINITE );
}

//////////////////////////////////////////////////////////////////////////
bool CryConditionVariable::TimedWait( LockType& lock, uint32 millis )
{
	EnterCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
	m_waitersCount ++;
	LeaveCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );

	bool ok = true;
	if (WAIT_TIMEOUT == SignalObjectAndWait( lock._get_win32_handle(), m_sema, millis, FALSE ))
		ok = false;

	EnterCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
	m_waitersCount --;
	bool lastWaiter = m_wasBroadcast && m_waitersCount == 0;
	LeaveCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );

	if (lastWaiter)
		SignalObjectAndWait( m_waitersDone, lock._get_win32_handle(), INFINITE, FALSE );
	else
		WaitForSingleObject( lock._get_win32_handle(), INFINITE );

	return ok;
}

//////////////////////////////////////////////////////////////////////////
void CryConditionVariable::NotifySingle()
{
	EnterCriticalSection((CRITICAL_SECTION*)&m_waitersCountLock);
	bool haveWaiters = m_waitersCount > 0;
	LeaveCriticalSection((CRITICAL_SECTION*)&m_waitersCountLock);
	if (haveWaiters)
		ReleaseSemaphore(m_sema, 1, 0);
}

//////////////////////////////////////////////////////////////////////////
void CryConditionVariable::Notify()
{
	EnterCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
	bool haveWaiters = false;
	if (m_waitersCount > 0)
	{
		m_wasBroadcast = 1;
		haveWaiters = true;
	}
	if (haveWaiters)
	{
		ReleaseSemaphore( m_sema, m_waitersCount, 0 );
		LeaveCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
		WaitForSingleObject( m_waitersDone, INFINITE );
		m_wasBroadcast = 0;
	}
	else
	{
		LeaveCriticalSection( (CRITICAL_SECTION*)&m_waitersCountLock );
	}
}

//////////////////////////////////////////////////////////////////////////
void CrySimpleThreadSelf::WaitForThread()
{
	assert(m_thread);
	if( GetCurrentThreadId() != m_threadId )
	{
		WaitForSingleObject( (HANDLE)m_thread, INFINITE );
	}
}

CrySimpleThreadSelf::~CrySimpleThreadSelf()
{
	if(m_thread)
		CloseHandle(m_thread);
}

void CrySimpleThreadSelf::StartThread(unsigned (__stdcall *func)(void*), void* argList)
{
	m_thread = (void*)_beginthreadex( NULL, 0, func, argList, CREATE_SUSPENDED, &m_threadId );
	assert(m_thread);
	ResumeThread((HANDLE)m_thread);
}

#endif //__CRYTHREADIMPL_WINDOWS_H__