////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2005.
// -------------------------------------------------------------------------
//  File name:   IFacialAnimation.h
//  Version:     v1.00
//  Created:     7/10/2005 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __FinalizingSpline_h__
#define __FinalizingSpline_h__

#include "ISplines.h"
#include "CryCustomTypes.h"

namespace spline
{
	//////////////////////////////////////////////////////////////////////////
	// FinalizingSpline
	//////////////////////////////////////////////////////////////////////////

	template <class Source>
	class	FinalizingSpline
	{
	public:
		using_type(Source, value_type);
		using_type(Source, key_type);

		// Adaptors for base spline functions.
		int num_keys() const
			{ return m_pSource->num_keys(); }
		void resize( int num )
			{ m_pSource->resize(num); }
		int insert_key( float t, const value_type& val )
			{ return m_pSource->insert_key(t, val); }
		void erase( int key )
			{ m_pSource->erase(key); }
		void find_keys_in_range(float startTime, float endTime, int& firstFoundKey, int& numFoundKeys) const
			{ m_pSource->find_keys_in_range(startTime, endTime, firstFoundKey, numFoundKeys); }
		void remove_keys_in_range(float startTime, float endTime)
			{ m_pSource->remove_keys_in_range(startTime, endTime); }
		key_type&	key( int n )
			{ return m_pSource->key(n); }
		const key_type& key( int n ) const
			{ return m_pSource->key(n); }
		void SetModified( bool bOn, bool bSort = false )
			{ m_pSource->SetModified(bOn, bSort ); }

		bool is_updated() const
		{
			return !m_pSource || m_pSource->is_updated();
		}

		void finalize()
		{
			// Optimise by removing source spline data.
			// Any further spline updates will crash 
			// (we could assert for non-null pointer, but crashing has the same effect).
			m_pSource = 0;
		}
		bool is_finalized() const
		{
			return !m_pSource;
		}

		void GetMemoryUsage(ICrySizer* pSizer, bool bSelf = false) const
		{
			if (bSelf && !pSizer->AddObjectSize(this))
				return;
			pSizer->AddObject(m_pSource);
		}

	protected:

		deep_ptr<Source,true>		m_pSource;
	};

	//////////////////////////////////////////////////////////////////////////
	// LookupTableSpline
	//////////////////////////////////////////////////////////////////////////
	
	template <class S, class value_type, const int nMAX_ENTRIES>
	class	LookupTableSplineInterpolater
	{
	public:
		ILINE static void fast_interpolate( float t, value_type& val, S *m_table)
		{
			t *= nMAX_ENTRIES - 1.0f;
			float frac = t - floorf(t);
			int idx = int(t);
			val = value_type(m_table[idx]) * (1.f - frac);
			val += value_type(m_table[idx + 1]) * frac;
		}
	};
	
#ifndef __SPU__
	template <class value_type, const int nMAX_ENTRIES>
	class	LookupTableSplineInterpolater <UnitFloat8, value_type, nMAX_ENTRIES>
	{
		enum { SHIFT_AMOUNT = 24 };

	public:
		ILINE static void fast_interpolate( float t, value_type& val, UnitFloat8 *m_table )
		{
			const float scale = (float)(1 << SHIFT_AMOUNT);
			uint32 ti = uint32(t * scale * (nMAX_ENTRIES - 1.0f));
			uint32 idx = ti >> SHIFT_AMOUNT;
			uint32 frac = ti - (idx << SHIFT_AMOUNT);
			uint32 vali = (uint32)m_table[idx + 1].GetStore() * frac;
			frac = (1 << SHIFT_AMOUNT) - frac;
			vali += (uint32)m_table[idx].GetStore() * frac;
			val = (value_type)vali * (1.0f / (255.0f * scale));
		}
	};
#endif

	template <class S, class Source>
	class	LookupTableSpline: public FinalizingSpline<Source>
	{
		typedef FinalizingSpline<Source> super_type;
		using super_type::m_pSource;
		using super_type::is_updated;

		enum { nSTORE_SIZE = 128 };
		enum { nMAX_ENTRIES = nSTORE_SIZE - 1 };
		enum { nMIN_VALUE = nMAX_ENTRIES };

	public:

		using_type(super_type, value_type);

		LookupTableSpline()
		{ 
			init(); 
		}

		LookupTableSpline( const LookupTableSpline& other )
			: super_type(other)
		{
			init();
			update();
		}
		void operator=( const LookupTableSpline& other )
		{
			super_type::operator=(other);
			update();
		}

		~LookupTableSpline()
		{
			delete[] m_table;
		}

		void interpolate( float t, value_type& val )
		{
			if (!is_updated())
				update();
			fast_interpolate( clamp(t, 0.0f, 1.0f), val );
		}

		SPU_NO_INLINE void fast_interpolate( float t, value_type& val ) const
		{
			LookupTableSplineInterpolater<S, value_type, nMAX_ENTRIES>::fast_interpolate(t, val, m_table);
		}

		ILINE void min_value( value_type& val ) const
		{
			val = value_type(m_table[nMIN_VALUE]);
		}

		void finalize()
		{
			if (!is_updated())
				update();
			super_type::finalize();
		}

		void GetMemoryUsage(ICrySizer* pSizer, bool bSelf = false) const
		{
			if (bSelf && !pSizer->AddObjectSize(this))
				return;
			super_type::GetMemoryUsage(pSizer);
			if (m_table)
				pSizer->AddObject(m_table, nSTORE_SIZE * sizeof(S));
		}

	protected:

		void update()
		{
			value_type minVal(1.0f);
			if (!m_table)
			{
				m_table = new S[nSTORE_SIZE];
			}
			if (!m_pSource || m_pSource->empty())
			{
				for (int i=0; i<nMAX_ENTRIES; i++)
				{
					m_table[i] = value_type(1.f);
				}
			}
			else
			{
				m_pSource->update();
				for (int i=0; i<nMAX_ENTRIES; i++)
				{
					value_type val;
					float t = float(i) * (1.0f/(float)(nMAX_ENTRIES-1));
					m_pSource->interpolate(t, val);
					minVal = min(minVal, val);
					m_table[i] = val;
				}
			}
			m_table[nMIN_VALUE] = minVal;
		}

		void init()
		{
			m_table = NULL;
		}

		bool is_updated() const
		{
			return super_type::is_updated() && m_table;
		}

		S*			m_table;
	};

	//////////////////////////////////////////////////////////////////////////
	// OptSpline
	// Minimises memory for key-based storage. Uses 8-bit compressed key values.
	//////////////////////////////////////////////////////////////////////////

	/*
		Choose basis vars t, u = 1-t, ttu, uut.
		This produces exact values at t = 0 and 1, even with compressed coefficients.
		For end points and slopes v0, v1, s0, s1,
		solve for coefficients a, b, c, d:

			v(t) = a u + b t + c uut + d utt
			s(t) = v'(t) = -a + b + c (1-4t+3t²) + d (2t-3t²)

			v(0) = a
			v(1) = b
			s(0) = -a + b + c
			s(1) = -a + b - d

		So

			a = v0
			b = v1
			c = s0 + v0 - v1
			d = -s1 - v0 + v1

		For compression, all values of v and t are limited to [0..1].
		Find the max possible slope values, such that values never exceed this range.

		If v0 = v1 = 0, then max slopes would have 
		
			c = d
			v(1/2) = 1
			c/8 + d/8 = 1
			c = 4

		If v0 = 0 and v1 = 1, then max slopes would have

			c = d
			v(1/3) = 1
			1/3 + c 4/9 + d 2/9 = 1
			c = 1
	*/

	template<class T, class Source>
	class	OptSpline: public FinalizingSpline<Source>
	{
		typedef OptSpline<T, Source> self_type;
		typedef FinalizingSpline<Source> super_type;

	public:
		using_type(super_type, value_type);
		using_type(super_type, key_type);

	protected:

		using super_type::m_pSource;

		static const int DIM = sizeof(value_type) / sizeof(float);

		template<class S>
		struct array
		{
			S	elems[DIM];

			ILINE S& operator [](int i)
			{
				assert(i >= 0 && i < DIM);
				return elems[i];
			}
			ILINE const S& operator [](int i) const
			{
				assert(i >= 0 && i < DIM);
				return elems[i];
			}
		};

		template<class S>
		static ILINE float* farr(S* f)
		{
			return reinterpret_cast<float*>(f);
		}

		template<class S>
		static ILINE const float* farr( const S* f)
		{
			return reinterpret_cast<const float*>(f);
		}

		typedef uint8 TStore;
		typedef array<uint8> VStore;
		typedef array<int8> SStore;

		static ILINE float FromStorage( TStore store )
		{
			return ufrac8_to_float(store);
		}
		static ILINE TStore ToStorage( float val )
		{
			return float_to_ufrac8(val);
		}
		static ILINE void FromStorage( value_type& val, const VStore& store )
		{
			float *pF = farr(&val);
			for (int i = 0; i < DIM; i++)
				pF[i] = ufrac8_to_float(store[i]);
		}
		static ILINE void ToStorage( VStore& store, const value_type& val )
		{
			const float *pF = farr(&val);
			for (int i = 0; i < DIM; i++)
				store[i] = float_to_ufrac8( pF[i] );
		}

		struct OptElem
		{
			TStore	st1;				// End time of this segment.
			VStore	sv1;				// End value of this segment.
			SStore	svc, svd;		// Coefficients for 4 uut and 4 utt.

			ILINE void eval(value_type& val, const VStore& sv0, float t) const
			{
				float u = 1.f - t,
							tu4v = t*u * (4.f / 128.f);

				float *pF = farr(&val);
				for (int i = 0; i < DIM; i++)
				{
					float elem = (float(sv0[i]) * u + float(sv1[i]) * t) * (1.f / 255.f);
					elem += (float(svc[i]) * u + float(svd[i]) * t) * tu4v;
					pF[i] = elem;
				}
			}

			// Compute coeffs based on 2 endpoints & slopes.
			void set( float t0, value_type v0, value_type s0, float t1, value_type v1, value_type s1 )
			{
				assert(t0 >= 0.f && t1 > t0 && t1 <= 1.f);

				st1 = ToStorage(t1);

				for (int i = 0; i < DIM; i++)
				{
					float v0i = farr(&v0)[i], 
								v1i = farr(&v1)[i],
								s0i = farr(&s0)[i],
								s1i = farr(&s1)[i];

					// Store values.
					sv1[i] = float_to_ufrac8(v1i);
					svc[i] = float_to_ifrac8(( s0i + v0i - v1i) / 4.f);
					svd[i] = float_to_ifrac8((-s1i - v0i + v1i) / 4.f);
				}
			}
		};

	public:

		~OptSpline()
		{ 
			delete[] m_elems; 
		}
		OptSpline()
		{ 
			init(); 
		}
		OptSpline( const self_type& in )
			: super_type(in)
		{ 
			init();
			update();
		}
		self_type& operator=( const self_type& in )
		{
			super_type::operator=(in);
			init();
			update();
			return *this;
		}

		void interpolate( float t, value_type& val )
		{
			update();
			fast_interpolate( t, val );
		}

		// Overrides.

		bool is_updated() const
		{
			if (m_pSource && !m_pSource->is_updated())
				return false;
			return !is_init();
		}

		void finalize()
		{
			update();
			super_type::finalize();
		}

		void GetMemoryUsage(ICrySizer* pSizer, bool bSelf = false) const
		{
			if (bSelf && !pSizer->AddObjectSize(this))
				return;
			super_type::GetMemoryUsage(pSizer);
			pSizer->AddObject(m_elems, m_elemcount * sizeof(OptElem));
		}

		// Additional methods.
		SPU_NO_INLINE void fast_interpolate( float t, value_type& val ) const
		{
			assert(is_updated());

			float prev_t = FromStorage(m_time0);
			if (t <= prev_t)
				FromStorage(val, m_value0);
			else
			{
				// Find spline segment.
				VStore prev_val = m_value0;
				for (range_iter<const OptElem> it(m_elems, m_elems+m_elemcount); it; ++it )
				{
					float cur_t = FromStorage(it->st1);
					if (t <= cur_t)
					{
						// Eval
						it->eval( val, prev_val, (t - prev_t) / (cur_t - prev_t) );
						return;
					}
					prev_t = cur_t;
					prev_val = it->sv1;
				}

				// Last point value.
				FromStorage(val, prev_val);
			}
		}

		SPU_NO_INLINE void min_value( value_type& val ) const
		{
			non_const(*this).update();
			VStore sval = m_value0;
			for (range_iter<const OptElem> it(m_elems, m_elems+m_elemcount); it; ++it )
				for (int i = 0; i < DIM; i++)
					sval[i] = min(sval[i], it->sv1[i]);
			FromStorage(val, sval);
		}

		SPU_NO_INLINE void max_value( value_type& val ) const
		{
			non_const(*this).update();
			VStore sval = m_value0;
			for (range_iter<const OptElem> it(m_elems, m_elems+m_elemcount); it; ++it )
				for (int i = 0; i < DIM; i++)
					sval[i] = max(sval[i], it->sv1[i]);
			FromStorage(val, sval);
		}

	protected:

		void init()
		{
			m_elems = 0;
			m_elemcount = 0;
			m_time0 = 0;
			ToStorage(m_value0, value_type(1.f));
		}

		bool is_init() const
		{
			return m_elemcount == 0 && m_time0 == 0;
		}

		void update()
		{
#if !defined(__SPU__) // on SPU we never will call update
			if (is_updated())
				return;

			if (!m_pSource)
				return;

			m_pSource->update();

			delete[] m_elems;
			bool has_data = false;

			int num_keys = m_pSource->num_keys();
			ToStorage(m_value0, num_keys > 0 ? m_pSource->key(0).value : value_type(1.f));
			if (num_keys > 1)
			{
				m_time0 = ToStorage(m_pSource->key(0).time);
				m_elemcount = check_cast<uint8>(num_keys-1);
				m_elems = new OptElem[m_elemcount];
				for (int i = 0; i < m_elemcount; i++)
				{
					// Coefficients for each key.
					const key_type& key = m_pSource->key(i);
					const key_type& next_key = m_pSource->key(i+1);
					if (key.value != next_key.value)
						has_data = true;
					m_elems[i].set( key.time, key.value, key.dd, next_key.time, next_key.value, next_key.ds );
				}
				if (!has_data)
					delete[] m_elems;
			}
			if (!has_data)
			{
				m_elems = 0;
				m_elemcount = 0;
				m_time0 = ToStorage(1.f);
			}
#endif
		}

		OptElem*			m_elems;			// It hurts to use manual array management rather than DynArray, but saves a few bytes.
		uint8					m_elemcount;	// Will combine with m_value0 for TStore types we use.
		TStore				m_time0;
		VStore				m_value0;
	};
};

#endif // __FinalizingSpline_h__
