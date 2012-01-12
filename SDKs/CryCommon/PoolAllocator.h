//---------------------------------------------------------------------------
// Copyright 2006 Crytek GmbH
// Created by: Michael Smith
// Modified 2008-06, Scott Peter
//	Refactored to utilise storage management of new HeapAllocator class
//---------------------------------------------------------------------------

#ifndef __POOLALLOCATOR_H__
#define __POOLALLOCATOR_H__

//---------------------------------------------------------------------------
// Memory allocation class. Allocates, frees, and reuses fixed-size blocks of 
// memory, a scheme sometimes known as Simple Segregated Memory.
//
// Allocation is amortized constant time. The normal case is very fast -
// basically just a couple of dereferences. If many blocks are allocated,
// the system may occasionally need to allocate a further bucket of blocks
// for itself. Deallocation is strictly fast constant time.
//
// Each PoolAllocator allocates blocks of a single size and alignment, specified 
// by template arguments. There is no per-block space overhead, except for 
// alignment. The free list mechanism uses the memory of the block itself 
// when it is deallocated. 
// 
// In this implementation memory claimed by the system is never deallocated,
// until the entire allocator is deallocated. This is to ensure fast
// allocation/deallocation - reference counting the bucket quickly would
// require a pointer to the bucket be stored, whereas now no memory is used
// while the block is allocated.
//
// This allocator is suitable for use with STL lists - see STLPoolAllocator
// for an STL-compatible interface.
//
// The class can optionally support multi-threading, using the second
// template parameter. By default it is multithread-safe.
// See Synchronization.h.
//
// The class is implemented using a HeapAllocator.
//---------------------------------------------------------------------------

#include "HeapAllocator.h"

namespace stl
{
	//////////////////////////////////////////////////////////////////////////
	// Fixed-size pool allocator, using a shared heap.
	template <typename THeap>
	class SharedSizePoolAllocator
	{
		template <typename T> friend struct PoolCommonAllocator;
	protected:

		using_type(THeap, Lock);

		struct ObjectNode
		{
			ObjectNode* pNext;
		};

		static size_t AllocSize(size_t nSize)
		{
			return max(nSize, sizeof(ObjectNode));
		}
		static size_t AllocAlign(size_t nSize, size_t nAlign)
		{
			return nAlign > 0 ? nAlign : min(nSize, alignof(void*));
		}

	public:

		SharedSizePoolAllocator(THeap& heap, size_t nSize, size_t nAlign = 0, bool freePagesWhenEmpty = false)
		: _pHeap(&heap),
			_nAllocSize(AllocSize(nSize)),
			_nAllocAlign(AllocAlign(nSize, nAlign)),
			_pFreeList(0),
			_freePagesWhenEmpty(freePagesWhenEmpty)
		{
		}

		~SharedSizePoolAllocator()
		{
			// All allocated objects should be freed by now.
			Lock lock(*_pHeap);
			Validate(lock);
			for (ObjectNode* pFree = _pFreeList; pFree; )
			{
				ObjectNode* pNext = pFree->pNext;
				_pHeap->Deallocate(pFree, _nAllocSize, lock);
				pFree = pNext;
			}
		}

		// Raw allocation.
		void* Allocate()
		{
			{
				Lock lock(*_pHeap);
				if (_pFreeList)
				{
					ObjectNode* pFree = _pFreeList;
					_pFreeList = _pFreeList->pNext;
					Validate(lock);
					_Counts.nUsed++;
					return pFree;
				}
			}

			// No free pointer, allocate a new one.
			void* pNewMemory = _pHeap->Allocate(_nAllocSize, _nAllocAlign);
			if (pNewMemory)
			{
				_Counts.nUsed++;
				_Counts.nAlloc++;
			}
			return pNewMemory;
		}

		void Deallocate(void* pObject)
		{
			typename THeap::PageNodeHandle* pPageNode = NULL;
			if (pObject)
			{
				Lock lock(*_pHeap);
				assert(_pHeap->CheckPtr(pObject, lock));

				ObjectNode* pNode = static_cast<ObjectNode*>(pObject);

				// Add the object to the front of the free list.
				pNode->pNext = _pFreeList;
				_pFreeList = pNode;
				_Counts.nUsed--;

				if (_freePagesWhenEmpty && _Counts.nUsed == 0)
				{
					pPageNode = _pHeap->RemovePagesAlreadyLocked( lock );
					_pFreeList = NULL;
				}

				Validate(lock);
			}
			if (pPageNode)
			{
				_pHeap->FreeMemory(pPageNode);
			}
		}

		void FreeMemoryIfEmpty()
		{
			typename THeap::PageNodeHandle* pPageNode = NULL;
			{
				Lock lock(*_pHeap);

				if (_Counts.nUsed == 0)
				{
					pPageNode = _pHeap->RemovePagesAlreadyLocked( lock );
					_pFreeList = NULL;
				}

				Validate(lock);
			}
			if (pPageNode)
			{
				_pHeap->FreeMemory(pPageNode);
			}
		}

		size_t GetMemSize(const void* pObject) const
		{
			return Align(_nAllocSize, _nAllocAlign);
		}

		SMemoryUsage GetCounts() const
		{
			return _Counts;
		}
		SMemoryUsage GetTotalMemory() const
		{
			return SMemoryUsage(_Counts.nAlloc * _nAllocSize, _Counts.nUsed * _nAllocSize);
		}

		void GetMemoryUsage( ICrySizer *pSizer ) const
		{
			pSizer->AddObject(this, _Counts.nAlloc * _nAllocSize);
		}

	protected:

		void Validate(const Lock& lock) const
		{
			_pHeap->Validate(lock);
			_Counts.Validate();
			//assert(GetTotalMemory().nAlloc <= _pHeap->GetTotalMemory().nUsed);
		}

		void Reset()
		{
			Lock lock(*_pHeap);
			//assert(_Counts.nUsed == 0);
			_pFreeList = 0;
			_Counts.Clear();
		}
		
	protected:
		const size_t			_nAllocSize, _nAllocAlign;
		SMemoryUsage			_Counts;

		THeap*						_pHeap;
		ObjectNode*				_pFreeList;
		bool							_freePagesWhenEmpty;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SPoolMemoryUsage: SMemoryUsage
	{
		size_t nPool;

		SPoolMemoryUsage(size_t _nAlloc = 0, size_t _nPool = 0, size_t _nUsed = 0)
			: SMemoryUsage(_nAlloc, _nUsed), nPool(_nPool)
		{
			Validate();
		}

		size_t nPoolFree() const
		{
			return nPool - nUsed;
		}
		size_t nNonPoolFree() const
		{
			return nAlloc - nPool;
		}
		void Validate() const
		{
			assert(nUsed <= nPool);
			assert(nPool <= nAlloc);
		}
		void Clear()
		{
			nAlloc = nUsed = nPool = 0;
		}

		void operator += (SPoolMemoryUsage const& op)
		{
			nAlloc += op.nAlloc;
			nPool += op.nPool;
			nUsed += op.nUsed;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// SizePoolAllocator with owned heap
	template <typename THeap>
	class SizePoolAllocator: public SharedSizePoolAllocator<THeap>
	{
		typedef SharedSizePoolAllocator<THeap> super;
		using_type(THeap, Lock);
		using super::AllocSize;
		using super::Reset;

	public:

		SizePoolAllocator(size_t nSize, size_t nAlign = 0, size_t nBucketSize = 0, bool freePagesWhenEmpty = false)
		: _Heap(nBucketSize * AllocSize(nSize)), super(_Heap, nSize, nAlign, freePagesWhenEmpty)
		{
		}

		~SizePoolAllocator()
		{
			// Ignore the free list, as owned heap freed all at once.
			Reset();
		}

		void FreeMemory( bool bDeallocate = true)
		{
			Reset();
			if (bDeallocate)
				_Heap.Clear();
			else
				_Heap.Reset();
		}

		SPoolMemoryUsage GetTotalMemory() const
		{
			return SPoolMemoryUsage (_Heap.GetTotalMemory().nAlloc, super::GetTotalMemory().nAlloc, super::GetTotalMemory().nUsed);
		}
		size_t GetTotalAllocatedMemory() const
		{
			return _Heap.GetTotalMemory().nAlloc;
		}
		size_t GetTotalAllocatedNodeSize() const
		{
			return super::GetTotalMemory().nUsed;
		}

	protected:
		THeap				_Heap;
	};

	//////////////////////////////////////////////////////////////////////////
	// Templated size version of SizePoolAllocator
	template <int S, typename L = PSyncMultiThread, int A = 0>
	class PoolAllocator: public SizePoolAllocator< HeapAllocator<L> >
	{
	public:
		PoolAllocator(size_t nBucketSize = 0, bool freePagesWhenEmpty = false)
		:	SizePoolAllocator< HeapAllocator<L> >(S, A, nBucketSize, freePagesWhenEmpty)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////////
	template <int S, int A = 0>
	class PoolAllocatorNoMT : public SizePoolAllocator< HeapAllocator<PSyncNone> >
	{
	public:
		PoolAllocatorNoMT(size_t nBucketSize = 0, bool freePagesWhenEmpty = false)
		:	SizePoolAllocator< HeapAllocator<PSyncNone> >(S, A, nBucketSize, freePagesWhenEmpty)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////////
	template<typename T, typename L = PSyncMultiThread, size_t A = 0>
	class TPoolAllocator: public SizePoolAllocator< HeapAllocator<L> >
	{
	public:
		TPoolAllocator(size_t nBucketSize = 0)
			: SizePoolAllocator< HeapAllocator<L> >(sizeof(T), max(alignof(T), A), nBucketSize)
		{}
	};

	// Legacy verbose typedefs.
	typedef PSyncNone PoolAllocatorSynchronizationSinglethreaded;
	typedef PSyncMultiThread PoolAllocatorSynchronizationMultithreaded;

	//////////////////////////////////////////////////////////////////////////
	// Allocator maintaining multiple type-specific pools, sharing a common heap source.
	template<typename THeap>
	struct PoolCommonAllocator
	{
		typedef SharedSizePoolAllocator<THeap> TPool;

	protected:
		struct TPoolNode: SharedSizePoolAllocator<THeap>
		{
			TPoolNode* pNext;

			TPoolNode(THeap& heap, TPoolNode*& pList, size_t nSize, size_t nAlign)
				: SharedSizePoolAllocator<THeap>(heap, nSize, nAlign)
			{
				pNext = pList;
				pList = this;
			}
		};

	public:

		PoolCommonAllocator()
			: _pPoolList(0)
		{
		}
		~PoolCommonAllocator()
		{
			TPoolNode* pPool = _pPoolList; 
			while (pPool)
			{
				TPoolNode* pNextPool = pPool->pNext;
				delete pPool;
				pPool = pNextPool;
			}
		}

		TPool* CreatePool(size_t nSize, size_t nAlign = 0)
		{
			return new TPoolNode(_Heap, _pPoolList, nSize, nAlign);
		}

		SPoolMemoryUsage GetTotalMemory()
		{
			SMemoryUsage mem;
			for (TPoolNode* pPool = _pPoolList; pPool; pPool = pPool->pNext)
				mem += pPool->GetTotalMemory();
			return SPoolMemoryUsage(_Heap.GetTotalMemory().nAlloc, mem.nAlloc, mem.nUsed);
		}

		bool FreeMemory( bool bDeallocate = true)
		{
			SPoolMemoryUsage mem = GetTotalMemory();
			if (mem.nUsed != 0)
				return false;

			for (TPoolNode* pPool = _pPoolList; pPool; pPool = pPool->pNext)
				pPool->Reset();

			if (bDeallocate)
				_Heap.Clear();
			else
				_Heap.Reset();
			return true;
		}

	protected:
		THeap				_Heap;
		TPoolNode*	_pPoolList;
	};

	//////////////////////////////////////////////////////////////////////////
	// The additional TInstancer type provides a way of instantiating multiple instances
	// of this class, without static variables.
	template<typename THeap, typename TInstancer = int>
	struct StaticPoolCommonAllocator
	{
		ILINE static PoolCommonAllocator<THeap>& StaticAllocator()
		{
			static PoolCommonAllocator<THeap> s_Allocator;
			return s_Allocator;
		}

		typedef SharedSizePoolAllocator<THeap> TPool;

		template<class T>
			ILINE static TPool& TypeAllocator()
			{
				static TPool* sp_Pool = CreatePoolOnGlobalHeap(sizeof(T), alignof(T));
				return *sp_Pool;
			}

		template<class T>
			ILINE static void* Allocate(T*& p)
				{ return p = (T*)TypeAllocator<T>().Allocate(); }

		template<class T>
			ILINE static void Deallocate(T* p)
				{ return TypeAllocator<T>().Deallocate(p); }

		template<class T>
			ILINE static size_t GetMemSize(const T* p)
				{ return TypeAllocator<T>().GetMemSize(p); }

		static SPoolMemoryUsage GetTotalMemory()
			{ return StaticAllocator().GetTotalMemory(); }

	private:
		ILINE static TPool* CreatePoolOnGlobalHeap(size_t nSize, size_t nAlign = 0)
		{
			ScopedSwitchToGlobalHeap globalHeap;
			return StaticAllocator().CreatePool(nSize, nAlign);
		}
	};
};


#endif //__POOLALLOCATOR_H__
