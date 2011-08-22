#ifndef BUCKETALLOCATORIMPL_H
#define BUCKETALLOCATORIMPL_H

#include "BitFiddling.h"

#ifdef USE_GLOBAL_BUCKET_ALLOCATOR

#define BUCKET_ASSERTS 0
#define PROFILE_BUCKET_CLEANUP 0

//#define BUCKET_ALLOCATOR_MAP_DOWN
#if defined(_WIN32) && !defined(XENON)
#define BUCKET_ALLOCATOR_4K
#endif

#if BUCKET_ASSERTS
#define BucketAssert(expr) if (!(expr)) { __debugbreak(); }
#else
#define BucketAssert(expr)
#endif

namespace
{
	template <typename T>
	void SortLL(T*& root)
	{
		if (!root)
			return;

		int k = 1;
		T* p;

		for (k = 1; ; k <<= 1)
		{
			T* nl = root;
			T** c = &root;

			int mrgr = 0;

			do
			{
				T* a = nl;
				T* b = nl;

				for (int i = k; i && b; -- i, p = b, b = b->next)
					;
				p->next = NULL;

				nl = b;

				for (int i = k; i && nl; -- i, p = nl, nl = nl->next)
					;
				p->next = NULL;

				while (a && b)
				{
					if (a < b)
					{
						*c = a;
						a = a->next;
					}
					else
					{
						*c = b;
						b = b->next;
					}

					c = (T**) &((*c)->next);
				}

				if (a)
				{
					*c = a;

					for (p = a; p->next; p = p->next)
						;
					c = (T**) &p->next;
				}
				else if(b)
				{
					*c = b;

					for (p = b; p->next; p = p->next)
						;
					c = (T**) &p->next;
				}

				++ mrgr;
			}
			while (nl);

			if (mrgr == 1)
				break;
		}
	}
}

#if defined(XENON) && BUCKET_ASSERTS
#pragma optimize("",off)
#endif

template <typename TraitsT>
BucketAllocator<TraitsT>::BucketAllocator(void* baseAddress, bool allowExpandCleanups)
	: m_baseAddress(reinterpret_cast<UINT_PTR>(baseAddress))
	, m_disableExpandCleanups(allowExpandCleanups == false)
{
}


template <typename TraitsT>
BucketAllocator<TraitsT>::~BucketAllocator()
{
	if (m_baseAddress)
	{
		for (
			uint32 mapCount = sizeof(m_pageMap) / sizeof(m_pageMap[0]), mapIdx = 0;
			mapIdx != mapCount;
			++ mapIdx)
		{
			while (m_pageMap[mapIdx])
			{
				uint32 mapVal = m_pageMap[mapIdx];

				// Find index of 1 bit in mapVal - aka a live page
				mapVal = mapVal & ((~mapVal) + 1);
				mapVal = IntegerLog2(mapVal);
				UINT_PTR mapAddress = m_baseAddress + PageLength * (mapIdx * 32 + mapVal);
				m_pageMap[mapIdx] &= ~(1 << mapVal);

				this->UnMap(mapAddress);
			}
		}

		this->UnreserveAddressSpace(m_baseAddress);
	}
}

template <typename TraitsT>
typename BucketAllocator<TraitsT>::FreeBlockHeader* BucketAllocator<TraitsT>::CreatePage(FreeBlockHeader* after)
{
	using namespace BucketAllocatorDetail;

	Page* page = reinterpret_cast<Page*>(this->AllocatePageStorage());
	if (!page)
	{
		return NULL;
	}

	FreeBlockHeader* freeHdr = reinterpret_cast<FreeBlockHeader*>((&page->hdr) + 1);

	memset(&page->hdr, 0, sizeof(page->hdr));

	freeHdr->start = reinterpret_cast<UINT_PTR>(freeHdr);
	freeHdr->end = reinterpret_cast<UINT_PTR>(page + 1);
	freeHdr->prev = after;
	if (after)
	{
		freeHdr->next = after->next;
		after->next = freeHdr;
	}
	else
	{
		freeHdr->next = NULL;
		m_freeBlocks = freeHdr;
	}

#if CAPTURE_REPLAY_LOG
	CryGetIMemReplay()->MarkBucket(-1, 8, &page->hdr, sizeof(PageHeader));
	CryGetIMemReplay()->MarkBucket(-3, 4, freeHdr, freeHdr->end - freeHdr->start);
#endif

	return freeHdr;
}

template <typename TraitsT>
bool BucketAllocator<TraitsT>::DestroyPage(Page* page)
{
	BucketAssert(IsInAddressRange(page));

	if (DeallocatePageStorage(page))
	{
#if CAPTURE_REPLAY_LOG
		CryGetIMemReplay()->UnMarkBucket(-1, &page->hdr);
#endif

		return true;
	}

	return false;
}

template <typename TraitsT>
bool BucketAllocator<TraitsT>::Refill(uint8 bucket)
{
	using namespace BucketAllocatorDetail;

	typename SyncingPolicy::RefillLock lock(*this);

	// Validate that something else hasn't refilled this bucket whilst waiting for the lock

	for (size_t flIdx = bucket * NumGenerations, flIdxEnd = flIdx + NumGenerations; flIdx != flIdxEnd; ++ flIdx)
	{
		if (GetFreeListHead(m_freeLists[flIdx]))
			return true;
	}

	size_t itemSize = TraitsT::GetSizeForBucket(bucket);

	bool useForward;
	UINT_PTR alignmentMask = 0;

	if ((SmallBlockLength % itemSize) == 0)
	{
		useForward = false;
	}






	else if ((itemSize % 16) == 0)
	{
		useForward = true;

		// For allocs whose size is a multiple of 16, ensure they are aligned to 16 byte boundary, in case any aligned
		// XMVEC types are members
		alignmentMask = 15;
	}
	else if ((itemSize % 8) == 0)
	{
		useForward = true;
	}
	else
	{
		useForward = false;
	}

	size_t numItems;
	FreeBlockHeader* fbh = FindFreeBlock(useForward, alignmentMask, itemSize, numItems);
	if (!numItems)
	{
		// Failed to find a matching free block - to avoid allocating more from the system, try and garbage collect some space.
		if (!m_disableExpandCleanups)
		{
			CleanupInternal(false);
			fbh = FindFreeBlock(useForward, alignmentMask, itemSize, numItems);
		}

		if (!numItems)
		{
			// Cleanup failed to yield any small blocks that can be used for this refill, so grab another page from the OS.
			if (!CreatePage(fbh))
			{
				return false;
			}

			fbh = FindFreeBlock(useForward, alignmentMask, itemSize, numItems);
		}
	}

	UINT_PTR segments[4];

	segments[0] = fbh->start;
	segments[3] = fbh->end;

	if (useForward)
	{
		segments[1] = fbh->start;
		segments[2] = ((fbh->start + alignmentMask) & ~alignmentMask) + itemSize * numItems;
	}
	else
	{
		segments[1] = (fbh->end & SmallBlockAlignMask) - numItems * itemSize;
		segments[2] = fbh->end & SmallBlockAlignMask;
	}

	BucketAssert(fbh && IsInAddressRange((void*) fbh->start));

	UINT_PTR baseAddress = (segments[1] + alignmentMask) & ~alignmentMask;
	UINT_PTR endAddress = segments[2];
	UINT_PTR blockBase = baseAddress & PageAlignMask;

	size_t smallBlockIdx = (baseAddress - blockBase) / SmallBlockLength;
	size_t smallBlockEnd = ((endAddress - itemSize - blockBase) + SmallBlockOffsetMask) / SmallBlockLength;
	size_t numSmallBlocks = smallBlockEnd - smallBlockIdx;

	BucketAssert(useForward || !(endAddress & SmallBlockOffsetMask));
	BucketAssert(!useForward || !(baseAddress & 7));
	BucketAssert(numSmallBlocks > 0);
	BucketAssert(baseAddress >= fbh->start);
	BucketAssert(endAddress <= fbh->end);
	BucketAssert(IsInAddressRange((void*)baseAddress));
	BucketAssert(IsInAddressRange((void*)(endAddress - 1)));

#if CAPTURE_REPLAY_LOG
	CryGetIMemReplay()->UnMarkBucket(-3, fbh);
#endif

	if (fbh->next)
		fbh->next->prev = fbh->prev;
	if (fbh->prev)
		fbh->prev->next = fbh->next;
	else
		m_freeBlocks = fbh->next;

	InsertFreeBlock(InsertFreeBlock(fbh->prev, segments[0], segments[1]), segments[2], segments[3]);

	// Can't touch fbh beyond this point.

	Page* page = reinterpret_cast<Page*>(blockBase);

	for (UINT_PTR sbId = smallBlockIdx, sbBase = baseAddress; sbId != smallBlockEnd; ++ sbId)
	{
		UINT_PTR sbBaseRoundedUp = (sbBase & SmallBlockAlignMask) + SmallBlockLength;

		page->hdr.SetBucketId(sbId, bucket, endAddress > sbBaseRoundedUp);
		page->hdr.SetBaseOffset(sbId, sbBase & SmallBlockOffsetMask);

		BucketAssert(page->hdr.GetBaseOffset(sbId) == (sbBase & SmallBlockOffsetMask));

		// naive implementation
		for (; sbBase < sbBaseRoundedUp; sbBase += itemSize)
			;
	}

#if CAPTURE_REPLAY_LOG
	{
		int alignment = (itemSize <= 32) ? 4 : 8;

		UINT_PTR sbBase = baseAddress;
		UINT_PTR sbEnd = baseAddress;

		do 
		{
			for (; (sbEnd & SmallBlockAlignMask) == (sbBase & SmallBlockAlignMask); sbEnd += itemSize)
				;
			sbEnd = min(sbEnd, endAddress);

			CryGetIMemReplay()->MarkBucket(itemSize, alignment, (void*) sbBase, sbEnd - sbBase);
			sbBase = sbEnd;
		}
		while (sbBase != endAddress);
	}
#endif

	typename SyncingPolicy::FreeListHeader& freeList = m_freeLists[bucket * NumGenerations + NumGenerations - 1];














#if defined(PS3) || defined(_WIN32)

	// On PS3 we can call lwarx and stwcx directly, so the entire new free list can be pushed on in one go

	for (size_t item = 0; item != (numItems - 1); ++ item)
	{
		AllocHeader* cur = reinterpret_cast<AllocHeader*>(baseAddress + itemSize * item);
		AllocHeader* next = reinterpret_cast<AllocHeader*>(baseAddress + itemSize * (item + 1));
		cur->next = next;

#ifdef BUCKET_ALLOCATOR_TRAP_DOUBLE_DELETES
		cur->magic = FreeListMagic;
#endif
	}

	AllocHeader* firstItem = reinterpret_cast<AllocHeader*>(baseAddress);
	AllocHeader* lastItem = reinterpret_cast<AllocHeader*>(baseAddress + itemSize * (numItems - 1));
	lastItem->next = NULL;

#ifdef BUCKET_ALLOCATOR_TRAP_DOUBLE_DELETES
	lastItem->magic = FreeListMagic;
#endif

	this->PushListOnto(freeList, firstItem, lastItem);

#endif

	return true;
}

template <typename TraitsT>
typename BucketAllocator<TraitsT>::FreeBlockHeader* BucketAllocator<TraitsT>::FindFreeBlock(bool useForward, UINT_PTR alignmentMask, size_t itemSize, size_t& numItems)
{
	FreeBlockHeader* fbh = m_freeBlocks;
	if (useForward)
	{
		FreeBlockHeader* prev = NULL;
		UINT_PTR minSize = static_cast<UINT_PTR>(itemSize) + alignmentMask + 1U;
		for (; fbh && (fbh->end - fbh->start) < minSize; prev = fbh, fbh = fbh->next)
			;

		if (!fbh)
		{
			numItems = 0;
			return prev;
		}

		UINT_PTR fbhStart = (fbh->start + alignmentMask) & ~alignmentMask;
		numItems = GetRefillItemCountForBucket_Spill(fbhStart, itemSize);

		size_t remainingInBlock = fbh->end - fbhStart;
		if (remainingInBlock < itemSize * numItems)
			numItems = remainingInBlock / itemSize;
	}
	else
	{
		numItems = GetRefillItemCountForBucket_LCM(itemSize);

		FreeBlockHeader* prev = NULL;
		for (; fbh && ((fbh->end & SmallBlockAlignMask) - fbh->start) < (int)itemSize; prev = fbh, fbh = fbh->next)
			;

		if (!fbh)
		{
			numItems = 0;
			return prev;
		}

		size_t remainingInBlock = (fbh->end & SmallBlockAlignMask) - fbh->start;
		if (remainingInBlock < itemSize * numItems)
			numItems = remainingInBlock / itemSize;
	}

	return fbh;
}

template <typename TraitsT>
void BucketAllocator<TraitsT>::CleanupInternal(bool sortFreeLists)
{
	using namespace BucketAllocatorDetail;

	typename SyncingPolicy::CleanupWriteLock cleanLock(*this);





#if PROFILE_BUCKET_CLEANUP
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
#endif

	UINT_PTR bucketHeapBase = this->GetBucketCommittedBase();
	size_t pageCapacity = this->GetBucketStorageCapacity() / PageLength;

	if (pageCapacity == 0)
		return;

	CleanupAllocator alloc;

	SmallBlockCleanupInfo** pageInfos = (SmallBlockCleanupInfo**) alloc.Calloc(pageCapacity, sizeof(SmallBlockCleanupInfo*));
	if (!pageInfos)
		return;

	AllocHeader* freeLists[NumBuckets * NumGenerations];

	for (size_t flIdx = 0; flIdx != NumBuckets * NumGenerations; ++ flIdx)
	{
		do 
		{
			freeLists[flIdx] = GetFreeListHead(m_freeLists[flIdx]);
		}
		while (CryInterlockedCompareExchangePointer((void * volatile *) &this->GetFreeListHead(m_freeLists[flIdx]), NULL, freeLists[flIdx]) != freeLists[flIdx]);
	}





	// For each small block, count the number of items that are free

	uint8 bucketsTouched[NumBuckets] = {0};

	for (size_t bucketIdx = 0; bucketIdx != NumBuckets; ++ bucketIdx)
	{
		if (!m_bucketTouched[bucketIdx])
			continue;

		bucketsTouched[bucketIdx] = 1;

		for (size_t freeList = bucketIdx * NumGenerations, freeListEnd = freeList + NumGenerations; freeList != freeListEnd; ++ freeList)
		{
			for (AllocHeader* hdr = freeLists[freeList]; hdr; hdr = hdr->next)
			{
				UINT_PTR allocPtr = reinterpret_cast<UINT_PTR>(hdr);
				size_t pageId = (allocPtr - bucketHeapBase) / PageLength;
				size_t sbId = (allocPtr & PageOffsetMask) / SmallBlockLength;

				SmallBlockCleanupInfo*& sbInfos = pageInfos[pageId];
				if (!sbInfos)
				{
					sbInfos = (SmallBlockCleanupInfo*) alloc.Calloc(SmallBlocksPerPage, sizeof(SmallBlockCleanupInfo));
					if (!sbInfos)
					{
						FreeCleanupInfo(alloc, pageInfos, pageCapacity);
						return;
					}
				}

				++ sbInfos[sbId].freeItemCount;
			}
		}
	}

	// Add existing free blocks to info vec so they will get coalesced with the newly freed blocks

	for (FreeBlockHeader* fbh = m_freeBlocks; fbh; fbh = fbh->next)
	{
		size_t pageId = (fbh->start - bucketHeapBase) / PageLength;
		UINT_PTR pageBase = bucketHeapBase + pageId * PageLength;
		UINT_PTR startSbId = (fbh->start - pageBase) / SmallBlockLength;
		UINT_PTR endSbId = (fbh->end - pageBase) / SmallBlockLength;
		SmallBlockCleanupInfo*& cleanupInfos = pageInfos[pageId];

		BucketAssert(startSbId != endSbId);

		if (!cleanupInfos)
		{
			cleanupInfos = (SmallBlockCleanupInfo*) alloc.Calloc(SmallBlocksPerPage, sizeof(SmallBlockCleanupInfo));
			if (!cleanupInfos)
			{
				FreeCleanupInfo(alloc, pageInfos, pageCapacity);
				return;
			}
		}

		for (UINT_PTR sbId = startSbId; sbId < endSbId; ++ sbId)
		{
			SmallBlockCleanupInfo& sbInfo = cleanupInfos[sbId];
			sbInfo.cleaned = 1;
		}

#if CAPTURE_REPLAY_LOG
		CryGetIMemReplay()->UnMarkBucket(-3, fbh);
#endif
	}

	m_freeBlocks = NULL;

	// At this point, info should be fully populated with the details of available small blocks from the free lists.

	// Do a pass over the small block cleanup infos to mark up blocks that are going to be unbound

	for (size_t pageId = 0; pageId != pageCapacity; ++ pageId)
	{
		SmallBlockCleanupInfo* sbInfos = pageInfos[pageId];

		if (!sbInfos)
			continue;

		Page* page = reinterpret_cast<Page*>(bucketHeapBase + PageLength * pageId);
		PageHeader* pageHdr = &page->hdr;

		for (size_t sbId = 0; sbId < SmallBlocksPerPage; ++ sbId)
		{
			SmallBlockCleanupInfo& sbInfo = sbInfos[sbId];

			if (sbInfo.cleaned)
				continue;

			size_t itemSize = TraitsT::GetSizeForBucket(page->hdr.GetBucketId(sbId));
			sbInfo.SetItemSize(itemSize);

			if (sbInfo.freeItemCount == 0)
				continue;

			uint32 maximumFreeCount = pageHdr->GetItemCountForBlock(sbId);

			BucketAssert(maximumFreeCount > 0);
			BucketAssert(sbInfo.freeItemCount <= maximumFreeCount);

			if (maximumFreeCount == sbInfo.freeItemCount)
			{
				sbInfo.cleaned = 1;

#if CAPTURE_REPLAY_LOG
				UINT_PTR sbStart = reinterpret_cast<UINT_PTR>(&page->smallBlocks[sbId]) + page->hdr.GetBaseOffset(sbId);
				CryGetIMemReplay()->UnMarkBucket(itemSize, (void*) sbStart);
#endif
			}
		}
	}

	// Walk over the free lists again, checking each item against the page info
	// vector to determine whether it's still a valid free item and removing those that aren't.

	int unboundSize = 0;

	for (size_t bucketId = 0; bucketId != NumBuckets; ++ bucketId)
	{
		if (!bucketsTouched[bucketId])
			continue;

		size_t freeListBase = bucketId * NumGenerations;

		for (size_t genId = 0; genId != NumGenerations; ++ genId)
		{
			size_t freeList = freeListBase + genId;

			for (AllocHeader** hdr = &freeLists[freeList]; *hdr;)
			{
				UINT_PTR hdri = reinterpret_cast<UINT_PTR>(*hdr);
				size_t pageId = (hdri - bucketHeapBase) / PageLength;
				size_t sbId = (hdri & PageOffsetMask) / SmallBlockLength;

				SmallBlockCleanupInfo* cleanupInfos = pageInfos[pageId];
				SmallBlockCleanupInfo& pageInfo = cleanupInfos[sbId];

				if (pageInfo.cleaned)
				{
					// Remove this item from the free list, as the small block that it lives in has been unbound.
					*hdr = (*hdr)->next;
					unboundSize += pageInfo.GetItemSize();
				}
				else
				{
					Page* page = reinterpret_cast<Page*>(hdri & PageAlignMask);
					assert (page);

					uint8 sbStability = std::min(page->hdr.GetStability(sbId) + 1, 255);
					size_t sbGen = TraitsT::GetGenerationForStability(sbStability);

					if (sbGen != genId)
					{
						AllocHeader* item = *hdr;

						// Remove this item from this free list, as it has changed generation and should be moved to another free list.
						*hdr = item->next;

						// Push it onto the right free list
						item->next = freeLists[freeListBase + sbGen];
						freeLists[freeListBase + sbGen] = item;
					}
					else
					{
						hdr = (AllocHeader**) &(*hdr)->next;
					}
				}
			}

		}
	}

	// Finally, do another pass over the small block cleanups again and build the new free block list, now
	// that the free lists won't alias it.

	FreeBlockHeader* freeBlocks = NULL;

	for (size_t pageId = 0; pageId != pageCapacity; ++ pageId)
	{
		SmallBlockCleanupInfo* sbInfos = pageInfos[pageId];

		if (!sbInfos)
			continue;

		Page* page = reinterpret_cast<Page*>(bucketHeapBase + sizeof(Page) * pageId);

		for (size_t sbId = 0; sbId != SmallBlocksPerPage;)
		{
			if (!sbInfos[sbId].cleaned)
			{
				page->hdr.IncrementBlockStability(sbId);
				++ sbId;
				continue;
			}

			size_t startSbId = sbId;

			for (; sbId != SmallBlocksPerPage && sbInfos[sbId].cleaned; ++ sbId)
			{
				page->hdr.ResetBlockStability(sbId);
			}

			UINT_PTR fbhStart = 0;

			if (startSbId > 0)
			{
				size_t baseOffset = 0;

				if (page->hdr.DoesSmallBlockSpill(startSbId - 1))
				{
					size_t sbOffset = page->hdr.GetBaseOffset(startSbId - 1);
					size_t itemSize = TraitsT::GetSizeForBucket(page->hdr.GetBucketId(startSbId - 1));
					size_t itemCount = (SmallBlockLength - sbOffset) / itemSize + 1;

					baseOffset = sbOffset + itemSize * itemCount - SmallBlockLength;
				}

				// Need to make sure that the starting edge is 8 byte aligned for PS3
				baseOffset = (baseOffset + 7) & ~7;

				fbhStart = reinterpret_cast<UINT_PTR>(&page->smallBlocks[startSbId]) + baseOffset;
			}
			else
			{
				fbhStart = reinterpret_cast<UINT_PTR>(&page->hdr + 1);
			}

			UINT_PTR fbhEnd = (sbId != SmallBlocksPerPage)
				? reinterpret_cast<UINT_PTR>(&page->smallBlocks[sbId]) + page->hdr.GetBaseOffset(sbId)
				: reinterpret_cast<UINT_PTR>(page + 1);

			if (((fbhStart & PageOffsetMask) == sizeof(PageHeader)) && ((fbhEnd & PageOffsetMask) == 0))
			{
				// Free block extends from the end of the page header to the end of the page - i.e. the page is empty
				if (DestroyPage(reinterpret_cast<Page*>(fbhStart & PageAlignMask)))
				{
					// Don't add it to the free block list. For obvious reasons.
					continue;
				}
			}

			// Add the block to the free block list.
			FreeBlockHeader* fbh = reinterpret_cast<FreeBlockHeader*>(fbhStart);
			fbh->start = fbhStart;
			fbh->end = fbhEnd;

			BucketAssert(!(fbhStart & 7));
			BucketAssert(fbhEnd > fbhStart);

			fbh->next = NULL;
			fbh->prev = freeBlocks;
			if (freeBlocks)
				freeBlocks->next = fbh;
			else
				m_freeBlocks = fbh;
			freeBlocks = fbh;
		}
	}

#if CAPTURE_REPLAY_LOG
	for (FreeBlockHeader* fbh = m_freeBlocks; fbh; fbh = fbh->next)
		CryGetIMemReplay()->MarkBucket(-3, 4, fbh, fbh->end - fbh->start);
#endif

#if BUCKET_ASSERTS
	for (FreeBlockHeader* fbh = m_freeBlocks; fbh; fbh = fbh->next)
	{
		BucketAssert(IsInAddressRange(fbh));

		BucketAssert(fbh->start == reinterpret_cast<UINT_PTR>(fbh));
		BucketAssert((fbh->start & SmallBlockAlignMask) != (fbh->end & SmallBlockAlignMask));
		BucketAssert((fbh->start & PageAlignMask) == ((fbh->end - 1) & PageAlignMask));

		{
			bool foundfbh = false;
			for (size_t flid = 0; flid != NumBuckets * NumGenerations && !foundfbh; ++ flid)
			{
				for (AllocHeader* ah = freeLists[flid]; ah && !foundfbh; ah = ah->next)
				{
					if ((void*)ah == (void*) fbh)
						foundfbh = true;
				}
			}
			BucketAssert(!foundfbh);
		}
	}
#endif

	FreeCleanupInfo(alloc, pageInfos, pageCapacity);

	if (sortFreeLists)
	{
		for (size_t fl = 0; fl < NumGenerations * NumBuckets; ++ fl)
		{
#if BUCKET_ASSERTS
			int length = 0;
			for (AllocHeader* ah = freeLists[fl]; ah; ah = ah->next)
				++ length;
#endif

			AllocHeader** root = &freeLists[fl];
			SortLL(*root);

#if BUCKET_ASSERTS
			int lengthPostSort = 0;
			for (AllocHeader* ah = freeLists[fl]; ah; ah = ah->next)
				++ lengthPostSort;
			BucketAssert(length == lengthPostSort);
#endif
		}
	}

	for (size_t flIdx = 0; flIdx < NumBuckets * NumGenerations; ++ flIdx)
	{
		AllocHeader* item = freeLists[flIdx];
		AllocHeader* next;

		while (item)
		{
			next = item->next;
			item->next = NULL;

			this->PushOnto(m_freeLists[flIdx], item);

			item = next;
		}
	}

	memset((void*) m_bucketTouched, 0, sizeof(m_bucketTouched));

#if PROFILE_BUCKET_CLEANUP
	LARGE_INTEGER end, freq;
	QueryPerformanceCounter(&end);
	QueryPerformanceFrequency(&freq);

	{
		char msg[256];
		sprintf(msg, "[of] cleanup took %fms, unbound %i\n", (end.QuadPart - start.QuadPart) / (freq.QuadPart / 1000.0), unboundSize);
		OutputDebugString(msg);
	}
#endif
}

template <typename TraitsT>
void BucketAllocator<TraitsT>::FreeCleanupInfo(CleanupAllocator& alloc, SmallBlockCleanupInfo** infos, size_t infoCapacity)
{
	for (size_t pageId = 0; pageId != infoCapacity; ++ pageId)
	{
		if (infos[pageId])
			alloc.Free(infos[pageId]);
	}
	alloc.Free(infos);
}

template <typename TraitsT>
void BucketAllocator<TraitsT>::cleanup()
{
	typename SyncingPolicy::RefillLock lock(*this);
	CleanupInternal(true);
}

template <typename TraitsT>
void* BucketAllocator<TraitsT>::AllocatePageStorage()
{
	if (!m_baseAddress)
	{
		m_baseAddress = this->ReserveAddressSpace(NumPages, PageLength);
	}

	void* result = NULL;

	// Search from the last page mapped onwards, wrapping if necessary

	uint32 pageMidIdx = m_lastPageMapped % NumPages;

	for (uint32 pageIdx = (pageMidIdx + 1) != NumPages ? (pageMidIdx + 1) : 0;
		pageIdx != pageMidIdx;
		pageIdx = (pageIdx + 1) != NumPages ? (pageIdx + 1) : 0)
	{
		uint32 pageCellIdx = pageIdx / 32;
		uint32 pageBitMask = 1U << (pageIdx % 32);

		assert( pageCellIdx < sizeof(m_pageMap)/sizeof(m_pageMap[0]) );
		if ((m_pageMap[pageCellIdx] & pageBitMask) == 0)
		{
			UINT_PTR mapAddress = m_baseAddress + PageLength * pageIdx;
			result = reinterpret_cast<void*>(this->Map(mapAddress, PageLength));
			if (!result)
				return NULL;

			m_pageMap[pageCellIdx] |= pageBitMask;
			m_lastPageMapped = pageIdx;
			break;
		}
	}

	if (!result)
	{
		return NULL;
	}

	m_committed += PageLength;

#if CAPTURE_REPLAY_LOG
	CryGetIMemReplay()->MarkBucket(-2, 4, result, PageLength);
#endif

	return result;
}

template <typename TraitsT>
bool BucketAllocator<TraitsT>::DeallocatePageStorage(void* ptr)
{
	this->UnMap(reinterpret_cast<UINT_PTR>(ptr));

	UINT_PTR pageId = (reinterpret_cast<UINT_PTR>(ptr) - m_baseAddress) / PageLength;
	m_pageMap[pageId / 32] &= ~(1 << (pageId % 32));

	m_committed -= PageLength;

#if CAPTURE_REPLAY_LOG
	CryGetIMemReplay()->UnMarkBucket(-2, ptr);
#endif

	return true;
}

template <typename TraitsT>
size_t BucketAllocator<TraitsT>::GetBucketStorageSize()
{
	return m_committed;
}

template <typename TraitsT>
size_t BucketAllocator<TraitsT>::GetBucketStorageCapacity()
{
	if (m_baseAddress)
		return NumPages * PageLength;
	return 0;
}

template <typename TraitsT>
size_t BucketAllocator<TraitsT>::GetBucketConsumedSize()
{
#ifdef BUCKET_ALLOCATOR_TRACK_CONSUMED
	return m_consumed;
#else
	return 0;
#endif
}

template <typename TraitsT>
UINT_PTR BucketAllocator<TraitsT>::GetBucketCommittedBase()
{
	return m_baseAddress;
}

#if defined(XENON) || defined(_WIN32)

inline UINT_PTR BucketAllocatorDetail::SystemAllocator::ReserveAddressSpace(size_t numPages, size_t pageLen)
{
	BucketAssert(pageLen == 64 * 1024);
#ifdef BUCKET_ALLOCATOR_4K
	UINT_PTR addr = reinterpret_cast<UINT_PTR>(VirtualAlloc(NULL, numPages * pageLen, MEM_RESERVE, PAGE_READWRITE));
	addr = (addr + pageLen - 1) & ~(pageLen - 1);
	return addr;
#else
	return reinterpret_cast<UINT_PTR>(VirtualAlloc(NULL, numPages * pageLen, MEM_RESERVE | MEM_LARGE_PAGES, PAGE_READWRITE));
#endif
}

inline void BucketAllocatorDetail::SystemAllocator::UnreserveAddressSpace(UINT_PTR base)
{
	VirtualFree(reinterpret_cast<LPVOID>(base), 0, MEM_RELEASE);
}

inline UINT_PTR BucketAllocatorDetail::SystemAllocator::Map(UINT_PTR base, size_t len)
{
	BucketAssert(len == 64 * 1024);
#ifdef BUCKET_ALLOCATOR_4K
	return (UINT_PTR) VirtualAlloc(reinterpret_cast<LPVOID>(base), len, MEM_COMMIT, PAGE_READWRITE);
#else
	return (UINT_PTR) VirtualAlloc(reinterpret_cast<LPVOID>(base), len, MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
#endif
}

inline void BucketAllocatorDetail::SystemAllocator::UnMap(UINT_PTR addr)
{
	// Disable warning about only decommitting pages, and not releasing them
#pragma warning( push )
#pragma warning( disable : 6250 )
	VirtualFree(reinterpret_cast<LPVOID>(addr), 64 * 1024, MEM_DECOMMIT);
#pragma warning( pop )
}

inline BucketAllocatorDetail::SystemAllocator::CleanupAllocator::CleanupAllocator()
{
	m_base = VirtualAlloc(NULL, 1 * 1024 * 1024, MEM_RESERVE, PAGE_READWRITE);
	if (!m_base)
		__debugbreak();
	m_end = m_base;
}

inline BucketAllocatorDetail::SystemAllocator::CleanupAllocator::~CleanupAllocator()
{
	VirtualFree(m_base, 0, MEM_RELEASE);
}

inline void* BucketAllocatorDetail::SystemAllocator::CleanupAllocator::Calloc(size_t num, size_t sz)
{
	size_t size = num * sz;

	UINT_PTR end = reinterpret_cast<UINT_PTR>(m_end);
	UINT_PTR endAligned = (end + 4095) & ~4095;
	UINT_PTR sizeNeeded = ((end + size - endAligned) + 4095) & ~4095;

	if (sizeNeeded)
		VirtualAlloc(reinterpret_cast<void*>(endAligned), sizeNeeded, MEM_COMMIT, PAGE_READWRITE);

	void* result = m_end;
	m_end = reinterpret_cast<void*>(end + size);

	return result;
}

inline void BucketAllocatorDetail::SystemAllocator::CleanupAllocator::Free(void* ptr)
{
	// Will be freed automatically when the allocator is destroyed
}

#endif

#endif

#endif
