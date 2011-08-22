//---------------------------------------------------------------------------
// Copyright 2010 Crytek GmbH
// Created by: Michael Kopietz
// Modified: -
//	
//---------------------------------------------------------------------------

#ifndef __CCRYPOOLALLOC__
#define __CCRYPOOLALLOC__

#if defined(POOLALLOCTESTSUIT)
//cheat just for unit testing on windows
typedef int8_t		int8;;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;
typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
#define ILINE			inline
#endif



#if defined(LINUX) || defined(PS3)
#define CPA_ALLOC							memalign
#define CPA_FREE							free
#else
#define CPA_ALLOC							_aligned_malloc
#define CPA_FREE							_aligned_free
#endif
#define CPA_ASSERT						assert
#define CPA_ASSERT_STATIC(X)	{uint8 assertdata[(X)?0:1];}
#define CPA_BREAK							__debugbreak()

#include "List.h"
#include "Memory.h"
#include "Container.h"
#include "Allocator.h"
#include "Defrag.h"
#include "STLWrapper.h"
#include "Inspector.h"
#include "Fallback.h"
#if !defined(POOLALLOCTESTSUIT)
#include "ThreadSafe.h"
#endif

#undef CPA_ASSERT
#undef CPA_ASSERT_STATIC
#undef CPA_BREAK

#endif

