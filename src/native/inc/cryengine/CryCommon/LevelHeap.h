#ifndef LEVELHEAP_H
#define LEVELHEAP_H

#if USE_LEVEL_HEAP

#include "CryMemoryAllocator.h"
#include "CryDLMalloc.h"

class CLevelHeap
{
public:
	static void RegisterCVars();
	static bool CanBeUsed() { return s_sys_LevelHeap; }

public:
	CLevelHeap();
	~CLevelHeap();

	void Initialise();

	ILINE bool IsValid() const { return m_baseAddress != 0; }
	ILINE bool IsInAddressRange(void* ptr) const
	{
		UINT_PTR ptri = reinterpret_cast<UINT_PTR>(ptr);
		return m_baseAddress <= ptri && ptri < m_endAddress;
	}

	void *Calloc(size_t nmemb, size_t size);

	void *Malloc(size_t sz)
	{
		void* ptr = NULL;

		if (sz <= m_buckets.MaxSize)
			ptr = m_buckets.allocate(sz);
		else
			ptr = DLMalloc(sz);

		TrackAlloc(ptr);
		return ptr;
	}

	size_t Free(void * ptr)
	{
		size_t sz = 0;

		if (reinterpret_cast<UINT_PTR>(ptr) >= m_dlEndAddress)
			sz = m_buckets.deallocate(ptr);
		else
			sz = DLFree(ptr);

		TrackFree(sz);
		return sz;
	}

	void *Realloc(void * ptr, size_t sz);
	void *ReallocAlign(void * ptr, size_t size, size_t alignment);
	void *Memalign(size_t boundary, size_t size);
	size_t UsableSize(void* ptr);

	void Cleanup();

	void GetState(size_t& numLiveAllocsOut, size_t& sizeLiveAllocsOut) const;
	void EnableExpandCleanups(bool enable) { m_buckets.EnableExpandCleanups(enable); }
	void ReplayRegisterAddressRanges();

private:
	static void* DLMMap(void* self, size_t sz);
	static int DLMUnMap(void* self, void* mem, size_t sz);

private:
	static void* PlatformReserve(size_t sz);
	static void* PlatformMapPage(void* base);
	static void PlatformUnMapPage(void* base);

private:
	CLevelHeap(const CLevelHeap&);
	CLevelHeap& operator = (const CLevelHeap&);

private:
	void* DLMalloc(size_t sz);
	size_t DLFree(void* ptr);

#if TRACK_LEVEL_HEAP_USAGE
	void TrackAlloc(void* ptr);
	void TrackFree(size_t sz);
#else
	void TrackAlloc(void*) {}
	void TrackFree(size_t) {}
#endif

private:
	enum
	{
		DLAddressSpace = 256 * 1024 * 1024,
		BucketAddressSpace = 64 * 1024 * 1024,
	};

	typedef BucketAllocator<BucketAllocatorDetail::DefaultTraits<BucketAddressSpace, BucketAllocatorDetail::SyncPolicyLocked, false> > LevelBuckets;

private:
	static int s_sys_LevelHeap;

private:
	CryCriticalSectionNonRecursive m_lock;

	dlmspace m_mspace;

	LevelBuckets m_buckets;

	volatile long m_active;

	int m_dlmallocContents;

#if TRACK_LEVEL_HEAP_USAGE
	volatile int m_numLiveAllocs;
	volatile int m_sizeLiveAllocs;
#endif

public:
	UINT_PTR m_baseAddress;
	UINT_PTR m_dlEndAddress;
	UINT_PTR m_endAddress;

	uint32 m_pageBitmap[DLAddressSpace / ((64*1024) * 32)];
};

#endif

#endif
