////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   platform.h
//  Version:     v1.00
//  Created:     11/12/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: Platform dependend stuff.
//               Include this file instead of windows.h
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_H_
#define _PLATFORM_H_
#pragma once

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0600
#endif

// Alignment support.
#if defined(_MSC_VER)
	#define alignof _alignof
#elif defined(__GNUC__)
	#define alignof __alignof__
#endif

// Include Configuration settings for Standart Template Library
#include <STLPortConfig.h>

#if defined(LINUX)
#define _FILE_OFFSET_BITS 64 // define large file support > 2GB
#endif

// Compile with unit testing enabled
#if !defined(PS3) && !defined(_XBOX_VER) && !defined(LINUX)
#define CRY_UNIT_TESTING
#endif

#ifndef PS3
	#define __FUNC__ __FUNCTION__
	#define InvokeJobOnSPU(Name) false
#endif

#if defined(_DEBUG) && !defined(PS3) && !defined(LINUX)
	#include <crtdbg.h>
#endif






















	#define CHECK_SIMD_ALIGNMENT_P(p)
	#define CHECK_SIMD_ALIGNMENT
	#define ppu_volatile volatile





	#define ppu_virtual virtual


#undef GCC411_OR_LATER
// we have to use it because of VS doesn't support restrict reference variables







	#define __db16cycl__
	#define RESTRICT_REFERENCE
	#define FLATTEN


enum ETunerIDs
{
	eTI_AI = 1,
	eTI_Audio,
	eTI_RendWorld,
	eTI_PumpLoggedEv,
	eTI_ScriptSys,
	eTI_EntitySys,
	eTI_ActionPreUpdate
};
#ifdef SNTUNER
	# include <lib/libsntuner.h>
	struct SNTunerAutoMarker
	{
		static inline void StartMarker(unsigned int id, const char* pName){snStartMarker(id, pName);}
		static inline void StopMarker(unsigned int id){snStopMarker(id);}
		inline SNTunerAutoMarker(const char* pName){snPushMarker(pName);}
		inline ~SNTunerAutoMarker(){snPopMarker();}
	};
#else
	struct SNTunerAutoMarker
	{
		static inline void StartMarker(unsigned int, const char*){}
		static inline void StopMarker(unsigned int){}
		inline SNTunerAutoMarker(const char*){}
	};
#endif

#if !defined(GCC411_OR_LATER) || defined(__SPU__)
	#define __vecreg
#endif

#ifndef CHECK_REFERENCE_COUNTS //define that in your StdAfx.h to override per-project
# define CHECK_REFERENCE_COUNTS 0 //default value
#endif

#if CHECK_REFERENCE_COUNTS
# define CHECK_REFCOUNT_CRASH(x) { if(!(x)) *((int*)0)=0;}
#else
# define CHECK_REFCOUNT_CRASH(x)
#endif

#ifndef GARBAGE_MEMORY_ON_FREE //define that in your StdAfx.h to override per-project
# define GARBAGE_MEMORY_ON_FREE 0 //default value
#endif

#if GARBAGE_MEMORY_ON_FREE
# ifndef GARBAGE_MEMORY_RANDOM			//define that in your StdAfx.h to override per-project
#  define GARBAGE_MEMORY_RANDOM 1	//0 to change it to progressive pattern
# endif
#endif

//////////////////////////////////////////////////////////////////////////
// Available predefined compiler macros for Visual C++.
//		_MSC_VER										// Indicates MS Visual C compiler version
//		_WIN32, _WIN64, _XBOX_VER		// Indicates target OS
//		_M_IX86, _M_PPC							// Indicates target processor
//		_DEBUG											// Building in Debug mode
//		_DLL												// Linking with DLL runtime libs
//		_MT													// Linking with multi-threaded runtime libs
//////////////////////////////////////////////////////////////////////////

//
// Translate some predefined macros.
//

// NDEBUG disables std asserts, etc.
// Define it automatically if not compiling with Debug libs, or with ADEBUG flag.
#if !defined(_DEBUG) && !defined(ADEBUG) && !defined(NDEBUG) && !defined(PS3)
	#define NDEBUG
#endif




	#define MATH_H <math.h>


// Xenon target. (We generally use _XBOX but should really use XENON).









//render thread settings, as this is accessed inside 3dengine and renderer and needs to be compile time defined, we need to do it here
//enable this macro to strip out the overhead for render thread
//	#define STRIP_RENDER_THREAD
#ifdef STRIP_RENDER_THREAD
	#define RT_COMMAND_BUF_COUNT 1
#else
	//can be enhanced to triple buffering, FlushFrame needs to be adjusted and RenderObj would become 132 bytes
	#define RT_COMMAND_BUF_COUNT 2
#endif


// We use WIN macros without _.
#if defined(_WIN32) && !defined(XENON) && !defined(LINUX32) && !defined(LINUX64) && !defined(WIN32)
	#define WIN32
#endif
#if defined(_WIN64) && !defined(WIN64)
	#define WIN64
#endif








	#define __passinreg
	#define __passinreg_vec


// All windows targets built as DLLs.
#if defined(WIN32)
	#ifndef _USRDLL
		#define _USRDLL
	#endif
#else
	//#define _LIB
#endif

#if defined(LINUX) || defined(PS3)
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#define PLATFORM_I64(x) x##ll
#else
#define PRIX64 "I64X"
#define PRIx64 "I64x"
#define PRId64 "I64d"
#define PRIu64 "I64u"
#define PLATFORM_I64(x) x##i64
#endif

#include "ProjectDefines.h"							// to get some defines available in every CryEngine project 

//////////////////////////////////////////////////////////////////////////
// Include standart CRT headers used almost everywhere.
//////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
//////////////////////////////////////////////////////////////////////////

// Function attribute for printf/scanf-style parameters.
// This enables extended argument checking by GCC.
//
// Usage:
// Put this after the function or method declaration (not the definition!),
// between the final closing parenthesis and the semicolon.
// The first parameter indicates the 1-based index of the format string
// parameter, the second parameter indicates the 1-based index of the first
// variable parameter.  Example:
//   void foobar(int a, const char *fmt, ...) PRINTF_PARAMS(2, 3);
//
// For va_list based printf style functions, specfy 0 as the second parameter.
// Example:
//   void foobarv(int a, const char *fmt, va_list ap) PRINTF_PARAMS(2, 0);
//
// Note that 'this' is counted as a method argument. For non-static methods,
// add 1 to the indices.
#if defined(__GNUC__) && !defined(__SPU__) && !defined(_RELEASE)
  #define PRINTF_PARAMS(...) __attribute__ ((format (printf, __VA_ARGS__)))
  #define SCANF_PARAMS(...) __attribute__ ((format (scanf, __VA_ARGS__)))
#else
  #define PRINTF_PARAMS(...)
	#define SCANF_PARAMS(...)
#endif

// Storage class modifier for thread local storage.
#if defined(__GNUC__)
	#define THREADLOCAL __thread
#else
	#define THREADLOCAL __declspec(thread)
#endif


//////////////////////////////////////////////////////////////////////////
// define Read Write Barrier macro needed for lockless programming
//////////////////////////////////////////////////////////////////////////
#if (defined(PS3) && !defined(__SPU__)) || defined(XENON)
	#define READ_WRITE_BARRIER {__lwsync();}
	//#pragma intrinsic(_ReadWriteBarrier)
	//#define READ_WRITE_BARRIER \
		//_ReadWriteBarrier() \
		//AcquireLockBarrier()
#else
	#define READ_WRITE_BARRIER
#endif
//////////////////////////////////////////////////////////////////////////

//default stack size for threads, currently only used on PS3
#ifdef LINUX
#define SIMPLE_THREAD_STACK_SIZE_KB (64)
#else
#define SIMPLE_THREAD_STACK_SIZE_KB (32)
#endif


// DLL import / export






#if defined(__GNUC__) || defined(PS3)











		#define DLL_EXPORT __attribute__ ((visibility("default")))
		#define DLL_IMPORT __attribute__ ((visibility("default")))




#else
	#define DLL_EXPORT __declspec(dllexport)
	#define DLL_IMPORT __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
// Define BIT macro for use in enums and bit masks.
#define BIT(x) (1<<(x))
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Help message, all help text in code must be wrapped in this define.
// Only include these in non RELEASE builds
#if !defined(_RELEASE)
	#define _HELP(x) x
#else
	#define _HELP(x) ""
#endif
//////////////////////////////////////////////////////////////////////////

//will be defined for SPUs and PS3 therefore only
#ifndef SPU_DEBUG_BREAK
	#define SPU_DEBUG_BREAK
#endif
//////////////////////////////////////////////////////////////////////////
// Globally Used Defines.
//////////////////////////////////////////////////////////////////////////
// CPU Types: _CPU_X86,_CPU_AMD64,_CPU_G5
// Platform: WIN23,WIN64,LINUX32,LINUX64,_XBOX
// CPU supported functionality: _CPU_SSE
//////////////////////////////////////////////////////////////////////////




  #if defined(_MSC_VER)
    #include "MSVCspecific.h"
    #define PREFAST_SUPPRESS_WARNING(W) __pragma(warning(suppress:W))
  #else
    #define PREFAST_SUPPRESS_WARNING(W) 
  #endif



#if defined(WIN32) && !defined(WIN64)
#include "Win32specific.h"
#endif

#if defined(WIN64)
#include "Win64specific.h"
#endif

#if defined(LINUX64)
#include "Linux64Specific.h"
#endif

#if defined(LINUX32)
#include "Linux32Specific.h"
#endif









#if !defined(TARGET_DEFAULT_ALIGN)
# error "No default alignment specified for target architecture"
#endif 

#include <stdio.h>

// Includes core CryEngine modules definitions.
#include "CryModuleDefs.h"

/////////////////////////////////////////////////////////////////////////
// CryModule memory manager routines must always be included.
// They are used by any module which doesn't define NOT_USE_CRY_MEMORY_MANAGER
// No Any STL includes must be before this line.
//////////////////////////////////////////////////////////////////////////
#if 1  //#ifndef NOT_USE_CRY_MEMORY_MANAGER
#define USE_NEWPOOL
#include "CryMemoryManager.h"
#else
inline int IsHeapValid()
{
#if defined(_DEBUG) && !defined(RELEASE_RUNTIME) && !defined(XENON) && !defined(PS3)
	return _CrtCheckMemory();
#else
	return true;
#endif
}
#endif // NOT_USE_CRY_MEMORY_MANAGER

// Memory manager breaks strdup
// Use something higher level, like CryString
// PS3 headers require this, does not compile otherwise
#if !defined(PS3)
	#undef strdup
	#define strdup dont_use_strdup
	#undef snPause
	#define snPause()
#endif

#if !defined __CRYCG__
#define __CRYCG_NOINLINE__
#if defined __cplusplus
#if !defined SPU_MAIN_PTR
#define SPU_MAIN_PTR(PTR) (PTR)
#endif
#if !defined SPU_MAIN_REF
#define SPU_MAIN_REF(REF) (REF)
#endif
#if !defined SPU_LOCAL_PTR
#define SPU_LOCAL_PTR(PTR) (PTR)
#endif
#if !defined SPU_LOCAL_REF
#define SPU_LOCAL_REF(REF) (REF)
#endif
#if !defined SPU_LINK_PTR
#define SPU_LINK_PTR(PTR, LINK) (PTR)
#endif
#if !defined SPU_LINK_REF
#define SPU_LINK_REF(REF, LINK) (PTR)
#endif
#endif /* __cplusplus */
#if !defined SPU_DOMAIN_MAIN
#define SPU_DOMAIN_MAIN
#endif
#if !defined SPU_DOMAIN_LOCAL
#define SPU_DOMAIN_LOCAL
#endif
#if !defined SPU_DOMAIN_LINK
#define SPU_DOMAIN_LINK(ID)
#endif
#if !defined SPU_VERBATIM_BLOCK
#define SPU_VERBATIM_BLOCK(X) ((void)0)
#endif 
#if !defined SPU_FRAME_PROFILER
#define SPU_FRAME_PROFILER(X){}
#endif
#endif /* __CRYCG__ */

//defines necessary stuff for SPU Software Cache
//needs to be included for all platforms (mostly empty decls. there)

//////////////////////////////////////////////////////////////////////////
#ifndef DEPRICATED
#define DEPRICATED
#endif

//////////////////////////////////////////////////////////////////////////
// compile time error stuff
//////////////////////////////////////////////////////////////////////////
template<bool> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) \
	{ CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; } 

// Assert dialog box macros
#include "CryAssert.h"

// Replace standard assert calls by our custom one
// Works only ifdef USE_CRY_ASSERT && _DEBUG && WIN32
#ifndef assert
#define assert CRY_ASSERT
#endif

#define COMPILE_TIME_ASSERT(pred) PREFAST_SUPPRESS_WARNING(6326) switch(0) { case 0: case (pred): ; }

//////////////////////////////////////////////////////////////////////////
// Platform dependent functions that emulate Win32 API.
// Mostly used only for debugging!
//////////////////////////////////////////////////////////////////////////
void   CryDebugBreak();
void   CrySleep( unsigned int dwMilliseconds );
int    CryMessageBox( const char *lpText,const char *lpCaption,unsigned int uType);
int    CryCreateDirectory( const char *lpPathName,void *lpSecurityAttributes );
int    CryGetCurrentDirectory( unsigned int nBufferLength,char *lpBuffer );
short  CryGetAsyncKeyState( int vKey );
unsigned int CryGetFileAttributes( const char *lpFileName );






	bool   CrySetFileAttributes( const char *lpFileName,uint32 dwFileAttributes );
	unsigned int CryGetCurrentThreadId();


#if defined(LINUX)
#define CrySwprintf swprintf
#else
#define CrySwprintf _snwprintf
#endif

inline void CryHeapCheck()
{
#if !defined(LINUX) && !defined (PS3)
  int Result = _heapchk();
  assert(Result!=_HEAPBADBEGIN);
  assert(Result!=_HEAPBADNODE);
  assert(Result!=_HEAPBADPTR);
  assert(Result!=_HEAPEMPTY);
  assert(Result==_HEAPOK);
#endif
}

// Returns not null 0 if the passed pointer comes from the heap.
// Note: currently only available on ps3
inline unsigned IsPointerFromHeap(void* __ptr)
{



  return 1; 

}

// Returns not null if the passed pointer resides on the stack of the
// current object 
// Note: currently only available on ps3
inline unsigned IsPointerFromStack(void* __ptr)
{



  return 0; 

}

// Returns not null if the passed pointer resides in the code, data or
// bss segment of the executable.
// Note: currently only available on ps3
inline unsigned IsPointerFromSegment(void* __ptr)
{



  return 0; 

}

// Useful function to clean the structure.
template <class T>
inline void ZeroStruct( T &t ) { memset( &t,0,sizeof(t) ); }

// Useful functions to init new memory.
template<class T>
inline void Construct(T& t)		
	{ new(&t) T(); }

template<class T, class U>
inline void Construct(T& t, U const& u)
	{	new(&t) T(u); }

// Cast one type to another, asserting there is no conversion loss.
// Usage: DestType dest = check_cast<DestType>(src);
template<class D, class S>
inline D check_cast(S const& s)
{
	D d = D(s);
	assert(S(d) == s);
	return d;
}

// Convert one type to another, asserting there is no conversion loss.
// Usage: DestType dest;  check_convert(dest, src);
template<class D, class S>
inline D& check_convert(D& d, S const& s)
{
	d = D(s);
	assert(S(d) == s);
	return d;
}

// Convert one type to another, asserting there is no conversion loss.
// Usage: DestType dest;  check_convert(dest) = src;
template<class D>
struct CheckConvert
{
	CheckConvert(D& d)
		: dest(&d) {}

	template<class S>
	D& operator=(S const& s)
	{
		return check_convert(*dest, s);
	}

protected:
	D*	dest;
};

template<class D>
inline CheckConvert<D> check_convert(D& d)
{
	return d;
}

//---------------------------------------------------------------------------
// Use NoCopy as a base class to easily prevent copy init & assign for any class.
struct NoCopy
{
	NoCopy() {}
private:
	NoCopy(const NoCopy&);
	NoCopy& operator =(const NoCopy&);
};

//---------------------------------------------------------------------------
// Quick const-manipulation macros

// Declare a const and variable version of a function simultaneously.
#define CONST_VAR_FUNCTION(head, body) \
	inline head body \
	inline const head const body

template<class T> inline
T& non_const(const T& t)
	{ return const_cast<T&>(t); }

#define using_type(super, type) \
	typedef typename super::type type;

//---------------------------------------------------------------------------
// Align function works on integer or pointer values.
// Only support power-of-two alignment.
template<typename T> inline
T Align(T nData, size_t nAlign)
{
	assert((nAlign & (nAlign-1)) == 0);
	size_t size = ((size_t)nData + (nAlign-1)) & ~(nAlign-1);
	return T(size);
}

template<typename T> inline
bool IsAligned(T nData, size_t nAlign)
{
	assert((nAlign & (nAlign-1)) == 0);
	return (size_t(nData) & (nAlign-1)) == 0;
}

#if !defined(_SPU_JOB)
#if !defined(NOT_USE_CRY_STRING) 
  #include "CryString.h"
  #if !defined(RESOURCE_COMPILER)
		typedef CryStringT<char> string;	
		typedef CryStringT<wchar_t> wstring;
	#else
		typedef CryStringLocalT<char> string;
		typedef CryStringLocalT<wchar_t> wstring;
	#endif
#else // NOT_USE_CRY_STRING
	#ifndef __SPU__
		#include <string>				// STL string
		typedef std::string string;
		typedef std::wstring wstring;
	#endif
#endif // NOT_USE_CRY_STRING

#include <functional>

// The 'string_less' class below provides less functor implementation for
// 'string' supporting direct comparison against plain C strings and stack
// strings.  This is most effective in combination with STLPORT, where various
// 'find' and related methods are templated on the parameter type.  For
// STLPORT, 'string_less' will be used as a specialization for
// 'std::less<string>'.

struct string_less : public std::binary_function<string, string, bool>
{
  bool operator ()(const string &s1, const char *s2) const
  {
    return s1.compare(s2) < 0;
  }
  bool operator ()(const char *s1, const string &s2) const
  {
    return s2.compare(s1) > 0;
  }
  bool operator ()(const string &s1, const string &s2) const
  {
    return s1 < s2;
  }

#if !defined(NOT_USE_CRY_STRING)
  template <size_t S>
  bool operator()(const string &s1, const CryStackStringT<char, S> &s2) const
  {
    return s1.compare(s2.c_str()) < 0;
  }
  template <size_t S>
  bool operator()(const CryStackStringT<char, S> &s1, const string &s2) const
  {
    return s1.compare(s2.c_str()) < 0;
  }
#endif // !defined(NOT_USE_CRY_STRING)
};

#if defined(USING_STLPORT)
namespace std
{
  template <> struct less< ::string > : public string_less { };
}
#endif // defined(USING_STLPORT)

// Include support for meta-type data.
#include "TypeInfo_decl.h"

// Include array.
#include <CryArray.h>

// Wrapper code for non-windows builds.
#if defined(LINUX)
	#include "Linux_Win32Wrapper.h"
#endif




#if !defined(NOT_USE_CRY_STRING) && (!defined(__SPU__) || defined(__CRYCG__))
	// Fixed-Sized (stack based string)
	// put after the platform wrappers because of missing wcsicmp/wcsnicmp functions
	#include "CryFixedString.h"
#endif


// need this in a common header file and any other file would be too misleading
enum ETriState
{
	eTS_false,
	eTS_true,
	eTS_maybe
};

#endif // !defined (_SPU_JOB)

#define SAFE_DELETE_VOID_ARRAY(p) { if(p) { delete[] (unsigned char*)(p);   (p)=NULL; } }

#if defined __CRYCG__
	#ifndef SPU_ENTRY
		#define SPU_ENTRY(job_name) __attribute__ ((crycg_attr (entry, "job = " #job_name)))	
	#endif
	#ifndef SPU_INDIRECT
		#define SPU_INDIRECT(...) __attribute__ ((crycg_attr (entry, "indirect = " #__VA_ARGS__)))
	#endif
	#ifndef SPU_INDIRECT_TAG
    #define SPU_INDIRECT_TAG(tag) __attribute__ ((crycg_attr (indirect_tag,#tag)))
	#endif
	#ifndef SPU_LOCAL
    #define SPU_LOCAL __attribute__ ((crycg_attr (spu_local,"__ALL__")))
	#endif
	#ifndef SPU_LOCAL_FOR_JOBS
    #define SPU_LOCAL_FOR_JOBS(...) __attribute__ ((crycg_attr (spu_local,__VA_ARGS__)))
	#endif
	#ifndef SPU_EXTERN
		#define SPU_EXTERN __attribute__ ((crycg_attr ("spu_extern"))) extern
	#endif
	#ifndef SPU_DRIVER
		#define SPU_DRIVER(...) __attribute__ ((crycg_attr (driver, #__VA_ARGS__)))
  #endif
	#ifndef SPU_DEBUG
		#define SPU_DEBUG __attribute__ ((crycg_attr ("noinline"))) __attribute__ ((crycg_attr ("spu_debug")))
	#endif
	#ifndef SPU_NO_INLINE
		#define SPU_NO_INLINE __attribute__ ((crycg_attr ("noinline")))
	#endif
#else
	#ifndef SPU_ENTRY
		#define SPU_ENTRY(job_name)
	#endif
	#ifndef SPU_INDIRECT
		#define SPU_INDIRECT(...)
	#endif
	#ifndef SPU_INDIRECT_TAG
		#define SPU_INDIRECT_TAG(tag) 
	#endif
	#ifndef SPU_LOCAL 
		#define SPU_LOCAL 
	#endif
	#ifndef SPU_LOCAL_FOR_JOBS
    #define SPU_LOCAL_FOR_JOBS(...)
	#endif
	#ifndef SPU_EXTERN
		#define SPU_EXTERN extern
	#endif
	#ifndef SPU_DRIVER
		#define SPU_DRIVER(...)
	#endif
	#ifndef SPU_DEBUG
		#define SPU_DEBUG
	#endif
	#ifndef SPU_NO_INLINE
		#define SPU_NO_INLINE
	#endif
	#ifndef SPU_DEFAULT_TO_LOCAL
		#define SPU_DEFAULT_TO_LOCAL(...)
	#endif
#endif












	#ifdef __GNUC__
		#define NO_INLINE __attribute__ ((noinline))
	#else
		#define NO_INLINE _declspec(noinline)
	#endif


//only for PS3 this will take effect, win32 does not support alignment
#if !defined(_ALIGN)



		#define _ALIGN(num) 

#endif

//Align for Win and Xbox360
#if !defined(_MS_ALIGN)
		#define _MS_ALIGN(num) 
#endif

#if !defined(PS3)
	//dummy definitions to avoid ifdef's
	ILINE void SPUAddCacheWriteRangeAsync(const unsigned int, const unsigned int){}
	#define __cache_range_write_async(a,b)
	#define __spu_flush_cache_line(a)
	#define __flush_cache_range(a,b)
	#define __flush_cache()
	#define DECLARE_SPU_JOB(func_name, typedef_name)
	#define DECLARE_SPU_CLASS_JOB(func_name, typedef_name, class_name)
	
	#undef IF
	#undef WHILE
  #undef IF_UNLIKELY	
  #undef IF_LIKELY
  #define IF(a, b) if((a))
	#define WHILE(a, b) while((a))
	#define IF_UNLIKELY(a) if((a))
	#define IF_LIKELY(a) if((a))

#endif //!defined(PS3)


#if defined(WIN32) || defined(WIN64) || defined(XENON)
#ifndef XENON
	extern "C" {
		__declspec(dllimport) unsigned long __stdcall TlsAlloc();
		__declspec(dllimport) void* __stdcall TlsGetValue(unsigned long dwTlsIndex);
		__declspec(dllimport) int __stdcall TlsSetValue(unsigned long dwTlsIndex, void* lpTlsValue);
	}
#endif

	#define TLS_DECLARE(type,var) extern int var##idx;
	#define TLS_DEFINE(type,var) \
	int var##idx; \
	struct Init##var { \
		Init##var() { var##idx = TlsAlloc(); } \
	}; \
	Init##var g_init##var;
	#define TLS_GET(type,var) (type)TlsGetValue(var##idx)
	#define TLS_SET(var,val) TlsSetValue(var##idx,(void*)(val))
#else
#if defined(LINUX)
	#define TLS_DECLARE(type,var) extern __thread type var;
	#define TLS_DEFINE(type,var) __thread type var = 0;
	#define TLS_GET(type,var) (var)
	#define TLS_SET(var,val) (var=(val))
#else
	#define TLS_DECLARE(type,var) extern THREADLOCAL type var;
	#define TLS_DEFINE(type,var) THREADLOCAL type var;
	#define TLS_GET(type,var) (var)
	#define TLS_SET(var,val) (var=(val))
#endif // defined(LINUX)
#endif

	//provide empty macros for non-ps3 systems which don't need these
#if !defined(PS3)
	#define FILE_IO_WRAPPER_NO_PATH_ADJUSTMENT
	#define FILE_IO_WRAPPER_STREAMING_FILE
	#define FILE_IO_WRAPPER_DIRECT_ACCESS_FILE
#endif

#ifndef PS3_NO_TAIL_PADDING
	// Use this macro at the end of a struct/class definition to prevent
	// GCC from overlaying the tail padding of the class with data members
	// from other classes.  The macro adds an empty array member which is
	// aligned to the alignment of the containing class.  Using this macro
	// does _not_ affect the size of the containing class.
	#define PS3_NO_TAIL_PADDING(CLASS)
#endif

#ifndef PS3_ALIGNMENT_BARRIER
	// Enforce the specified alignment onto the next data member of the
	// containing class.  When used at the end of the class, this macro
	// eliminates tail padding up to the specified alignment.
        //
        // Note: At most one alignment barrier may be used per class.
	#define PS3_ALIGNMENT_BARRIER(ALIGN)
#endif

#ifndef UNIQUE_IFACE
	// UNIQUE_IFACE expands to nothing on _all_ platforms, but is recognized by
	// the de-virtualization tool.  If placed in front of an interface
	// struct/class, all methods are assumed to be unique-virtual.
  #define UNIQUE_IFACE
#endif

#ifndef VIRTUAL
	// VIRTUAL should be used for pure virtual methods with only a single
	// implementation - it should be applied to both the method
	// declaration/definition in the interface _and_ the implementation (as an
	// alternative the 'virtual' keyword may be omitted from the
	// implementation).  On platforms not using automatic de-virtualization
	// VIRTUAL simply expands to 'virtual'.
  #define VIRTUAL virtual
#endif

#ifndef UNIQUE_VIRTUAL_WRAPPER
	// Expands to the name if the wrapper include file for an interface
	// containing unique-virtual methods.  Expands to an empty include file on
	// platforms not supporting de-virtualization.
  #define UNIQUE_VIRTUAL_WRAPPER(NAME) <NoIfaceWrapper.h>
	#include <NoIfaceWrapper.h>
#endif

#ifndef DEVIRTUALIZE_HEADER_FIX
	// Expands to hea1er when using devirtualization, this ensures that only
	// devirtualized header files are included. Expands to an empty include file on
	// platforms not supporting de-virtualization.
	#define DEVIRTUALIZE_HEADER_FIX( HEADER ) <NoIfaceWrapper.h>
	#include <NoIfaceWrapper.h>
#endif

#ifndef DEVIRTUALIZATION_VTABLE_FIX
	// Expands  to a dummy function declaration used to force generation of a vtable in a
	// translation unit. Expands to nothing if the plattform doesn't support devirtualization
	#define DEVIRTUALIZATION_VTABLE_FIX
#endif

#ifndef DEVIRTUALIZATION_VTABLE_FIX_IMPL
	// Expands to a dummy function definition to get the compiler to generate a vtable object in the
	// translation unit. Expands to nothing if the plattform doesn't support devirtualization
	#define DEVIRTUALIZATION_VTABLE_FIX_IMPL( CLASS )
#endif

//memory transfer operations proxys for non SPU compilation (also for PC)
#ifndef __SPU__ 
	#define memtransfer_from_main(pDest, pSrc, cSize, cSyncPointID) memcpy(pDest, pSrc, cSize)
	#define memtransfer_to_main(pDest, pSrc, cSize, cSyncPointID) memcpy(pDest, pSrc, cSize)
	#define memtransfer_from_main_fenced memtransfer_from_main
	#define memtransfer_to_main_fenced memtransfer_to_main
	#define memtransfer_sync(cSyncPointID)
	#define memtransfer_pending(id) false
	#define __spu_flush_cache()
	#define __spu_cache_barrier()
#endif

typedef unsigned char	uchar;
typedef unsigned int uint;
typedef const char* cstr;

#if !defined(LINUX) && !defined(PS3)
	typedef int socklen_t;
#endif

// Include MultiThreading support.
#ifndef __SPU__ 
#  include "CryThread.h"
#endif
#include "MultiThread.h"

//////////////////////////////////////////////////////////////////////////
// Include most commonly used STL headers
// They end up in precompiled header and make compilation faster.
// 
// Note: Don't include them when spu code is compiled!
//////////////////////////////////////////////////////////////////////////
#if !defined(_SPU_JOB)
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <algorithm>
#include <functional>
#include "stl/STLAlignedAlloc.h"
#endif

//////////////////////////////////////////////////////////////////////////

// In RELEASE disable printf and fprintf
#if defined(_RELEASE) && (defined(PS3) || defined(XENON))&& !defined(RELEASE_LOGGING)
	#undef printf
	#define printf(...)	
	#undef fprintf
	#define fprintf(...)
  #undef OutputDebugString
  #define OutputDebugString(...)
#endif

#endif // _PLATFORM_H_
