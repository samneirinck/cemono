////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2009.
// -------------------------------------------------------------------------
//  File name:   RangeIter.h
//  Version:     v1.00
//  Created:     2009-05 by Scott.
//  Description: range_iter<>: Range-based iterator, faster and more elegant than std iterators.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////


#ifndef _RANGE_ITER_H_
#define _RANGE_ITER_H_
#pragma once

/* Iteration examples

		// Array-based

			Array<Type> Array; ...
			for (range_iter<Type> p(Array); p; ++p)
				Process(*p, p->Parent());

			// Better than:
			for (Type* p = Array.begin(); p != Array.end(); ++p)
				Process(*p, p->Parent());

			for (int i = 0; i < Array.size(); ++i)
				Process(Array[i], Array[i].Parent());

		// Reverse

			for (range_iter<Type> p(Array); p; --p)
				Process(*~p, (~p)->Parent());

		// General container

			std::list<Type> List; ...
			for (container_iter< std::list<Type> > it(List); it; ++it)
				Process(*it, it->Parent());

			// Better than:
			for (std::list<Type>::iterator it = List.begin(); it != List.end(); ++it)
				Process(*it, it->Parent());
*/

template<class T, class IT = T*> 
struct range_iter
{
	// Set iterator range
	range_iter( IT start, IT finish )
		: m_start(start), m_finish(finish)
	{}

	// Set to entire container.
	template<class CONT>
	range_iter( CONT& cont )
		: m_start(cont.begin()), m_finish(cont.end())
	{}

	// Access
	IT begin() const
		{ return m_start; }
	IT end() const
		{ return m_finish; }

	// Iteration operators.
	operator bool() const
		{ return m_start != m_finish; }
	IT& operator++()
		{ return ++m_start; }
	IT operator++(int)
		{ return m_start++; }

	operator IT() const
		{ return m_start; }
	IT operator ->() const
		{ return m_start; }
	T& operator*() const
		{ return *m_start; }

	template<class CONT>
	void erase( CONT& cont )
	{
		m_start = cont.erase(m_start);
		m_finish = cont.end();
	}

	// Reverse iteration variations.
	IT& operator--()
		{ return --m_finish; }
	IT operator--(int)
		{ return m_finish--; }

	IT operator ~() const
	{
		IT last = m_finish;
		return --last;
	}

	template<class CONT>
	void rev_erase( CONT& cont )
	{
		m_finish = cont.erase(--m_finish);
		--m_finish;
		m_start = cont.begin();
	}

protected:
	IT		m_start, m_finish;
};

#define for_range(Type, r, args)	for (range_iter<Type> r args; r; ++r)

//
// Type shortcut for containers with iterators.
// [A template typedef would take only 1 line, but C++ sucks]

template<class C> 
struct container_iter
: range_iter<typename C::value_type, typename C::iterator>
{
	typedef typename C::value_type	T;
	typedef typename C::iterator		IT;

	container_iter( IT start, IT finish )
		: range_iter<T, IT>(start, finish)
	{}

	container_iter( C& cont )
		: range_iter<T, IT>(cont)
	{}
};

#define for_container(Type, r, cont)	for (container_iter<Type> r(cont); r; ++r)

#endif
