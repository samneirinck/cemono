//---------------------------------------------------------------------------
// Copyright 2008 Crytek GmbH
// Created by: Scott Peter
//---------------------------------------------------------------------------

#ifndef _HEAP_ALLOCATOR_H
#define _HEAP_ALLOCATOR_H

#include "Synchronization.h"

//---------------------------------------------------------------------------
#define bMEM_ACCESS_CHECK		0
#define bMEM_HEAP_CHECK			0

namespace stl
{
	class HeapSysAllocator
	{
	public:
	#if bMEM_ACCESS_CHECK
		static void* SysAlloc(size_t nSize)
			{ return VirtualAlloc(0, nSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); }
		static void SysDealloc(void* ptr)
			{ VirtualFree(ptr, 0, MEM_DECOMMIT); }
	#else
		static void* SysAlloc(size_t nSize)
			{ return malloc(nSize); }
		static void SysDealloc(void* ptr)
			{ free(ptr); }
	#endif
	};

	class GlobalHeapSysAllocator
	{
	public:
		static void* SysAlloc(size_t nSize)
		{
			ScopedSwitchToGlobalHeap useGlobalHeap;
			return malloc(nSize);
		}
		static void SysDealloc(void* ptr)
		{
			ScopedSwitchToGlobalHeap useGlobalHeap;
			free(ptr);
		}
	};

	// Round up to next multiple of nAlign. Handles any postive integer.
	inline size_t RoundUpTo(size_t nSize, size_t nAlign)
	{
		assert(nAlign > 0);
		nSize += nAlign-1;
		return nSize - nSize % nAlign;
	}

	/*---------------------------------------------------------------------------
	HeapAllocator
		A memory pool that can allocate arbitrary amounts of memory of arbitrary size
		and alignment. The heap may be freed all at once. Individual block deallocation 
		is not provided.

		Usable as a base class to implement more general-purpose allocators that
		track, free, and reuse individual memory blocks.

		The class can optionally support multi-threading, using the second
		template parameter. By default it is multithread-safe.
		See Synchronization.h.

		Allocation details: Maintains a linked list of pages.
		All pages after first are in order of most free memory first.
		Allocations are from the smallest free page available.

	---------------------------------------------------------------------------*/

	struct SMemoryUsage
	{
		size_t nAlloc, nUsed;

		SMemoryUsage(size_t _nAlloc = 0, size_t _nUsed = 0)
			: nAlloc(_nAlloc), nUsed(_nUsed)
		{
			Validate();
		}

		size_t nFree() const
		{
			return nAlloc - nUsed;
		}
		void Validate() const
		{
			assert(nUsed <= nAlloc);
		}
		void Clear()
		{
			nAlloc = nUsed = 0;
		}

		void operator += (SMemoryUsage const& op)
		{
			nAlloc += op.nAlloc;
			nUsed += op.nUsed;
		}
	};

	template <typename L = PSyncMultiThread, bool bMULTI_PAGE = true, typename SysAl = HeapSysAllocator>
	class HeapAllocator: public L, private SysAl
	{
	public:
		typedef AutoLock<L> Lock;

		enum {DefaultAlignment = sizeof(void*)};
		enum {DefaultPageSize = 0x1000};

		HeapAllocator(size_t nPageSize = DefaultPageSize)
		:	_nPageSize(nPageSize ? nPageSize : DefaultPageSize),
			_pPageList(0)
		{
		}

		~HeapAllocator()
		{
			Clear();
		}

		//
		// Raw memory allocation.
		//
		void* Allocate(size_t nSize, size_t nAlign = DefaultAlignment)
		{
			for (;;)
			{
				Lock lock(*this);

				// Try allocating from head page first.
				if (_pPageList)
				{
					if (void* ptr = _pPageList->Allocate(nSize, nAlign))
					{
						_TotalMem.nUsed += nSize;
						return ptr;
					}

					if (_pPageList->pNext && _pPageList->pNext->GetMemoryFree() > _pPageList->GetMemoryFree())
					{
						SortPage(_pPageList, lock);
						Validate(lock);

						// Try allocating from new head, which has the most free memory.
						// If this fails, we know no further pages will succeed.
						if (void* ptr = _pPageList->Allocate(nSize, nAlign))
						{
							_TotalMem.nUsed += nSize;
							return ptr;
						}
					}
					if (!bMULTI_PAGE)
						return 0;
				}

				// Allocate the new page of the required size.
				int nAllocSize = sizeof(PageNode) + nAlign - 1 + RoundUpTo(nSize, _nPageSize);

				void* pAlloc = this->SysAlloc(nAllocSize);
				PageNode* pPageNode = new(pAlloc) PageNode(nAllocSize);

				// Insert at head of list.
				pPageNode->pNext = _pPageList;
				_pPageList = pPageNode;

				_TotalMem.nAlloc += nAllocSize;

				Validate(lock);
			}
		}

		void Deallocate(void* ptr, size_t nSize, const Lock& lock)
		{
			// Just to maintain counts, can't reuse memory.
			assert(CheckPtr(ptr, lock));
			assert(_TotalMem.nUsed >= nSize);
			_TotalMem.nUsed -= nSize;
		}

		//
		// Templated type allocation.
		//
		template<typename T>
		T* New(size_t nAlign = 0)
		{
			return new(Allocate(sizeof(T), nAlign ? nAlign : alignof(T))) T;
		}

		template<typename T>
		T* NewArray(size_t nCount, size_t nAlign = 0)
		{
			return new(Allocate(sizeof(T)*nCount, nAlign ? nAlign : alignof(T))) T[nCount];
		}

		//
		// Maintenance.
		//
		SMemoryUsage GetTotalMemory() const
		{
			return _TotalMem;
		}

		struct PageNodeHandle
		{
		};

		PageNodeHandle* RemovePagesAlreadyLocked(Lock& lock)
		{
			// Remove the pages from the object.
			PageNodeHandle* pPageNode;
			Validate(lock);
			pPageNode = _pPageList;
			_pPageList = 0;
			_TotalMem.Clear();
			return pPageNode;
		}

		void FreeMemory(PageNodeHandle* handle)
		{
			PageNode* pPageNode = static_cast<PageNode*>(handle);
			// Loop through all the pages, freeing the memory.
			while (pPageNode != 0)
			{
				// Read the "next" pointer before deleting.
				PageNode* pNext = pPageNode->pNext;

				// Delete the current page.
				this->SysDealloc(pPageNode);

				// Move to the next page in the list.
				pPageNode = pNext;
			}
		}

		void Clear()
		{
			PageNodeHandle* pPageNode;
			{
				// Remove the pages from the object.
				Lock lock(*this);
				Validate(lock);
				pPageNode = RemovePagesAlreadyLocked(lock);
			}

			// Loop through all the pages, freeing the memory.
			FreeMemory(pPageNode);
		}

		void Reset()
		{
			// Reset all pages, allowing memory re-use.
			Lock lock(*this);
			Validate(lock);
			size_t nPrevSize = ~0;
			for (PageNode** ppPage = &_pPageList; *ppPage; )
			{
				(*ppPage)->Reset();
				if ((*ppPage)->GetMemoryAlloc() > nPrevSize)
				{
					// Move page to sorted location near beginning.
					SortPage(*ppPage, lock);

					// ppPage is now next page, so continue loop.
					continue;
				}
				nPrevSize = (*ppPage)->GetMemoryAlloc();
				ppPage = &(*ppPage)->pNext;
			}
			_TotalMem.nUsed = 0;
			Validate(lock);
		}

		//
		// Validation.
		//
		bool CheckPtr(void* ptr, const Lock&) const
		{
			if (!ptr)
				return true;
			for (PageNode* pNode = _pPageList; pNode; pNode = pNode->pNext)
			{
				if (pNode->CheckPtr(ptr))
					return true;
			}
			return false;
		}

		void Validate(const Lock&) const
		{
		#ifdef _DEBUG
			// Check page validity, and memory counts.
			SMemoryUsage MemCheck;

			for (PageNode* pPage = _pPageList; pPage; pPage = pPage->pNext)
			{
				pPage->Validate();
				if (pPage != _pPageList && pPage->pNext)
					assert(pPage->GetMemoryFree() >= pPage->pNext->GetMemoryFree());
				MemCheck.nAlloc += pPage->GetMemoryAlloc();
				MemCheck.nUsed += pPage->GetMemoryUsed();
			}
			assert(MemCheck.nAlloc == _TotalMem.nAlloc);
			assert(MemCheck.nUsed >= _TotalMem.nUsed);
		#endif

		#if bMEM_HEAP_CHECK
			static int nCount = 0, nInterval = 0;
			if (nCount++ >= nInterval)
			{
				nInterval++;
				nCount = 0;
				assert(IsHeapValid());
			}
		#endif
		}

		void GetMemoryUsage( ICrySizer *pSizer ) const
		{
			Lock lock(non_const(*this));
			for (PageNode *pNode = _pPageList; pNode; pNode = pNode->pNext)
			{
				pSizer->AddObject(pNode, pNode->GetMemoryAlloc());
			}
		}

	protected:

		struct PageNode : PageNodeHandle
		{
			PageNode* pNext;
			char*			pEndAlloc;
			char*			pEndUsed;

			char* StartUsed() const
			{
				return (char*)(this+1);
			}

			PageNode(size_t nAlloc)
			{
				pNext = 0;
				pEndAlloc = (char*)this + nAlloc;
				pEndUsed = StartUsed();
			}

			void* Allocate(size_t nSize, size_t nAlign)
			{
				// Align current mem.
				char* pNew = Align(pEndUsed, nAlign);
				if (pNew + nSize > pEndAlloc)
					return 0;
				pEndUsed = pNew + nSize;
				MEMSTAT_USAGE(this, GetMemoryUsed());
				return pNew;
			}

			bool CanAllocate(size_t nSize, size_t nAlign)
			{
				return Align(pEndUsed, nAlign) + nSize <= pEndAlloc;
			}

			void Reset()
			{
				pEndUsed = StartUsed();
				MEMSTAT_USAGE(this, GetMemoryUsed());
			}

			size_t GetMemoryAlloc() const
			{
				return pEndAlloc - (char*)this;
			}
			size_t GetMemoryUsed() const
			{
				return pEndUsed - StartUsed();
			}
			size_t GetMemoryFree() const
			{
				return pEndAlloc - pEndUsed;
			}

			void Validate() const
			{
				assert(pEndAlloc >= (char*)this);
				assert(pEndUsed >= StartUsed() && pEndUsed <= pEndAlloc);
			}

			bool CheckPtr(void* ptr) const
			{
				return (char*)ptr >= StartUsed() && (char*)ptr < pEndUsed;
			}
		};

		void SortPage(PageNode*& rpPage, const Lock&)
		{
			// Unlink rpPage.
			PageNode* pPage = rpPage;
			rpPage = pPage->pNext;

			// Insert into list based on free memory.
			PageNode** ppBefore = &_pPageList;
			while (*ppBefore && (*ppBefore)->GetMemoryFree() > pPage->GetMemoryFree())
				ppBefore = &(*ppBefore)->pNext;

			// Link before rpList.
			pPage->pNext = *ppBefore;
			*ppBefore = pPage;
		}

	private:
		const size_t	_nPageSize;		// Pages allocated at this size, or multiple thereof if needed.
		PageNode*			_pPageList;		// All allocated pages.
		SMemoryUsage	_TotalMem;		// Track memory allocated and used.
	};
}

#endif //_HEAP_ALLOCATOR_H
