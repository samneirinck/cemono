/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2009.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: 
  
 -------------------------------------------------------------------------
  History:
  - 11:5:2009   : Created by Andrey Honich

*************************************************************************/
#include DEVIRTUALIZE_HEADER_FIX(IMemory.h)

#ifndef __IMEMORY_H__
#define __IMEMORY_H__

#if _MSC_VER > 1000
#	pragma once
#endif

struct IMemoryBlock : public CMultiThreadRefCount
{
  virtual void * GetData() = 0;
  virtual int GetSize() = 0;
};
TYPEDEF_AUTOPTR(IMemoryBlock);

//////////////////////////////////////////////////////////////////////////
struct ICustomMemoryBlock : public IMemoryBlock
{
	// Copy region from from source memory to the specified output buffer
	virtual void CopyMemoryRegion( void *pOutputBuffer,size_t nOffset,size_t nSize ) = 0;
};

//////////////////////////////////////////////////////////////////////////
struct ICustomMemoryHeap : public CMultiThreadRefCount
{
	virtual ICustomMemoryBlock* AllocateBlock( size_t nAllocateSize,const char *sUsage ) = 0;
	virtual void AllocateHeap( size_t const nSize,char const* const sUsage ) = 0;
	virtual void GetMemoryUsage( ICrySizer *pSizer ) = 0;
	virtual void Defrag() = 0;
};

class IGeneralMemoryHeap
{
public:
	virtual void Release() = 0;

	virtual bool IsInAddressRange(void* ptr) const = 0;

	virtual void *Calloc(size_t nmemb, size_t size,const char *sUsage) = 0;
	virtual void *Malloc(size_t sz,const char *sUsage) = 0;

	// Attempts to free the allocation. Returns the size of the allocation if successful, 0 if the heap doesn't own the address.
	virtual size_t Free(void * ptr) = 0;
	virtual void *Realloc(void * ptr, size_t sz,const char *sUsage) = 0;
	virtual void *ReallocAlign(void * ptr, size_t size, size_t alignment,const char *sUsage) = 0;
	virtual void *Memalign(size_t boundary, size_t size,const char *sUsage) = 0;

	// Get the size of the allocation. Returns 0 if the ptr doesn't belong to the heap.
	virtual size_t UsableSize(void* ptr) const = 0;

protected:
	virtual ~IGeneralMemoryHeap() {}
};

#endif //__IMEMORY_H__