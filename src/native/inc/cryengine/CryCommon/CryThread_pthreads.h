#ifndef _CryThread_pthreads_h_
#define _CryThread_pthreads_h_ 1

#include <ISystem.h>
#include <ILog.h>

#ifndef __SPU__
#  include <sys/types.h>
#  include <sys/time.h>
#  include <pthread.h>
#  include <sched.h>
#endif


















	#define RegisterThreadName(id,name)
	#define UnRegisterThreadName(id)


#if !defined _CRYTHREAD_HAVE_LOCK
template<class LockClass> class _PthreadCond;
template<int PthreadMutexType> class _PthreadLockBase;

template<int PthreadMutexType> class _PthreadLockAttr
{
	friend class _PthreadLockBase<PthreadMutexType>;

	_PthreadLockAttr(int dummy)
	{
		pthread_mutexattr_init(&m_Attr);
		pthread_mutexattr_settype(&m_Attr, PthreadMutexType);
	}
	~_PthreadLockAttr()
	{
		pthread_mutexattr_destroy(&m_Attr);
	}

protected:
  pthread_mutexattr_t m_Attr;
};

template<int PthreadMutexType> class _PthreadLockBase
{
	static _PthreadLockAttr<PthreadMutexType> m_Attr;

protected:
	static pthread_mutexattr_t &GetAttr() { return m_Attr.m_Attr; }
};

template<class LockClass, int PthreadMutexType> class _PthreadLock
  : public _PthreadLockBase<PthreadMutexType>
{
  friend class _PthreadCond<LockClass>;

//#if defined(_DEBUG)
public:
//#endif
  pthread_mutex_t m_Lock;

public:
  _PthreadLock() : LockCount(0)
	{
		pthread_mutex_init(
				&m_Lock,
				&_PthreadLockBase<PthreadMutexType>::GetAttr());
	}
  ~_PthreadLock() { pthread_mutex_destroy(&m_Lock); }

  void Lock() { pthread_mutex_lock(&m_Lock); CryInterlockedIncrement(&LockCount); }
  
  bool TryLock()
  { 
	const int rc = pthread_mutex_trylock(&m_Lock);
	if (0 == rc)
	{
		CryInterlockedIncrement(&LockCount);
		return true;
	}
	return false;
  }
  
  void Unlock() { CryInterlockedDecrement(&LockCount); pthread_mutex_unlock(&m_Lock); }

	// Get the POSIX pthread_mutex_t.
	// Warning:
	// This method will not be available in the Win32 port of CryThread.
	pthread_mutex_t &Get_pthread_mutex_t() { return m_Lock; }

	bool IsLocked()
	{
#if defined(LINUX)
		// implementation taken from CrysisWars
		return LockCount > 0;
#else
		return true;
#endif
	}

private:
	volatile int LockCount;
};

#if defined CRYLOCK_HAVE_FASTLOCK
	#if defined(_DEBUG) && defined(PTHREAD_MUTEX_ERRORCHECK_NP)
	template<> class CryLockT<CRYLOCK_FAST>
		: public _PthreadLock<CryLockT<CRYLOCK_FAST>, PTHREAD_MUTEX_ERRORCHECK_NP>
	#else
	template<> class CryLockT<CRYLOCK_FAST>
		: public _PthreadLock<CryLockT<CRYLOCK_FAST>, PTHREAD_MUTEX_FAST_NP>
	#endif
	{
		CryLockT(const CryLockT<CRYLOCK_FAST>&);
		void operator = (const CryLockT<CRYLOCK_FAST>&);

	public:
		CryLockT() { }
	};
#endif // CRYLOCK_HAVE_FASTLOCK

template<> class CryLockT<CRYLOCK_RECURSIVE>
  : public _PthreadLock<CryLockT<CRYLOCK_RECURSIVE>, PTHREAD_MUTEX_RECURSIVE>
{
	CryLockT(const CryLockT<CRYLOCK_RECURSIVE>&);
	void operator = (const CryLockT<CRYLOCK_RECURSIVE>&);

public:
	CryLockT() { }
};

#ifndef LINUX
#if defined CRYLOCK_HAVE_FASTLOCK
class CryMutex : public CryLockT<CRYLOCK_FAST> {};
#else
class CryMutex : public CryLockT<CRYLOCK_RECURSIVE> {};
#endif
#endif // LINUX

template<class LockClass> class _PthreadCond
{
  pthread_cond_t m_Cond;

public:
  _PthreadCond() { pthread_cond_init(&m_Cond, NULL); }
  ~_PthreadCond() { pthread_cond_destroy(&m_Cond); }
  void Notify() { pthread_cond_broadcast(&m_Cond); }
  void NotifySingle() { pthread_cond_signal(&m_Cond); }
  void Wait(LockClass &Lock) { pthread_cond_wait(&m_Cond, &Lock.m_Lock); }
	bool TimedWait(LockClass &Lock, uint32 milliseconds)
	{
		struct timeval now;
    struct timespec timeout;
    int err;

    gettimeofday(&now, NULL);
		while (true)
		{
			timeout.tv_sec = now.tv_sec + milliseconds / 1000;
			uint64 nsec = (uint64)now.tv_usec * 1000 + (uint64)milliseconds * 1000000;
			if (nsec >= 1000000000)
			{
				timeout.tv_sec += (long)(nsec / 1000000000);
				nsec %= 1000000000;
			}
			timeout.tv_nsec = (long)nsec;
			err = pthread_cond_timedwait(&m_Cond, &Lock.m_Lock, &timeout);
			if (err == EINTR)
			{
				// Interrupted by a signal.
				continue;
			}
			else if (err == ETIMEDOUT)
			{
				return false;
			}
			else
				assert(err == 0);
			break;
		}
		return true;
	}

	// Get the POSIX pthread_cont_t.
	// Warning:
	// This method will not be available in the Win32 port of CryThread.
	pthread_cond_t &Get_pthread_cond_t() { return m_Cond; }
};

#ifdef LINUX
template <class LockClass> class CryConditionVariableT : public _PthreadCond<LockClass> {};

#if defined CRYLOCK_HAVE_FASTTLOCK
template<>
class CryConditionVariableT< CryLockT<CRYLOCK_FAST> > : public _PthreadCond< CryLockT<CRYLOCK_FAST> >
{
	typedef CryLockT<CRYLOCK_FAST> LockClass;
	CryConditionVariableT(const CryConditionVariableT<LockClass>&);
	CryConditionVariableT<LockClass>& operator = (const CryConditionVariableT<LockClass>&);

public:
	CryConditionVariableT() { }
};
#endif // CRYLOCK_HAVE_FASTLOCK

template<>
class CryConditionVariableT< CryLockT<CRYLOCK_RECURSIVE> > : public _PthreadCond< CryLockT<CRYLOCK_RECURSIVE> >
{
	typedef CryLockT<CRYLOCK_RECURSIVE> LockClass;
	CryConditionVariableT(const CryConditionVariableT<LockClass>&);
	CryConditionVariableT<LockClass>& operator = (const CryConditionVariableT<LockClass>&);

public:
	CryConditionVariableT() { }
};

#if !defined(_CRYTHREAD_CONDLOCK_GLITCH)
typedef CryConditionVariableT< CryLockT<CRYLOCK_RECURSIVE> > CryConditionVariable;
#else
typedef CryConditionVariableT< CryLockT<CRYLOCK_FAST> > CryConditionVariable;
#endif

#define _CRYTHREAD_HAVE_LOCK 1

#else // LINUX

#if defined CRYLOCK_HAVE_FASTLOCK
template<>
class CryConditionVariable : public _PthreadCond< CryLockT<CRYLOCK_FAST> >
{
	typedef CryLockT<CRYLOCK_FAST> LockClass;
	CryConditionVariable(const CryConditionVariable&);
	CryConditionVariable& operator = (const CryConditionVariable&);

public:
	CryConditionVariable() { }
};
#endif // CRYLOCK_HAVE_FASTLOCK

template<>
class CryConditionVariable : public _PthreadCond< CryLockT<CRYLOCK_RECURSIVE> >
{
	typedef CryLockT<CRYLOCK_RECURSIVE> LockClass;
	CryConditionVariable(const CryConditionVariable&);
	CryConditionVariable& operator = (const CryConditionVariable&);

public:
	CryConditionVariable() { }
};

#define _CRYTHREAD_HAVE_LOCK 1

#endif // LINUX
#endif // !defined _CRYTHREAD_HAVE_LOCK

#if !defined _CRYTHREAD_HAVE_RWLOCK && !defined __SPU__
class CryRWLock
{
	pthread_rwlock_t m_Lock;

	CryRWLock(const CryRWLock &);
	CryRWLock &operator= (const CryRWLock &);

public:
	CryRWLock() { pthread_rwlock_init(&m_Lock, NULL); }
	~CryRWLock() { pthread_rwlock_destroy(&m_Lock); }
	void RLock() { pthread_rwlock_rdlock(&m_Lock); }
	bool TryRLock() { return pthread_rwlock_tryrdlock(&m_Lock) != EBUSY; }
	void WLock() { pthread_rwlock_wrlock(&m_Lock); }
	bool TryWLock() { return pthread_rwlock_trywrlock(&m_Lock) != EBUSY; }
	void Lock() { WLock(); }
	bool TryLock() { return TryWLock(); }
	void Unlock() { pthread_rwlock_unlock(&m_Lock); }
};

// Indicate that this implementation header provides an implementation for
// CryRWLock.
#define _CRYTHREAD_HAVE_RWLOCK 1
#endif // !defined _CRYTHREAD_HAVE_RWLOCK

//////////////////////////////////////////////////////////////////////////
// CryEvent(Timed) represent a synchronization event
//////////////////////////////////////////////////////////////////////////
class CryEventTimed
{
public:
	ILINE CryEventTimed(){}
	ILINE ~CryEventTimed(){}

	// Reset the event to the unsignalled state.
	void Reset();
	// Set the event to the signalled state.
	void Set();
	// Access a HANDLE to wait on.
	void* GetHandle() const { return NULL; };
	// Wait indefinitely for the object to become signalled.
	void Wait();
	// Wait, with a time limit, for the object to become signalled. 
	bool Wait( const uint32 timeoutMillis );

private:
	// Lock for synchronization of notifications.
	CryCriticalSection m_lockNotify;
#ifdef LINUX
	CryConditionVariableT< CryLockT<CRYLOCK_RECURSIVE> > m_cond;
#else
	CryConditionVariable m_cond;
#endif
	volatile bool m_flag;
};

#define PS3_USE_SEMAPHORE	1

#if PS3 && PS3_USE_SEMAPHORE
//a semaphore based implementation, do not use on spu executed code
class CryEvent
{
public:
	ILINE CryEvent() {Init();}
	ILINE ~CryEvent()
	{
#if !defined( __SPU__ )
		if( CELL_OK != sys_semaphore_destroy(m_semaphore) )
#endif
			snPause();
	}

	// Reset the event to the unsignalled state.
	ILINE void Reset() {}

	// Set the event to the signalled state.
	ILINE void Set( int count = 1 )
	{
#if !defined( __SPU__ )
		if( CELL_OK != sys_semaphore_post(m_semaphore, (sys_semaphore_value_t)count) )
#endif
			snPause();
	}
	
	// Wait indefinitely for the object to become signalled.
	ILINE void Wait()
	{
#if !defined( __SPU__ )
		if( CELL_OK != sys_semaphore_wait(m_semaphore, 0 ) )
#endif
			snPause();
	}
	
	// Wait with timeout for the object to become signalled.
	ILINE bool Wait(const uint32 timeoutMillis)
	{
		bool bInTime = true;
#if !defined( __SPU__ )
		usecond_t microTimeout = timeoutMillis*1000;
		int ret = sys_semaphore_wait(m_semaphore, microTimeout );
		if (ret == ETIMEDOUT)
		{	
			//-- To be consistent with CryEventTimed, return false if the wait timed out.
			bInTime = false;
		}
		if( CELL_OK != ret && ETIMEDOUT != ret )
#endif
			snPause();
		return bInTime;
	}

private:
	sys_semaphore_t m_semaphore;

	NO_INLINE void Init()
	{
#if !defined( __SPU__ )
		sys_semaphore_attribute_t attr;
		sys_semaphore_attribute_initialize(attr);
		sys_semaphore_attribute_name_set(attr.name, "Semaphore" );
		if( CELL_OK != sys_semaphore_create(&m_semaphore, &attr, 0, 0x7fffffff) )
#endif
			snPause();
	}
};
#else
	typedef CryEventTimed CryEvent;
#endif

class CrySimpleThreadSelf
{
protected:
	static THREADLOCAL CrySimpleThreadSelf *m_Self;
};

template<class Runnable>
class CrySimpleThread
	: public CryRunnable,
		protected CrySimpleThreadSelf
{
public:
	typedef void (*ThreadFunction)(void *);
	typedef CryRunnable RunnableT;

private:
#if !defined(NO_THREADINFO)
	CryThreadInfo m_Info;
#endif
  pthread_t m_ThreadID;
	unsigned m_CpuMask;
	Runnable *m_Runnable;
	struct
	{
		ThreadFunction m_ThreadFunction;
		void *m_ThreadParameter;
	} m_ThreadFunction;
	bool m_bIsStarted;
	bool m_bIsRunning;

protected:
	virtual void Terminate()
	{
		// This method must be empty.
		// Derived classes overriding Terminate() are not required to call this
		// method.
	}

private:
#ifndef __SPU__
#if !defined(NO_THREADINFO)
	static void SetThreadInfo(CrySimpleThread<Runnable> *self)
	{
		pthread_t thread = pthread_self();
		self->m_Info.m_ID = (uint32)thread;
	}
#else
	static void SetThreadInfo(CrySimpleThread<Runnable> *self) { }
#endif

  static inline void SetThreadLocalData()
  {








  }

	static void *PthreadRunRunnable(void *thisPtr)
	{
		CrySimpleThread<Runnable> *const self = (CrySimpleThread<Runnable> *)thisPtr;
		m_Self = self;
		self->m_bIsStarted = true;
		self->m_bIsRunning = true;
		SetThreadInfo(self);
		self->m_Runnable->Run();
		self->m_bIsRunning = false;
		self->Terminate();
		m_Self = NULL;
    SetThreadLocalData();
		return NULL;
	}

	static void *PthreadRunThis(void *thisPtr)
	{
		CrySimpleThread<Runnable> *const self = (CrySimpleThread<Runnable> *)thisPtr;
		m_Self = self;
		self->m_bIsStarted = true;
		self->m_bIsRunning = true;
		SetThreadInfo(self);
		self->Run();
		self->m_bIsRunning = false;
		self->Terminate();
		m_Self = NULL;
    SetThreadLocalData();
		return NULL;
	}
#endif
	CrySimpleThread(const CrySimpleThread<Runnable>&);
	void operator = (const CrySimpleThread<Runnable>&);

public:
  CrySimpleThread()
		: m_CpuMask(0), m_bIsStarted(false), m_bIsRunning(false)
	{
		m_ThreadFunction.m_ThreadFunction = NULL;
		m_ThreadFunction.m_ThreadParameter = NULL;
#if !defined(NO_THREADINFO)
#if !defined(__SPU__)
		m_Info.m_Name = "<Thread>";
#endif
		m_Info.m_ID = 0;
#endif
		memset(&m_ThreadID, 0, sizeof m_ThreadID);
		m_Runnable = NULL;
	}

	void* GetHandle() { return (void*)m_ThreadID; }

  virtual ~CrySimpleThread()
	{
#ifndef __SPU__
		if (IsStarted())
		{
			// Note: We don't want to cache a pointer to ISystem and/or ILog to
			// gain more freedom on when the threading classes are used (e.g.
			// threads may be started very early in the initialization).
			ISystem *pSystem = GetISystem();
			ILog *pLog = NULL;
			if (pSystem != NULL)
				pLog = pSystem->GetILog();




			Cancel();
			WaitForThread();
		}
#endif
	}

#if !defined(NO_THREADINFO)
	CryThreadInfo &GetInfo() { return m_Info; }
	const char *GetName()
	{
#if !defined(__SPU__)
		return m_Info.m_Name.c_str();


#endif
	}

	// Set the name of the called thread.
	//
	// WIN32:
	// If the thread is started, then the VC debugger is informed about the new
	// thread name.  If the thread is not started, then the VC debugger will be
	// informed lated when the thread is started through one of the Start()
	// methods.
	//
	// If the parameter Name is NULL, then the name of the thread is kept
	// unchanged.  This may be used to sent the current thread name to the VC
	// debugger.
	void SetName(const char *Name)
	{
		CryThreadSetName(-1, Name);
#if !defined(__SPU__)
		if (Name != NULL)
		{
			m_Info.m_Name = Name;
		}
#endif
#if defined(WIN32)
		if (IsStarted())
		{
			// The VC debugger gets the information about a thread's name through
			// the exception 0x406D1388.
			struct
			{
				DWORD Type;
				const char *Name;
				DWORD ID;
				DWORD Flags;
			} Info = { 0x1000, NULL, 0, 0 };
			Info.ID = (DWORD)m_Info.m_ID;
			__try
			{
				RaiseException(
						0x406D1388, 0, sizeof Info / sizeof(DWORD), (ULONG_PTR *)&Info);
			}
			__except (EXCEPTION_CONTINUE_EXECUTION)
			{
			}
		}
#endif
	}
#else
#if !defined(NO_THREADINFO)
	CryThreadInfo &GetInfo()
	{
		static CryThreadInfo dummyInfo = { "<dummy>", 0 };
		return dummyInfo;
	}
#endif
	const char *GetName() { return "<dummy>"; }
	void SetName(const char *Name) { }
#endif

  virtual void Run()
	{
		// This Run() implementation supports the void StartFunction() method.
		// However, code using this class (or derived classes) should eventually
		// be refactored to use one of the other Start() methods.  This code will
		// be removed some day and the default implementation of Run() will be
		// empty.
		if (m_ThreadFunction.m_ThreadFunction != NULL)
		{
			m_ThreadFunction.m_ThreadFunction(m_ThreadFunction.m_ThreadParameter);
		}
	}

	// Cancel the running thread.
	//
	// If the thread class is implemented as a derived class of CrySimpleThread,
	// then the derived class should provide an appropriate implementation for
	// this method.  Calling the base class implementation is _not_ required.
	//
	// If the thread was started by specifying a Runnable (template argument),
	// then the Cancel() call is passed on to the specified runnable.
	//
	// If the thread was started using the StartFunction() method, then the
	// caller must find other means to inform the thread about the cancellation
	// request.
	virtual void Cancel()
	{
		if (IsStarted() && m_Runnable != NULL)
		{
			UnRegisterThreadName(m_ThreadID);
			m_Runnable->Cancel();
		}
	}

	virtual void Start(Runnable &runnable, unsigned cpuMask = 0, const char* name = NULL, int32 StackSize=(SIMPLE_THREAD_STACK_SIZE_KB*1024))
  {



#ifndef __SPU__
    assert(m_ThreadID == 0);
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&threadAttr, StackSize);
#ifndef LINUX
		threadAttr.name = (char*)name;
#endif
		m_CpuMask = cpuMask;
#if defined(PTHREAD_NPTL)
		if (cpuMask != ~0 && cpuMask != 0)
		{
			cpu_set_t cpuSet;
			CPU_ZERO(&cpuSet);
			for (int cpu = 0; cpu < sizeof(cpuMask) * 8; ++cpu)
			{
				if (cpuMask & (1 << cpu))
				{
					CPU_SET(cpu, &cpuSet);
				}
			}
			pthread_attr_setaffinity_np(&threadAttr, sizeof cpuSet, &cpuSet);
		}
#endif
		m_Runnable = &runnable;
    int err = pthread_create(
				&m_ThreadID,
				&threadAttr,
				PthreadRunRunnable,
				this);
		RegisterThreadName(m_ThreadID, name);
    assert(err == 0);
#endif
  }

	virtual void Start(unsigned cpuMask = 0, const char* name = NULL,int32 Priority=THREAD_PRIORITY_NORMAL, int32 StackSize=(SIMPLE_THREAD_STACK_SIZE_KB*1024))
	{



#ifndef __SPU__
    assert(m_ThreadID == 0);
    pthread_attr_t threadAttr;
		sched_param schedParam;
    pthread_attr_init(&threadAttr);
		pthread_attr_getschedparam(&threadAttr,&schedParam);
		schedParam.sched_priority	=	Priority;
		pthread_attr_setschedparam(&threadAttr,&schedParam);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&threadAttr, StackSize);
#ifndef LINUX
		threadAttr.name = (char*)name;
#endif
		m_CpuMask = cpuMask;
#if defined(PTHREAD_NPTL)
		if (cpuMask != ~0 && cpuMask != 0)
		{
			cpu_set_t cpuSet;
			CPU_ZERO(&cpuSet);
			for (int cpu = 0; cpu < sizeof(cpuMask) * 8; ++cpu)
			{
				if (cpuMask & (1 << cpu))
				{
					CPU_SET(cpu, &cpuSet);
				}
			}
			pthread_attr_setaffinity_np(&threadAttr, sizeof cpuSet, &cpuSet);
		}
#endif
    int err = pthread_create(
				&m_ThreadID,
				&threadAttr,
				PthreadRunThis,
				this);
		RegisterThreadName(m_ThreadID, name);
    assert(err == 0);
#endif
	}

	void StartFunction(
			ThreadFunction threadFunction,
			void *threadParameter = NULL,
			unsigned cpuMask = 0
			)
	{
		m_ThreadFunction.m_ThreadFunction = threadFunction;
		m_ThreadFunction.m_ThreadParameter = threadParameter;
		Start(cpuMask);
	}

	static CrySimpleThread<Runnable> *Self()
	{
		return reinterpret_cast<CrySimpleThread<Runnable> *>(m_Self);
	}

  void Exit()
  {
#ifndef __SPU__
    assert(m_ThreadID == pthread_self());
		m_bIsRunning = false;
		Terminate();
		m_Self = NULL;
    pthread_exit(NULL);
		UnRegisterThreadName(m_ThreadID);
#endif
  }

  void WaitForThread()
  {
#ifndef __SPU__
		if( pthread_self() != m_ThreadID )
		{
			int err = pthread_join(m_ThreadID, NULL);
			assert(err == 0);
		}
		m_bIsStarted = false;
		memset(&m_ThreadID, 0, sizeof m_ThreadID);
#endif
  }

	unsigned SetCpuMask(unsigned cpuMask)
	{
		int oldCpuMask = m_CpuMask;
		if (cpuMask == m_CpuMask)
			return oldCpuMask;
		m_CpuMask = cpuMask;
#if defined(PTHREAD_NPTL)
		cpu_set_t cpuSet;
		CPU_ZERO(&cpuSet);
		if (cpuMask != ~0 && cpuMask != 0)
		{
			for (int cpu = 0; cpu < sizeof(cpuMask) * 8; ++cpu)
			{
				if (cpuMask & (1 << cpu))
				{
					CPU_SET(cpu, &cpuSet);
				}
			}
		}
		else
		{
			CPU_ZERO(&cpuSet);
			for (int cpu = 0; cpu < sizeof(cpuSet) * 8; ++cpu)
			{
				CPU_SET(cpu, &cpuSet);
			}
		}
		pthread_attr_setaffinity_np(&threadAttr, sizeof cpuSet, &cpuSet);
#endif
		return oldCpuMask;
	}

	unsigned GetCpuMask() { return m_CpuMask; }

	void Stop()
	{
		m_bIsStarted = false;
	}

	bool IsStarted() const { return m_bIsStarted; }
	bool IsRunning() const { return m_bIsRunning; }
};





#endif

// vim:ts=2

