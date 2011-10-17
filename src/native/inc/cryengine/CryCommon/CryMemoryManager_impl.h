////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   CryMemoryManager_impl.h
//  Version:     v1.00
//  Created:     27/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Provides implementation for CryMemoryManager globally defined functions.
//               This file included only by platform_impl.h, do not include it directly in code!
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
 
#ifndef __CryMemoryManager_impl_h__
#define __CryMemoryManager_impl_h__
#pragma once

#ifdef _LIB
	#include "ISystem.h"
#endif

#include "CryLibrary.h"

#define TRACE_ALLOCATION_LIMIT 100000































const int CM_MaxMemSize[eCryM_Num] = 
{
  0
};


// In debug builds, pass the standard allocator routine in to override alloc behavior.
#if defined(_DEBUG) && !defined(PS3)
	// Standard lib alloc routine (used in debug builds).
	#undef malloc
	#undef calloc
	#undef realloc
	#undef free
#endif

#define DEBUG_ALLOC_FLAG	0x80000000

#ifndef XENON
#define DLL_ENTRY_CRYMALLOC "CryMalloc"
#define DLL_ENTRY_CRYFREE "CryFree"
#define DLL_ENTRY_CRYREALLOC "CryRealloc"
#define DLL_ENTRY_CRYGETMEMSIZE "CryGetMemSize"
#define DLL_ENTRY_CRYCRTMALLOC "CrySystemCrtMalloc"
#define DLL_ENTRY_CRYCRTFREE "CrySystemCrtFree"
#define DLL_ENTRY_CRYCRTSIZE "CrySystemCrtSize"
#define DLL_ENTRY_GETMEMMANAGER "CryGetIMemoryManagerInterface"









#endif

#ifdef _LIB
  const char *CM_Name[eCryM_Num] = 
  {
    "Cry3DEngine",
    "CryAction",
    "CryAISystem",
    "CryAnimation",
    "CryEntitySystem",
    "CryFont",
    "CryInput",
    "CryMovie",
    "CryNetwork",
    "CryPhysics",
	"CryScaleform",
    "CryScriptSystem",
    "CrySoundSystem",
    "CrySystem",
    "CryGame",
    "CryRender",
    "Launcher",
	"Cemono",
  };
#endif























































//////////////////////////////////////////////////////////////////////////
// _PoolHelper definition.
//////////////////////////////////////////////////////////////////////////
struct _CryMemoryManagerPoolHelper
{



  typedef void *(*FNC_CryMalloc)(size_t size, size_t& allocated);

	typedef void *(*FNC_CryCrtMalloc)(size_t size);



  typedef void *(*FNC_CryRealloc)(void *memblock, size_t size, size_t& allocated);

  typedef size_t (*FNC_CryFree)(void *p);
	typedef void (*FNC_CryGetIMemoryManagerInterface)( void **p );
	typedef void *(*FNC_CryCrtRealloc)(void *p, size_t size);

#ifndef _LIB
  static volatile long allocatedMemory;
  static volatile long freedMemory;
  static volatile long requestedMemory;
  static volatile int numAllocations;
#else
  static uint64 allocatedMemory[eCryM_Num];
  static uint64 freedMemory[eCryM_Num];
  static uint64 requestedMemory[eCryM_Num];
  static int numAllocations[eCryM_Num];
#endif

//	typedef size_t (*FNC_CryFree)(void);
	typedef size_t (*FNC_CryGetMemSize)(void *p, size_t);
	typedef int (*FNC_CryStats)(char *buf);
	
	static FNC_CryMalloc _CryMalloc;
	static FNC_CryRealloc _CryRealloc;
	static FNC_CryFree _CryFree;
	static FNC_CryGetMemSize _CryGetMemSize;
	static FNC_CryCrtMalloc _CryCrtMalloc;
	static FNC_CryFree _CryCrtFree;
	static FNC_CryCrtRealloc _CryCrtRealloc;
	static FNC_CryFree _CryCrtSize;
	static FNC_CryGetIMemoryManagerInterface _CryGetIMemoryManagerInterface;


	static int m_bInitialized;







	static void Init()
	{
#ifdef LINUX
		if (m_bInitialized)
			return;
#endif

		m_bInitialized = 1;
		
#ifndef _LIB
    allocatedMemory = 0;
    freedMemory = 0;
    requestedMemory = 0;
    numAllocations = 0;











		HMODULE hMod;
		int iter;
#ifdef LINUX
		for(iter=0,hMod=::dlopen(NULL, RTLD_LAZY); hMod; iter++)
#else
		for(iter=0,hMod=GetModuleHandle(0); hMod; iter++)
#endif
		{
			_CryMalloc=(FNC_CryMalloc)CryGetProcAddress(hMod,DLL_ENTRY_CRYMALLOC);
			_CryRealloc=(FNC_CryRealloc)CryGetProcAddress(hMod,DLL_ENTRY_CRYREALLOC);
			_CryFree=(FNC_CryFree)CryGetProcAddress(hMod,DLL_ENTRY_CRYFREE);
			_CryGetMemSize=(FNC_CryGetMemSize)CryGetProcAddress(hMod,DLL_ENTRY_CRYGETMEMSIZE);
			_CryCrtMalloc=(FNC_CryCrtMalloc)CryGetProcAddress(hMod,DLL_ENTRY_CRYCRTMALLOC);
			_CryCrtSize=(FNC_CryFree)CryGetProcAddress(hMod,DLL_ENTRY_CRYCRTSIZE);
			_CryCrtFree=(FNC_CryFree)CryGetProcAddress(hMod,DLL_ENTRY_CRYCRTFREE);
			_CryGetIMemoryManagerInterface=(FNC_CryGetIMemoryManagerInterface)CryGetProcAddress(hMod,DLL_ENTRY_GETMEMMANAGER);

			if ((_CryMalloc && _CryRealloc && _CryFree && _CryGetMemSize && _CryCrtMalloc && _CryCrtFree && _CryCrtSize && _CryGetIMemoryManagerInterface) || iter==1)
				break;
#ifdef LINUX
			hMod = CryLoadLibrary("CrySystem.so");
#else
      hMod = CryLoadLibrary("CrySystem.dll"); 
#endif
		}
		if (!hMod || !_CryMalloc || !_CryRealloc || !_CryFree || !_CryGetMemSize ||! _CryCrtMalloc || !_CryCrtFree || !_CryCrtSize || !_CryGetIMemoryManagerInterface)
		{
#ifndef AVOID_MEMORY_ERROR
			#ifdef WIN32
				MessageBox(NULL, "Could not access CrySystem.dll (check working directory)", "Memory Manager", MB_OK);
      #else
      if (!hMod)
        OutputDebugString("Could not access CrySystem.dll (check working directory)");
      else
        OutputDebugString("Could not get Memory Functions in CrySystem.dll");
			#endif
			exit(1);
#endif
		};

#else
    for (int i=0; i<eCryM_Num; i++)
    {
      allocatedMemory[i] = 0;
      freedMemory[i] = 0;
      requestedMemory[i] = 0;
      numAllocations[i] = 0;
    }









		_CryMalloc=CryMalloc;
		_CryRealloc=CryRealloc;
		_CryFree=CryFree;
		_CryGetMemSize=CryGetMemSize;
		_CryCrtMalloc=CrySystemCrtMalloc;
		_CryCrtRealloc=CrySystemCrtRealloc;
		_CryCrtFree=(FNC_CryFree)CrySystemCrtFree;
		_CryCrtSize=(FNC_CryFree)CrySystemCrtSize;
		_CryGetIMemoryManagerInterface=(FNC_CryGetIMemoryManagerInterface)CryGetIMemoryManagerInterface;


#endif
	}

#ifndef _LIB
	static void FakeAllocation( long size )
	{
		if (!m_bInitialized)
			Init();
		CryInterlockedExchangeAdd(&allocatedMemory, size);
		CryInterlockedExchangeAdd(&requestedMemory, size);
		GetISystem()->GetIMemoryManager()->FakeAllocation( size );
	}
#else //_LIB
	static void FakeAllocation( long size,ECryModule eCM )
	{
		if (!m_bInitialized)
			Init();
		allocatedMemory[eCM] += size;
		requestedMemory[eCM] += size;
		GetISystem()->GetIMemoryManager()->FakeAllocation( size );
	}
#endif //_LIB

	//////////////////////////////////////////////////////////////////////////
	static IMemoryManager* GetIMemoryManager()
	{
		if (!m_bInitialized)
			Init();
		void *ptr = 0;
		#ifdef _LIB
			CryGetIMemoryManagerInterface((void**)&ptr);
		#else
			_CryGetIMemoryManagerInterface((void**)&ptr);
		#endif
		return (IMemoryManager*)ptr;
	}
 
#ifndef _LIB
	static void GetMemoryInfo( CryModuleMemoryInfo *pMmemInfo )
	{
		pMmemInfo->allocated = allocatedMemory;
		pMmemInfo->freed = freedMemory;
		pMmemInfo->requested = requestedMemory;
		pMmemInfo->num_allocations = numAllocations;
#ifdef CRY_STRING
		pMmemInfo->CryString_allocated = string::_usedMemory(0) + wstring::_usedMemory(0);
#endif
		pMmemInfo->STL_allocated = 0;
		pMmemInfo->STL_wasted = 0;
#if defined(CRY_STL_ALLOC) && defined(STLPORT)
		std::vector<int> dummy;
		//@TODO: Timur, Fix it back
		//pMmemInfo->STL_allocated = dummy.get_allocator().get_heap_size();
		pMmemInfo->STL_wasted = 0;//dummy.get_allocator().get_wasted_in_allocation() + dummy.get_allocator().get_wasted_in_blocks();
#endif
	}
#else
  static void GetMemoryInfo( CryModuleMemoryInfo *pMmemInfo, ECryModule eCM )
  {
    pMmemInfo->allocated = allocatedMemory[eCM];
    pMmemInfo->freed = freedMemory[eCM];
    pMmemInfo->requested = requestedMemory[eCM];
    pMmemInfo->num_allocations = numAllocations[eCM];
#ifdef CRY_STRING
		pMmemInfo->CryString_allocated = string::_usedMemory(0) + wstring::_usedMemory(0);
#endif
  }
#endif

	//////////////////////////////////////////////////////////////////////////
	// Local version of allocations, does memory counting per module.
	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static ILINE void*	Malloc(size_t size)
#else
  static ILINE void*	Malloc(size_t size, ECryModule eCM)
#endif
	{
		if (!m_bInitialized)
			Init();

#ifdef TRACE_ALL_ALLOCATIONS
		if (size > TRACE_ALLOCATION_LIMIT) {
			FILE * f = fopen("d:\\allocations.txt", "a");
			fprintf(f, "Alloc:%s:%i\n", DEFINE_MODULE_NAME, size);
			fclose(f);
		}
#endif

		size_t allocated;
#ifndef __MEMORY_VALIDATOR_workaround



		void * p = _CryMalloc( size, allocated );

# ifndef _LIB
    CryInterlockedExchangeAdd(&allocatedMemory, allocated);
		//allocatedMemory += allocated;
# else
	allocatedMemory[eCM] += allocated;
# endif
#else
		void * p = _CryMalloc( size);
    allocated = _CryGetMemSize(p)
    CryInterlockedExchangeAdd(&allocatedMemory, allocated);
		//allocatedMemory += ;
#endif

#ifndef _LIB
    //requestedMemory += size;
    CryInterlockedExchangeAdd(&requestedMemory, size);
		//numAllocations++;
    CryInterlockedIncrement(&numAllocations);
#else
    requestedMemory[eCM] += size;
    numAllocations[eCM]++;
    if (CM_MaxMemSize[eCM]>0 && requestedMemory[eCM]/(1024*1024) > CM_MaxMemSize[eCM])
    {
      CryFatalError("Memory usage for module '%s' exceed limit (%d Mb)", CM_Name[eCM], CM_MaxMemSize[eCM]);
    }
#endif

		return p;
	}

	//special alignment version of malloc







































	//////////////////////////////////////////////////////////////////////////
	// Local version of allocations, does memory counting per module.
	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static ILINE void*	Calloc(size_t num,size_t size)
	{
		void *p = (char*)Malloc(num*size);
		memset(p, 0, num*size );
		return p;
	}
#else
  static ILINE void*	Calloc(size_t num,size_t size, ECryModule eCM)
  {
    void *p = (char*)Malloc(num*size, eCM);
    memset(p, 0, num*size );
    return p;
  }
#endif

	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static ILINE void*	Realloc(void *memblock,size_t size)
#else



		static ILINE void*	Realloc(void *memblock,size_t size, ECryModule eCM)

#endif
	{
		if (!m_bInitialized)
			Init();

#ifdef TRACE_ALL_ALLOCATIONS
		if (size >	TRACE_ALLOCATION_LIMIT) {
			FILE * f = fopen("d:\\allocations.txt", "a");
			fprintf(f, "realloc:%s:%i\n", DEFINE_MODULE_NAME, size);
			fclose(f);
		}

#endif

		if (memblock == NULL)
		{
			//numAllocations++;
			//allocatedMemory += size;
#ifndef _LIB
			void *p = Malloc(size);
			return p;
#else




      return Malloc(size, eCM);

#endif
		}

//		int* t = (int*)memblock;
		size_t oldsize = _CryGetMemSize(memblock, size);//*--t;	
		size_t allocated;
#ifndef __MEMORY_VALIDATOR_workaround



		void * p=  _CryRealloc( memblock, size, allocated );

# ifndef _LIB
    CryInterlockedExchangeAdd(&allocatedMemory, allocated);
		/*allocatedMemory += allocated;*/
# else
    allocatedMemory[eCM] += allocated;
# endif
#else
		void * p=  _CryRealloc( memblock, size);
		size_t allocated = _CryGetMemSize(p);
    CryInterlockedExchangeAdd(&allocatedMemory, allocated);
#endif

#ifndef _LIB
    CryInterlockedIncrement(&numAllocations);
    //numAllocations++;

    CryInterlockedExchangeAdd(&requestedMemory, size);
    //requestedMemory += size;
    long tt = oldsize &= ~DEBUG_ALLOC_FLAG;
    CryInterlockedExchangeAdd(&freedMemory, tt);
    //freedMemory += tt;
#else
    numAllocations[eCM]++;
    requestedMemory[eCM] += size;
    freedMemory[eCM] += oldsize &= ~DEBUG_ALLOC_FLAG;
    if (CM_MaxMemSize[eCM]>0 && requestedMemory[eCM]/(1024*1024) > CM_MaxMemSize[eCM])
    {
      CryFatalError("Memory usage for module '%s' exceed limit (%d Mb)", CM_Name[eCM], CM_MaxMemSize[eCM]);
    }
#endif
/*
		if (p)
			return p;
		else
		*/
		return p;
			
	}
	//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
	static ILINE size_t Free( void *memblock )
#else
  static ILINE size_t Free( void *memblock, ECryModule eCM )
#endif
	{
		size_t freed = 0;


		if (!m_bInitialized)
			Init();
		if (memblock != 0)
		{
			/*
			if (size & DEBUG_ALLOC_FLAG)
			{
				// Free using custom allocator.
				size &= ~DEBUG_ALLOC_FLAG;
				TPFAlloc pa = (TPFAlloc)*--t;
				pa(t, 0);
			}
			else
			*/
			freed = _CryFree( memblock );
#ifndef _LIB				
      CryInterlockedExchangeAdd(&freedMemory, freed);
			//freedMemory += freed;
#else
      freedMemory[eCM] += freed;
#endif
		}

#ifdef TRACE_ALL_ALLOCATIONS
		if (freed > TRACE_ALLOCATION_LIMIT) {
			FILE * f = fopen("d:\\allocations.txt", "a");
			fprintf(f, "Free:%s:%i\n", DEFINE_MODULE_NAME, freed);
			fclose(f);
		}

#endif
		return freed;
	}

  static ILINE size_t	MemSize(void* ptr, size_t sz)
	{
		if (!m_bInitialized)
			Init();

		size_t realSize = _CryGetMemSize( ptr, sz );

		return realSize;
	}
};

//////////////////////////////////////////////////////////////////////////
// Static variables.
//////////////////////////////////////////////////////////////////////////
#ifndef _LIB
volatile long _CryMemoryManagerPoolHelper::allocatedMemory = 0;
volatile long _CryMemoryManagerPoolHelper::freedMemory = 0;
volatile long _CryMemoryManagerPoolHelper::requestedMemory = 0;
volatile int _CryMemoryManagerPoolHelper::numAllocations = 0;
#else
uint64 _CryMemoryManagerPoolHelper::allocatedMemory[eCryM_Num];
uint64 _CryMemoryManagerPoolHelper::freedMemory[eCryM_Num];
uint64 _CryMemoryManagerPoolHelper::requestedMemory[eCryM_Num];
int _CryMemoryManagerPoolHelper::numAllocations[eCryM_Num];
#endif
_CryMemoryManagerPoolHelper::FNC_CryMalloc _CryMemoryManagerPoolHelper::_CryMalloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryGetMemSize _CryMemoryManagerPoolHelper::_CryGetMemSize = NULL;
_CryMemoryManagerPoolHelper::FNC_CryRealloc _CryMemoryManagerPoolHelper::_CryRealloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryFree _CryMemoryManagerPoolHelper::_CryFree = NULL;
_CryMemoryManagerPoolHelper::FNC_CryCrtMalloc _CryMemoryManagerPoolHelper::_CryCrtMalloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryCrtRealloc _CryMemoryManagerPoolHelper::_CryCrtRealloc = NULL;
_CryMemoryManagerPoolHelper::FNC_CryFree _CryMemoryManagerPoolHelper::_CryCrtFree = NULL;
_CryMemoryManagerPoolHelper::FNC_CryFree _CryMemoryManagerPoolHelper::_CryCrtSize = NULL;
_CryMemoryManagerPoolHelper::FNC_CryGetIMemoryManagerInterface _CryMemoryManagerPoolHelper::_CryGetIMemoryManagerInterface = NULL;
int _CryMemoryManagerPoolHelper::m_bInitialized = 0;
//////////////////////////////////////////////////////////////////////////

#ifndef _LIB

#  if !defined(NOT_USE_CRY_MEMORY_MANAGER)
//////////////////////////////////////////////////////////////////////////
void* CryModuleMalloc( size_t size ) throw()
{
	return _CryMemoryManagerPoolHelper::Malloc(size);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleCalloc( size_t num,size_t size ) throw()
{
	return _CryMemoryManagerPoolHelper::Calloc(num,size);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleRealloc( void *ptr,size_t size )  throw()
{
	return _CryMemoryManagerPoolHelper::Realloc(ptr,size);
};
//////////////////////////////////////////////////////////////////////////
size_t  CryModuleFree( void *ptr ) throw()
{
	return _CryMemoryManagerPoolHelper::Free(ptr);
};
size_t CryModuleMemSize( void* ptr, size_t sz) throw()
{
	return _CryMemoryManagerPoolHelper::MemSize(ptr, sz);
}

#  endif

//////////////////////////////////////////////////////////////////////////
CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo )
{
#if !defined(NOT_USE_CRY_MEMORY_MANAGER)
	_CryMemoryManagerPoolHelper::GetMemoryInfo(pMemInfo);
#endif
};

void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo)
{
#if !defined(NOT_USE_CRY_MEMORY_MANAGER)
	_CryMemoryManagerPoolHelper::GetMemoryInfo(pInfo);
#endif
}

//FIX: just emulate of allocation of aligned memory for PC
void* CryModuleMemalign( size_t size, size_t alignment ) throw()
{
#  if !defined(NOT_USE_CRY_MEMORY_MANAGER)
	uint8 * p = (uint8 *)_CryMemoryManagerPoolHelper::Malloc(size + alignment);
#else
	uint8 * p = (uint8 *)malloc(size + alignment);
#endif
	if (p)
	{
		uint8 offset = (BYTE)(alignment - ((UINT_PTR)p & (alignment - 1)));
		p += offset;
		p[-1] = offset;
	}
	return p;
};

void CryModuleMemalignFree(void* p) throw()
{
	if(p)
	{
		uint8* pb = static_cast<uint8*>(p);
		pb -= pb[-1];
#  if !defined(NOT_USE_CRY_MEMORY_MANAGER)
		_CryMemoryManagerPoolHelper::Free(pb);
#else
		free(pb);
#endif
	}
};


#else















//FIX: just emulate of allocation of aligned memory for PC
	void* CryModuleMemalign( size_t size, size_t alignment, ECryModule eCM ) throw()
	{
		uint8 * p = (uint8 *)_CryMemoryManagerPoolHelper::Malloc(size + alignment, eCM);
		if (p)
		{
			uint8 offset = (BYTE)(alignment - ((UINT_PTR)p & (alignment - 1)));
			p += offset;
			p[-1] = offset;
		}
		return p;
	};

	void CryModuleMemalignFree(void* p, ECryModule eCM) throw()
	{
		if(p)
		{
			uint8* pb = static_cast<uint8*>(p);
			pb -= pb[-1];
			_CryMemoryManagerPoolHelper::Free(pb, eCM);
		}
	};


//////////////////////////////////////////////////////////////////////////
void* CryModuleMalloc( size_t size, ECryModule eCM ) throw()
{
  return _CryMemoryManagerPoolHelper::Malloc(size, eCM);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleCalloc( size_t num, size_t size, ECryModule eCM ) throw()
{
  return _CryMemoryManagerPoolHelper::Calloc(num,size,eCM);
};
//////////////////////////////////////////////////////////////////////////
void* CryModuleRealloc( void *ptr, size_t size, ECryModule eCM )  throw()
{



  return _CryMemoryManagerPoolHelper::Realloc(ptr,size, eCM);

};






//////////////////////////////////////////////////////////////////////////
size_t  CryModuleFree( void *ptr, ECryModule eCM ) throw()
{
  return _CryMemoryManagerPoolHelper::Free(ptr, eCM);
};
//////////////////////////////////////////////////////////////////////////
size_t CryModuleMemSize(void* ptr, size_t sz) throw()
{
	return _CryMemoryManagerPoolHelper::MemSize(ptr, sz);
}
//////////////////////////////////////////////////////////////////////////
CRY_MEM_USAGE_API void CryModuleGetMemoryInfo( CryModuleMemoryInfo *pMemInfo, ECryModule eCM )
{
  _CryMemoryManagerPoolHelper::GetMemoryInfo(pMemInfo, eCM);
};
//////////////////////////////////////////////////////////////////////////
void CryGetMemoryInfoForModule(CryModuleMemoryInfo * pInfo)
{
	_CryMemoryManagerPoolHelper::GetMemoryInfo(pInfo, eCryModule);
};
#endif

void *CryCrtMalloc(size_t size)
{
#if defined(NOT_USE_CRY_MEMORY_MANAGER) || (defined(_DEBUG) && !defined(PS3))
	return malloc(size);
#else
#ifdef LINUX
	_CryMemoryManagerPoolHelper::Init();
#endif
	return _CryMemoryManagerPoolHelper::_CryCrtMalloc(size);
#endif
}

size_t CryCrtFree(void *p)
{
#if defined(NOT_USE_CRY_MEMORY_MANAGER) || (defined(_DEBUG) && !defined(PS3))
	free(p);
	return 0;
#else
	return _CryMemoryManagerPoolHelper::_CryCrtFree(p);
#endif
};

size_t CryCrtSize(void *p)
{
#if defined(NOT_USE_CRY_MEMORY_MANAGER) || (defined(_DEBUG) && !defined(PS3))
#ifdef LINUX
	return malloc_usable_size(p);
#else
	return _msize(p);
#endif
#else
	return _CryMemoryManagerPoolHelper::_CryCrtSize(p);
#endif
};


// If using CryMemoryManager, redefine new & delete for entire module.
#if (!defined(_DEBUG) || defined(PS3)) && !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)
	#if !defined(_LIB) && !defined(NEW_OVERRIDEN)
		void * __cdecl operator new   (size_t size) { return CryModuleMalloc(size); } 
		void * __cdecl operator new[] (size_t size) { return CryModuleMalloc(size); }; 
		void __cdecl operator delete  (void *p) { CryModuleFree(p); };
		void __cdecl operator delete[](void *p) { CryModuleFree(p); };
	#endif//_LIB
#endif //!defined(_DEBUG) && !defined(NOT_USE_CRY_MEMORY_MANAGER) && !defined(__SPU__)

//////////////////////////////////////////////////////////////////////////
#if !defined(_LIB) && !defined(PS3)
IMemoryManager *CryGetIMemoryManager()
{
	static IMemoryManager* memMan = _CryMemoryManagerPoolHelper::GetIMemoryManager();
	return memMan;
}
#endif


















































































































































































// ~memReplay

#endif // __CryMemoryManager_impl_h__
