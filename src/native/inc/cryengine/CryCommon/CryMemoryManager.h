////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   CryMemoryManager.h
//  Version:     v1.00
//  Created:     27/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Defines functions for CryEngine custom memory manager.
//               See also CryMemoryManager_impl.h, it must be included only once per module.
//               CryMemoryManager_impl.h is included by platform_impl.h
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
 
#ifndef __CryMemoryManager_h__
#define __CryMemoryManager_h__
#pragma once

//////////////////////////////////////////////////////////////////////////
// Define this if you want to use slow debug memory manager in any config.
//////////////////////////////////////////////////////////////////////////
//#define DEBUG_MEMORY_MANAGER
//////////////////////////////////////////////////////////////////////////

// THIS DEFINE ESPECIALLY FOR SOLVING TROUBLES WITH MEMORY ALLOCATOR. SHOULD BE DISABLED IN OTHER CASES
//#define __MEMORY_VALIDATOR_workaround

// That mean we use node_allocator for all small allocations

#define NOT_STANDARD_CRT

#ifdef DEBUG_MEMORY_MANAGER
	#define NOT_USE_CRY_MEMORY_MANAGER
#endif

#include "platform.h"

#include <stdarg.h>

#ifndef STLALLOCATOR_CLEANUP
#define STLALLOCATOR_CLEANUP
#endif

//#ifdef NOT_USE_CRY_MEMORY_MANAGER
//#define CryModuleMalloc CRYMEMORYMANAGER_API malloc
//#define CryModuleFree CRYMEMORYMANAGER_API free
//#endif

#if !defined(PS3)
	#include <malloc.h>






#endif

#if defined(__cplusplus)
#if defined(PS3) || defined(LINUX)
#define NOT_STANDARD_CRT
	#include <new>
#else
	#include <new.h>
#endif
// required for the inclusion of annotated CryXXX Memory Methods
#if defined(__CRYCG__)
  #include "PS3CryCache.h"
#endif
#endif

#ifndef _LIB
	#ifdef CRYSYSTEM_EXPORTS
		#define CRYMEMORYMANAGER_API DLL_EXPORT
	#else
		#define CRYMEMORYMANAGER_API DLL_IMPORT
	#endif
#else //_LIB
	#define CRYMEMORYMANAGER_API
#endif //_LIB



#ifdef __cplusplus

//////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_MEMORY_MANAGER
	#ifdef _DEBUG
		#define _DEBUG_MODE
	#endif
#endif //DEBUG_MEMORY_MANAGER

#if defined(_DEBUG) && !defined(PS3) && !defined(LINUX)
	#include <crtdbg.h>
#endif //_DEBUG

// checks if the heap is valid in debug; in release, this function shouldn't be called
// returns non-0 if it's valid and 0 if not valid
ILINE int IsHeapValid()
{
#if (defined(_DEBUG) && !defined(RELEASE_RUNTIME) && !defined(PS3)) || (defined(DEBUG_MEMORY_MANAGER))
	return _CrtCheckMemory();
#else
	return true;
#endif
}

#ifdef DEBUG_MEMORY_MANAGER
// Restore debug mode define
	#ifndef _DEBUG_MODE
		#undef _DEBUG
	#endif //_DEBUG_MODE
#endif //DEBUG_MEMORY_MANAGER
//////////////////////////////////////////////////////////////////////////

#endif //__cplusplus

struct ICustomMemoryHeap;
class IGeneralMemoryHeap;

#if !defined(_LIB) && !defined(PS3)
#define MEMMAN_METHOD(...) virtual __VA_ARGS__ = 0
#else
#define MEMMAN_METHOD(...) static __VA_ARGS__
#endif

// Description:
//	 Interfaces that allow access to the CryEngine memory manager.
struct IMemoryManager
{
	typedef unsigned char HeapHandle;
	enum { BAD_HEAP_HANDLE = 0xFF };

	struct SProcessMemInfo
	{
		uint64 PageFaultCount;
		uint64 PeakWorkingSetSize;
		uint64 WorkingSetSize;
		uint64 QuotaPeakPagedPoolUsage;
		uint64 QuotaPagedPoolUsage;
		uint64 QuotaPeakNonPagedPoolUsage;
		uint64 QuotaNonPagedPoolUsage;
		uint64 PagefileUsage;
		uint64 PeakPagefileUsage;

		uint64 TotalPhysicalMemory;
		int64  FreePhysicalMemory;

		uint64 TotalVideoMemory;
		int64 FreeVideoMemory;
	};

#if !defined(_LIB) && !defined(PS3)
	virtual ~IMemoryManager(){}
#else
	static IMemoryManager* GetInstance();
#endif

	MEMMAN_METHOD( bool GetProcessMemInfo( SProcessMemInfo &minfo ) );

	// Description:
	//	 Used to add memory block size allocated directly from the crt or OS to the memory manager statistics.
	MEMMAN_METHOD( void FakeAllocation( long size ) );

	// Initialise the level heap.
	MEMMAN_METHOD( void InitialiseLevelHeap() );

	// Switch the default heap to the level heap.
	MEMMAN_METHOD( void SwitchToLevelHeap() );

	// Switch the default heap to the global heap.
	MEMMAN_METHOD( void SwitchToGlobalHeap() );

	// Enable the global heap for this thread only. Returns previous heap selection, which must be passed to LocalSwitchToHeap.
	MEMMAN_METHOD( int LocalSwitchToGlobalHeap() );

	// Switch to a specific heap for this thread only. Usually used to undo a previous LocalSwitchToGlobalHeap
	MEMMAN_METHOD( void LocalSwitchToHeap(int heap) );

	// Fetch the violation status of the level heap
	MEMMAN_METHOD( bool GetLevelHeapViolationState(bool& usingLevelHeap, size_t& numAllocs, size_t& allocSize) );

	//////////////////////////////////////////////////////////////////////////
	// Heap Tracing API
	MEMMAN_METHOD( HeapHandle TraceDefineHeap( const char *heapName,size_t size,const void* pBase ) );
	MEMMAN_METHOD( void TraceHeapAlloc( HeapHandle heap,void* mem, size_t size, size_t blockSize, const char *sUsage,const char *sNameHint=0 ) );
	MEMMAN_METHOD( void TraceHeapFree( HeapHandle heap,void* mem, size_t blockSize ) );
	MEMMAN_METHOD( void TraceHeapSetColor( uint32 color ) );
	MEMMAN_METHOD( uint32 TraceHeapGetColor() );
	MEMMAN_METHOD( void TraceHeapSetLabel( const char *sLabel ) );
	//////////////////////////////////////////////////////////////////////////

	// Retrieve access to the MemReplay implementation class.
	MEMMAN_METHOD( struct IMemReplay* GetIMemReplay() );

	// Create an instance of ICustomMemoryHeap
	MEMMAN_METHOD( ICustomMemoryHeap* const CreateCustomMemoryHeapInstance(bool const bCanUseGPUMemory) );

	MEMMAN_METHOD( IGeneralMemoryHeap* CreateGeneralMemoryHeap(void* base, size_t sz,const char *sUsage) );
};

#undef MEMMAN_METHOD

// Global function implemented in CryMemoryManager_impl.h
#if defined(_LIB) || defined(PS3)
inline IMemoryManager* CryGetIMemoryManager()
{
	extern IMemoryManager g_memoryManager;
	return &g_memoryManager;
}
#else
IMemoryManager *CryGetIMemoryManager();
#endif

class STraceHeapAllocatorAutoColor
{
public:
	explicit STraceHeapAllocatorAutoColor( uint32 color ) { m_color = CryGetIMemoryManager()->TraceHeapGetColor(); CryGetIMemoryManager()->TraceHeapSetColor(color); }
	~STraceHeapAllocatorAutoColor() { CryGetIMemoryManager()->TraceHeapSetColor(m_color); };
protected:
	uint32 m_color;
	STraceHeapAllocatorAutoColor() {};
};

#define TRACEHEAP_AUTOCOLOR(color) STraceHeapAllocatorAutoColor _auto_color_(color);


// Summary:
//		Structure filled by call to CryModuleGetMemoryInfo().
struct CryModuleMemoryInfo
{
	uint64 requested;
	// Total Ammount of memory allocated.
	uint64 allocated;
	// Total Ammount of memory freed.
	uint64 freed;
	// Total number of memory allocations.
	int num_allocations;
	// Allocated in CryString.
	uint64 CryString_allocated;
	// Allocated in STL.
	uint64 STL_allocated;
	// Amount of memory wasted in pools in stl (not usefull allocations).
	uint64 STL_wasted;
};

struct CryReplayInfo
{
	uint64 uncompressedLength;
	uint64 writtenLength;
	uint32 trackingSize;
	const char* filename;
};

#if defined(NOT_USE_CRY_MEMORY_MANAGER) //&& !defined(JOB_LIB_COMP)
#define CryModuleMalloc malloc
#define CryModuleCalloc calloc
#define CryModuleRealloc realloc






#if defined(LINUX)
#define CryModuleFree free
#elif !defined(__CRYCG__) && !defined(_SPU_JOB)
inline size_t CryModuleFree(void *p) {
	size_t size = 0;
	if (p)
	{
		size = _msize(p);
		free(p);
	}
	return size;
}











#endif

#if defined(__cplusplus) && defined(LINUX)
	inline void * __cdecl operator new   (size_t  size) throw(std::bad_alloc) { return CryModuleMalloc(size); }
	inline void * __cdecl operator new (size_t size, const std::nothrow_t &nothrow) { return CryModuleMalloc(size); };
	inline void * __cdecl operator new[](size_t size) throw(std::bad_alloc) { return CryModuleMalloc(size); }; 
	inline void * __cdecl operator new[](size_t size, const std::nothrow_t &nothrow) { return CryModuleMalloc(size); }
	inline void __cdecl operator delete  (void *p) { CryModuleFree(p); };
	inline void __cdecl operator delete[](void *p) { CryModuleFree(p); };
#endif //__cplusplus

#endif //NOT_USE_CRY_MEMORY_MANAGER

//////////////////////////////////////////////////////////////////////////
// Extern declarations of globals inside CrySystem.
//////////////////////////////////////////////////////////////////////////
#if !defined(__SPU__)
#ifdef __cplusplus 
extern "C" {
#endif //__cplusplus
#ifndef __MEMORY_VALIDATOR_workaround




CRYMEMORYMANAGER_API void *CryMalloc(size_t size, size_t& allocated);
CRYMEMORYMANAGER_API void *CryRealloc(void *memblock,size_t size, size_t& allocated);

CRYMEMORYMANAGER_API size_t CryFree(void *p);
CRYMEMORYMANAGER_API size_t CryGetMemSize(void *p, size_t size);
CRYMEMORYMANAGER_API int  CryStats(char *buf);
CRYMEMORYMANAGER_API void CryFlushAll();
CRYMEMORYMANAGER_API void CryCleanup();
CRYMEMORYMANAGER_API int  CryGetUsedHeapSize();
CRYMEMORYMANAGER_API int  CryGetWastedHeapSize();
CRYMEMORYMANAGER_API void *CrySystemCrtMalloc(size_t size);
CRYMEMORYMANAGER_API void *CrySystemCrtRealloc(void * p, size_t size);
CRYMEMORYMANAGER_API size_t CrySystemCrtFree(void *p);
CRYMEMORYMANAGER_API size_t CrySystemCrtSize(void *p);
CRYMEMORYMANAGER_API void CryGetIMemoryManagerInterface( void **pIMemoryManager );

// This function is local in every module
/*CRYMEMORYMANAGER_API*/ void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo);
#else
CRYMEMORYMANAGER_API void *CryMalloc(size_t size);
CRYMEMORYMANAGER_API void *CryRealloc(void *memblock,size_t size);
CRYMEMORYMANAGER_API size_t CryFree(void *p);
CRYMEMORYMANAGER_API size_t CryGetMemSize(void *p, size_t size);
CRYMEMORYMANAGER_API int  CryStats(char *buf);
CRYMEMORYMANAGER_API void CryFlushAll();
CRYMEMORYMANAGER_API void CryCleanup();
CRYMEMORYMANAGER_API int  CryGetUsedHeapSize();
CRYMEMORYMANAGER_API int  CryGetWastedHeapSize();
CRYMEMORYMANAGER_API void *CrySystemCrtMalloc(size_t size);
CRYMEMORYMANAGER_API void *CrySystemCrtRealloc(void * p, size_t size);
CRYMEMORYMANAGER_API size_t CrySystemCrtFree(void *p);
CRYMEMORYMANAGER_API size_t CrySystemCrtSize(void *p);
CRYMEMORYMANAGER_API void CryGetIMemoryManagerInterface( void **pIMemoryManager );

// This function is local in every module
/*CRYMEMORYMANAGER_API*/ void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo);
#endif
#ifdef __cplusplus
}

#endif //__cplusplus
#endif //!defined(__SPU__)

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Cry Memory Manager accessible in all build modes.
//////////////////////////////////////////////////////////////////////////
#if !defined(USING_CRY_MEMORY_MANAGER)
	#define USING_CRY_MEMORY_MANAGER
#endif

#ifndef _LIB
#define CRY_MEM_USAGE_API extern "C" DLL_EXPORT
#else //_USRDLL
#define CRY_MEM_USAGE_API
#endif //_USRDLL

/////////////////////////////////////////////////////////////////////////
// Extern declarations,used by overriden new and delete operators.
//////////////////////////////////////////////////////////////////////////
extern "C"
{
#ifndef _LIB

#if !defined(__CRYCG__) && !defined(NOT_USE_CRY_MEMORY_MANAGER)
	// For the code scanner we need the annotated declarations from
	// PS3CryCache.h.
  void* CryModuleMemalign(size_t size, size_t alignment) throw();
	void CryModuleMemalignFree(void*) throw();
	void* CryModuleMalloc(size_t size) throw();
	void* CryModuleCalloc(size_t num,size_t size) throw();
	void* CryModuleRealloc(void *memblock,size_t size) throw();
	size_t CryModuleFree(void *ptr) throw();
	size_t CryModuleMemSize(void* ptr, size_t sz) throw();
#else
	#if !defined(__CRYCG__)
		void* CryModuleMemalign(size_t size, size_t alignment) throw();
		void CryModuleMemalignFree(void*) throw();
	#endif // !__CRYCG__
#endif // !__CRYCG__ && ! NOT_USE_CRY_MEMORY_MANAGER

#elif !defined(NOT_USE_CRY_MEMORY_MANAGER) // _LIB

  void* CryModuleMemalign(size_t size, size_t alignment, ECryModule eCM=eCryModule);
	void CryModuleMemalignFree(void*, ECryModule eCM=eCryModule);
	void* CryModuleMalloc(size_t size, ECryModule eCM=eCryModule) throw();
  void* CryModuleCalloc(size_t num, size_t size, ECryModule eCM=eCryModule) throw();
  void* CryModuleRealloc(void *memblock, size_t size, ECryModule eCM=eCryModule) throw();





  #if !defined(NOT_USE_CRY_MEMORY_MANAGER)
		size_t CryModuleFree(void *ptr, ECryModule eCM=eCryModule) throw();
	#endif
	size_t CryModuleMemSize(void* ptr, size_t sz) throw();
#ifndef PS3
  ILINE void* _LibCryModuleMalloc(size_t size) throw() { return CryModuleMalloc(size); }
  ILINE void* _LibCryModuleCalloc(size_t num,size_t size) throw() { return CryModuleCalloc(num, size); }
  ILINE void* _LibCryModuleRealloc(void *memblock,size_t size) throw() { return CryModuleRealloc(memblock, size); }
	ILINE void  _LibCryModuleFree(void *ptr) throw() { CryModuleFree(ptr); }
	ILINE size_t _LibCryModuleMemSize(void* ptr, size_t sz) throw() { return CryModuleMemSize(ptr, sz); }
#endif
#endif
}

#if !defined(PS3)
// Align alloc helpers
inline void* SetAlignPrefix(void* ptr, size_t alignment)
{
	uint8* p = static_cast<uint8*>(ptr);
	if (p)
	{
		uint8 offset = (uint8)(alignment - ((UINT_PTR)p & (alignment - 1)));
		p += offset;
		p[-1] = offset;
	}
	return p;
}
inline void* GetAlignPrefix(void* ptr)
{
	uint8* p = static_cast<uint8*>(ptr);
	if (p)
	{
		p -= p[-1];
	}
	return p;
}

#ifndef _LIB
inline void* CryModuleMemalign( size_t size, size_t alignment ) throw()
{
	return SetAlignPrefix(malloc(size + alignment), alignment);
}
inline void CryModuleMemalignFree(void* p) throw()
{
	free(GetAlignPrefix(p));
}
#endif

#endif












#if !defined(_LIB) && !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__CRYCG__)
	ILINE void* CryModuleMalloc(size_t size, ECryModule) throw() { return CryModuleMalloc(size); };
	ILINE void* CryModuleCalloc(size_t num, size_t size, ECryModule) throw() { return CryModuleCalloc(num, size); };
	ILINE void* CryModuleRealloc(void *memblock, size_t size, ECryModule) throw() { return CryModuleRealloc(memblock, size); };
	ILINE size_t CryModuleFree(void *ptr, ECryModule) throw() { return CryModuleFree(ptr); }
#endif

#if defined(_LIB) && !defined(NEW_OVERRIDEN)
	CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo, ECryModule eCM );
	#if !defined(_DEBUG) || defined(PS3)
    #if !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)














        #if !defined(XENON)
          ILINE void * __cdecl operator new   (size_t size) throw (std::bad_alloc) { return CryModuleMalloc(size, eCryModule); }
        #endif // !defined(XENON)


		ILINE void * __cdecl operator new   (size_t size) { return CryModuleMalloc(size, eCryModule); }

        #if !defined(XENON)
          ILINE void * __cdecl operator new[] (size_t size) throw (std::bad_alloc) { return CryModuleMalloc(size, eCryModule); }
        #endif // !defined(XENON)

        ILINE void * __cdecl operator new[] (size_t size) { return CryModuleMalloc(size, eCryModule); }
        ILINE void __cdecl operator delete  (void *p) throw (){ CryModuleFree(p, eCryModule); };
        ILINE void __cdecl operator delete[](void *p) throw (){ CryModuleFree(p, eCryModule); };

    #else // !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)














    #endif // else !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)
	#endif // !defined(_DEBUG) || defined(PS3)
#else
	CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo );
#endif


//////////////////////////////////////////////////////////////////////////

// Redirect standard memory routines to CryModule functions in this module,
// unless _DEBUG or NOT_USE_CRY_MEMORY_MANAGER

#if defined(__SPU__) || ((!defined(_DEBUG) || defined(PS3)) && !defined(NOT_USE_CRY_MEMORY_MANAGER))
	#undef malloc
	#undef calloc
	#undef realloc
	#undef free
	#undef memalign
# ifndef __CRYCG__
# ifndef _LIB
	#define malloc        CryModuleMalloc
	#define calloc        CryModuleCalloc
	#define realloc       CryModuleRealloc
	#define free          CryModuleFree
# else
  #define malloc        _LibCryModuleMalloc
  #define calloc        _LibCryModuleCalloc
  #define realloc       _LibCryModuleRealloc
  #define free          _LibCryModuleFree
# endif
# endif
#endif

// Description:
//	Useful debug macro.
//	We use memset otherwise windows will no allocate physical memory.
//	We free last one so biggest block left is n MB.
#define MEMORY_DEBUG_POINT(pSystem,no)	\
	{ assert(no);	static ICVar *pVar = pSystem->GetIConsole()->GetCVar("sys_memory_debug");	assert(pVar);\
		if(pVar && pVar->GetIVal()==no)	{\
			void *pMem=0; uint32 dwSumMB=0,dwSizeMB=10;		\
			while(pMem=malloc(dwSizeMB*1024*1024))\
			{ dwSumMB+=dwSizeMB;memset(pMem,no,dwSizeMB*1024*1024);} \
			if(pMem)free(pMem);\
			pSystem->GetILog()->Log("MEMORY_DEBUG_POINT %d activated, %d MB",no,dwSumMB); } }




// Need for our allocator to avoid deadlock in cleanup
/*CRYMEMORYMANAGER_API */void *CryCrtMalloc(size_t size);
/*CRYMEMORYMANAGER_API */size_t CryCrtFree(void *p);
// wrapper for _msize on PC
size_t CryCrtSize(void * p);

#include "CryMemReplay.h"

#if !defined( NOT_USE_CRY_MEMORY_MANAGER) && !defined(JOB_LIB_COMP) // && !defined(_STLP_BEGIN_NAMESPACE) // Avoid non STLport version
#include "CryMemoryAllocator.h"




#endif

//////////////////////////////////////////////////////////////////////////

// Single function which handles all alloc/dealloc cases.
typedef void* (*TPFAlloc)( void* ptr, size_t size );

class ScopedSwitchToGlobalHeap
{



















public:
	ILINE ScopedSwitchToGlobalHeap() {}

};























#endif // __CryMemoryManager_h__
