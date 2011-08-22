
//////////////////////////////////////////////////////////////////////
//
//	Crytek CryENGINE Source code
//	
//	File:VertexFormats.h - 
//
//	History:
//	-Feb 23,2001:Created by Marco Corbetta
//
//////////////////////////////////////////////////////////////////////


#ifndef VERTEXFORMATS_H
#define VERTEXFORMATS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <CryArray.h>

enum EVertexFormat
{
  eVF_Unknown = 0,

  // Base stream
  eVF_P3F_C4B_T2F = 1,
  eVF_P3S_C4B_T2S = 2,
  eVF_P3S_N4B_C4B_T2S = 3,

  eVF_P3F_C4B_I4B_PS4F = 4, // Particles.
  eVF_TP3F_C4B_T2F = 5, // Fonts (28 bytes).
  eVF_TP3F_T2F_T3F = 6,  // Miscellaneus.
  eVF_P3F_T3F = 7,       // Miscellaneus.
  eVF_P3F_T2F_T3F = 8,   // Miscellaneus.

  // Additional streams
  eVF_T2F = 9,           // Light maps TC (8 bytes).
  eVF_W4B_I4B_P3F = 10,  // Skinned weights/indices stream.
  eVF_C4B_C4B = 11,      // SH coefficients.
  eVF_P3F_P3F_I4B = 12,  // Shape deformation stream.
  eVF_P3F_MT = 13,       // Morph target stream.

	// Stream formats for SPU skinning
  eVF_C4B_T2S = 14,     // General (Position is merged with Tangent stream)

  eVF_Max = 15,
};


//#define TANG_FLOATS 1
#ifdef TANG_FLOATS
#define int16f float
#else
#define int16f short
#endif






















typedef Vec4_tpl<int16f> Vec4sf;		// Used for tangents only.


// bNeedNormals=1 - float normals; bNeedNormals=2 - byte normals
_inline EVertexFormat VertFormatForComponents(bool bNeedCol, bool bHasTC, bool bHasPS, bool bHasNormal)
{
  EVertexFormat RequestedVertFormat;

  if (bHasPS)
    RequestedVertFormat = eVF_P3F_C4B_I4B_PS4F;
  else
  if (bHasNormal)
    RequestedVertFormat = eVF_P3S_N4B_C4B_T2S;
  else
    RequestedVertFormat = eVF_P3S_C4B_T2S;

  return RequestedVertFormat;
}

struct UCol
{
  union
  {
    uint32 dcolor;
    uint8  bcolor[4];
  };

  AUTO_STRUCT_INFO
};

#ifndef FP16_MESH
struct Vec3f16 : public Vec3
#else
struct Vec3f16 : public CryHalf4
#endif
{
  _inline Vec3f16()
  {
  }
  _inline Vec3f16(f32 _x, f32 _y, f32 _z)
  {
#ifndef FP16_MESH
    x = _x;
    y = _y;
    z = _z;
#else












    x = CryConvertFloatToHalf(_x);
    y = CryConvertFloatToHalf(_y);
    z = CryConvertFloatToHalf(_z);
    w = CryConvertFloatToHalf(1.0f);

#endif
  }
  float operator[](int i) const
  {
#ifdef FP16_MESH
    assert(i <= 3);
    return CryConvertHalfToFloat(((CryHalf *)this)[i]);
#else
    assert(i <= 2);
    return ((f32 *)this)[i];
#endif
  }
//  _inline Vec3f16& operator = (const Vec3f16& sl)
//  {
//    x = sl.x;
//    y = sl.y;
//    z = sl.z;
//#ifdef FP16_MESH
//    w = sl.w;
//#endif
//    return *this;
//  }
  _inline Vec3f16& operator = (const Vec3& sl)
  {
#ifndef FP16_MESH
    x = sl.x;
    y = sl.y;
    z = sl.z;
#else












    x = CryConvertFloatToHalf(sl.x);
    y = CryConvertFloatToHalf(sl.y);
    z = CryConvertFloatToHalf(sl.z);
    w = CryConvertFloatToHalf(1.0f);

#endif
    return *this;
  }
  _inline Vec3f16& operator = (const Vec4A& sl)
  {
#ifndef FP16_MESH
    x = sl.x;
    y = sl.y;
    z = sl.z;
#else











    x = CryConvertFloatToHalf(sl.x);
    y = CryConvertFloatToHalf(sl.y);
    z = CryConvertFloatToHalf(sl.z);
    w = CryConvertFloatToHalf(sl.w);

#endif
    return *this;
  }
  _inline Vec3 ToVec3() const
  {
    Vec3 v;
#ifndef FP16_MESH
    v.x = x;
    v.y = y;
    v.z = z;
#else
    v.x = CryConvertHalfToFloat(x);
    v.y = CryConvertHalfToFloat(y);
    v.z = CryConvertHalfToFloat(z);
#endif
    return v;
  }
};

#ifndef FP16_MESH
struct Vec2f16 : public Vec2
#else
struct Vec2f16 : public CryHalf2
#endif
{
  _inline Vec2f16()
  {
  }
  _inline Vec2f16(f32 _x, f32 _y)
  {
#ifndef FP16_MESH
    x = _x;
    y = _y;
#else
    x = CryConvertFloatToHalf(_x);
    y = CryConvertFloatToHalf(_y);
#endif
  }
  Vec2f16& operator = (const Vec2f16& sl)
  {
    x = sl.x;
    y = sl.y;
    return *this;
  }
  Vec2f16& operator = (const Vec2& sl)
  {
#ifndef FP16_MESH
    x = sl.x;
    y = sl.y;
#else
    x = CryConvertFloatToHalf(sl.x);
    y = CryConvertFloatToHalf(sl.y);
#endif
    return *this;
  }
  float operator[](int i) const
  {
    assert(i <= 1);
#ifdef FP16_MESH
    return CryConvertHalfToFloat(((CryHalf *)this)[i]);
#else
    return ((f32 *)this)[i];
#endif
  }
  _inline Vec2 ToVec2() const
  {
    Vec2 v;
#ifndef FP16_MESH
    v.x = x;
    v.y = y;
#else
    v.x = CryConvertHalfToFloat(x);
    v.y = CryConvertHalfToFloat(y);
#endif
    return v;
  }
};


struct SVF_P3F_C4B_T2F
{
  Vec3 xyz;
  UCol color;
  Vec2 st;
};
struct SVF_TP3F_C4B_T2F
{
  Vec4 pos;
  UCol color;
  Vec2 st;
};
struct SVF_P3S_C4B_T2S
{
  Vec3f16 xyz;
  UCol color;
  Vec2f16 st;
};
struct SVF_P3S_N4B_C4B_T2S
{
  Vec3f16 xyz;
  UCol normal;
  UCol color;
  Vec2f16 st;
};

struct SVF_T2F
{
  Vec2 st;
};
struct SVF_W4B_I4B
{
  UCol weights;
  UCol indices;
};
struct SVF_C4B_C4B
{
  UCol coef0;
  UCol coef1;
};
struct SVF_P3F_P3F_I4B
{
  Vec3 thin;	
  Vec3 fat;	
  UCol index;
};
struct SVF_P3F
{
  Vec3 xyz;	
};
struct SVF_P3F_T3F
{
  Vec3 p;
  Vec3 st;
};
struct SVF_P3F_T2F_T3F
{
  Vec3 p;
  Vec2 st0;
  Vec3 st1;
};
struct SVF_TP3F_T2F_T3F
{
  Vec4 p;
  Vec2 st0;
  Vec3 st1;
};
struct SVF_P3F_C4B_I4B_PS4F
{
  Vec3 xyz;
  UCol color;
  struct SpriteInfo
  {



    uint8		tex_x, tex_y, tex_z, backlight;		// xyzw
    // PC DX9 swapped in shaders to						// zyxw

  } info;
  Vec2 xaxis;
  Vec2 yaxis;
};

struct SVF_C4B_T2S
{
  UCol color;
  Vec2f16 st;
};

//=============================================================
// Tangent vectors packing

_inline int16f tPackF2B(const float f)
{
#ifdef TANG_FLOATS
  return f;
#else
  return (int16f)(f * 32767.0f);
#endif
}
_inline float tPackB2F(const int16f i)
{
#ifdef TANG_FLOATS
  return i;
#else
  return (float)((float)i / 32767.0f);
#endif
}

_inline Vec4sf tPackF2Bv(const Vec4& v)
{
  Vec4sf vs;
  vs.x = tPackF2B(v.x);
  vs.y = tPackF2B(v.y);
  vs.z = tPackF2B(v.z);
  vs.w = tPackF2B(v.w);

  return vs;
}
_inline Vec4sf tPackF2Bv(const Vec3& v)
{
  Vec4sf vs;
  vs.x = tPackF2B(v.x);
  vs.y = tPackF2B(v.y);
  vs.z = tPackF2B(v.z);
  vs.w = tPackF2B(1.0f);

  return vs;
}
_inline Vec4 tPackB2F(const Vec4sf& v)
{
  Vec4 vs;
  vs.x = tPackB2F(v.x);
  vs.y = tPackB2F(v.y);
  vs.z = tPackB2F(v.z);
  vs.w = tPackB2F(v.w);

  return vs;
}
_inline void tPackB2F(const Vec4sf& v, Vec4& vDst)
{
  vDst.x = tPackB2F(v.x);
  vDst.y = tPackB2F(v.y);
  vDst.z = tPackB2F(v.z);
  vDst.w = 1.0f;
}
_inline void tPackB2FScale(const Vec4sf& v, Vec4& vDst, const Vec3& vScale)
{
  vDst.x = (float)v.x * vScale.x;
  vDst.y = (float)v.y * vScale.y;
  vDst.z = (float)v.z * vScale.z;
  vDst.w = 1.0f;
}
_inline void tPackB2FScale(const Vec4sf& v, Vec3& vDst, const Vec3& vScale)
{
  vDst.x = (float)v.x * vScale.x;
  vDst.y = (float)v.y * vScale.y;
  vDst.z = (float)v.z * vScale.z;
}

_inline void tPackB2F(const Vec4sf& v, Vec3& vDst)
{
  vDst.x = tPackB2F(v.x);
  vDst.y = tPackB2F(v.y);
  vDst.z = tPackB2F(v.z);
}

struct SPipTangents
{
  Vec4sf Tangent;
  Vec4sf Binormal;
};
struct SQTangents
{
  Vec4sf Tangent;
};

//==================================================================================================

typedef SVF_P3F_C4B_T2F SAuxVertex;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Sizes
//extern const int m_VertexSize[];

// we don't care about truncation of the struct member offset, because
// it's a very small integer (even fits into a signed byte)
#pragma warning(push)
#pragma warning(disable:4311)

//============================================================================
// Custom vertex streams definitions
// NOTE: If you add new stream ID also include vertex declarations creating in 
//       CD3D9Renderer::EF_InitD3DVertexDeclarations (D3DRendPipeline.cpp)

// Stream IDs
enum EStreamIDs
{
  VSF_GENERAL,									// General vertex buffer
  VSF_TANGENTS,									// Tangents buffer
  VSF_QTANGENTS,		  					// Tangents buffer
  VSF_HWSKIN_INFO,							// HW skinning buffer
  VSF_HWSKIN_MORPHTARGET_INFO,  // HW skinning (morph targets) buffer
#if !defined(XENON) && !defined(PS3) 
  VSF_HWSKIN_SHAPEDEFORM_INFO,  // HW skinning (shape deformation) buffer
#endif
  // <- Insert new stream IDs here
  VSF_NUM,											// Number of vertex streams
  
  VSF_MORPHBUDDY = 8,           // Morphing (from m_pMorphBuddy)
  VSF_MORPHBUDDY_WEIGHTS = 15,  // Morphing weights
};

// Stream Masks (Used during updating)
enum EStreamMasks
{
  VSM_GENERAL    = 1 << VSF_GENERAL,
  VSM_TANGENTS   = ((1<<VSF_TANGENTS) | (1<<VSF_QTANGENTS)),
  VSM_HWSKIN     = 1 << VSF_HWSKIN_INFO,
  VSM_HWSKIN_MORPHTARGET       = 1 << VSF_HWSKIN_MORPHTARGET_INFO,
#if !defined(XENON) && !defined(PS3) 
  VSM_HWSKIN_SHAPEDEFORM       = 1 << VSF_HWSKIN_SHAPEDEFORM_INFO,
#endif

  VSM_MORPHBUDDY = 1 << VSF_MORPHBUDDY,

  VSM_MASK     = ((1 << VSF_NUM)-1),
};

//==================================================================================================================

#pragma warning(pop)


#endif

