
//////////////////////////////////////////////////////////////////////
//
//	Crytek Common Source code
//
//	File:Cry_Math.h
//	Description: Misc mathematical functions
//
//	History:
//	-Jan 31,2001: Created by Marco Corbetta
//	-Jan 04,2003: SSE and 3DNow optimizations by Andrey Honich
//
//////////////////////////////////////////////////////////////////////

//
#ifndef CRY_SIMD_H
#define CRY_SIMD_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <platform.h>







#if defined(LINUX64)
	#define PrefetchLine(ptr, off) cryPrefetchT0SSE((void*)((UINT_PTR)ptr + off))
	#define ResetLine128(ptr, off) (void)(0)
#else
	#define	PrefetchLine(ptr, off) cryPrefetchT0SSE((void*)((unsigned int)ptr + off))
	#define ResetLine128(ptr, off) (void)(0)
#endif

//========================================================================================

// cryMemcpy flags
#define MC_CPU_TO_GPU 0x10
#define MC_GPU_TO_CPU 0x20
#define MC_CPU_TO_CPU 0x40

#ifdef CRYANIMATION_EXPORTS
	#define CRYANIMATION_API	DLL_EXPORT
#else
  #ifndef CRYANIMATION_API
		#define CRYANIMATION_API	DLL_IMPORT
	#endif
#endif

//#ifndef _XBOX
 extern int g_CpuFlags;
//#else
 //extern CRYANIMATION_API int g_CpuFlags;
//#endif

//
#define CPUF_SSE   1
#define CPUF_SSE2  2
#define CPUF_3DNOW 4
#define CPUF_MMX   8

#ifdef _CPU_SSE

#ifdef _CPU_X86
#include <xmmintrin.h>
#endif

#define _MM_PREFETCH( MemPtr,Hint ) _mm_prefetch( (MemPtr),(Hint) );
#define _MM_PREFETCH_LOOP( nCount,MemPtr,Hint ) { for (int p = 0; p < nCount; p+=64) { _mm_prefetch( (const char*)(MemPtr)+p,Hint); } }
#else //_CPU_SSE
#define _MM_PREFETCH( MemPtr,Hint )
#define _MM_PREFETCH_LOOP( nCount,MemPtr,Hint )
#endif //_CPU_SSE


































	void cryMemcpy( void* Dst, const void* Src, int Count );


#if defined(XENON) || (defined(PS3) && !defined(__SPU__) && defined(PS3OPT))

	ILINE void cryVecMemcpyLoop4(void * __restrict dest, const void * __restrict src, int size)
	{
		//assumes src and dest are 16 byte aligned, size is multiple of 64 and src and dest do not overlap
		assert(	!((unsigned int)dest < (unsigned int)src && (unsigned int)dest + size > (unsigned int)src) &&
						!((unsigned int)dest > (unsigned int)src && (unsigned int)src + size > (unsigned int)dest) &&
						( ((unsigned int)dest | (unsigned int)src) & 15) == 0 && (size & 63) == 0);

		for(int i=0; i<size; i+=64)
		{
			__stvx(__lvlx(src, i), dest, i);
			__stvx(__lvlx(src, i+16), dest, i+16);
			__stvx(__lvlx(src, i+32), dest, i+32);
			__stvx(__lvlx(src, i+48), dest, i+48);
		}
	}

	template<int TSIZE>
	void cryVecMemcpy(void * __restrict dest, const void * __restrict src)
	{
		COMPILE_TIME_ASSERT((TSIZE&0xf)==0);
		COMPILE_TIME_ASSERT(TSIZE>0);
		
		assert(	!((unsigned int)dest < (unsigned int)src && (unsigned int)dest + TSIZE > (unsigned int)src) &&
						!((unsigned int)dest > (unsigned int)src && (unsigned int)src + TSIZE > (unsigned int)dest) );

		if(TSIZE>128)
		{
			//copy in blocks of 64 bytes
			cryVecMemcpyLoop4(dest,src,TSIZE & ~0x3f);

			//calc remaining copies and start index
			int rem = TSIZE & 0x3f;
			int index = TSIZE & ~0x3f;

			if ( rem >= 16 ) __stvx(__lvlx(src, index), dest, index);
			if ( rem >= 32 ) __stvx(__lvlx(src, index+16), dest, index+16);
			if ( rem >= 48 ) __stvx(__lvlx(src, index+32), dest, index+32);		
		}
		else
		{
			__stvx(__lvlx(src, 0), dest, 0);

			if(TSIZE>16) __stvx(__lvlx(src, 16), dest, 16);
			if(TSIZE>32) __stvx(__lvlx(src, 32), dest, 32);
			if(TSIZE>48) __stvx(__lvlx(src, 48), dest, 48);
			if(TSIZE>64) __stvx(__lvlx(src, 64), dest, 64);
			if(TSIZE>80) __stvx(__lvlx(src, 80), dest, 80);
			if(TSIZE>96) __stvx(__lvlx(src, 96), dest, 96);
			if(TSIZE>112) __stvx(__lvlx(src, 112), dest, 112);
		}
	}

#endif

inline float AngleMod(float a)
{
  a = (float)((360.0/65536) * ((int)(a*(65536/360.0)) & 65535));
  return a;
}

inline unsigned short Degr2Word(float f)
{
  return (unsigned short)(AngleMod(f)/360.0f*65536.0f);
}
inline float Word2Degr(unsigned short s)
{
  return (float)s / 65536.0f * 360.0f;
}






/*****************************************************
MISC FUNCTIONS
*****************************************************/


//////////////////////////////////////
#if defined(_CPU_X86)
ILINE float __fastcall Ffabs(float f) {
	*((unsigned *) & f) &= ~0x80000000;
	return (f);
}
#else
inline float Ffabs(float x) { return fabsf(x); }
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(_CPU_X86)
inline int fastftol_positive(float f)
{
  int i;

  f -= 0.5f;
#if defined(_MSC_VER)
  __asm fld [f]
  __asm fistp [i]
#elif defined(__GNUC__)
  __asm__ ("fld %[f]\n fistpl %[i]" : [i] "+m" (i) : [f] "m" (f));
#else
#error
#endif
  return i;
}
#else
inline int fastftol_positive (float f)
{
  assert(f >= 0.f);
  return (int)floorf(f);
}
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(_CPU_X86)
inline int fastround_positive(float f)
{
	int i;
  assert(f >= 0.f);
#if defined(_MSC_VER)
	__asm fld [f]
	__asm fistp [i]
#elif defined(__GNUC__)
	__asm__ ("fld %[f]\n fistpl %[i]" : [i] "+m" (i) : [f] "m" (f));
#else
#error
#endif
	return i;
}
#else
inline int fastround_positive(float f)
{
  assert(f >= 0.f);
	return (int) (f+0.5f);
}
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(_CPU_X86)
ILINE int __fastcall FtoI(float  x)
{
  int	   t;
#if defined(_MSC_VER)
  __asm
  {
    fld   x
    fistp t
  }
#elif defined(__GNUC__)
	__asm__ ("fld %[x]\n fistpl %[t]" : [t] "+m" (t) : [x] "m" (x));
#else
#error
#endif
  return t;
}
#else
inline int FtoI(float x) { return (int)x; }
#endif



//////////////////////////////////////
#define rnd()	(((float)cry_rand())/RAND_MAX)  // Floating point random number generator ( 0 -> 1)

//////////////////////////////////////
inline void multMatrices(double dst[16], const double a[16], const double b[16])
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      dst[i * 4 + j] =
        b[i * 4 + 0] * a[0 * 4 + j] +
        b[i * 4 + 1] * a[1 * 4 + j] +
        b[i * 4 + 2] * a[2 * 4 + j] +
        b[i * 4 + 3] * a[3 * 4 + j];
    }
  }
}

//////////////////////////////////////
inline void multMatrices(float dst[16], const float a[16], const float b[16])
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      dst[i * 4 + j] =
        b[i * 4 + 0] * a[0 * 4 + j] +
        b[i * 4 + 1] * a[1 * 4 + j] +
        b[i * 4 + 2] * a[2 * 4 + j] +
        b[i * 4 + 3] * a[3 * 4 + j];
    }
  }
}

//////////////////////////////////////
// transform vector
inline void matmult_trans_only(float a[3], float b[4][4], float result[3])
{
  result[0] = a[0] * b[0][0] + a[1] * b[1][0] + a[2] * b[2][0] + b[3][0];
  result[1] = a[0] * b[0][1] + a[1] * b[1][1] + a[2] * b[2][1] + b[3][1];
  result[2] = a[0] * b[0][2] + a[1] * b[1][2] + a[2] * b[2][2] + b[3][2];
}


inline void multMatrixf_Transp2(float *product, const float *m1, const float *m2)
{
  float temp[16];

#define A(row,col)  m1[(col<<2)+row]
#define B(row,col)  m2[(col<<2)+row]
#define P(row,col)  temp[(col<<2)+row]

  int i;
  for (i=0; i<4; i++)
  {
    float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
    P(i,0) = ai0 * B(0,0) + ai1 * B(0,1) + ai2 * B(0,2);
    P(i,1) = ai0 * B(1,0) + ai1 * B(1,1) + ai2 * B(1,2);
    P(i,2) = ai0 * B(2,0) + ai1 * B(2,1) + ai2 * B(2,2);
    P(i,3) = ai0 * B(3,0) + ai1 * B(3,1) + ai2 * B(3,2) + ai3;
  }

  cryMemcpy(product, temp, sizeof(temp));

#undef A
#undef B
#undef P
}

//==========================================================================================
// 3DNow! optimizations

#pragma warning(push)
#pragma warning(disable:4731) // frame pointer register 'ebp' modified by inline assembly code


/*!
Compute inverse of 4x4 transformation SINGLE-PRECISION matrix.
Code lifted from Brian Paul's Mesa freeware OpenGL implementation.
Return true for success, false for failure (singular matrix)
*/
inline bool QQinvertMatrixf(float *out, const float *m)
{
#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
		r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
		r0[4] = 1.0f, r0[5] = r0[6] = r0[7] = 0.0f,

		r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
		r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
		r1[5] = 1.0f, r1[4] = r1[6] = r1[7] = 0.0f,

		r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
		r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
		r2[6] = 1.0f, r2[4] = r2[5] = r2[7] = 0.0f,

		r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
		r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
		r3[7] = 1.0f, r3[4] = r3[5] = r3[6] = 0.0f;

	/* choose pivot - or die */
	if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
	if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
	if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
	if (0.0 == r0[0])
	{
		return 0;
	}

	/* eliminate first variable     */
	m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5];
	if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6];
	if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7];
	if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
	if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
	if (0.0 == r1[1])
	{
		return 0;
	}

	/* eliminate second variable */
	m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
	if (0.0 == r2[2])
	{
		return 0;
	}

	/* eliminate third variable */
	m3 = r3[2]/r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
		r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
		r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0 == r3[3])
	{
		return 0;
	}

	s = 1.0f/r3[3];              /* now back substitute row 3 */
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];                 /* now back substitute row 2 */
	s  = 1.0f/r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];                 /* now back substitute row 1 */
	s  = 1.0f/r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];                 /* now back substitute row 0 */
	s  = 1.0f/r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
		MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
		MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
		MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
		MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
		MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
		MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
		MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7];

	return 1;

#undef MAT
#undef SWAP_ROWS
}

#if defined _CPU_X86 && !defined(LINUX)
// ***************************************************************************
inline void cryPrecacheSSE(const void *src, int nbytes)
{
  _asm
  {
    mov esi, src
    mov ecx, nbytes
    // 64 bytes per pass
    shr ecx, 6
    jz endLabel

loopMemToL1:
    prefetchnta 64[ESI] // Prefetch next loop, non-temporal
    prefetchnta 96[ESI]

    movq mm1,  0[ESI] // Read in source data
    movq mm2,  8[ESI]
    movq mm3, 16[ESI]
    movq mm4, 24[ESI]
    movq mm5, 32[ESI]
    movq mm6, 40[ESI]
    movq mm7, 48[ESI]
    movq mm0, 56[ESI]

    add esi, 64
    dec ecx
    jnz loopMemToL1

    emms

endLabel:
  }
}
// ***************************************************************************
inline void cryPrecacheMMX(const void *src, int nbytes)
{
  _asm
  {
    mov esi, src
    mov ecx, nbytes
    // 64 bytes per pass
    shr ecx, 6
    jz endLabel

loopMemToL1:
    movq mm1,  0[ESI] // Read in source data
    movq mm2,  8[ESI]
    movq mm3, 16[ESI]
    movq mm4, 24[ESI]
    movq mm5, 32[ESI]
    movq mm6, 40[ESI]
    movq mm7, 48[ESI]
    movq mm0, 56[ESI]

    add esi, 64
    dec ecx
    jnz loopMemToL1

    emms

endLabel:
  }
}


#endif

inline void cryPrefetchNTSSE(const void *src)
{
#if defined(WIN32) && !defined(WIN64) && !defined(LINUX)
	_asm
  {
    mov esi, src
    prefetchnta [ESI] // Prefetch non-temporal
  }
#endif
}



ILINE void cryPrefetchT0SSE(const void *src)
{
#if defined(WIN32) && !defined(WIN64)
  _asm
  {
    mov esi, src
    prefetchT0 [ESI] // Prefetch
  }
#else
	_MM_PREFETCH( (char*)src, _MM_HINT_T0 );
#endif
}

//=================================================================================

// Very optimized memcpy() routine for AMD Athlon and Duron family.
// This code uses any of FOUR different basic copy methods, depending
// on the transfer size.
// NOTE:  Since this code uses MOVNTQ (also known as "Non-Temporal MOV" or
// "Streaming Store"), and also uses the software prefetch instructions,
// be sure you're running on Athlon/Duron or other recent CPU before calling!

#define TINY_BLOCK_COPY 64       // Upper limit for movsd type copy.
// The smallest copy uses the X86 "movsd" instruction, in an optimized
// form which is an "unrolled loop".

#define IN_CACHE_COPY 64 * 1024  // Upper limit for movq/movq copy w/SW prefetch.
// Next is a copy that uses the MMX registers to copy 8 bytes at a time,
// also using the "unrolled loop" optimization.   This code uses
// the software prefetch instruction to get the data into the cache.

#define UNCACHED_COPY 197 * 1024 // Upper limit for movq/movntq w/SW prefetch.
// For larger blocks, which will spill beyond the cache, it's faster to
// use the Streaming Store instruction MOVNTQ.   This write instruction
// bypasses the cache and writes straight to main memory.  This code also
// uses the software prefetch instruction to pre-read the data.
// USE 64 * 1024 FOR THIS VALUE IF YOU'RE ALWAYS FILLING A "CLEAN CACHE".

#define BLOCK_PREFETCH_COPY  infinity // No limit for movq/movntq w/block prefetch.
#define CACHEBLOCK 80h // Number of 64-byte blocks (cache lines) for block prefetch.
// For the largest size blocks, a special technique called Block Prefetch
// can be used to accelerate the read operations.   Block Prefetch reads
// one address per cache line, for a series of cache lines, in a short loop.
// This is faster than using software prefetch.  The technique is great for
// getting maximum read bandwidth, especially in DDR memory systems.


#if defined _CPU_X86 && !defined(LINUX)
// Inline assembly syntax for use with Visual C++
inline void cryMemcpy( void* Dst, const void* Src, int Count )
{
  if (g_CpuFlags & CPUF_SSE)
  {
	  __asm
	  {
		  mov		ecx, [Count]	; number of bytes to copy
		  mov		edi, [Dst]		; destination
		  mov		esi, [Src]		; source
		  mov		ebx, ecx		; keep a copy of count

		  cld
		  cmp		ecx, TINY_BLOCK_COPY
		  jb		$memcpy_ic_3	; tiny? skip mmx copy

		  cmp		ecx, 32*1024		; dont align between 32k-64k because
		  jbe		$memcpy_do_align	;  it appears to be slower
		  cmp		ecx, 64*1024
		  jbe		$memcpy_align_done
	  $memcpy_do_align:
		  mov		ecx, 8			; a trick thats faster than rep movsb...
		  sub		ecx, edi		; align destination to qword
		  and		ecx, 111b		; get the low bits
		  sub		ebx, ecx		; update copy count
		  neg		ecx				; set up to jump into the array
		  add		ecx, offset $memcpy_align_done
		  jmp		ecx				; jump to array of movsbs

	  align 4
		  movsb
		  movsb
		  movsb
		  movsb
		  movsb
		  movsb
		  movsb
		  movsb

	  $memcpy_align_done:			; destination is dword aligned
		  mov		ecx, ebx		; number of bytes left to copy
		  shr		ecx, 6			; get 64-byte block count
		  jz		$memcpy_ic_2	; finish the last few bytes

		  cmp		ecx, IN_CACHE_COPY/64	; too big 4 cache? use uncached copy
		  jae		$memcpy_uc_test

	  // This is small block copy that uses the MMX registers to copy 8 bytes
	  // at a time.  It uses the "unrolled loop" optimization, and also uses
	  // the software prefetch instruction to get the data into the cache.
	  align 16
	  $memcpy_ic_1:			; 64-byte block copies, in-cache copy

		  prefetchnta [esi + (200*64/34+192)]		; start reading ahead

		  movq	mm0, [esi+0]	; read 64 bits
		  movq	mm1, [esi+8]
		  movq	[edi+0], mm0	; write 64 bits
		  movq	[edi+8], mm1	;    note:  the normal movq writes the
		  movq	mm2, [esi+16]	;    data to cache; a cache line will be
		  movq	mm3, [esi+24]	;    allocated as needed, to store the data
		  movq	[edi+16], mm2
		  movq	[edi+24], mm3
		  movq	mm0, [esi+32]
		  movq	mm1, [esi+40]
		  movq	[edi+32], mm0
		  movq	[edi+40], mm1
		  movq	mm2, [esi+48]
		  movq	mm3, [esi+56]
		  movq	[edi+48], mm2
		  movq	[edi+56], mm3

		  add		esi, 64			; update source pointer
		  add		edi, 64			; update destination pointer
		  dec		ecx				; count down
		  jnz		$memcpy_ic_1	; last 64-byte block?

	  $memcpy_ic_2:
		  mov		ecx, ebx		; has valid low 6 bits of the byte count
	  $memcpy_ic_3:
		  shr		ecx, 2			; dword count
		  and		ecx, 1111b		; only look at the "remainder" bits
		  neg		ecx				; set up to jump into the array
		  add		ecx, offset $memcpy_last_few
		  jmp		ecx				; jump to array of movsds

	  $memcpy_uc_test:
		  cmp		ecx, UNCACHED_COPY/64	; big enough? use block prefetch copy
		  jae		$memcpy_bp_1

	  $memcpy_64_test:
		  or		ecx, ecx		; tail end of block prefetch will jump here
		  jz		$memcpy_ic_2	; no more 64-byte blocks left

	  // For larger blocks, which will spill beyond the cache, it's faster to
	  // use the Streaming Store instruction MOVNTQ.   This write instruction
	  // bypasses the cache and writes straight to main memory.  This code also
	  // uses the software prefetch instruction to pre-read the data.
	  align 16
	  $memcpy_uc_1:				; 64-byte blocks, uncached copy

		  prefetchnta [esi + (200*64/34+192)]		; start reading ahead

		  movq	mm0,[esi+0]		; read 64 bits
		  add		edi,64			; update destination pointer
		  movq	mm1,[esi+8]
		  add		esi,64			; update source pointer
		  movq	mm2,[esi-48]
		  movntq	[edi-64], mm0	; write 64 bits, bypassing the cache
		  movq	mm0,[esi-40]	;    note: movntq also prevents the CPU
		  movntq	[edi-56], mm1	;    from READING the destination address
		  movq	mm1,[esi-32]	;    into the cache, only to be over-written
		  movntq	[edi-48], mm2	;    so that also helps performance
		  movq	mm2,[esi-24]
		  movntq	[edi-40], mm0
		  movq	mm0,[esi-16]
		  movntq	[edi-32], mm1
		  movq	mm1,[esi-8]
		  movntq	[edi-24], mm2
		  movntq	[edi-16], mm0
		  dec		ecx
		  movntq	[edi-8], mm1
		  jnz		$memcpy_uc_1	; last 64-byte block?

		  jmp		$memcpy_ic_2		; almost done

	  // For the largest size blocks, a special technique called Block Prefetch
	  // can be used to accelerate the read operations.   Block Prefetch reads
	  // one address per cache line, for a series of cache lines, in a short loop.
	  // This is faster than using software prefetch.  The technique is great for
	  // getting maximum read bandwidth, especially in DDR memory systems.
	  $memcpy_bp_1:			; large blocks, block prefetch copy

		  cmp		ecx, CACHEBLOCK			; big enough to run another prefetch loop?
		  jl		$memcpy_64_test			; no, back to regular uncached copy

		  mov		eax, CACHEBLOCK / 2		; block prefetch loop, unrolled 2X
		  add		esi, CACHEBLOCK * 64	; move to the top of the block
	  align 16
	  $memcpy_bp_2:
		  mov		edx, [esi-64]		; grab one address per cache line
		  mov		edx, [esi-128]		; grab one address per cache line
		  sub		esi, 128			; go reverse order to suppress HW prefetcher
		  dec		eax					; count down the cache lines
		  jnz		$memcpy_bp_2		; keep grabbing more lines into cache

		  mov		eax, CACHEBLOCK		; now that its in cache, do the copy
	  align 16
	  $memcpy_bp_3:
		  movq	mm0, [esi   ]		; read 64 bits
		  movq	mm1, [esi+ 8]
		  movq	mm2, [esi+16]
		  movq	mm3, [esi+24]
		  movq	mm4, [esi+32]
		  movq	mm5, [esi+40]
		  movq	mm6, [esi+48]
		  movq	mm7, [esi+56]
		  add		esi, 64				; update source pointer
		  movntq	[edi   ], mm0		; write 64 bits, bypassing cache
		  movntq	[edi+ 8], mm1		;    note: movntq also prevents the CPU
		  movntq	[edi+16], mm2		;    from READING the destination address
		  movntq	[edi+24], mm3		;    into the cache, only to be over-written,
		  movntq	[edi+32], mm4		;    so that also helps performance
		  movntq	[edi+40], mm5
		  movntq	[edi+48], mm6
		  movntq	[edi+56], mm7
		  add		edi, 64				; update dest pointer

		  dec		eax					; count down

		  jnz		$memcpy_bp_3		; keep copying
		  sub		ecx, CACHEBLOCK		; update the 64-byte block count
		  jmp		$memcpy_bp_1		; keep processing chunks

	  // The smallest copy uses the X86 "movsd" instruction, in an optimized
	  // form which is an "unrolled loop".   Then it handles the last few bytes.
	  align 4
		  movsd
		  movsd			; perform last 1-15 dword copies
		  movsd
		  movsd
		  movsd
		  movsd
		  movsd
		  movsd
		  movsd
		  movsd			; perform last 1-7 dword copies
		  movsd
		  movsd
		  movsd
		  movsd
		  movsd
		  movsd

	  $memcpy_last_few:		; dword aligned from before movsds
		  mov		ecx, ebx	; has valid low 2 bits of the byte count
		  and		ecx, 11b	; the last few cows must come home
		  jz		$memcpy_final	; no more, lets leave
		  rep		movsb		; the last 1, 2, or 3 bytes

	  $memcpy_final:
		  emms				; clean up the MMX state
		  sfence				; flush the write buffer
	  //	mov		eax, [dest]	; ret value = destination pointer
	  }
  }
  else
  {
    memcpy(Dst, Src, Count);
  }
}

inline void cryPrefetch(const void* Src, int nCount)
{
  nCount >>= 6;
  if (nCount > 0)
  {
    _asm
    {
      mov esi, Src;
      mov ecx, nCount;
mPr0:
      align 16
      dec ecx;
      mov eax, [esi];
      mov eax,0;
      lea esi, [esi+40h];
      jne mPr0;
    }
  }
  else
  {
    _asm
    {
      mov esi, Src;
      mov ecx, nCount;
mPr1:
      align 16
      inc ecx;
      mov eax, [esi];
      mov eax,0;
      lea esi, [esi-40h];
      jne mPr1;
    }
  }
}

inline void cryMemcpy (void* inDst, const void* inSrc, int nCount, int nFlags)
{
  cryMemcpy(inDst, inSrc, nCount);
}

//==========================================================================================
// SSE optimizations

// Matrix multiplication using SSE instructions set
// IMPORTANT NOTE: much faster if matrices m1 and product are 16 bytes aligned
inline void multMatrixf_Transp2_SSE(float *product, const float *m1, const float *m2)
{
  __asm
  {
    mov         eax, m2;
    mov         ecx, m1;
    mov         edx, product;
    test        dl,0Fh
    jne         lNonAligned
    test        cl,0Fh
    jne         lNonAligned
    movss       xmm0,dword ptr [eax]
    movaps      xmm1,xmmword ptr [ecx]
    shufps      xmm0,xmm0,0
    movss       xmm2,dword ptr [eax+10h]
    mulps       xmm0,xmm1
    shufps      xmm2,xmm2,0
    movaps      xmm3,xmmword ptr [ecx+10h]
    movss       xmm4,dword ptr [eax+20h]
    mulps       xmm2,xmm3
    shufps      xmm4,xmm4,0
    addps       xmm0,xmm2
    movaps      xmm2,xmmword ptr [ecx+20h]
    mulps       xmm4,xmm2
    movaps      xmm6,xmmword ptr [ecx+30h]
    addps       xmm0,xmm4
    movaps      xmmword ptr [edx],xmm0
    movss       xmm0,dword ptr [eax+4]
    movss       xmm4,dword ptr [eax+14h]
    shufps      xmm0,xmm0,0
    shufps      xmm4,xmm4,0
    mulps       xmm0,xmm1
    mulps       xmm4,xmm3
    movss       xmm5,dword ptr [eax+24h]
    addps       xmm0,xmm4
    shufps      xmm5,xmm5,0
    mulps       xmm5,xmm2
    addps       xmm0,xmm5
    movaps      xmmword ptr [edx+10h],xmm0
    movss       xmm0,dword ptr [eax+8]
    movss       xmm4,dword ptr [eax+18h]
    shufps      xmm0,xmm0,0
    shufps      xmm4,xmm4,0
    mulps       xmm0,xmm1
    mulps       xmm4,xmm3
    movss       xmm5,dword ptr [eax+28h]
    addps       xmm0,xmm4
    shufps      xmm5,xmm5,0
    mulps       xmm5,xmm2
    addps       xmm0,xmm5
    movaps      xmmword ptr [edx+20h],xmm0
    movss       xmm0,dword ptr [eax+0ch]
    movss       xmm4,dword ptr [eax+1ch]
    shufps      xmm0,xmm0,0
    shufps      xmm4,xmm4,0
    mulps       xmm0,xmm1
    mulps       xmm4,xmm3
    movss       xmm1,dword ptr [eax+2ch]
    addps       xmm0,xmm4
    shufps      xmm1,xmm1,0
    mulps       xmm1,xmm2
    addps       xmm1,xmm6
    addps       xmm0,xmm1
    movaps      xmmword ptr [edx+30h],xmm0
    jmp         lEnd
lNonAligned:
    movlps      xmm0,qword ptr [ecx]
    movhps      xmm0,qword ptr [ecx+8]
    movlps      xmm1,qword ptr [ecx+10h]
    movhps      xmm1,qword ptr [ecx+18h]
    movlps      xmm2,qword ptr [ecx+20h]
    movhps      xmm2,qword ptr [ecx+28h]
    movlps      xmm3,qword ptr [ecx+30h]
    movhps      xmm3,qword ptr [ecx+38h]
    movss       xmm4,dword ptr [eax]
    movss       xmm5,dword ptr [eax+10h]
    movss       xmm6,dword ptr [eax+20h]
    shufps      xmm4,xmm4,0
    shufps      xmm5,xmm5,0
    shufps      xmm6,xmm6,0
    mulps       xmm4,xmm0
    mulps       xmm5,xmm1
    mulps       xmm6,xmm2
    addps       xmm4,xmm5
    addps       xmm4,xmm6
    movss       xmm5,dword ptr [eax+4]
    movss       xmm6,dword ptr [eax+14h]
    movss       xmm7,dword ptr [eax+24h]
    shufps      xmm5,xmm5,0
    shufps      xmm6,xmm6,0
    shufps      xmm7,xmm7,0
    mulps       xmm5,xmm0
    mulps       xmm6,xmm1
    mulps       xmm7,xmm2
    addps       xmm5,xmm6
    addps       xmm5,xmm7
    movss       xmm6,dword ptr [eax+8]
    movss       xmm7,dword ptr [eax+18h]
    shufps      xmm6,xmm6,0
    shufps      xmm7,xmm7,0
    mulps       xmm6,xmm0
    mulps       xmm7,xmm1
    addps       xmm6,xmm7
    movss       xmm7,dword ptr [eax+28h]
    shufps      xmm7,xmm7,0
    mulps       xmm7,xmm2
    addps       xmm6,xmm7
    movss       xmm7,dword ptr [eax+0ch]
    shufps      xmm7,xmm7,0
    mulps       xmm0,xmm7
    movss       xmm7,dword ptr [eax+1ch]
    shufps      xmm7,xmm7,0
    mulps       xmm1,xmm7
    movss       xmm7,dword ptr [eax+2ch]
    shufps      xmm7,xmm7,0
    mulps       xmm2,xmm7
    movlps      qword ptr [edx],xmm4
    movhps      qword ptr [edx+8],xmm4
    addps       xmm0,xmm1
    movlps      qword ptr [edx+10h],xmm5
    movhps      qword ptr [edx+18h],xmm5
    addps       xmm2,xmm3
    movlps      qword ptr [edx+20h],xmm6
    movhps      qword ptr [edx+28h],xmm6
    addps       xmm0,xmm2
    movlps      qword ptr [edx+30h],xmm0
    movhps      qword ptr [edx+38h],xmm0
lEnd:
  }
}

#else

const int PREFNTA_BLOCK = 0x4000;

#ifndef PS3
#ifndef XENON
ILINE void cryMemcpy(void* Dst, const void* Src, int n) {
	char* dst=(char*)Dst;
	char* src=(char*)Src;
	while (n > PREFNTA_BLOCK) 
	{

		_MM_PREFETCH_LOOP( PREFNTA_BLOCK,src,_MM_HINT_NTA );

		memcpy(dst, src, PREFNTA_BLOCK);
		src += PREFNTA_BLOCK;
		dst += PREFNTA_BLOCK;
		n -= PREFNTA_BLOCK;
	}
	_MM_PREFETCH_LOOP( n,src,_MM_HINT_NTA );
	memcpy(dst, src, n);
}

ILINE void cryMemcpy( void* Dst, const void* Src, int n, int nFlags )
{
	char* dst=(char*)Dst;
	char* src=(char*)Src;
	while (n > PREFNTA_BLOCK) 
	{
		_MM_PREFETCH_LOOP( PREFNTA_BLOCK,src,_MM_HINT_NTA );
		memcpy(dst, src, PREFNTA_BLOCK);
		src += PREFNTA_BLOCK;
		dst += PREFNTA_BLOCK;
		n -= PREFNTA_BLOCK;
	}
	_MM_PREFETCH_LOOP( n,src,_MM_HINT_NTA );
	memcpy(dst, src, n);
}























#endif//XENON
#endif//PS3
#endif

inline void mathMatrixMultiply_Transp2(float *pOut, const float *pM1, const float *pM2, int OptFlags)
{
#if defined _CPU_X86 && !defined(LINUX)
  if (OptFlags & CPUF_SSE)
    multMatrixf_Transp2_SSE(pOut, pM1, pM2);
  else
#endif
    multMatrixf_Transp2(pOut, pM1, pM2);
}

#define mathMatrixRotationZ(pOut, angle) (*(Matrix44*)pOut) = GetTransposed44( Matrix44(Matrix34::CreateRotationZ(angle)) )
#define mathMatrixRotationY(pOut, angle) (*(Matrix44*)pOut) = GetTransposed44( Matrix44(Matrix34::CreateRotationY(angle)) )
#define mathMatrixRotationX(pOut, angle) (*(Matrix44*)pOut) = GetTransposed44( Matrix44(Matrix34::CreateRotationX(angle)) )
#define mathMatrixTranslation(pOut, x, y, z) (*(Matrix44*)pOut) = GetTransposed44( Matrix44(Matrix34::CreateTranslationMat(Vec3(x,y,z))) )
#define mathMatrixScaling(pOut, sx, sy, sz) (*(Matrix44*)pOut) = GetTransposed44( Matrix44(Matrix34::CreateScale(Vec3(sx,sy,sz))) )

template <class T> inline void ExchangeVals(T& X, T& Y)
{
	const T Tmp = X;
	X = Y;
	Y = Tmp;
}

inline void mathRotateX(float *pMatr, float fDegr)
{
  Matrix44A rm;
 // float cossin[2];  sincos_tpl(DEG2RAD(fDegr), cossin);
	f32 s,c; sincos_tpl(DEG2RAD(fDegr), &s,&c);
  rm(0,0) = 1; rm(0,1) = 0; rm(0,2) = 0; rm(0,3) = 0;
  rm(1,0) = 0; rm(1,1) = c; rm(1,2) = s; rm(1,3) = 0;
  rm(2,0) = 0; rm(2,1) = -s; rm(2,2) = c; rm(2,3) = 0;
  rm(3,0) = 0; rm(3,1) = 0; rm(3,2) = 0; rm(3,3) = 1;
  Matrix44 *pM = (Matrix44 *)pMatr;
  pM->Multiply(rm, *pM);
}
inline void mathRotateY(float *pMatr, float fDegr)
{
  Matrix44A rm;
 // float cossin[2];  sincos_tpl(DEG2RAD(fDegr), cossin);
	f32 s,c; sincos_tpl(DEG2RAD(fDegr), &s,&c);
  rm(0,0) = c; rm(0,1) = 0; rm(0,2) = -s; rm(0,3) = 0;
  rm(1,0) = 0; rm(1,1) = 1; rm(1,2) = 0; rm(1,3) = 0;
  rm(2,0) = s; rm(2,1) = 0; rm(2,2) = c; rm(2,3) = 0;
  rm(3,0) = 0; rm(3,1) = 0; rm(3,2) = 0; rm(3,3) = 1;
  Matrix44 *pM = (Matrix44 *)pMatr;
  pM->Multiply(rm, *pM);
}
inline void mathRotateZ(float *pMatr, float fDegr)
{
  Matrix44A rm;
 // float cossin[2];  sincos_tpl(DEG2RAD(fDegr), cossin);
	f32 s,c; sincos_tpl(DEG2RAD(fDegr), &s,&c);
  rm(0,0) = c; rm(0,1) = s; rm(0,2) = 0; rm(0,3) = 0;
  rm(1,0) = -s; rm(1,1) = c; rm(1,2) = 0; rm(1,3) = 0;
  rm(2,0) = 0; rm(2,1) = 0; rm(2,2) = 1; rm(2,3) = 0;
  rm(3,0) = 0; rm(3,1) = 0; rm(3,2) = 0; rm(3,3) = 1;
  Matrix44 *pM = (Matrix44 *)pMatr;
  pM->Multiply(rm, *pM);
}

inline void mathScale(float *pMatr, Vec3 vScale, int )
{
  pMatr[0] *= vScale.x;   pMatr[4] *= vScale.y;   pMatr[8]  *= vScale.z;
  pMatr[1] *= vScale.x;   pMatr[5] *= vScale.y;   pMatr[9]  *= vScale.z;
  pMatr[2] *= vScale.x;   pMatr[6] *= vScale.y;   pMatr[10] *= vScale.z;
  pMatr[3] *= vScale.x;   pMatr[7] *= vScale.y;   pMatr[11] *= vScale.z;
}
 
inline void mathMatrixPerspectiveFov(Matrix44A* pMatr, f32 fovY, f32 Aspect, f32 zn, f32 zf)
{
	f32 yScale = 1.0f/cry_tanf(fovY/2.0f);
	f32 xScale = yScale / Aspect;

	(*pMatr)(0,0) = xScale;		(*pMatr)(0,1) = 0;			(*pMatr)(0,2) = 0;						(*pMatr)(0,3) = 0; 
	(*pMatr)(1,0) = 0;				(*pMatr)(1,1) = yScale; (*pMatr)(1,2) = 0;						(*pMatr)(1,3) = 0; 
	(*pMatr)(2,0) = 0;				(*pMatr)(2,1) = 0;			(*pMatr)(2,2) = zf/(zn-zf);		(*pMatr)(2,3) = -1.0f; 
	(*pMatr)(3,0) = 0;				(*pMatr)(3,1) = 0;			(*pMatr)(3,2) = zn*zf/(zn-zf); (*pMatr)(3,3) = 0; 
}



inline void mathMatrixOrtho(Matrix44A* pMatr, f32 w, f32 h, f32 zn, f32 zf)
{

	(*pMatr)(0,0) = 2.0f/w;  (*pMatr)(0,1) = 0;			 (*pMatr)(0,2) = 0;							(*pMatr)(0,3) = 0;
	(*pMatr)(1,0) = 	0;		 (*pMatr)(1,1) = 2.0f/h; (*pMatr)(1,2) = 0;							(*pMatr)(1,3) = 0;
	(*pMatr)(2,0) =		0;     (*pMatr)(2,1) = 0;			 (*pMatr)(2,2) = 1.0f/(zn-zf);	(*pMatr)(2,3) = 0;
	(*pMatr)(3,0) =		0;		 (*pMatr)(3,1) = 0;			 (*pMatr)(3,2) = zn/(zn-zf);		(*pMatr)(3,3) =	1;

}

inline void mathMatrixOrthoOffCenter(Matrix44A* pMatr, f32 l, f32 r, f32 b, f32 t, f32 zn, f32 zf)
{
	(*pMatr)(0,0) = 2.0f/(r-l);		(*pMatr)(0,1) = 0;					(*pMatr)(0,2) = 0;						(*pMatr)(0,3) =	0;
	(*pMatr)(1,0) = 	0;          (*pMatr)(1,1) = 2.0f/(t-b);	(*pMatr)(1,2) = 0;						(*pMatr)(1,3) = 0;
	(*pMatr)(2,0) =		0;          (*pMatr)(2,1) =	 0;					(*pMatr)(2,2) =	1.0f/(zn-zf);	(*pMatr)(2,3) =	0;
	(*pMatr)(3,0) =	(l+r)/(l-r);  (*pMatr)(3,1) =(t+b)/(b-t); (*pMatr)(3,2) = zn/(zn-zf);		(*pMatr)(3,3) = 1.0f;
}


inline void mathMatrixOrthoOffCenterLH(Matrix44A* pMatr, f32 l, f32 r, f32 b, f32 t, f32 zn, f32 zf)
{
	(*pMatr)(0,0) = 2.0f/(r-l);		(*pMatr)(0,1) = 0;					(*pMatr)(0,2) = 0;						(*pMatr)(0,3) =	0;
	(*pMatr)(1,0) = 	0;          (*pMatr)(1,1) = 2.0f/(t-b);	(*pMatr)(1,2) = 0;						(*pMatr)(1,3) = 0;
	(*pMatr)(2,0) =		0;          (*pMatr)(2,1) =	 0;					(*pMatr)(2,2) =	1.0f/(zf-zn);	(*pMatr)(2,3) =	0;
	(*pMatr)(3,0) =	(l+r)/(l-r);  (*pMatr)(3,1) =(t+b)/(b-t); (*pMatr)(3,2) = zn/(zn-zf);		(*pMatr)(3,3) = 1.0f;
}


inline void mathMatrixPerspectiveOffCenter(Matrix44A* pMatr, f32 l, f32 r, f32 b, f32 t, f32 zn, f32 zf)
{
	(*pMatr)(0,0) =	2*zn/(r-l);			(*pMatr)(0,1) =  0;						(*pMatr)(0,2) = 0;							(*pMatr)(0,3) = 0;
	(*pMatr)(1,0) = 0;            	(*pMatr)(1,1) = 2*zn/(t-b);   (*pMatr)(1,2) = 0;							(*pMatr)(1,3) =	0;
	(*pMatr)(2,0) =	(l+r)/(r-l);		(*pMatr)(2,1) =	(t+b)/(t-b);  (*pMatr)(2,2) =	zf/(zn-zf);			(*pMatr)(2,3) =	-1;
	(*pMatr)(3,0) =	0;							(*pMatr)(3,1) =	0;            (*pMatr)(3,2) =	zn*zf/(zn-zf);	(*pMatr)(3,3) =	0;
}


//RH
inline void mathMatrixLookAt(Matrix44A* pMatr, const Vec3& Eye, const Vec3& At, const Vec3& Up)
{
	Vec3 vLightDir = (Eye - At);
	Vec3 zaxis = vLightDir.GetNormalized();
	Vec3 xaxis = (Up.Cross(zaxis)).GetNormalized();
	Vec3 yaxis =zaxis.Cross(xaxis);

	(*pMatr)(0,0) = xaxis.x;		(*pMatr)(0,1) = yaxis.x;	(*pMatr)(0,2) = zaxis.x;	(*pMatr)(0,3) = 0; 
	(*pMatr)(1,0) = xaxis.y;		(*pMatr)(1,1) = yaxis.y;	(*pMatr)(1,2) = zaxis.y;	(*pMatr)(1,3) = 0; 
	(*pMatr)(2,0) = xaxis.z;		(*pMatr)(2,1) = yaxis.z;	(*pMatr)(2,2) = zaxis.z;	(*pMatr)(2,3) = 0; 
	(*pMatr)(3,0) = -xaxis.Dot(Eye);	(*pMatr)(3,1) = -yaxis.Dot(Eye);	(*pMatr)(3,2) = -zaxis.Dot(Eye);	(*pMatr)(3,3) = 1; 
}


inline void mathVec4Transform(f32 out[4], const f32 m[16], const f32 in[4])
{
#define M(row,col)  m[col*4+row]
	out[0] =	M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
	out[1] =	M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
	out[2] =	M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
	out[3] =	M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

//fix: replace by 3x4 Matrix transformation and move to crymath
inline void mathVec3Transform(f32 out[4], const f32 m[16], const f32 in[3])
{
#define M(row,col)  m[col*4+row]
	out[0] =	M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * 1.0f;
	out[1] =	M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * 1.0f;
	out[2] =	M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * 1.0f;
	out[3] =	M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * 1.0f;
#undef M
}

#define mathVec3TransformF(pOut, pV, pM) mathVec3Transform( (f32*)pOut, (const f32*)pM, (f32*)pV )
#define mathVec4TransformF(pOut, pV, pM) mathVec4Transform( (f32*)pOut, (const f32*)pM, (f32*)pV )
#define mathVec3NormalizeF(pOut, pV) (*(Vec3*)pOut) = (((Vec3*)pV)->GetNormalizedSafe())
#define mathVec2NormalizeF(pOut, pV) (*(Vec2*)pOut) = (((Vec2*)pV)->GetNormalizedSafe())


//fix replace viewport by int16 array
//fix for d3d viewport
inline Vec3* mathVec3Project(Vec3 *pvWin, const Vec3 *pvObj,
															const int32 pViewport[4],
															const Matrix44A* pProjection,
															const Matrix44A* pView,
															const Matrix44A*	pWorld
														)
{
	Vec4 in, out;

	in.x = pvObj->x;
	in.y = pvObj->y;
	in.z = pvObj->z;
	in.w = 1.0f;
	mathVec4Transform((f32*)&out, (f32*)pWorld, (f32*)&in);
	mathVec4Transform((f32*)&in, (f32*)pView, (f32*)&out);
	mathVec4Transform((f32*)&out, (f32*)pProjection, (f32*)&in);

	if (out.w == 0.0f)
		return NULL;

	out.x /= out.w;
	out.y /= out.w;
	out.z /= out.w;

	//output coords
	pvWin->x = pViewport[0] + (1 + out.x) * pViewport[2] / 2;
	pvWin->y = pViewport[1] + (1 - out.y) * pViewport[3] / 2;  //flip coords for y axis

	//FIX: update fViewportMinZ fViewportMaxZ support for Viewport everywhere
	float fViewportMinZ = 0, fViewportMaxZ = 1.0f;

	pvWin->z = fViewportMinZ + out.z*(fViewportMaxZ-fViewportMinZ);

	return pvWin;
}

inline Vec3* mathVec3UnProject(Vec3 *pvObj, const Vec3 *pvWin,
														 const int32 pViewport[4],
														 const Matrix44A* pProjection,
														 const Matrix44A* pView,
														 const Matrix44A* pWorld,
														 int32 OptFlags
														 )
{
	Matrix44A m, mA;
	Vec4 in, out;

	//FIX: update fViewportMinZ fViewportMaxZ support for Viewport everywhere
	float fViewportMinZ = 0, fViewportMaxZ = 1.0f;

	in.x = (pvWin->x - pViewport[0]) * 2 / pViewport[2] - 1.0f;
	in.y = 1.0f - ( (pvWin->y - pViewport[1]) * 2 / pViewport[3] ); //flip coords for y axis
	in.z = (pvWin->z - fViewportMinZ)/(fViewportMaxZ - fViewportMinZ);
	in.w = 1.0f;

	//prepare inverse projection matrix
  //mA = (*pWorld) * (*pView);
	//mA =  mA * (*pProjection);
	m.Invert(mA);

	mathVec4Transform((f32*)&out, m.GetData(), (f32*)&in);
	if (out.w == 0.0f)
		return NULL;

	pvObj->x = out.x / out.w;
	pvObj->y = out.y / out.w;
	pvObj->z = out.z / out.w;

	return pvObj;
}


inline Vec3* mathVec3ProjectArray(	Vec3 *pOut,
																	uint32 OutStride,
																	const Vec3 *pV,
																	uint32 VStride,
																	const int32 pViewport[4],
																	const Matrix44A* pProjection,
																	const Matrix44A* pView,
																	const Matrix44A* pWorld,
																	uint32 n,
																	int32
																	)
{
	Matrix44A m;
	Vec4 in, out;

	int8* pOutT = (int8*)pOut;
	int8* pInT = (int8*)pV;

	Vec3* pvWin;
	Vec3* pvObj;

	//FIX: update fViewportMinZ fViewportMaxZ support for Viewport everywhere
	float fViewportMinZ = 0, fViewportMaxZ = 1.0f;

	m =  ((*pWorld) * (*pView)) * (*pProjection);

	for (uint32 i=0; i<n; i++)
	{

		pvObj = (Vec3*)pInT;
		pvWin = (Vec3*)pOutT;

		in.x = pvObj->x;
		in.y = pvObj->y;
		in.z = pvObj->z;
		in.w = 1.0f;

		mathVec4Transform((f32*)&out, m.GetData(), (f32*)&in);

		if (out.w == 0.0f)
			return NULL;

    float fInvW = 1.0f / out.w;
		out.x *= fInvW;
		out.y *= fInvW;
		out.z *= fInvW;

		//output coords
		pvWin->x = pViewport[0] + (1 + out.x) * pViewport[2] / 2;
		pvWin->y = pViewport[1] + (1 - out.y) * pViewport[3] / 2;  //flip coords for y axis

		pvWin->z = fViewportMinZ + out.z*(fViewportMaxZ-fViewportMinZ);

		pOutT += OutStride;
		pInT += VStride;
	}

	return pOut;
}

inline Vec3* mathVec3UnprojectArray(	Vec3 *pOut,
																	  uint32 OutStride,
																		const Vec3 *pV,
																		uint32 VStride,
																		const int32 pViewport[4],
																		const Matrix44* pProjection,
																		const Matrix44* pView,
																		const Matrix44* pWorld,
																		uint32 n,
																		int32 OptFlags
																	)
{
	Vec4 in, out;
	Matrix44 m, mA;

	int8* pOutT = (int8*)pOut;
	int8* pInT = (int8*)pV;

	Vec3* pvWin;
	Vec3* pvObj;

	//FIX: update fViewportMinZ fViewportMaxZ support for Viewport everywhere
	float fViewportMinZ = 0, fViewportMaxZ = 1.0f;

	mA =  ((*pWorld) * (*pView)) * (*pProjection);
	m.Invert(mA);

	for (uint32 i=0; i<n; i++)
	{
		pvWin = (Vec3*)pInT;
		pvObj = (Vec3*)pOutT;

		in.x = (pvWin->x - pViewport[0]) * 2 / pViewport[2] - 1.0f;
		in.y = 1.0f - ( (pvWin->y - pViewport[1]) * 2 / pViewport[3] ); //flip coords for y axis
		in.z = (pvWin->z - fViewportMinZ)/(fViewportMaxZ - fViewportMinZ);
		in.w = 1.0f;

		mathVec4Transform((f32*)&out, m.GetData(), (f32*)&in);

		assert(out.w != 0.0f);

		if (out.w == 0.0f)
			return NULL;

		pvObj->x = out.x / out.w;
		pvObj->y = out.y / out.w;
		pvObj->z = out.z / out.w;

		pOutT += OutStride;
		pInT += VStride;
	}

	return pOut;
}

#define mathVec3TransformCoordArrayF(pOut, OutStride, pV, VStride, pM, n) mathVec3TransformCoordArray((Vec3*)pOut, OutStride, (Vec3*)pV, VStride, (Matrix44*)pM, n)

inline Vec3* mathVec3TransformCoordArray(	Vec3 *pOut,
																					uint32 OutStride,
																					const Vec3 *pV,
																					uint32 VStride,
																					const Matrix44* pM,
																					uint32 n
																				)
{
	Vec4 in, out;

	int8* pOutT = (int8*)pOut;
	int8* pInT = (int8*)pV;

	Vec3* pvOut;
	Vec3* pvIn;

	for (uint32 i=0; i<n; i++)
	{
		pvOut = (Vec3*)pOutT;
		pvIn = (Vec3*)pInT;

		in.x = pvIn->x;
		in.y = pvIn->y;
		in.z = pvIn->z;
		in.w = 1.0f;

		mathVec4Transform((f32*)&out, pM->GetData(), (f32*)&in);

		if (out.w == 0.0f)
			return NULL;

		pvOut->x = out.x / out.w;
		pvOut->y = out.y / out.w;
		pvOut->z = out.z / out.w;

		pOutT += OutStride;
		pInT += VStride;
	}

	return pOut;
}


#pragma warning(pop)




































































































































	//implement something usual to bring one memory location into L1 data cache
	ILINE void CryPrefetch(const void* const cpSrc)
	{
		cryPrefetchT0SSE(cpSrc);
	}

	// Indicates src may be an unmapped address (unsupported on the SPU).
	// Should be safe for use when prefetching beyond the end of a sequence from within a loop.
	#define CryPrefetchUnsafe CryPrefetch


	#define CryPrefetchInl CryPrefetch

















































class CIntToFloat
{
public:
#if !defined(__SPU__) // prevents softfloat double converstions on spu 
	ILINE explicit CIntToFloat(int64 i)
		: tmp((float)i)	{	}
#endif 




	ILINE float Convert()
	{
		return tmp;
	}
	ILINE float Reinterpret()const
	{
		return tmp;
	}
private:
	float tmp;
};



#endif //math

