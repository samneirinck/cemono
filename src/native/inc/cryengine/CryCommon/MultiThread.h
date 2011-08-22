////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2005.
// -------------------------------------------------------------------------
//  File name:   MultiThread.h
//  Version:     v1.00
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MultiThread_h__
#define __MultiThread_h__
#pragma once

#define WRITE_LOCK_VAL (1<<16)







//as PowerPC operates via cache line reservation, lock variables should reside ion their own cache line
template <class T>
struct SAtomicVar
{
	T val;



	inline operator T()const{return val;}
	inline operator T()volatile const{return val;}
	inline void operator =(const T& rV){val = rV;return *this;}
	inline void Assign(const T& rV){val = rV;}
	inline void Assign(const T& rV)volatile{val = rV;}
	inline T* Addr() {return &val;}
	inline volatile T* Addr() volatile {return &val;}

	inline bool operator<(const T& v)const{return val < v;}
	inline bool operator<(const SAtomicVar<T>& v)const{return val < v.val;}
	inline bool operator>(const T& v)const{return val > v;}
	inline bool operator>(const SAtomicVar<T>& v)const{return val > v.val;}
	inline bool operator<=(const T& v)const{return val <= v;}
	inline bool operator<=(const SAtomicVar<T>& v)const{return val <= v.val;}
	inline bool operator>=(const T& v)const{return val >= v;}
	inline bool operator>=(const SAtomicVar<T>& v)const{return val >= v.val;}
	inline bool operator==(const T& v)const{return val == v;}
	inline bool operator==(const SAtomicVar<T>& v)const{return val == v.val;}
	inline bool operator!=(const T& v)const{return val != v;}
	inline bool operator!=(const SAtomicVar<T>& v)const{return val != v.val;}
	inline T operator*(const T& v)const{return val * v;}
	inline T operator/(const T& v)const{return val / v;}
	inline T operator+(const T& v)const{return val + v;}
	inline T operator-(const T& v)const{return val - v;}

	inline bool operator<(const T& v)volatile const{return val < v;}
	inline bool operator<(const SAtomicVar<T>& v)volatile const{return val < v.val;}
	inline bool operator>(const T& v)volatile const{return val > v;}
	inline bool operator>(const SAtomicVar<T>& v)volatile const{return val > v.val;}
	inline bool operator<=(const T& v)volatile const{return val <= v;}
	inline bool operator<=(const SAtomicVar<T>& v)volatile const{return val <= v.val;}
	inline bool operator>=(const T& v)volatile const{return val >= v;}
	inline bool operator>=(const SAtomicVar<T>& v)volatile const{return val >= v.val;}
	inline bool operator==(const T& v)volatile const{return val == v;}
	inline bool operator==(const SAtomicVar<T>& v)volatile const{return val == v.val;}
	inline bool operator!=(const T& v)volatile const{return val != v;}
	inline bool operator!=(const SAtomicVar<T>& v)volatile const{return val != v.val;}
	inline T operator*(const T& v)volatile const{return val * v;}
	inline T operator/(const T& v)volatile const{return val / v;}
	inline T operator+(const T& v)volatile const{return val + v;}
	inline T operator-(const T& v)volatile const{return val - v;}
}





;

typedef SAtomicVar<int> TIntAtomic;
typedef SAtomicVar<unsigned int> TUIntAtomic;
typedef SAtomicVar<float> TFloatAtomic;

#ifdef __SNC__
	#ifndef __add_db16cycl__
		#define __add_db16cycl__ __db16cyc();
	#endif
#else
	#define USE_INLINE_ASM
	//#define ADD_DB16_CYCLES
	#undef __add_db16cycl__
	#ifdef ADD_DB16_CYCLES
		#define __add_db16cycl__ __asm__ volatile("db16cyc");
	#else	
		#define __add_db16cycl__ 
	#endif
#endif

#if !defined(__SPU__)
	void CrySpinLock(volatile int *pLock,int checkVal,int setVal);
  void CryReleaseSpinLock(volatile int*, int);
	#if !defined(PS3)
		long   CryInterlockedIncrement( int volatile *lpAddend );
		long   CryInterlockedDecrement( int volatile *lpAddend );
		long   CryInterlockedExchangeAdd(long volatile * lpAddend, long Value);
		long	 CryInterlockedCompareExchange(long volatile * dst, long exchange, long comperand);
		void*	 CryInterlockedCompareExchangePointer(void* volatile * dst, void* exchange, void* comperand);
	#endif
	void*  CryCreateCriticalSection();
  void   CryCreateCriticalSectionInplace(void*);
	void   CryDeleteCriticalSection( void *cs );
  void   CryDeleteCriticalSectionInplace( void *cs );
	void   CryEnterCriticalSection( void *cs );
	bool   CryTryCriticalSection( void *cs );
	void   CryLeaveCriticalSection( void *cs );

















































































































































































































































































ILINE void CrySpinLock(volatile int *pLock,int checkVal,int setVal)
{ 





#ifdef _CPU_X86
# ifdef __GNUC__
	register int val;
	__asm__ __volatile__ (
		"0:     mov %[checkVal], %%eax\n"
		"       lock cmpxchg %[setVal], (%[pLock])\n"
		"       jnz 0b"
		: "=m" (*pLock)
		: [pLock] "r" (pLock), "m" (*pLock),
		  [checkVal] "m" (checkVal),
		  [setVal] "r" (setVal)
		: "eax", "cc", "memory"
		);
# else //!__GNUC__
	__asm
	{
		mov edx, setVal
		mov ecx, pLock
Spin:
		// Trick from Intel Optimizations guide
#ifdef _CPU_SSE
		pause
#endif 
		mov eax, checkVal
		lock cmpxchg [ecx], edx
		jnz Spin
	}
# endif //!__GNUC__
#else // !_CPU_X86



























































































	// NOTE: The code below will fail on 64bit architectures!
	while(_InterlockedCompareExchange((volatile long*)pLock,setVal,checkVal)!=checkVal) ;

#endif

}

ILINE void CryReleaseSpinLock(volatile int *pLock,int setVal)
{ 
  *pLock = setVal;



}

//////////////////////////////////////////////////////////////////////////
ILINE void CryInterlockedAdd(volatile int *pVal, int iAdd)
{
#ifdef _CPU_X86
# ifdef __GNUC__
	__asm__ __volatile__ (
		"        lock add %[iAdd], (%[pVal])\n"
		: "=m" (*pVal)
		: [pVal] "r" (pVal), "m" (*pVal), [iAdd] "r" (iAdd)
		);
# else
	__asm
	{
		mov edx, pVal
		mov eax, iAdd
		lock add [edx], eax
	}
# endif
#else

































	// NOTE: The code below will fail on 64bit architectures!




#if defined(_WIN64)
  _InterlockedExchangeAdd((volatile long*)pVal,iAdd);
#else
  InterlockedExchangeAdd((volatile long*)pVal,iAdd);
#endif

#endif
}

#endif //__SPU__

//special define to guard SPU driver compilation
#if !defined(JOB_LIB_COMP) && !defined(_SPU_JOB)

ILINE void CryReadLock(volatile int *rw, bool yield)
{
	CryInterlockedAdd(rw,1);
#ifdef NEED_ENDIAN_SWAP




















	volatile char *pw=(volatile char*)rw+1;







	{
		for(;*pw;);
	}

#else
	volatile char *pw=(volatile char*)rw+2;







	{
		for(;*pw;);
	}
#endif
}

ILINE void CryReleaseReadLock(volatile int* rw)
{
	CryInterlockedAdd(rw,-1);
}

ILINE void CryWriteLock(volatile int* rw)
{
	CrySpinLock(rw,0,WRITE_LOCK_VAL);
}

ILINE void CryReleaseWriteLock(volatile int* rw)
{
	CryInterlockedAdd(rw,-WRITE_LOCK_VAL);
}

//////////////////////////////////////////////////////////////////////////
struct ReadLock
{
	ILINE ReadLock(volatile int &rw)
	{
		CryInterlockedAdd(prw=&rw,1);
#ifdef NEED_ENDIAN_SWAP









		volatile char *pw=(volatile char*)&rw+1; for(;*pw;);

#else
		volatile char *pw=(volatile char*)&rw+2; for(;*pw;);
#endif
	}
	ILINE ReadLock(volatile int &rw, bool yield)
	{
		CryReadLock(prw=&rw, yield);
	}
	~ReadLock()
	{
		CryReleaseReadLock(prw);
	}
private:
	volatile int *prw;
};

struct ReadLockCond
{
	ILINE ReadLockCond(volatile int &rw,int bActive)
	{
		if (bActive)
		{
			CryInterlockedAdd(&rw,1);
			bActivated = 1;
#ifdef NEED_ENDIAN_SWAP









			volatile char *pw=(volatile char*)&rw+1; for(;*pw;);

#else
			volatile char *pw=(volatile char*)&rw+2; for(;*pw;);
#endif
		}
		else
		{
			bActivated = 0;
		}
		prw = &rw; 
	}
	void SetActive(int bActive=1) { bActivated = bActive; }
	void Release() { CryInterlockedAdd(prw,-bActivated); }
	~ReadLockCond()
	{



		CryInterlockedAdd(prw,-bActivated);
	}

private:
	volatile int *prw;
	int bActivated;
};

//////////////////////////////////////////////////////////////////////////
struct WriteLock
{
	ILINE WriteLock(volatile int &rw) { CryWriteLock(&rw); prw=&rw; }
	~WriteLock() { CryReleaseWriteLock(prw); }
private:
	volatile int *prw;
};

//////////////////////////////////////////////////////////////////////////
struct WriteAfterReadLock
{
	ILINE WriteAfterReadLock(volatile int &rw) { CrySpinLock(&rw,1,WRITE_LOCK_VAL+1); prw=&rw; }
	~WriteAfterReadLock() { CryInterlockedAdd(prw,-WRITE_LOCK_VAL); }
private:
	volatile int *prw;
};

//////////////////////////////////////////////////////////////////////////
struct WriteLockCond
{
	ILINE WriteLockCond(volatile int &rw,int bActive=1)
	{
		if (bActive)
			CrySpinLock(&rw,0,iActive=WRITE_LOCK_VAL);
		else 
			iActive = 0;
		prw = &rw; 
	}
	ILINE WriteLockCond() { prw=&(iActive=0); }
	~WriteLockCond() { 



		CryInterlockedAdd(prw,-iActive); 
	}
	void SetActive(int bActive=1) { iActive = -bActive & WRITE_LOCK_VAL; }
	void Release() { CryInterlockedAdd(prw,-iActive); }
	volatile int *prw;
	int iActive;
};

//////////////////////////////////////////////////////////////////////////
















































































































#endif//JOB_LIB_COMP

#endif // __MultiThread_h__
