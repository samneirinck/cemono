//---------------------------------------------------------------------------
// Copyright 2005 Crytek GmbH
// Created by: Scott Peter
//---------------------------------------------------------------------------

#ifndef _CRY_ARRAY_H_
#define _CRY_ARRAY_H_
#pragma once

#include "RangeIter.h"

#if defined __CRYCG__
	#ifndef SPU_NO_INLINE
			#define SPU_NO_INLINE __attribute__ ((crycg_attr ("noinline")))
	#endif
#else
	#ifndef SPU_NO_INLINE
		#define SPU_NO_INLINE 
	#endif
#endif

/*---------------------------------------------------------------------------
Public classes:

		Array<T>
		StaticArray<T, nSIZE>
		DynArrayRef<T>
		DynArray<T>
		FixedDynArray<T>
		StaticFixedArray<T, nSIZE>

Support classes are placed in namespaces NArray and NAlloc to reduce global name usage.
---------------------------------------------------------------------------*/

// Useful functions and macros.

template<class T>
inline size_t mem_offset(T* a, T* b)
{ 
	assert(b >= a);
	return size_t(b) - size_t(a); 
}

template<class T>
inline T* mem_adjust(T* ptr, T* a, T* b)
	{ return (T*)( size_t(ptr) + size_t(b) - size_t(a) ); }

namespace NArray
{
	// We should never have these defined as macros.
	#undef min
	#undef max

	// Define our own min/max here, to avoid including entire <algorithm>.
	template<class T> inline T min( T a, T b )
		{ return a < b ? a : b; }
	template<class T> inline T max( T a, T b )
		{ return a > b ? a : b; }

	/*---------------------------------------------------------------------------
	// STORAGE prototype for Array<T,STORAGE>.
	struct Storage<typename T>
	{
		typedef size_type;
		typedef difference_type;

		[const] T* begin() [const];
		size_type size() const;
	};

	---------------------------------------------------------------------------*/

	//---------------------------------------------------------------------------
	// Array<T,S>: Non-growing array.
	// S serves as base class, and implements storage scheme: begin(), size()

	template<class T> struct ArrayStorage;
};

template< class T, class S = NArray::ArrayStorage<T> >
struct Array: S
{
	// Tedious redundancy.
	using_type(S, size_type)
	using_type(S, difference_type)

	using S::size;
	using S::begin;

	// STL-compatible typedefs.
	typedef T							value_type;
	typedef T*						pointer;
	typedef const T*			const_pointer;
	typedef T&						reference;
	typedef const T&			const_reference;

	typedef T*						iterator;
	typedef const T*			const_iterator;

	// Construction.
	Array()
		{}

	// Forward single- and double-argument constructors.
	template<class I>
	explicit Array(const I& i)
		: S(i)
		{}

	template<class I1, class I2>
	Array(const I1& i1, const I2& i2)
		: S(i1, i2)
		{}

	// Accessors.
	inline bool empty() const
		{ return size() == 0; }
	inline size_type size_mem() const
		{ return size() * sizeof(T); }

	CONST_VAR_FUNCTION( T* end(),								
		{ return begin()+size(); } )

	CONST_VAR_FUNCTION( T* rbegin(),
		{ return begin()+size()-1; } )

	CONST_VAR_FUNCTION( T* rend(),
		{ return begin()-1; } )

	CONST_VAR_FUNCTION( T& front(),
	{ 
		assert(!empty()); 
		return *begin(); 
	} )

	CONST_VAR_FUNCTION( T& back(),
	{ 
		assert(!empty()); 
		return *rbegin();
	} )

	CONST_VAR_FUNCTION( T& at(size_type i),
	{
		CRY_ASSERT_TRACE(i >= 0 && i < size(), ("Index %lld is out of range (array size is %lld)", (long long int) i, (long long int) size()));
		return begin()[i];
	} )

	CONST_VAR_FUNCTION( T& operator [](size_type i), 
	{
		CRY_ASSERT_TRACE(i >= 0 && i < size(), ("Index %lld is out of range (array size is %lld)", (long long int) i, (long long int) size()));
		return begin()[i];
	} )

	CONST_VAR_FUNCTION( T* GetForPrecache(int i),
	{ 
		return i >= size() ? begin() : begin() + i;
	} )

	// Conversion to canonical array type.
	operator Array<T>()
		{ return Array<T>(begin(), size()); }
	operator Array<const T>() const
		{ return Array<const T>(begin(), size()); }

	// Additional conversion via operator() to full or sub array.
	Array<T> operator ()(size_type i = 0, size_type count = 0)
	{
		assert(i >= 0 && count >= 0 && i+count <= size());
		return Array<T>( SPU_MAIN_PTR(begin()+i), count ? count : size()-i);
	}
	Array<const T> operator ()(size_type i = 0, size_type count = 0) const
	{
		assert(i >= 0 && count >= 0 && i+count <= size());
		return Array<const T>(begin()+i, count ? count : size()-i);
	}

	// Basic element assignment functions.

	// Copy values to existing elements.
	void assign(const T& val)
	{
		for_range (T, it, (*this))
			*it = val;
	}

	void assign(Array<const T> array)
	{
		assert(array.size() == size());
		const T* src = array.begin();
		for_range (T, it, (*this))
			*it = *src++;
	}

protected:

	// Init raw elements to default or copied values.
	iterator init(iterator start, size_type count)
	{
		assert(start >= begin() && start+count <= end());
		for_range (T, it, (start, start+count))
			new(&*it) T;
		return start;
	}
	iterator init(iterator start, size_type count, const T& val)
	{
		assert(start >= begin() && start+count <= end());
		for_range (T, it, (start, start+count))
			new(&*it) T(val);
		return start;
	}
	iterator init(iterator start, Array<const T> array)
	{
		assert(start >= begin() && start+array.size() <= end());
		const_iterator src = array.begin();
		for_range (T, it, (start, start+array.size()))
			new(&*it) T(*src++);
		return start;
	}

	// Destroy in reverse order, to match construction order.
	void destroy(iterator start, iterator finish)
	{
		assert(start >= begin() && finish <= end());
		while (finish-- > start)
			finish->~T();
	}
	void destroy()
	{
		destroy(begin(), end());
	}
};

// Type-inferring constructor.

template<class T>
inline Array<T> ArrayT(T* elems, int count)
{
	return Array<T>(elems, count);
}

//---------------------------------------------------------------------------
// ArrayStorage: Default STORAGE Array<T,STORAGE>.
// Simply contains a pointer and count to an existing array,
// performs no allocation or deallocation.

namespace NArray
{
	template<class T>
	struct ArrayStorage
	{
		// Use default int for allocation and indexing types, for convenience,
		// and because we don't require arrays larger than 2 GB.
		typedef int size_type;
		typedef int difference_type;

		// Construction.
		inline ArrayStorage()
			: m_aElems(0), m_nCount(0)
			{}
		inline ArrayStorage(T* elems, size_type count)
			: m_aElems(elems), m_nCount(count)
			{}
		inline ArrayStorage(T* start, T* finish)
			: m_aElems(start), m_nCount(check_cast<size_type>(finish-start))
			{}

		void set(T* elems, size_type count)
		{
			m_aElems = elems;
			m_nCount = count;
		}

		// Basic storage.
		CONST_VAR_FUNCTION( T* begin(),
			{ return m_aElems; } )
		inline size_type size() const	
			{ return m_nCount; }

		// Modifiers, alter range in place.
		void erase_front(size_type count = 1)
		{ 
			assert(count <= m_nCount);
			m_nCount -= count;
			m_aElems += count;
		}

		void erase_back(size_type count = 1)
		{ 
			assert(count <= m_nCount);
			m_nCount -= count;
		}

	protected:
		T*				m_aElems;
		size_type	m_nCount;
	};

	//---------------------------------------------------------------------------
	// StaticArrayStorage: Alternate STORAGE scheme for Array<T,STORAGE>.
	// Array is statically sized inline member.

	template<class T, int nSIZE> 
	struct StaticArrayStorage
	{
		typedef T value_type;
		typedef int size_type;
		typedef int difference_type;

		// Basic storage.
		CONST_VAR_FUNCTION( T* begin(),
			{ return m_aElems; } )
		inline static size_type size()
			{ return nSIZE; }

	protected:
		T				m_aElems[nSIZE];
	};

	// StaticArray<T,nSIZE>
	// A superior alternative to static C arrays.
	// Provides standard STL-like Array interface, including bounds-checking.
	//		standard:		Type array[256];
	//		structured:	StaticArray<Type,256> array;
};

template<class T, int nSIZE> 
struct StaticArray: Array< T, NArray::StaticArrayStorage<T,nSIZE> >
{
};

namespace NAlloc
{
	struct CrossModuleAlloc;
};

namespace NArray
{
	/*---------------------------------------------------------------------------
	// STORAGE prototype for DynArray<T,STORAGE>
	// Extends ArrayStorage with resizing functionality.

	struct DynStorage<T>: Storage<T>
	{
		size_type capacity();
		size_type max_size();
		void resize( size_type new_size, size_type nCap = 0 );
		~DynStorage();
	};
	---------------------------------------------------------------------------*/

	template<class T, class A = NAlloc::CrossModuleAlloc, int nALIGN = 0> struct SmallDynStorage;
};

//---------------------------------------------------------------------------

// DynArrayRef<T,STORAGE>: Non-growing access to a DynArray (see below).
// Does not specify allocation scheme, any DynArray can convert to it.
// Simply an Array alias.

template< class T, class S = NArray::SmallDynStorage<T> >
struct DynArrayRef: Array<T,S>
{
};

//---------------------------------------------------------------------------
// DynArray<T,A,S>: Extension of Array allowing dynamic allocation.
// S specifies storage scheme, as with Array, but adds resize(), capacity(), ...
// A specifies the actual memory allocation function: alloc()

template< class T, class S = NArray::SmallDynStorage<T> >
struct DynArray: DynArrayRef<T,S>
{
	typedef DynArray<T,S> self_type;
	typedef DynArrayRef<T,S> super_type;

	// Tedious redundancy for GCC.
	using_type(super_type, size_type);
	using_type(super_type, difference_type);
	using_type(super_type, iterator);
	using_type(super_type, const_iterator);

	using super_type::size;
	using super_type::capacity;
	using super_type::empty;
	using super_type::begin;
	using super_type::end;
	using super_type::back;
	using super_type::destroy;

	//
	// Construction.
	//
	inline DynArray()
		{}

	// Copying from a generic array type.
	DynArray(Array<const T> a)
	{
		push_back(a);
	}
	self_type& operator =(Array<const T> a)
	{
		if (a.begin() >= begin() && a.end() <= end())
		{
			// Assigning from (partial) self; remove undesired elements.
			erase((T*)a.end(), end());
			erase(begin(), (T*)a.begin());
		}
		else
		{
			// Assert no overlap.
			assert(a.end() <= begin() || a.begin() >= end());
			if (a.size() == size())
			{
				// If same size, perform element copy.
				assign(a);
			}
			else
			{
				// If different sizes, destroy then copy init elements.
				destroy();
				resize_raw(a.size());
				init(begin(), a);
			}
		}
		return *this;
	}

	// Copy init/assign.
	inline DynArray(const self_type& a)
	{
		push_back(a());
	}
	inline self_type& operator =(const self_type& a)
	{
		return *this = a();
	}

	// Init/assign from basic storage type.
	inline DynArray(const S& a)
	{
		push_back(Array<const T>(a.begin(), a.size()));
	}
	inline self_type& operator =(const S& a)
	{
		return *this = Array<const T>(a.begin(), a.size());
	}

	inline ~DynArray()
	{
		destroy();
	}

	inline size_type available() const
	{ 
		return capacity() - size(); 
	}

	//
	// Allocation modifiers.
	//

	// Resize without initialising new members.
	void resize_raw(size_type count, size_type cap)
	{
		S::resize(count, cap);

		MEMSTAT_USAGE(begin(), sizeof(T) * size());
	}
	void resize_raw(size_type count)
	{
		S::resize(count, capacity());
	}

	void reserve(size_type count)
	{
		if (count > capacity())
			resize_raw(size(), count);
	}

	T* grow_raw(size_type count = 1)
	{
		assert(count >= 0);
		resize_raw(size()+count, 0);
		return end() - count;
	}
	iterator grow(size_type count)
	{
		return init(grow_raw(count), count);
	}
	iterator grow(size_type count, const T& val)
	{
		return init(grow_raw(count), count, val);
	}

	void shrink()
	{
		S::resize(size(), size());

		MEMSTAT_USAGE(begin(), sizeof(T) * size());
	}
	SPU_NO_INLINE void resize(size_type new_size)
	{
		if (new_size > size())
			grow(new_size - size());
		else
		{
			destroy(begin()+new_size, end());
			resize_raw(new_size);
		}
	}
	SPU_NO_INLINE void resize(size_type new_size, const T& val)
	{
		if (new_size > size())
			grow(new_size - size(), val);
		else
		{
			destroy(begin()+new_size, end());
			resize_raw(new_size);
		}
	}

	iterator push_back()
	{
		return new(grow_raw()) T;
	}
	iterator push_back(const T& val)
	{ 
		return new(grow_raw()) T(val);
	}
	iterator push_back(Array<const T> array)
	{
		return init( grow_raw(array.size()), array);
	}

	T* insert_raw(iterator it, size_type count = 1)
	{
		assert(it >= begin() && it <= end());

		// Realloc, then move elements.
		T* old_begin = begin();
		grow_raw(count);
		it = mem_adjust(it, old_begin, begin());

		memmove(it+count, it, mem_offset(it+count, end()));
		return it;
	}

	iterator insert(iterator it)
	{
		return new(insert_raw(it)) T;
	}
	iterator insert(iterator it, const T& val)
	{
		return new(insert_raw(it)) T(val);
	}
	iterator insert(iterator it, Array<const T> array)
	{
		return init( insert_raw(it, array.size()), array);
	}
	iterator insert(iterator it, const_iterator start, const_iterator finish)
	{
		return insert( it, Array<T>(start, check_cast<size_type>(finish-start)) );
	}
	iterator insert(iterator it, size_type count, const T* pval = 0)
	{
		return init(insert_raw(it, count), count);
	}
	iterator insert(iterator it, size_type count, const T& val)
	{
		return init(insert_raw(it, count), count, val);
	}

	void pop_back()
	{
		if (!empty())
		{
			back().~T();
			resize_raw(size()-1);
		}
	}

	iterator erase(iterator start, iterator finish)
	{
		// Destroy, then delete elems.
		destroy(start, finish);
		memmove(start, finish, mem_offset(finish, end()));

		// Resize mem.
		T* old_begin = begin();
		size_type count = check_cast<size_type>(finish - start);
		resize_raw(size() - count);
		return mem_adjust(start, old_begin, begin());
	}

	iterator erase(iterator it)
	{
		return erase(it, it+1);
	}

	size_type erase(size_type i, size_type count = 1)
	{
		// Destroy, then delete elems.
		destroy(begin()+i, begin()+i+count);

		T* dst = SPU_MAIN_PTR( begin()+i );
		T* src = SPU_MAIN_PTR( dst + count );
		size_type num = (size()-i-count)*sizeof(T);

		memmove(dst, src, num);

		// Resize mem.
		resize_raw(size() - count);
		return i-1;
	}

	void clear()
	{
		if (capacity())
		{
			destroy();
			resize_raw(0, 0);
		}
	}
};

//---------------------------------------------------------------------------
// FixedDynStorage: STORAGE scheme for DynArray<T,STORAGE> which simply
// uses a non-growable array type, and adds a current count.

namespace NArray
{
	template<class ARRAY>
	struct FixedDynStorage
	{
		using_type(ARRAY, size_type);
		using_type(ARRAY, difference_type);
		using_type(ARRAY, value_type);
		typedef value_type T;

		// Construction.
		FixedDynStorage()
			: m_nCount(0)
		{}

		void set( const ARRAY& array )
		{ 
			m_Array = array; 
			m_nCount = 0;
		}

		CONST_VAR_FUNCTION( T* begin(),
			{ return m_Array.begin(); } )
		size_type size() const
			{ return m_nCount; }
		size_type get_alloc_size() const
			{ return 0; }

		inline size_type capacity() const
			{ return m_Array.size(); }
		inline size_type max_size() const
			{ return m_Array.size(); }

		void resize( size_type new_size, size_type new_cap = 0 )
		{
			// capacity unchangeable, just assert new_cap within range.
			assert(new_size >= 0 && new_size <= max_size());
			assert(new_cap <= max_size());
			m_nCount = new_size;
		}

	protected:
		size_type		m_nCount;
		ARRAY				m_Array;
	};
};

template<class T, class ARRAY = Array<T> >
struct FixedDynArray: DynArray< T, NArray::FixedDynStorage<ARRAY> >
{
	FixedDynArray()
		{}
	FixedDynArray( const ARRAY& array )
		{ set(array); }
};

template<class T, int nSIZE>
struct StaticDynArray: FixedDynArray< T, NArray::StaticArrayStorage<T,nSIZE> >
{
};

/*---------------------------------------------------------------------------
// ALLOC prototype for ArrayStorage.

struct Alloc
{
	static void* alloc( void* pCur, size_t nCurBytes, bool& bStoredAlloc, size_t& nNewBytes, bool bSlack = false, int nAlign = 1, int nPrefixBytes = 0 );
	static size_t alloc_size( void* pCur, size_t nCurBytes, bool bStoredAlloc, int nAlign = 1, int nPrefixBytes = 0 );
};

---------------------------------------------------------------------------*/

namespace NArray
{
	template<class A>
	void AllocFree( void* pCur, bool bStoredAlloc, int nAlign = 1, int nPrefixBytes = 0 )
	{
		size_t nNewBytes = 0;
		A::alloc(pCur, 0, bStoredAlloc, nNewBytes, false, nAlign, nPrefixBytes);
	}

	//---------------------------------------------------------------------------
	// FastDynStorage: STORAGE scheme for Array<T,STORAGE> and descendents.
	// Simple extension to ArrayStorage: size & capacity fields are inline, 3 words storage, fast access.

	template<class T, class A = NAlloc::CrossModuleAlloc, int nALIGN = 0>
	struct FastDynStorage: ArrayStorage<T>, NoCopy
	{
		typedef FastDynStorage<T,A,nALIGN> self_type;
		typedef ArrayStorage<T> super_type;
		using_type(super_type, size_type);

		static int alignment()
			{ return nALIGN ? nALIGN : alignof(T); }

		// Construction.
		FastDynStorage()
			: m_nCapacity(0), m_bStoredAlloc(0)
		{
			MEMSTAT_REGISTER_CONTAINER(this, EMemStatContainerType::MSC_Vector, T);
		}

		~FastDynStorage()
		{
			if (m_aElems)
				AllocFree<A>( m_aElems, m_bStoredAlloc, alignment() );

			MEMSTAT_UNREGISTER_CONTAINER(this);
		}

		inline size_type capacity() const
			{ return m_nCapacity; }
		inline size_type get_alloc_size() const
			{ return A::alloc_size(m_aElems, capacity() * sizeof(T), m_bStoredAlloc, alignment()); }

		using super_type::m_aElems;
		using super_type::m_nCount;

		void resize( size_type new_size, size_type new_cap = 0 )
		{
			bool bSlack = new_cap == 0;
			new_cap = NArray::max(new_size, new_cap);
			if (bSlack && new_size > 0)
				new_cap = NArray::max(new_cap, capacity());
			if (new_cap != capacity())
			{
				bool bStoredAlloc = m_bStoredAlloc;
				size_t nNewBytes = new_cap * sizeof(T);
				m_aElems = (T*) A::alloc( m_aElems, m_nCount * sizeof(T), bStoredAlloc, nNewBytes, bSlack, alignment() );
				m_bStoredAlloc = bStoredAlloc;
				m_nCapacity = size_type(nNewBytes / sizeof(T));
				assert((int)m_nCapacity >= new_cap);				// Check for overflow or bad alloc.

				MEMSTAT_BIND_TO_CONTAINER(this, m_aElems);
				//assert(IsAligned(m_aElems, alignment()));
			}
			m_nCount = new_size;
		}

	protected:
		uint32		m_nCapacity: 31;
		uint32		m_bStoredAlloc: 1;
	};

	//---------------------------------------------------------------------------
	// SmallDynStorage: STORAGE scheme for Array<T,STORAGE> and descendents.
	// Array is just a single pointer, size and capacity information stored before the array data.
	// Slightly slower than FastDynStorage, optimal for saving space, especially when array likely to be empty.

	template<class T, class A, int nALIGN> 
	struct SmallDynStorage: NoCopy
	{
		typedef SmallDynStorage<T,A,nALIGN> self_type;

		typedef int size_type;
		typedef int difference_type;

		// Construction.
		SmallDynStorage()
		{
			set_null(); 

			MEMSTAT_REGISTER_CONTAINER(this, EMemStatContainerType::MSC_Vector, T);
		}

		~SmallDynStorage()
		{
			if (!is_null())
				AllocFree<A>( m_aElems, header()->stored_alloc(), alignment(), sizeof(Header) );

			MEMSTAT_UNREGISTER_CONTAINER(this);
		}

		void swap(self_type& a)
		{
			T* pTemp = m_aElems;
			m_aElems = a.m_aElems;
			a.m_aElems = pTemp;
			MEMSTAT_SWAP_CONTAINERS(this, &a);
		}

		// Basic storage.
		CONST_VAR_FUNCTION( T* begin(),
			{ return m_aElems; } )
		inline size_type size() const
			{ return header()->size(); }
		inline size_type capacity() const
			{ return header()->capacity(); }
		inline size_type get_alloc_size() const
			{ return is_null() ? 0 : A::alloc_size( m_aElems, capacity() * sizeof(T), header()->stored_alloc(), alignment(), sizeof(Header) ); }

		void resize( size_type new_size, size_type new_cap = 0 )
		{
			if (new_size == 0 && new_cap == 0)
			{
				// Free
				if (!is_null())
				{
					AllocFree<A>( m_aElems, header()->stored_alloc(), alignment(), sizeof(Header) );
					set_null();
				}
			}
			else
			{
				bool bSlack = new_cap == 0;
				new_cap = NArray::max(new_size, new_cap);
				if (bSlack && new_size > 0)
					new_cap = NArray::max(new_cap, capacity());
				if (new_cap != capacity())
				{
					bool bStoredAlloc = header()->stored_alloc();
					size_t nNewBytes = new_cap * sizeof(T);
					m_aElems = (T*) SPU_MAIN_PTR( A::alloc( is_null() ? 0 : m_aElems, size() * sizeof(T), 
						bStoredAlloc, nNewBytes, bSlack, alignment(), sizeof(Header) ));

					MEMSTAT_BIND_TO_CONTAINER(this, m_aElems);

					// Store actual allocated capacity.
					assert(nNewBytes >= new_cap*sizeof(T));
					header()->set_stored_alloc(bStoredAlloc);
					new_cap = size_type(nNewBytes / sizeof(T));
				}
				header()->set_sizes(new_size, new_cap);
			}
		}

	protected:

		T*				m_aElems;

		struct Header
		{
			bool is_null() const
				{ return nSize + bCapacity == 0; }
			size_type size() const
				{ return nSize; }

			size_type	capacity() const
			{ 
				if (!bCapacity)
					return nSize;
				uint8* pCap = SPU_MAIN_PTR( (uint8*)(elems() + nSize) );
				if (*pCap)
					// Stored as byte.
					return nSize + *pCap;
				else
				{
					// Stored in next aligned word.
					pCap = SPU_MAIN_PTR( Align(pCap+1, alignof(size_type)) );
					return nSize + *(size_type*)pCap;
				}
			}

			void set_sizes( size_type s, size_type c )
			{
				// Store size, and assert against overflow.
				nSize = s; assert(nSize == s); 
				if (c <= s)
				{
					bCapacity = false;
				}
				else
				{
					// Store extra capacity after elements.
					bCapacity = true;
					uint8* pCap = SPU_MAIN_PTR( (uint8*)(elems() + s) );
					size_type nExtra = c - s;
					if (nExtra < 256)
						// If it fits in a byte, store it there.
						*pCap = nExtra;
					else
					{
						// Otherwise, mark it 0, store it in next aligned word.
						*pCap++ = 0;
						pCap = SPU_MAIN_PTR( Align(pCap, alignof(size_type)) );
						*(size_type*)pCap = nExtra;
					}
				}
			}

			bool stored_alloc() const
				{ return (bool)bStoredAlloc; }
			void set_stored_alloc( bool b )
				{ bStoredAlloc = b; }

		protected:
			uint32		nSize:				30,
								bCapacity:		1,			// Bit indicates capacity > size, stored after elems.
								bStoredAlloc:	1;			// Bit indicates custom alloc pointer stored before header.
																			// Ensures compatibility across Debug/Release modules.
																			// If start allocated in debug module, actual allocation function stored as extra data.
																			// Stored debug allocator or default release allocator used in subsequent reallocs or frees.
																			// No additional data stored in release modules.

			T* elems() const				
			{
				const Header* tmp = SPU_MAIN_PTR( this+1 );
				return SPU_MAIN_PTR( (T*)tmp );
			}
		};

		static int alignment()
		{
			int nTypeAlignment = nALIGN ? nALIGN : alignof(T);
			return nTypeAlignment > alignof(Header) ? nTypeAlignment : alignof(Header);
		}

		CONST_VAR_FUNCTION( Header* header(),
		{
			assert(m_aElems);
			return ((Header*)m_aElems)-1;
		} )

		struct EmptyHeader
		{
			Header	head;
			char		pad[nALIGN ? nALIGN : alignof(T)];
		}; 

		// workaround for SPUS which can't handle function static variables:
		// use a global empty header var and assign ptr to function static
		// on spu use the global var




		void set_null()
		{
#if !defined(__SPU__)
			// m_aElems is never 0, for empty array points to a static empty header.
			// Declare a big enough static var to account for alignment.
			static EmptyHeader s_EmptyHeader;

			// The actual header pointer can be anywhere in the struct, it's all initialised to 0.
			static T* s_EmptyElems = (T*)Align(s_EmptyHeader.pad, alignment());

			m_aElems = s_EmptyElems;







#endif
		}
		inline bool is_null() const
			{ return header()->is_null(); }
	};





};

//---------------------------------------------------------------------------
namespace NAlloc
{
	//---------------------------------------------------------------------------
	// Alloc adapters, to provide alignment, prefix, and stored allocator functionality
	// to a base allocator.
	//

	template<class A>
	struct AllocPrefix
	{

		static void* alloc( void* pCur, size_t& nNewBytes, size_t nCurBytes, bool bSlack, int nAlign, int nPrefixBytes )
		{
			nPrefixBytes = Align(nPrefixBytes, nAlign);
			if (pCur)
				pCur = (char*)pCur - nPrefixBytes;
			if (nNewBytes)
				nNewBytes += nPrefixBytes;



			(void)nCurBytes; // mark as used
			void* pNew = A::alloc( pCur, nNewBytes, bSlack, nAlign );

			if (nNewBytes)
				nNewBytes -= nPrefixBytes;
			if (pNew)
				pNew = (char*)pNew + nPrefixBytes;
			return pNew;
		}
	};

	template<class A>
	struct AllocAlign
	{
		static void* alloc( void* pCur, size_t nCurBytes, size_t& nNewBytes, bool bSlack = false, int nAlign = 1, int nPrefixBytes = 0 )
		{
			assert(nAlign > 0);
			bSlack = bSlack && pCur && nNewBytes > nCurBytes;
			if (nAlign <= A::max_alignment)
				return AllocPrefix<A>::alloc( pCur, nNewBytes, nCurBytes, bSlack, nAlign, nPrefixBytes );
			
			char* pNew = NULL;
			if (nNewBytes)
			{
				nNewBytes += nPrefixBytes + nAlign + sizeof(int);
				char* pNewAlloc = (char*)A::alloc( 0, nNewBytes, bSlack );
				pNew = SPU_MAIN_PTR(Align(pNewAlloc + nPrefixBytes + sizeof(int), nAlign) - nPrefixBytes);

				// Store alignment offset.
				int nOffset = check_cast<int>(pNew - pNewAlloc);
				assert(nOffset <= nAlign);

				((int*)SPU_MAIN_PTR(pNew))[-1] = nOffset;
				nNewBytes -= nPrefixBytes + nOffset;

				if (nCurBytes)
				{
					// Copy old data.
					assert(pCur);
					memcpy( SPU_MAIN_PTR(pNew), SPU_MAIN_PTR( (char*)pCur - nPrefixBytes ), NArray::min(nCurBytes, nNewBytes) + nPrefixBytes );
				}
				SPU_MAIN_PTR(pNew += nPrefixBytes);
			}
			if (pCur)
			{
				// Free old data.
				pCur = (char*)pCur - nPrefixBytes;
				pCur = (char*)pCur - ((int*)pCur)[-1];
				A::alloc( pCur, nCurBytes = 0 );
			}
			return SPU_MAIN_PTR(pNew);
		}
	};

	// since this file is also included for compiling SPU jobs, we get double implementations
	// for all non-template classes, so an ifdef is needed
#if !defined(_SPU_JOB)

	typedef void* (*TPFAlloc)( void* pCur, size_t nCurBytes, size_t& nNewBytes, bool bSlack, int nAlign, int nPrefixBytes );

	inline size_t ReallocSize( size_t nMinSize )
	{
		// Choose an efficient realloc size, when growing an existing (non-zero) block.
		// Find the next power-of-two, minus a bit of slack for presumed system alloc overhead.
		const size_t nSlack = 16;



			const size_t nDoubleLimit = 1<<16;

		nMinSize += nSlack;
		size_t nAlloc = 32;
		while (nAlloc < nMinSize)
			nAlloc <<= 1;
		if (nAlloc > nDoubleLimit)
		{
			nAlloc = NArray::max(nAlloc>>3, nDoubleLimit);
			nAlloc = Align(nMinSize, nAlloc);
		}
		return nAlloc - nSlack;
	}

	//---------------------------------------------------------------------------
	// Allocators for DynArray.

	// ModuleAlloc: Base DynArray<> allocator, using CryModuleRealloc.
	struct ModuleAlloc
	{
		struct Standard
		{
			union align_type { void* p; double d; };		

			// Guaranteed align of stdc realloc.
			static const int max_alignment = alignof(align_type);

			static void* alloc( void* pCur, size_t& nNewBytes, bool bSlack = false, int nAlign = 1 )
			{
				if (bSlack && nNewBytes)
					nNewBytes = ReallocSize(nNewBytes);




				assert(nAlign <= max_alignment);
				return SPU_MAIN_PTR( CryModuleRealloc( SPU_MAIN_PTR(pCur), nNewBytes) );
			}













			
		};

		static void* alloc( void* pCur, size_t nCurBytes, bool& bStoredAlloc, size_t& nNewBytes, bool bSlack = false, int nAlign = 1, int nPrefixBytes = 0 )
		{
			assert(!bStoredAlloc);
			void* pNew = AllocAlign<Standard>::alloc(pCur, nCurBytes, nNewBytes, bSlack, nAlign, nPrefixBytes);
			//assert(IsAligned(pNew, nAlign));
			return pNew;
		}

		static size_t alloc_size( void* pCur, size_t nCurBytes, bool bStoredAlloc, int nAlign = 1, int nPrefixBytes = 0 )
		{
			if (!pCur)
				return 0;

			nCurBytes = Align(nCurBytes, nAlign);
			if (bStoredAlloc)
				nPrefixBytes += sizeof(TPFAlloc);
			if (nAlign > Standard::max_alignment)
			{
				pCur = (char*)pCur - nPrefixBytes;
				nPrefixBytes += ((int*)pCur)[-1];
			}
			else
				nPrefixBytes = Align(nPrefixBytes, nAlign);
			return nCurBytes + nPrefixBytes;
		}
	};

	// CrossModuleAlloc: Default DynArray<> allocator, uses stdlib realloc in debug modules.
	// Safe for use across modules, as all non-default/Profile allocations store the alloc function pointer.

	struct CrossModuleAlloc: ModuleAlloc
	{
#if !defined(PS3)

		#ifdef _DEBUG
			struct Debug: Standard
			{
				static void* alloc( void* pCur, size_t& nNewBytes, bool bSlack = false, int nAlign = 1 )
				{
					assert(nAlign <= max_alignment);
					if (bSlack && nNewBytes)
						nNewBytes = ReallocSize(nNewBytes);
					if (!nNewBytes)
					{
						free(pCur);
						return NULL;
					}
					return realloc(pCur, nNewBytes);
				}
			};
		#endif

		static void* alloc( void* pCur, size_t nCurBytes, bool& bStoredAlloc, size_t& nNewBytes, bool bSlack = false, int nAlign = 1, int nPrefixBytes = 0 )
		{
			TPFAlloc pAlloc = AllocAlign<Standard>::alloc;
			if (!pCur)
			{
				// Default alloc when none stored in block.
				#ifdef _DEBUG
					pAlloc = AllocAlign<Debug>::alloc;
					bStoredAlloc = true;
				#else
					bStoredAlloc = false;
				#endif
			}

			if (bStoredAlloc)
			{
				// Extract and store allocator before mem.
				nPrefixBytes += sizeof(TPFAlloc);
				if (pCur)
					pAlloc = *(TPFAlloc*)((char*)pCur - nPrefixBytes);
			}

			void* pNew = (*pAlloc)(pCur, nCurBytes, nNewBytes, bSlack, nAlign, nPrefixBytes);
			if (bStoredAlloc && pNew)
				*(TPFAlloc*)((char*)pNew - nPrefixBytes) = pAlloc;

			assert(IsAligned(pNew, nAlign));
			return pNew;
		}
#endif
	};

#endif // !_SPU_JOB
};

#if !defined(_SPU_JOB)
template<class T, int nALIGN = 0>
struct FastDynArray: DynArray<T, NArray::FastDynStorage<T, NAlloc::ModuleAlloc, nALIGN> >
{
};

#endif // !_SPU_JOB

//---------------------------------------------------------------------------
// Convenient array iteration macros

#define for_array(i, arr)									for (int i = 0; i < arr.size(); i++)
#define for_all(cont)											for_array (_i, cont) cont[_i]

#if !defined(__SPU__) || defined(__CRYCG__)
	#include "CryPodArray.h"
#endif

#endif
