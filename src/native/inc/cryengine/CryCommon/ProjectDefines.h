////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ProjectDefines.h
//  Version:     v1.00
//  Created:     3/30/2004 by MartinM.
//  Compilers:   Visual Studio.NET
//  Description: to get some defines available in every CryEngine project 
// -------------------------------------------------------------------------
//  History:
//    July 20th 2004 - Mathieu Pinard
//    Updated the structure to handle more easily different configurations
//
////////////////////////////////////////////////////////////////////////////

#ifndef PROJECTDEFINES_H
#define PROJECTDEFINES_H

#ifdef _RELEASE
	#define RELEASE
#endif

#if defined(LINUX)
#	define EXCLUDE_SCALEFORM_SDK
#	define EXCLUDE_CRI_SDK
















#elif defined(WIN32) || defined(WIN64)
//#	define EXCLUDE_SCALEFORM_SDK
//#	define EXCLUDE_CRI_SDK
#else
#	define EXCLUDE_SCALEFORM_SDK
#	define EXCLUDE_CRI_SDK
#endif

//#define GAME_IS_CRYSIS2


#define DYNTEX_USE_SHAREDRT
#define DYNTEX_ALLOW_SFDVIDEO

// see http://wiki/bin/view/CryEngine/TerrainTexCompression for more details on this
// 0=off, 1=on
#define TERRAIN_USE_CIE_COLORSPACE 0

// for consoles every bit of memory is important so files for documentation purpose are excluded
// they are part of regular compiling to verify the interface




//#define RELEASE_LOGGING
#if defined(_RELEASE) && !defined(RELEASE_LOGGING) && !defined(DEDICATED_SERVER)
#define EXCLUDE_NORMAL_LOG
#endif

#if defined(WIN32) || defined(WIN64)
#if defined(DEDICATED_SERVER)
// enable/disable map load slicing functionality from the build
#define MAP_LOADING_SLICING
#define SLICE_AND_SLEEP() do { GetISystemScheduler()->SliceAndSleep(__FUNC__, __LINE__); } while (0)
#define SLICE_SCOPE_DEFINE() CSliceLoadingMonitor sliceScope
#endif
#endif

#if !defined(MAP_LOADING_SLICING)
#define SLICE_AND_SLEEP()
#define SLICE_SCOPE_DEFINE()
#endif





#define EXCLUDE_UNIT_TESTS	0	
#ifdef _RELEASE
#undef EXCLUDE_UNIT_TESTS
#define EXCLUDE_UNIT_TESTS	1
#endif

#if ((defined(XENON) && !defined(_LIB)) || defined(WIN32)) && !defined(RESOURCE_COMPILER)
  #define CAPTURE_REPLAY_LOG 1
#endif














#if CAPTURE_REPLAY_LOG && defined(PS3_CRYSIZER_HEAP_TRAVERSAL)
	#define MALLOC_DUMMY_WRAP
	#undef CAPTURE_REPLAY_LOG
#endif

#if defined(RESOURCE_COMPILER) || defined(_RELEASE)
  #undef CAPTURE_REPLAY_LOG
#endif

#ifndef CAPTURE_REPLAY_LOG
  #define CAPTURE_REPLAY_LOG 0
#endif

#if (defined(PS3) || defined(XENON)) && !defined(PS3_CRYSIZER_HEAP_TRAVERSAL)
	#define USE_GLOBAL_BUCKET_ALLOCATOR
#endif

#if (defined(PS3) || defined(XENON)) && !defined(PS3_CRYSIZER_HEAP_TRAVERSAL)
#define USE_LEVEL_HEAP 1
#endif

#define OLD_VOICE_SYSTEM_DEPRECATED
//#define INCLUDE_PS3PAD
//#define EXCLUDE_SCALEFORM_SDK
#define EXCLUDE_CRI_SDK










#if defined(NOT_USE_CRY_MEMORY_MANAGER)
  // No replay functionality without the memory manager.
  #if CAPTURE_REPLAY_LOG
    #define MALLOC_DUMMY_WRAP 1
  #endif
  #undef CAPTURE_REPLAY_LOG
#endif

#ifdef CRYTEK_VISUALIZATION
#define CRYTEK_SDK_EVALUATION
#endif

















#if defined(RESOURCE_COMPILER) || defined(_RELEASE)
  #undef CAPTURE_REPLAY_LOG
#endif

#ifndef CAPTURE_REPLAY_LOG
  #define CAPTURE_REPLAY_LOG 0
#endif








#	define TAGES_EXPORT



// test -------------------------------------
//#define EXCLUDE_CVARHELP

#define _DATAPROBE
//#define ENABLE_COPY_PROTECTION

// GPU pass timers are enabled here for Release builds as well
// Disable them before shipping, otherwise game is linked against instrumented libraries on 360
//#ifndef PURE_XENON_RELEASE
//#define ENABLE_SIMPLE_GPU_TIMERS
//#endif








#if !defined(PHYSICS_STACK_SIZE)
# define PHYSICS_STACK_SIZE (128U<<10)
#endif 
#if !defined(EMBED_PHYSICS_AS_FIBER)
# define EMBED_PHYSICS_AS_FIBER 0
#endif 
#if EMBED_PHYSICS_AS_FIBER && !defined(EXCLUDE_PHYSICS_THREAD)
# error cannot embed physics as fiber if the physics timestep is threaded!
#endif 

#if !defined(USE_LEVEL_HEAP)
#define USE_LEVEL_HEAP 0
#endif

#if USE_LEVEL_HEAP && !defined(_RELEASE)
#define TRACK_LEVEL_HEAP_USAGE 1
#endif

#ifndef TRACK_LEVEL_HEAP_USAGE
#define TRACK_LEVEL_HEAP_USAGE 0
#endif

#if !defined(_RELEASE) && !defined(RESOURCE_COMPILER)
#ifndef ENABLE_PROFILING_CODE
	#define ENABLE_PROFILING_CODE
#endif
//lightweight profilers, disable for submissions, disables displayinfo inside 3dengine as well
#ifndef ENABLE_LW_PROFILERS
	#define ENABLE_LW_PROFILERS
#endif
#endif

#if !defined(_RELEASE)
#define USE_FRAME_PROFILER      // comment this define to remove most profiler related code in the engine
#define CRY_TRACE_HEAP
#endif


#undef ENABLE_STATOSCOPE
#if defined(ENABLE_PROFILING_CODE)
  #define ENABLE_STATOSCOPE 1
#endif

#if defined(ENABLE_PROFILING_CODE)
  #define ENABLE_SIMPLE_GPU_TIMERS
#endif

#if defined(ENABLE_PROFILING_CODE)
  #define USE_PERFHUD
#endif

#if defined(ENABLE_STATOSCOPE) && !defined(_RELEASE)
	#define FMOD_STREAMING_DEBUGGING 1
#endif





#if (defined(PS3) || defined(XENON)) && !defined(ENABLE_LW_PROFILERS)
#ifndef USE_NULLFONT
#define USE_NULLFONT 1
#endif
#define USE_NULLFONT_ALWAYS 1
#endif

#if !defined(XENON)
#define PIXBeginNamedEvent(x,y,...)
#define PIXEndNamedEvent()
#define PIXSetMarker(x,y,...)
#elif !defined(_RELEASE) && !defined(_DEBUG)
#define USE_PIX
#endif

#include "ProjectDefinesInclude.h"

#define FULL_ON_SCHEDULING 1

#endif // PROJECTDEFINES_H
