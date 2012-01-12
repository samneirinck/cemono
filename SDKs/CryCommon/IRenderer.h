
//////////////////////////////////////////////////////////////////////
//
//  Crytek CryENGINE Source code
//
//  File:Renderer.h - API Independent
//
//  History:
//  -Jan 31,2001:Originally created by Marco Corbetta
//  -: Taken over by Andrey Khonich
//
//////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(IRenderer.h)

#ifndef _IRENDERER_H
#define _IRENDERER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Cry_Geo.h" 

// Callback used for DXTCompress
typedef void (*MIPDXTcallback)( const void *buffer, size_t count, void * userData );

typedef void (*GpuCallbackFunc)(DWORD context);

// Callback for shadercache miss
typedef void (*ShaderCacheMissCallback)(const char* acShaderRequest);

//Please ensure that this is big enough for all of the EGPUProfileSegmentFlags in FrameProfiler.h
typedef uint8 gpu_profile_flags_t;

UNIQUE_IFACE struct ICaptureFrameListener 
{
	virtual ~ICaptureFrameListener (){}
	virtual unsigned char *OnNeedFrameData(void) = 0;
	virtual void OnFrameCaptured(void) = 0;
	virtual int OnGetFrameWidth(void) = 0;
	virtual int OnGetFrameHeight(void) = 0;
	virtual int OnGetCaptureFlags(void) = 0;

	enum ECaptureFrameFlags
	{
		eCFF_NoCaptureThisFrame = (0 << 1),
		eCFF_CaptureThisFrame = (1 << 1)
	}; 
};




// Forward declarations.
//////////////////////////////////////////////////////////////////////
typedef void* WIN_HWND;
typedef void* WIN_HINSTANCE;
typedef void* WIN_HDC;
typedef void* WIN_HGLRC;

class   CVertexBuffer;
class   CIndexBuffer;
class   CREMesh;
class		CMesh;
//class   CImage;
struct  CStatObj;
class   CVegetation;
struct  ShadowMapFrustum;
struct  IStatObj;
class   CObjManager;
struct  SPrimitiveGroup;
struct  ICharacterInstance;
class   CRendElementBase;
class   CRenderObject;
class   CTexMan;
//class   ColorF;
class   CShadowVolEdge;
class   CCamera;
class   CDLight;
struct  ILog;
struct  IConsole;
struct  ITimer;
struct  ISystem;
class   ICrySizer;
struct IRenderAuxGeom;
struct SREPointSpriteCreateParams;
struct SPointSpriteVertex;
struct RenderLMData;
struct SShaderParam;
struct SSkyLightRenderParams;
struct SParticleRenderInfo;
struct IFlashPlayer_RenderProxy;
struct IVideoPlayer;
struct IColorGradingController;
struct IStereoRenderer;
struct IFFont_RenderProxy;
struct STextDrawContext;
struct IRenderMesh;

//////////////////////////////////////////////////////////////////////
typedef unsigned char bvec4[4];
typedef float vec4_t[4];
typedef unsigned char byte;
typedef float vec2_t[2];

//DOC-IGNORE-BEGIN
#include "Cry_Color.h"
#include "Tarray.h"

#include <IFont.h>
//DOC-IGNORE-END

#define MAX_RECURSION_LEVELS 2

enum EScreenAspectRatio
{
  eAspect_Unknown,
  eAspect_4_3,
  eAspect_16_9,
  eAspect_16_10,
};

class CRenderCamera
{
public:

  // X: Right
  // Y: Up
  // Z: Back (-Z = Forward)
  Vec3 X, Y, Z;            // Normalized camera coordinate-axis vectors.
  Vec3 Orig;               // Location of origin of camera system in world coords.
  float wL,wR,wB,wT;       // Window extents defined as a rect on NearPlane.
  float Near,Far;          // Distances to near and far planes (in Viewing dir).

  CRenderCamera();
  CRenderCamera(const CRenderCamera &Cam);
  void Copy(const CRenderCamera &Cam);

  void LookAt(const Vec3& Eye, const Vec3& ViewRefPt, const Vec3& ViewUp);
  void Perspective(float Yfov, float Aspect, float Ndist, float Fdist);
  void Frustum(float l, float r, float b, float t, float Ndist, float Fdist);

//  void TightlyFitToSphere(const Vec3& Eye, const Vec3& ViewUp, const Vec3& Cntr, float RadX, float RadY);

  void GetLookAtParams(Vec3 *Eye, Vec3 *ViewRefPt, Vec3 *ViewUp) const;
  void GetPerspectiveParams(float *Yfov, float *Xfov, float *Aspect, float *Ndist, float *Fdist) const;
  void GetFrustumParams(float *l, float *r, float *b, float *t, float *Ndist, float *Fdist) const;
  const Vec3& wCOP() const; // WORLD COORDINATE CENTER-OF-PROJECTION (EYE)
  Vec3 ViewDir() const;     // VIEWING DIRECTION
  Vec3 ViewDirOffAxis() const;

  float* GetXform_Screen2Obj(float* M, int WW, int WH) const;
  float* GetXform_Obj2Screen(float* M, int WW, int WH) const;

  float* GetModelviewMatrix(float* M) const;
  float* GetProjectionMatrix(float* M) const;
  float* GetViewportMatrix(float* M, int WW, int WH) const;

  void SetModelviewMatrix(const float* M);

  float* GetInvModelviewMatrix(float* M) const;
  float* GetInvProjectionMatrix(float* M) const;
  float* GetInvViewportMatrix(float* M, int WW, int WH) const;

  Vec3 WorldToCam(const Vec3 &wP) const;
  float WorldToCamZ(const Vec3 &wP) const;
  Vec3 CamToWorld(const Vec3 &cP) const;

  void LoadIdentityXform();
  void Xform(const float M[16]);

  void Translate(const Vec3& trans);
  void Rotate(const float M[9]);

  void GetPixelRay(float sx, float sy, int ww, int wh, Vec3 *Start, Vec3 *Dir) const;

  void CalcVerts(Vec3 *V) const;    // CALCS EIGHT CORNERS OF VIEW-FRUSTUM

  void CalcTileVerts(Vec3 *V, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const;    // CALCS EIGHT CORNERS FOR TILE OF VIEW-FRUSTUM 

  void CalcRegionVerts(Vec3 *V, const Vec2& vMin, const Vec2& vMax) const;

  void CalcTiledRegionVerts(Vec3 *V, Vec2& vMin, Vec2& vMax, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const;

};

inline float* Frustum16fv(float* M, float l, float r, float b, float t, float n, float f)
{
  M[0]=(2*n)/(r-l); M[4]=0;           M[8]=(r+l)/(r-l);   M[12]=0;
  M[1]=0;           M[5]=(2*n)/(t-b); M[9]=(t+b)/(t-b);   M[13]=0;
  M[2]=0;           M[6]=0;           M[10]=-(f+n)/(f-n); M[14]=(-2*f*n)/(f-n);
  M[3]=0;           M[7]=0;           M[11]=-1;           M[15]=0;
  return M;
}

inline float* Viewing16fv(float* M, const Vec3 X, const Vec3 Y, const Vec3 Z, const Vec3 O)
{
  M[0]=X.x;  M[4]=X.y;  M[8]=X.z;  M[12]=-X|O;
  M[1]=Y.x;  M[5]=Y.y;  M[9]=Y.z;  M[13]=-Y|O;
  M[2]=Z.x;  M[6]=Z.y;  M[10]=Z.z; M[14]=-Z|O;
  M[3]=0;    M[7]=0;    M[11]=0;   M[15]=1;
  return M;
}


inline CRenderCamera::CRenderCamera()
{
  X.Set(1,0,0); Y.Set(0,1,0); Z.Set(0,0,1);
  Orig.Set(0,0,0);
  Near=1.4142f; Far=10; wL=-1; wR=1; wT=1; wB=-1;
}

inline CRenderCamera::CRenderCamera(const CRenderCamera &Cam)
{
  Copy(Cam);
}

inline void CRenderCamera::Copy(const CRenderCamera &Cam)
{
  X=Cam.X;  Y=Cam.Y;  Z=Cam.Z;  Orig=Cam.Orig;
  Near=Cam.Near;  Far=Cam.Far;
  wL=Cam.wL;  wR=Cam.wR;  wT=Cam.wT;  wB=Cam.wB;
}

inline void CRenderCamera::LookAt(const Vec3& Eye, const Vec3& ViewRefPt, const Vec3& ViewUp)
{
  Z = Eye-ViewRefPt;  Z.NormalizeSafe(); 
  X = ViewUp % Z;     X.NormalizeSafe();
  Y = Z % X;          Y.NormalizeSafe();
  Orig = Eye;
}

inline void CRenderCamera::Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
{
  Near = Ndist;  Far=Fdist;
  wT = tanf(Yfov*0.5f)*Near;  wB=-wT;
  wR = wT*Aspect; wL=-wR;
}

inline void CRenderCamera::Frustum(float l, float r, float b, float t, float Ndist, float Fdist)
{
  Near=Ndist;  Far=Fdist;
  wR=r;  wL=l;  wB=b;  wT=t;
}


inline void CRenderCamera::GetLookAtParams(Vec3 *Eye, Vec3 *ViewRefPt, Vec3 *ViewUp) const
{
  *Eye = Orig;
  *ViewRefPt = Orig - Z;
  *ViewUp = Y;
}

inline void CRenderCamera::GetPerspectiveParams(float *Yfov, float *Xfov, float *Aspect, float *Ndist, float *Fdist) const
{
  *Yfov = atanf(wT/Near) * 57.29578f * 2.0f;
  *Xfov = atanf(wR/Near) * 57.29578f * 2.0f;
  *Aspect = wT/wR;
  *Ndist = Near;
  *Fdist = Far;
}

inline void CRenderCamera::GetFrustumParams(float *l, float *r, float *b, float *t, float *Ndist, float *Fdist) const
{
  *l = wL;
  *r = wR;
  *b = wB;
  *t = wT;
  *Ndist = Near;
  *Fdist = Far;
}

inline const Vec3& CRenderCamera::wCOP() const
{
  return( Orig );
}

inline Vec3 CRenderCamera::ViewDir() const
{
  return(-Z);
}

inline Vec3 CRenderCamera::ViewDirOffAxis() const
{
  float x=(wL+wR)*0.5f, y=(wT+wB)*0.5f;  // MIDPOINT ON VIEWPLANE WINDOW
  Vec3 ViewDir = X*x + Y*y - Z*Near;
  ViewDir.Normalize();
  return ViewDir;
}

inline Vec3 CRenderCamera::WorldToCam(const Vec3& wP) const
{
  Vec3 sP(wP-Orig);
  Vec3 cP(X|sP,Y|sP,Z|sP);
  return cP;
}

inline float CRenderCamera::WorldToCamZ(const Vec3& wP) const
{
  Vec3 sP(wP-Orig);
  float zdist = Z|sP;
  return zdist;
}

inline Vec3 CRenderCamera::CamToWorld(const Vec3& cP) const
{
  Vec3 wP(X*cP.x + Y*cP.y + Z*cP.z + Orig);
  return wP;
}

inline void CRenderCamera::LoadIdentityXform()
{
  X.Set(1,0,0);
  Y.Set(0,1,0);
  Z.Set(0,0,1);
  Orig.Set(0,0,0);
}

inline void CRenderCamera::Xform(const float M[16])
{
  X.Set( X.x*M[0] + X.y*M[4] + X.z*M[8],
    X.x*M[1] + X.y*M[5] + X.z*M[9],
    X.x*M[2] + X.y*M[6] + X.z*M[10] );
  Y.Set( Y.x*M[0] + Y.y*M[4] + Y.z*M[8],
    Y.x*M[1] + Y.y*M[5] + Y.z*M[9],
    Y.x*M[2] + Y.y*M[6] + Y.z*M[10] );
  Z.Set( Z.x*M[0] + Z.y*M[4] + Z.z*M[8],
    Z.x*M[1] + Z.y*M[5] + Z.z*M[9],
    Z.x*M[2] + Z.y*M[6] + Z.z*M[10] );
  Orig.Set( Orig.x*M[0] + Orig.y*M[4] + Orig.z*M[8] + M[12],
    Orig.x*M[1] + Orig.y*M[5] + Orig.z*M[9] + M[13],
    Orig.x*M[2] + Orig.y*M[6] + Orig.z*M[10] + M[14] );

  float Scale = X.GetLength();
  X /= Scale;
  Y /= Scale;
  Z /= Scale;

  wL*=Scale;
  wR*=Scale;
  wB*=Scale;
  wT*=Scale;
  Near*=Scale;
  Far*=Scale;
};

inline void CRenderCamera::Translate(const Vec3& trans)
{
  Orig += trans;
}

inline void CRenderCamera::Rotate(const float M[9])
{
  X.Set( X.x*M[0] + X.y*M[3] + X.z*M[6],
    X.x*M[1] + X.y*M[4] + X.z*M[7],
    X.x*M[2] + X.y*M[5] + X.z*M[8] );
  Y.Set( Y.x*M[0] + Y.y*M[3] + Y.z*M[6],
    Y.x*M[1] + Y.y*M[4] + Y.z*M[7],
    Y.x*M[2] + Y.y*M[5] + Y.z*M[8] );
  Z.Set( Z.x*M[0] + Z.y*M[3] + Z.z*M[6],
    Z.x*M[1] + Z.y*M[4] + Z.z*M[7],
    Z.x*M[2] + Z.y*M[5] + Z.z*M[8] );
}

inline float* CRenderCamera::GetModelviewMatrix(float* M) const
{
  Viewing16fv(M,X,Y,Z,Orig);
  return M;
}

inline float* CRenderCamera::GetProjectionMatrix(float* M) const
{
  Frustum16fv(M,wL,wR,wB,wT,Near,Far);
  return(M);  
}

inline void CRenderCamera::GetPixelRay(float sx, float sy, int ww, int wh, Vec3 *Start, Vec3 *Dir) const
{
  Vec3 wTL = Orig + (X*wL) + (Y*wT) - (Z*Near);		// FIND LOWER-LEFT
  Vec3 dX = (X*(wR-wL))/(float)ww;					// WORLD WIDTH OF PIXEL
  Vec3 dY = (Y*(wT-wB))/(float)wh;					// WORLD HEIGHT OF PIXEL
  wTL += (dX*sx - dY*sy);							// INCR TO WORLD PIXEL
  wTL += (dX*0.5f - dY*0.5f);                       // INCR TO PIXEL CNTR
  *Start = Orig;
  *Dir = wTL-Orig;
}

inline void CRenderCamera::CalcVerts(Vec3 *V)  const
{
  float NearZ = -Near;
  V[0].Set(wR,wT,NearZ);
  V[1].Set(wL,wT,NearZ);
  V[2].Set(wL,wB,NearZ);
  V[3].Set(wR,wB,NearZ);

  float FarZ=-Far, FN=Far/Near;
  float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;
  V[4].Set(fwR,fwT,FarZ);
  V[5].Set(fwL,fwT,FarZ);
  V[6].Set(fwL,fwB,FarZ);
  V[7].Set(fwR,fwB,FarZ);

  for (int i=0; i<8; i++)
    V[i] = CamToWorld(V[i]); 
}

inline void CRenderCamera::CalcTileVerts(Vec3 *V, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY)  const
{
	float NearZ = -Near;

	float TileWidth = abs(wR - wL)/nGridSizeX ;
	float TileHeight = abs(wT - wB)/nGridSizeY;
  float TileL = wL + TileWidth * nPosX;
	float TileR = wL + TileWidth * (nPosX + 1);
	float TileB = wB + TileHeight * nPosY;
	float TileT = wB + TileHeight * (nPosY + 1);

	V[0].Set(TileR,TileT,NearZ);
	V[1].Set(TileL,TileT,NearZ);
	V[2].Set(TileL,TileB,NearZ);
	V[3].Set(TileR,TileB,NearZ);

	float FarZ=-Far, FN=Far/Near;
	float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

	float TileFarWidth = abs(fwR - fwL)/nGridSizeX ;
	float TileFarHeight = abs(fwT - fwB)/nGridSizeY;
	float TileFarL = fwL + TileFarWidth * nPosX;
	float TileFarR = fwL + TileFarWidth * (nPosX + 1);
	float TileFarB = fwB + TileFarHeight * nPosY;
	float TileFarT = fwB + TileFarHeight * (nPosY + 1);

	V[4].Set(TileFarR,TileFarT,FarZ);
	V[5].Set(TileFarL,TileFarT,FarZ);
	V[6].Set(TileFarL,TileFarB,FarZ);
	V[7].Set(TileFarR,TileFarB,FarZ);

	for (int i=0; i<8; i++)
		V[i] = CamToWorld(V[i]); 
}

inline void CRenderCamera::CalcTiledRegionVerts(Vec3 *V, Vec2& vMin, Vec2& vMax, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const
{
  float NearZ = -Near;

  Vec2 vTileMin, vTileMax;

  vMin.x = max(vMin.x, nPosX/nGridSizeX);
  vMax.x = min(vMax.x, (nPosX + 1)/nGridSizeX);

  vMin.y = max(vMin.y, nPosY/nGridSizeY);
  vMax.y = min(vMax.y, (nPosY + 1)/nGridSizeY);

  vTileMin.x = abs(wR - wL) * vMin.x;
  vTileMin.y = abs(wT - wB) * vMin.y;
  vTileMax.x = abs(wR - wL) * vMax.x;
  vTileMax.y = abs(wT - wB) * vMax.y;

  float TileWidth = abs(wR - wL)/nGridSizeX ;
  float TileHeight = abs(wT - wB)/nGridSizeY;

  float TileL = wL + vTileMin.x;
  float TileR = wL + vTileMax.x;
  float TileB = wB + vTileMin.y;
  float TileT = wB + vTileMax.y;

  V[0].Set(TileR,TileT,NearZ);
  V[1].Set(TileL,TileT,NearZ);
  V[2].Set(TileL,TileB,NearZ);
  V[3].Set(TileR,TileB,NearZ);

  float FarZ=-Far, FN=Far/Near;
  float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

  Vec2 vTileFarMin, vTileFarMax;

  vTileFarMin.x = abs(fwR - fwL) * vMin.x;
  vTileFarMin.y = abs(fwT - fwB) * vMin.y;
  vTileFarMax.x = abs(fwR - fwL) * vMax.x;
  vTileFarMax.y = abs(fwT - fwB) * vMax.y;

  float TileFarWidth = abs(fwR - fwL)/nGridSizeX ;
  float TileFarHeight = abs(fwT - fwB)/nGridSizeY;

  float TileFarL = fwL + vTileFarMin.x;
  float TileFarR = fwL + vTileFarMax.x;
  float TileFarB = fwB + vTileFarMin.y;
  float TileFarT = fwB + vTileFarMax.y;

  V[4].Set(TileFarR,TileFarT,FarZ);
  V[5].Set(TileFarL,TileFarT,FarZ);
  V[6].Set(TileFarL,TileFarB,FarZ);
  V[7].Set(TileFarR,TileFarB,FarZ);

  for (int i=0; i<8; i++)
    V[i] = CamToWorld(V[i]); 

  //recalculate light quad region for this particular quad 
  /*vMin.x = vMin.x - (vMin.x / nGridSizeX);
  vMax.x = vMax.x - (vMin.x / nGridSizeX);

  vMin.y = vMin.y - (vMin.y / nGridSizeY);
  vMax.y = vMax.y - (vMin.y / nGridSizeY);*/
}


inline void CRenderCamera::CalcRegionVerts(Vec3 *V, const Vec2& vMin, const Vec2& vMax) const
{
  float NearZ = -Near;

  Vec2 vTileMin, vTileMax;

  vTileMin.x = abs(wR - wL) * vMin.x;
  vTileMin.y = abs(wT - wB) * vMin.y;
  vTileMax.x = abs(wR - wL) * vMax.x;
  vTileMax.y = abs(wT - wB) * vMax.y;

  float TileL = wL + vTileMin.x;
  float TileR = wL + vTileMax.x;
  float TileB = wB + vTileMin.y;
  float TileT = wB + vTileMax.y;

  V[0].Set(TileR,TileT,NearZ);
  V[1].Set(TileL,TileT,NearZ);
  V[2].Set(TileL,TileB,NearZ);
  V[3].Set(TileR,TileB,NearZ);

  float FarZ=-Far, FN=Far/Near;
  float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

  Vec2 vTileFarMin, vTileFarMax;

  vTileFarMin.x = abs(fwR - fwL) * vMin.x;
  vTileFarMin.y = abs(fwT - fwB) * vMin.y;
  vTileFarMax.x = abs(fwR - fwL) * vMax.x;
  vTileFarMax.y = abs(fwT - fwB) * vMax.y;

  float TileFarL = fwL + vTileFarMin.x;
  float TileFarR = fwL + vTileFarMax.x;
  float TileFarB = fwB + vTileFarMin.y;
  float TileFarT = fwB + vTileFarMax.y;

  V[4].Set(TileFarR,TileFarT,FarZ);
  V[5].Set(TileFarL,TileFarT,FarZ);
  V[6].Set(TileFarL,TileFarB,FarZ);
  V[7].Set(TileFarR,TileFarB,FarZ);

  for (int i=0; i<8; i++)
    V[i] = CamToWorld(V[i]); 
}


class SBoundingVolume
{
public:
  SBoundingVolume() : m_vCenter(0, 0, 0), m_fRadius(0) {}
  ~SBoundingVolume() {}

  void SetCenter(const Vec3 &center)  { m_vCenter = center; }
  void SetRadius(float radius)        { m_fRadius = radius; }
  const Vec3& GetCenter() const       { return m_vCenter;   }
  float GetRadius() const             { return m_fRadius;   }

protected:
  Vec3    m_vCenter;
  float   m_fRadius;
};

class SMinMaxBox : public SBoundingVolume
{
public:
  SMinMaxBox()
  {
    Clear();
  }
  // Summary:
  //	Destructor
  virtual ~SMinMaxBox() {}

  void  AddPoint(const Vec3 &pt)
  {
    if(pt.x > m_max.x)
      m_max.x = pt.x;
    if(pt.x < m_min.x)
      m_min.x = pt.x;

    if(pt.y > m_max.y)
      m_max.y = pt.y;
    if(pt.y < m_min.y)
      m_min.y = pt.y;

    if(pt.z > m_max.z )
      m_max.z = pt.z;
    if(pt.z < m_min.z )
      m_min.z = pt.z;

	// Summary:
    //	 Updates the center and radius.
    UpdateSphere();
  }
  void  AddPoint(float x , float y , float z)
  {
    AddPoint(Vec3(x, y, z));
  }

  void  Union(const SMinMaxBox& box)  { AddPoint(box.GetMin()); AddPoint(box.GetMax()); }

  const Vec3 &GetMin() const     { return m_min; }
  const Vec3 &GetMax() const     { return m_max; }

  void  SetMin(const Vec3 &min)  { m_min = min; UpdateSphere(); }
  void  SetMax(const Vec3 &max)  { m_max = max; UpdateSphere(); }

  float GetWidthInX() const       { return m_max.x - m_min.x;}
  float GetWidthInY() const       { return m_max.y - m_min.y;}
  float GetWidthInZ() const       { return m_max.z - m_min.z;}

  bool  PointInBBox(const Vec3 &pt) const;

  bool  ViewFrustumCull(const CRenderCamera &cam, const Matrix44 &mat);

  void  Transform(const Matrix34& mat)
  {
    Vec3 verts[8];
    CalcVerts(verts);
    Clear();
    for (int i=0; i<8; i++)
    {
      AddPoint(mat.TransformPoint(verts[i]));
    }
  }

  // Summary:
  //	Resets the bounding box.
  void  Clear()
  {
    m_min = Vec3(999999.0f, 999999.0f, 999999.0f);
    m_max = Vec3(-999999.0f, -999999.0f, -999999.0f);
  }

protected:
  void UpdateSphere()
  {
    m_vCenter =  m_min;
    m_vCenter += m_max;
    m_vCenter *= 0.5f;

    Vec3 rad  =  m_max;
    rad      -= m_vCenter;
    m_fRadius =  rad.len();
  }
  void CalcVerts(Vec3 pVerts[8]) const
  {
    pVerts[0].Set(m_max.x, m_max.y, m_max.z); pVerts[4].Set(m_max.x, m_max.y, m_min.z);
    pVerts[1].Set(m_min.x, m_max.y, m_max.z); pVerts[5].Set(m_min.x, m_max.y, m_min.z);
    pVerts[2].Set(m_min.x, m_min.y, m_max.z); pVerts[6].Set(m_min.x, m_min.y, m_min.z);
    pVerts[3].Set(m_max.x, m_min.y, m_max.z); pVerts[7].Set(m_max.x, m_min.y, m_min.z);
  }

private:
  Vec3 m_min;   // Original object space BV.
  Vec3 m_max;
};



//////////////////////////////////////////////////////////////////////
#define R_CULL_DISABLE  0
#define R_CULL_NONE     0
#define R_CULL_FRONT    1
#define R_CULL_BACK     2

//////////////////////////////////////////////////////////////////////
#define R_FOGMODE_LINEAR  1
#define R_FOGMODE_EXP2    2

//////////////////////////////////////////////////////////////////////
#define R_DEFAULT_LODBIAS 0

//////////////////////////////////////////////////////////////////////
// All possible primitive types
enum eRenderPrimitiveType
{
	eptTriangleList = 0,
	eptTriangleStrip,
	eptLineList,
	eptLineStrip,
	eptPointList,



	eptNumRenderablePrimitives,	// should be always the concluding number of all GPU-native primitive types

	// non-real primitives, used for logical batching
	eptMultiTriangleStrips = eptNumRenderablePrimitives,
	eptHWSkinGroups
};

//////////////////////////////////////////////////////////////////////
#define FILTER_NONE      -1
#define FILTER_POINT      0
#define FILTER_LINEAR     1
#define FILTER_BILINEAR   2
#define FILTER_TRILINEAR  3
#define FILTER_ANISO2X    4
#define FILTER_ANISO4X    5
#define FILTER_ANISO8X    6
#define FILTER_ANISO16X   7

//////////////////////////////////////////////////////////////////////
#define TADDR_WRAP        0
#define TADDR_CLAMP       1
#define TADDR_MIRROR      2
#define TADDR_BORDER      3

//////////////////////////////////////////////////////////////////////
#define R_SOLID_MODE    0
#define R_WIREFRAME_MODE 1
#define R_POINT_MODE		2

#define R_GL_RENDERER   0
#define R_DX9_RENDERER  2
#define R_DX11_RENDERER 3
#define R_NULL_RENDERER 4
#define R_CUBAGL_RENDERER 5

//////////////////////////////////////////////////////////////////////
// Render features

#define RFT_HW_R2VB				1		// Render to vertex buffer supported.
#define RFT_OCCLUSIONQUERY 4
#define RFT_PALTEXTURE   8				// Support paletted textures.
#define RFT_HWGAMMA      0x10
#define RFT_ALLOWRECTTEX  0x20			// Allow non-power-of-two textures.
#define RFT_COMPRESSTEXTURE  0x40
#define RFT_ALLOWANISOTROPIC 0x100		// Allows anisotropic texture filtering.
#define RFT_SUPPORTZBIAS     0x200
#define RFT_ALLOWSECONDCOLOR 0x800
#define RFT_DETAILTEXTURE    0x1000
#define RFT_TEXGEN_REFLECTION 0x2000
#define RFT_TEXGEN_EMBOSS     0x4000
#define RFT_OCCLUSIONTEST     0x8000	// Support hardware occlusion test.

#define RFT_HW_GF2        0x10000		// GF2 class hardware (ATI Radeon 7500 as well :) ).
#define RFT_HW_GF3        0x20000		// NVidia GF3 class hardware (ATI Radeon 8500 as well :) ).
#define RFT_HW_ATI		  0x30000		// Unclassified ATI hardware.
#define RFT_HW_NVIDIA     0x40000		// Unclassified NVidia hardware.
#define RFT_HW_GFFX       0x50000		// Geforce FX class hardware.
#define RFT_HW_NV4X       0x60000		// NV4X class hardware.
#define RFT_HW_MASK       0x70000		// Graphics chip mask.

#define RFT_HW_HDR        0x80000		// Hardware supports high dynamic range rendering.

#define RFT_HW_PS20       0x100000		// Pixel shaders 2.0
#define RFT_HW_PS2X       0x200000		// Pixel shaders 2.X
#define RFT_HW_PS30       0x400000		// Pixel shaders 3.0
#define RFT_HW_PS40       0x800000		// Pixel shaders 4.0

#define RFT_ZLOCKABLE     0x4000000		// Depth buffer can be locked for read.
#define RFT_DIRECTACCESSTOVIDEOMEMORY   0x10000000
#define RFT_RGBA          0x20000000 // RGBA order (otherwise BGRA).
#define RFT_DEPTHMAPS     0x40000000 // Depth maps are supported.
#define RFT_HW_VERTEXTEXTURES 0x80000000 // Vertex texture fetching supported.

//====================================================================
// PrecacheResources flags

#define FPR_NEEDLIGHT     1
#define FPR_2D            2
#define FPR_HIGHPRIORITY  4
#define FPR_SYNCRONOUS    8
#define FPR_STARTLOADING	16
#define FPR_SINGLE_FRAME_PRIORITY_UPDATE 32

//=====================================================================
// SetRenderTarget flags
#define SRF_SCREENTARGET  1
#define SRF_USE_ORIG_DEPTHBUF 2
#define SRF_USE_ORIG_DEPTHBUF_FSAA 4

//====================================================================
// Draw shaders flags (EF_EndEf3d)

#define SHDF_ALLOWHDR               (1<<0)
#define SHDF_SORT                   (1<<1)
#define SHDF_ZPASS                  (1<<2)
#define SHDF_ZPASS_ONLY             (1<<3)
#define SHDF_DO_NOT_CLEAR_Z_BUFFER  (1<<4)
#define SHDF_ALLOWPOSTPROCESS       (1<<5)
#define SHDF_DO_NOT_RENDER_TARGET   (1<<6)
#define SHDF_CLEAR_SHADOW_MASK      (1<<7)
#define SHDF_ALLOW_AO               (1<<8)
#define SHDF_ALLOW_WATER            (1<<9)
#define SHDF_NOASYNC                (1<<10)
#define SHDF_NO_DRAWNEAR            (1<<11)
#define SHDF_NO_SCISSOR             (1<<12)
#define SHDF_STREAM_SYNC            (1<<13)
#define SHDF_NO_DRAWCAUSTICS        (1<<14)

//////////////////////////////////////////////////////////////////////
// Object states
#define OS_ALPHA_BLEND             0x1
#define OS_ADD_BLEND               0x2
#define OS_MULTIPLY_BLEND          0x4
#define OS_TRANSPARENT            (OS_ALPHA_BLEND | OS_ADD_BLEND | OS_MULTIPLY_BLEND)
#define OS_NODEPTH_TEST            0x8
#define OS_DIFFUSE_CUBEMAP         0x10
#define OS_ANIM_BLEND              0x20

// Render State flags
#define GS_BLSRC_MASK              0xf
#define GS_BLSRC_ZERO              0x1
#define GS_BLSRC_ONE               0x2
#define GS_BLSRC_DSTCOL            0x3
#define GS_BLSRC_ONEMINUSDSTCOL    0x4
#define GS_BLSRC_SRCALPHA          0x5
#define GS_BLSRC_ONEMINUSSRCALPHA  0x6
#define GS_BLSRC_DSTALPHA          0x7
#define GS_BLSRC_ONEMINUSDSTALPHA  0x8
#define GS_BLSRC_ALPHASATURATE     0x9
#define GS_BLSRC_SRCALPHA_A_ZERO   0xa // separate alpha blend state


#define GS_BLDST_MASK              0xf0
#define GS_BLDST_ZERO              0x10
#define GS_BLDST_ONE               0x20
#define GS_BLDST_SRCCOL            0x30
#define GS_BLDST_ONEMINUSSRCCOL    0x40
#define GS_BLDST_SRCALPHA          0x50
#define GS_BLDST_ONEMINUSSRCALPHA  0x60
#define GS_BLDST_DSTALPHA          0x70
#define GS_BLDST_ONEMINUSDSTALPHA  0x80
#define GS_BLDST_ONE_A_ZERO        0x90 // separate alpha blend state


#define GS_DEPTHWRITE              0x00000100

#define GS_COLMASK_RT1             0x00000200
#define GS_COLMASK_RT2             0x00000400
#define GS_COLMASK_RT3             0x00000800

#define GS_NOCOLMASK_R             0x00001000
#define GS_NOCOLMASK_G             0x00002000
#define GS_NOCOLMASK_B             0x00004000
#define GS_NOCOLMASK_A             0x00008000
#define GS_COLMASK_RGB             (GS_NOCOLMASK_A)
#define GS_COLMASK_A               (GS_NOCOLMASK_R | GS_NOCOLMASK_G | GS_NOCOLMASK_B)
#define GS_COLMASK_NONE            (GS_NOCOLMASK_R | GS_NOCOLMASK_G | GS_NOCOLMASK_B | GS_NOCOLMASK_A)
#define GS_COLMASK_MASK            GS_COLMASK_NONE
#define GS_COLMASK_SHIFT           12

#define GS_WIREFRAME               0x00010000
#define GS_POINTRENDERING          0x00020000
#define GS_NODEPTHTEST             0x00040000

#define GS_BLEND_OP_MASK           0x00080000
#define GS_BLOP_MAX								 0x00080000	

#define GS_BLEND_MASK              0x000800ff

#define GS_DEPTHFUNC_LEQUAL        0x00000000
#define GS_DEPTHFUNC_EQUAL         0x00100000
#define GS_DEPTHFUNC_GREAT         0x00200000
#define GS_DEPTHFUNC_LESS          0x00300000
#define GS_DEPTHFUNC_GEQUAL        0x00400000
#define GS_DEPTHFUNC_NOTEQUAL      0x00500000
#define GS_DEPTHFUNC_MASK          0x00700000

#define GS_STENCIL                 0x00800000

#define GS_ALPHATEST_MASK          0xf0000000
#define GS_ALPHATEST_GREATER       0x10000000
#define GS_ALPHATEST_LESS          0x20000000
#define GS_ALPHATEST_GEQUAL        0x40000000
#define GS_ALPHATEST_LEQUAL        0x80000000

#define FORMAT_8_BIT   8
#define FORMAT_24_BIT 24
#define FORMAT_32_BIT 32

// Read FrameBuffer type
enum ERB_Type
{
  eRB_BackBuffer,
  eRB_FrontBuffer,
  eRB_ShadowBuffer
};

enum EVertexCostTypes
{
	EVCT_STATIC = 0,
	EVCT_VEGETATION,
	EVCT_SKINNED,
	EVCT_NUM
};

//////////////////////////////////////////////////////////////////////

struct SDispFormat
{
  int m_Width;
  int m_Height;
  int m_BPP;
};

struct SAAFormat
{
  char szDescr[64];
  int nSamples;
  int nQuality;
  int nAPIType;
};

// Summary:
//	 Info about Terrain sector texturing.
struct SSectorTextureSet
{
  SSectorTextureSet(unsigned short nT0, unsigned short nT1)
		: stencilBox(Vec3(0,0,0),Vec3(0,0,0)),
			nodeBox(Vec3(0,0,0),Vec3(0,0,0))
  {
    nTex0 = nT0;
    nTex1 = nT1;
    fTerrainMaxZ=fTerrainMinZ=fTexOffsetX=fTexOffsetY=0; 
    fTexScale=1.f;
  }

  unsigned short nTex0, nTex1;
  float fTexOffsetX,fTexOffsetY,fTexScale,fTerrainMinZ,fTerrainMaxZ;
  AABB nodeBox;
  AABB stencilBox;
};

struct SBuildTerrainTextureInfo
{
  int nTexId_Diff;
  int nTexId_Bump;
  int nTexId_Spec;
  float fTiling;
  ColorF filterColor;
  int nSortOrder;
  float fSpecularAmount;
  float fDetailAmount;
  float fUseRemeshing;
  struct IMaterial * pMatTerrainLayer;
};

// Summary:
//	 Parameters for terrain texture builder.

#define VOX_TEX_NUM_SRC 3
#define VOX_TEX_NUM_DST 2

struct SBuildTerrainTextureParams
{
  SBuildTerrainTextureParams() { ZeroStruct(*this); }
  uint16 nDstTexDimX;
  uint16 nDstTexDimY;
  uint16 nSyncTextures;
  uint16 nSrcTexDimX;
  uint16 nSrcTexDimY;
  class IDynTexture * pDstTex_Diff;
  class IDynTexture * pDstTex_Bump;
  struct IRenderMesh * pRM;
  float fMeshNodeSize;
  float fLodDistance;
  float fTexRangeScale;
  SBuildTerrainTextureInfo arrSrcTextures[32];
  bool bHeightMapMode;
  bool bOverlayBlend;
  bool bMixMask;
  int nBumpDownScale;
  Vec3 vMMSrcNodeInfo;

  struct SBuildItem
  {
    SBuildItem() { pRenderNode=0;nTerrainLayerId=-1; ZeroStruct(nRemeshTexId); }
    struct IRenderNode * pRenderNode;
    int nTerrainLayerId;
    int nRemeshTexId[VOX_TEX_NUM_SRC];
  };

  PodArray<SBuildItem> * pDecalsAndRoadsAndTerrainLayers;
  Vec3 vMeshWSPos;
  Vec3 vBoxMin;
};

struct SShaderItem;
struct IParticleVertexCreator;

struct SAddParticlesToSceneJob
{
	enum EHalfResFlag
	{
		eHalfResNotAllow = 0,
		eHalfResAllow,
		eHalfResIgnore, // special case for empty particles
		eHalfResForce
	};
	SShaderItem* pShaderItem;
	CRenderObject* pRenderObject;
	IParticleVertexCreator* pPVC;
	EHalfResFlag eAllowHalfRes;
	bool bAfterWater;
	bool bCanUseGeomShader;
	bool bAllowMerging;
	int16 nCustomTexId;
	class CREParticle* pREParticle;

	void GetMemoryUsage( ICrySizer* pSizer ) const {}
} 
_ALIGN(16)
;

//////////////////////////////////////////////////////////////////////
/*struct IndexedVertexBuffer
{
  PodArray<unsigned short> indices;
  CVertexBuffer * pVertexBuffer;
  int strip_step;
  Vec3 vBoxMin,vBoxMax;
};*/

//////////////////////////////////////////////////////////////////////
//DOC-IGNORE-BEGIN
#include "IShader.h"
//DOC-IGNORE-END
#include <IRenderMesh.h>

// Flags passed in function FreeResources.
#define FRR_SHADERS   1
#define FRR_SHADERTEXTURES 2
#define FRR_TEXTURES  4
#define FRR_SYSTEM    8
#define FRR_RESTORE   0x10
#define FRR_REINITHW  0x20
#define FRR_DELETED_MESHES 0x40
#define FRR_FLUSH_TEXTURESTREAMING 0x80
#define FRR_OBJECTS		0x100
#define FRR_RENDERELEMENTS 0x200
#define FRR_RP_BUFFERS 0x400
#define FRR_SYSTEM_RESOURCES 0x800
#define FRR_POST_EFFECTS 0x1000
#define FRR_ALL      -1

// Refresh render resources flags.
// Flags passed in function RefreshResources.
#define FRO_SHADERS  1
#define FRO_SHADERTEXTURES  2
#define FRO_TEXTURES 4
#define FRO_GEOMETRY 8
#define FRO_FORCERELOAD 0x10

//=============================================================================
// Shaders render target stuff.

#define FRT_CLEAR_COLOR   0x1
#define FRT_CLEAR_DEPTH   0x2
#define FRT_CLEAR_STENCIL 0x4
#define FRT_CLEAR (FRT_CLEAR_COLOR | FRT_CLEAR_DEPTH | FRT_CLEAR_STENCIL)
#define FRT_CLEAR_FOGCOLOR 0x8
#define FRT_CLEAR_IMMEDIATE 0x10
#define FRT_CLEAR_COLORMASK 0x20

#define FRT_CAMERA_REFLECTED_WATERPLANE 0x40
#define FRT_CAMERA_REFLECTED_PLANE      0x80
#define FRT_CAMERA_CURRENT              0x100

#define FRT_USE_FRONTCLIPPLANE          0x200
#define FRT_USE_BACKCLIPPLANE           0x400

#define FRT_GENERATE_MIPS               0x800

#define FRT_RENDTYPE_CUROBJECT          0x1000
#define FRT_RENDTYPE_CURSCENE           0x2000
#define FRT_RENDTYPE_RECURSIVECURSCENE  0x4000
#define FRT_RENDTYPE_COPYSCENE          0x8000

// Summary:
//	 Flags used in DrawText function.
// See also:
//	 SDrawTextInfo
// Remarks:
//	 Text must be fixed pixel size.
enum EDrawTextFlags
{
  eDrawText_Center        = BIT(1),		// centered alignment, otherwise right or left
  eDrawText_Right         = BIT(2),		// right alignment, otherwise center or left
	eDrawText_CenterV       = BIT(8),		// center vertically, oterhwise top

	eDrawText_2D            = BIT(4),		// 3 component vector is used for xy screen position, otherwise it's 3d world space position

	eDrawText_FixedSize     = BIT(3),		// font size is defined in the actual pixel resolution, otherwise it's in the virtual 800x600
	eDrawText_800x600       = BIT(5),		// position are specified in the virtual 800x600 resolution, otherwise coordinates are in pixels

	eDrawText_Monospace     = BIT(6),		// non proportional font rendering (Font width is same for all characters)

	eDrawText_Framed				= BIT(7),		// draw a transparent, rectangular frame behind the text to ease readability independent from the background
};

//////////////////////////////////////////////////////////////////////////
// Description:
//	 This structure used in DrawText method of renderer.
//	 It provide all necessary information of how to render text on screen.
// See also:
//	 IRenderer::Draw2dText
struct SDrawTextInfo
{
  // Summary:
  //	One of EDrawTextFlags flags.
  // See also:
  //	EDrawTextFlags
  int     flags;
  // Summary:
  //	Text color, (r,g,b,a) all members must be specified.
  float   color[4];
  float xscale;
  float yscale;

  SDrawTextInfo()
  {
    flags = 0;
    color[0] = color[1] = color[2] = color[3] = 1;
    xscale=1.0f;
		yscale=1.0f;
  }
};

#define UIDRAW_TEXTSIZEFACTOR (12.0f)


//SLI/CROSSFIRE GPU maximum count
#define MAX_GPU_NUM 4

#define MAX_FRAME_ID_STEP_PER_FRAME 8
const int MAX_GSM_LODS_NUM = 8;

const f32 DRAW_NEAREST_MIN = 0.03f;
const f32 DRAW_NEAREST_MAX = 40.0f;

//===================================================================

//////////////////////////////////////////////////////////////////////////
#ifndef EXCLUDE_SCALEFORM_SDK
struct SSF_GlobalDrawParams
{
	enum EFillType
	{
		None,

		SolidColor,
		Texture,

		GlyphTexture,
		GlyphAlphaTexture,

		GlyphTextureYUV,
		GlyphTextureYUVA,

		GColor,
		G1Texture,
		G1TextureColor,
		G2Texture,
		G2TextureColor,
		G3Texture
	};
	EFillType fillType;

	enum EVertexFmt
	{
		Vertex_None,

		Vertex_XY16i,
		Vertex_XY16iC32,
		Vertex_XY16iCF32,
		Vertex_Glyph,
	};
	EVertexFmt vertexFmt;
	const void* pVertexPtr;
	uint32 numVertices;

	enum EIndexFmt
	{
		Index_None,

		Index_16
	};
	EIndexFmt indexFmt;
	const void* pIndexPtr;
	uint32 numIndices;

	const Matrix44* pTransMat;

	enum ETexState
	{
		TS_Clamp				= 0x01,

		TS_FilterLin		= 0x02,
		TS_FilterTriLin	= 0x04
	};

	struct STextureInfo
	{
		int texID;
		uint32 texState;
		Matrix34 texGenMat;
	};
	STextureInfo texture[2];

	int texID_YUVA[4];
	Vec2 texGenYUVAStereo;

	ColorF colTransform1st;
	ColorF colTransform2nd;

	uint32 blendModeStates;
	uint32 renderMaskedStates;

	bool isMultiplyDarkBlendMode;
	bool premultipliedAlpha;

	enum EAlphaBlendOp
	{
		Add,
		Substract,
		RevSubstract,
		Min,
		Max
	};
	EAlphaBlendOp blendOp;

	//////////////////////////////////////////////////////////////////////////
	SSF_GlobalDrawParams()
	{
		Reset();
	}

	//////////////////////////////////////////////////////////////////////////
	void Reset()
	{
		fillType = None;

		vertexFmt = Vertex_None;
		pVertexPtr = 0;
		numVertices = 0;

		indexFmt = Index_None;
		pIndexPtr = 0;
		numIndices = 0;

		pTransMat = 0;

		texture[0].texID = -1;
		texture[0].texState = 0;
		texture[0].texGenMat.SetIdentity();
		texture[1].texID = -1;
		texture[1].texGenMat.SetIdentity();
		texture[1].texState = 0;

		texID_YUVA[0] = texID_YUVA[1] = texID_YUVA[2] = texID_YUVA[3] = -1;

		colTransform1st = ColorF(0, 0, 0, 0);
		colTransform2nd = ColorF(0, 0, 0, 0);

		blendModeStates = 0;
		renderMaskedStates = 0;

		isMultiplyDarkBlendMode = false;
		premultipliedAlpha = false;

		blendOp = Add;
	}
};
#endif // #ifndef EXCLUDE_SCALEFORM_SDK

//////////////////////////////////////////////////////////////////////
UNIQUE_IFACE struct IRendererEventListener
{
	virtual void OnPostCreateDevice	() = 0;
	virtual void OnPostResetDevice	() = 0;
	virtual ~IRendererEventListener(){}
};

//////////////////////////////////////////////////////////////////////////
struct ILoadtimeFlashPlayerCallback
{
	virtual struct IFlashPlayer* OnLevelLoadStart() = 0;
	virtual void OnLevelLoadEnd() = 0;

protected: 
  virtual ~ILoadtimeFlashPlayerCallback() {}; 
};

//////////////////////////////////////////////////////////////////////
struct ILoadtimeCallback
{
	virtual void LoadtimeUpdate(float fDeltaTime) = 0;
	virtual void LoadtimeRender() = 0;
	virtual ~ILoadtimeCallback(){}
};

//////////////////////////////////////////////////////////////////////
struct STexturePoolAllocation
{
	STexturePoolAllocation(int allocsize, string path)
	{
		size = allocsize;
		texturepath = path;
	}
	int size;
	string texturepath;
};

//////////////////////////////////////////////////////////////////////
enum ERenderType
{
	eRT_Undefined,
	eRT_Null,
	eRT_DX9,
	eRT_DX11,
	eRT_Xbox360,
	eRT_PS3	
};

//////////////////////////////////////////////////////////////////////
// Enum for types of deferred lights
enum eDeferredLightType
{
	eDLT_DeferredLight = 0,
	eDLT_DeferredAmbientLight,
	eDLT_NumShadowCastingLights = eDLT_DeferredAmbientLight + 1,
	// these lights cannot cast shadows
	eDLT_DeferredCubemap = eDLT_NumShadowCastingLights,
	eDLT_DeferredNegative,
	eDLT_NumLightTypes,
};

//////////////////////////////////////////////////////////////////////
struct SCustomRenderInitArgs
{
	bool appStartedFromMediaCenter;
};


//////////////////////////////////////////////////////////////////////
// Description:
//	 Z-buffer as occlusion buffer definitions: used, shared and initialized in engine and renderer.
struct SHWOccZBuffer
{
	uint32* pHardwareZBuffer;
	uint32* pZBufferVMem;
	uint32 ZBufferSizeX;
	uint32 ZBufferSizeY;
	uint32 HardwareZBufferRSXOff;
	uint32 ZBufferVMemRSXOff;
	uint32 pad[2];	// Keep 32 byte aligned
	SHWOccZBuffer() : pHardwareZBuffer(NULL), pZBufferVMem(NULL), ZBufferSizeX(256), ZBufferSizeY(256),
		ZBufferVMemRSXOff(0), HardwareZBufferRSXOff(0){}
} 



;

struct MTRenderInfo 
{
	float fWaitForMain;
	float fWaitForRender;
	float fWaitForGPU;
	float fGPUIdlePerc;
	float fFrameTime;
	float fRenderTime;
	float fSpuMainLoad;
	float fSpuDXPSLoad;
};


typedef void (*OnPostRenderCallbackFunc)();

struct SRendererCallbacks
{
	OnPostRenderCallbackFunc OnPostRender;	
};

//////////////////////////////////////////////////////////////////////
struct IRenderer//: public IRendererCallbackServer
{
	virtual ~IRenderer(){}
	virtual void AddListener		(IRendererEventListener *pRendererEventListener) = 0;
	virtual void RemoveListener	(IRendererEventListener *pRendererEventListener) = 0;

	virtual ERenderType GetRenderType() const = 0;

	// Summary:
	//	Initializes the renderer, params are self-explanatory.
	virtual WIN_HWND Init(int x,int y,int width,int height,unsigned int cbpp, int zbpp, int sbits, bool fullscreen,WIN_HINSTANCE hinst, WIN_HWND Glhwnd=0, bool bReInit=false, const SCustomRenderInitArgs* pCustomArgs=0, bool bShaderCacheGen = false)=0;
	virtual void PostInit() = 0;

	// start active rendering of the intro movies while initializing the rest of the engine
	virtual void StartRenderIntroMovies() = 0;
	virtual void StopRenderIntroMovies(bool bWaitForFinished) = 0;
	virtual bool IsRenderingIntroMovies() const = 0;

	virtual bool IsPost3DRendererEnabled() const { return false; }

	virtual int GetFeatures()=0;
	virtual void GetVideoMemoryUsageStats( size_t& vidMemUsedThisFrame, size_t& vidMemUsedRecently, bool bGetPoolsSizes = false  )=0;
	virtual int GetCurrentNumberOfDrawCalls()=0;
	virtual void GetCurrentNumberOfDrawCalls(int &nGeneral,int &nShadowGen)=0;
	//Sums DIP counts for the EFSLIST_* passes that match the submitted mask.
	//Compose the mask with bitwise arithmetic, use (1 << EFSLIST_*) per list.
	//e.g. to sum general and transparent, pass in ( (1 << EFSLIST_GENERAL) | (1 << EFSLIST_TRANSP) )
	//Please note that this doesn't subtract the global count of DIPs skipped on the PS3 due to conditional rendering 3 as it isn't per-pass information
	virtual int GetCurrentNumberOfDrawCalls(const uint32 EFSListMask)=0;
	virtual float GetCurrentDrawCallRTTimes(const uint32 EFSListMask)=0;

	virtual const MTRenderInfo& GetMTRenderInfo() const  = 0;

	virtual void SetDebugRenderNode(IRenderNode* pRenderNode)=0;
	virtual bool IsDebugRenderNode(IRenderNode* pRenderNode) const = 0;

	virtual bool SetCurrentContext(WIN_HWND hWnd)=0;
	virtual bool CreateContext(WIN_HWND hWnd, bool bAllowFSAA=false)=0;
	virtual bool DeleteContext(WIN_HWND hWnd)=0;
	virtual void MakeMainContextActive()=0;
	virtual WIN_HWND GetCurrentContextHWND()=0;






	SRendererCallbacks m_sRendererCallbacks;

	// Summary:
	//	Shuts down the renderer.
	virtual void  ShutDown(bool bReInit=false)=0;
	virtual void  ShutDownFast()=0;

	// Description:
	//	Creates array of all supported video formats (except low resolution formats).
	// Return value:
	//	Number of formats in memory.
	virtual int EnumDisplayFormats(SDispFormat *Formats)=0;

	// Summary:
	//	Returns all supported by video card video AA formats.
	virtual int EnumAAFormats(const SDispFormat &rDispFmt, SAAFormat *Formats)=0;

	// Summary:
	//	Changes resolution of the window/device (doesn't require to reload the level.
	virtual bool  ChangeResolution(int nNewWidth, int nNewHeight, int nNewColDepth, int nNewRefreshHZ, bool bFullScreen, bool bForceReset)=0;

	// Summary:
	//	Shuts down the renderer.
	virtual void  Release()=0;

  // Summary:
  //	Creates default system shaders and textures.
  virtual void  InitSystemResources(int nFlags)=0;

	// Summary:
	//	Frees the allocated resources.
	virtual void  FreeResources(int nFlags)=0;

	// Note:
	//	Should be called at the beginning of every frame.
	virtual void  BeginFrame()=0;

	// See also:
	//	 r_ShowDynTextures
	virtual void RenderDebug(bool bRenderStats=true)=0;

	// Note:
	//	 Should be called at the end of every frame.
	virtual void  EndFrame()=0;

	// Force a swap on the backbuffer
	virtual void	ForceSwapBuffers()=0;

	// Summary:
	//		Try to flush the render thread commands to keep the render thread active during
	//		level loading, but simpy return if the render thread is still busy
	virtual void TryFlush() = 0;

	virtual void  GetViewport(int *x, int *y, int *width, int *height)=0;
	virtual void  SetViewport(int x, int y, int width, int height)=0;
	virtual	void	SetRenderTile(f32 nTilesPosX=0.f, f32 nTilesPosY=0.f, f32 nTilesGridSizeX=1.f, f32 nTilesGridSizeY=1.f)=0;
	virtual void  SetScissor(int x=0, int y=0, int width=0, int height=0)=0;

	virtual EScreenAspectRatio GetScreenAspect(int nWidth, int nHeight)=0;

	// Summary:
	//	Draws user primitives.
	virtual void DrawPrimitives(CVertexBuffer *src, int vert_num=4, const eRenderPrimitiveType prim_type=eptTriangleStrip)=0;

	virtual int  GetDynVBSize(EVertexFormat eVF=eVF_P3F_C4B_T2F) = 0;
	virtual void DrawDynVB(SVF_P3F_C4B_T2F *pBuf, uint16 *pInds, int nVerts, int nInds, const eRenderPrimitiveType nPrimType) = 0;

	// Summary:
	//	Draws a vertex buffer.
	virtual void  DrawBuffer(CVertexBuffer *pVBuf, CIndexBuffer *pIBuf, int nNumIndices, int nOffsIndex, const eRenderPrimitiveType nPrmode, int nVertStart=0, int nVertStop=0)=0;

	// Summary:
	//	Sets the renderer camera.
	virtual void  SetCamera(const CCamera &cam)=0;

	// Summary:
	//	Gets the renderer camera.
	virtual const CCamera& GetCamera()=0;

	// Summary:
	//	Sets delta gamma.
	virtual bool  SetGammaDelta(const float fGamma)=0;

	// Summary:
	//	Restores gamma 
	// Note:
	//	Reset gamma setting if not in fullscreen mode.
	virtual void  RestoreGamma(void)=0;

	// Summary:
	//	Changes display size.
	virtual bool  ChangeDisplay(unsigned int width,unsigned int height,unsigned int cbpp)=0;

	// Summary:
	//	Changes viewport size.
	virtual void  ChangeViewport(unsigned int x,unsigned int y,unsigned int width,unsigned int height,bool bMainViewport = false)=0;

	// Summary:
	//	Saves source data to a Tga file.
	// Note:
	//	Should not be here.
	virtual bool  SaveTga(unsigned char *sourcedata,int sourceformat,int w,int h,const char *filename,bool flip) const=0;

	// Summary:
	//	Sets the current binded texture.
	virtual void  SetTexture(int tnum)=0; 

	// Summary:
	//	Sets the white texture.
	virtual void  SetWhiteTexture()=0;

	// Summary:
	//	Draws a 2d image on the screen. 
	// Example:
	//	Hud etc.
	virtual void  Draw2dImage(float xpos,float ypos,float w,float h,int texture_id,float s0=0,float t0=0,float s1=1,float t1=1,float angle=0,float r=1,float g=1,float b=1,float a=1,float z=1)=0;

	// Summary:
	//	Adds a 2d image that should be drawn on the screen to an internal render list. The list can be drawn with Draw2dImageList.
	//	If several images will be drawn, using this function is more efficient than calling Draw2dImage as it allows better batching.
	//  The function supports placing images in stereo 3d space.
	// Arguments:
	//		stereoDepth - Places image in stereo 3d space. The depth is specified in camera space, the stereo params are the same that
	//									are used for the scene. A value of 0 is handled as a special case and always places the image on the screen plane.
	virtual void  Push2dImage(float xpos,float ypos,float w,float h,int texture_id,float s0=0,float t0=0,float s1=1,float t1=1,float angle=0,float r=1,float g=1,float b=1,float a=1,float z=1,float stereoDepth=0)=0;
	// Summary:
	//	Draws all images to the screen that were collected with Push2dImage.
	virtual void  Draw2dImageList()=0;

	// Summary:
	//	Draws a image using the current matrix.
	virtual void DrawImage(float xpos,float ypos,float w,float h,int texture_id,float s0,float t0,float s1,float t1,float r,float g,float b,float a,bool filtered=true)=0;


	// Description:
	//	Draws a image using the current matrix, more flexible than DrawImage
	//	order for s and t: 0=left_top, 1=right_top, 2=right_bottom, 3=left_bottom.
	virtual void DrawImageWithUV(float xpos,float ypos,float z,float width,float height,int texture_id,float *s,float *t,float r=1,float g=1,float b=1,float a=1,bool filtered=true)=0;

	// Summary:
	//	Sets the polygon mode 
	// Example:
	//	Wireframe, solid.
	virtual int SetWireframeMode(int mode)=0;

	// Summary:
	//	Gets screen width.
	virtual int   GetWidth() = 0;

	// Summary:
	//	Gets screen height.
	virtual int   GetHeight() = 0;

	// Summary:
	//	Gets Pixel Aspect Ratio.
	virtual float GetPixelAspectRatio() const = 0;

	// Summary:
	//	Gets memory status information
	virtual void GetMemoryUsage(ICrySizer* Sizer)=0;

	// Summary:
	//	Gets textures streaming bandwidth information
	virtual void GetBandwidthStats(float * fBandwidthRequested) = 0;

	// Summary:
	//	Gets textures streaming request count
	virtual void GetTextureRequests(float * requests) = 0;
	// Summary:
	//	Gets textures streaming render count
	virtual void GetTextureRenders(float * renders) = 0;
	// Summary:
	//	Gets textures streaming pool usage
	virtual void GetTexturePoolUsage(float * usage) = 0;
	// Summary:
	//	Gets textures streaming pool wanted usage
	virtual void GetTexturePoolWanted(float * wanted) = 0;
	// Summary:
	//	Gets textures streaming update count
	virtual void GetTextureUpdates(float * updates) = 0;
	virtual int GetOcclusionBuffer(uint16* pOutOcclBuffer, int32 nSizeX, int32 nSizeY, Matrix44* pmViewProj, Matrix44* pmCamBuffer) = 0;
	
	// Summary:
  	//   Gets a screenshot and save to a file
	// Returns:
	//   true=success
	virtual bool ScreenShot(const char *filename=NULL, int width=0)=0;

	// Summary:
	//	Gets current bpp.
	virtual int GetColorBpp()=0;

	// Summary:
	//	Gets current z-buffer depth.
	virtual int GetDepthBpp()=0;

	// Summary:
	//	Gets current stencil bits.
	virtual int GetStencilBpp()=0;

	// Summary:
	//  Returns true if stereo rendering is enabled.
	virtual bool IsStereoEnabled() const = 0;

	// Summary:
	//  Returns values of nearest rendering z-range max
	virtual float GetNearestRangeMax() const = 0;

	// Summary:
	//	Projects to screen.
	//  Returns true if successful.
	virtual bool ProjectToScreen(
		float ptx, float pty, float ptz,
		float *sx, float *sy, float *sz )=0;

	// Summary:
	//	Unprojects to screen.
	virtual int UnProject(
		float sx, float sy, float sz,
		float *px, float *py, float *pz,
		const float modelMatrix[16],
		const float projMatrix[16],
		const int    viewport[4])=0;

	// Summary:
	//	Unprojects from screen.
	virtual int UnProjectFromScreen(
		float  sx, float  sy, float  sz,
		float *px, float *py, float *pz)=0;

	// Remarks:
	//	For editor.
	virtual void  GetModelViewMatrix(float *mat)=0;

	// Remarks:
	//	For editor.
	virtual void  GetProjectionMatrix(float *mat)=0;

	// Remarks:
	//	For editor.
	virtual Vec3 GetUnProject(const Vec3 &WindowCoords,const CCamera &cam)=0;

	virtual bool WriteDDS(byte *dat, int wdt, int hgt, int Size, const char *name, ETEX_Format eF, int NumMips)=0;
	virtual bool WriteTGA(byte *dat, int wdt, int hgt, const char *name, int src_bits_per_pixel, int dest_bits_per_pixel )=0;
	virtual bool WriteJPG(byte *dat, int wdt, int hgt, char *name, int src_bits_per_pixel, int nQuality = 100 )=0;
	virtual void LoadJPGBuff(unsigned char *fbuffer, unsigned char **pic, int *width, int *height )=0;

	/////////////////////////////////////////////////////////////////////////////////
	//Replacement functions for Font

	virtual bool FontUploadTexture(class CFBitmap*, ETEX_Format eTF=eTF_A8R8G8B8)=0;
	virtual int  FontCreateTexture(int Width, int Height, byte *pData, ETEX_Format eTF=eTF_A8R8G8B8, bool genMips=false)=0;
	virtual bool FontUpdateTexture(int nTexId, int X, int Y, int USize, int VSize, byte *pData)=0;
	virtual void FontReleaseTexture(class CFBitmap *pBmp)=0;
	virtual void FontSetTexture(class CFBitmap*, int nFilterMode)=0;
	virtual void FontSetTexture(int nTexId, int nFilterMode)=0;
	virtual void FontSetRenderingState(unsigned int nVirtualScreenWidth, unsigned int nVirtualScreenHeight)=0;
	virtual void FontSetBlending(int src, int dst)=0;
	virtual void FontRestoreRenderingState()=0;

	virtual bool FlushRTCommands(bool bWait, bool bImmediatelly, bool bForce)=0;
	virtual void DrawStringW(IFFont_RenderProxy* pFont, float x, float y, float z, const wchar_t* pStr, const bool asciiMultiLine, const STextDrawContext& ctx) const = 0;

	virtual int  RT_CurThreadList()=0;
	virtual void RT_FlashRender(IFlashPlayer_RenderProxy* pPlayer, bool stereo) = 0;
	virtual void RT_FlashRenderPlaybackLockless(IFlashPlayer_RenderProxy* pPlayer, int cbIdx, bool stereo, bool finalPlayback) = 0;
	virtual void RT_FlashRemoveTexture(ITexture* pTexture) = 0;

	/////////////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////////////
	// External interface for shaders
	/////////////////////////////////////////////////////////////////////////////////
	virtual bool EF_PrecacheResource(SShaderItem *pSI, float fMipFactor, float fTimeToReady, int Flags, int nUpdateId, int nCounter=1)=0;
	virtual bool EF_PrecacheResource(IShader *pSH, float fMipFactor, float fTimeToReady, int Flags)=0;
	virtual bool EF_PrecacheResource(ITexture *pTP, float fMipFactor, float fTimeToReady, int Flags, int nUpdateId, int nCounter=1)=0;
	virtual bool EF_PrecacheResource(IRenderMesh *pPB, IMaterial * pMaterial, float fMipFactor, float fTimeToReady, int Flags, int nUpdateId)=0;
	virtual bool EF_PrecacheResource(CDLight *pLS, float fMipFactor, float fTimeToReady, int Flags, int nUpdateId)=0;

	virtual void PostLevelLoading() = 0;
	virtual void PostLevelUnload() = 0;

	virtual CREParticle* EF_AddParticlesToScene(const SAddParticlesToSceneJob& job, const SParticleRenderContext& context) = 0;
	virtual void EF_ProcessAddedParticles() = 0;
	virtual void EF_ComputeQueuedParticles() = 0;
	virtual void GetMemoryUsageParticleREs( ICrySizer * pSizer ) {}

	virtual CRenderObject* EF_AddPolygonToScene(SShaderItem& si, int numPts, const SVF_P3F_C4B_T2F *verts, const SPipTangents *tangs, CRenderObject *obj, uint16 *inds, int ninds, int nAW, bool bMerge = true)=0;
	virtual CRenderObject* EF_AddPolygonToScene(SShaderItem& si, CRenderObject* obj, int numPts, int ninds, SVF_P3F_C4B_T2F*& verts, SPipTangents*& tangs, uint16*& inds, int nAW, bool bMerge = true)=0;

  /////////////////////////////////////////////////////////////////////////////////
  // Shaders/Shaders management /////////////////////////////////////////////////////////////////////////////////

	virtual void				EF_SetShaderMissCallback(ShaderCacheMissCallback callback) = 0;	
	virtual const char* EF_GetShaderMissLogPath() = 0;
	
	/////////////////////////////////////////////////////////////////////////////////
	virtual string       *EF_GetShaderNames(int& nNumShaders)=0;
	// Summary:
	//	Loads shader for name (name).
	virtual IShader      *EF_LoadShader (const char *name, int flags=0, uint64 nMaskGen=0)=0;

	// Summary:
	//		Remaps shader gen mask to common global mask.
	virtual uint64      EF_GetRemapedShaderMaskGen( const char *name, uint64 nMaskGen = 0, bool bFixup = 0) = 0;

	virtual uint64      EF_GetShaderGlobalMaskGenFromString( const char *szShaderName, const char *szShaderGen, uint64 nMaskGen = 0 ) = 0;
	virtual const char  *EF_GetStringFromShaderGlobalMaskGen( const char *szShaderName, uint64 nMaskGen = 0 ) = 0;

	virtual const SShaderProfile &GetShaderProfile(EShaderType eST) const= 0;
	virtual void          EF_SetShaderQuality(EShaderType eST, EShaderQuality eSQ) = 0;

	// Summary:
	//	Gets renderer quality.
	virtual ERenderQuality EF_GetRenderQuality() const = 0;
	// Summary:
	//	Gets shader type quality.
	virtual EShaderQuality EF_GetShaderQuality(EShaderType eST) = 0;

	// Summary:
	//	Loads shader item for name (name).
	virtual SShaderItem   EF_LoadShaderItem (const char *szName, bool bShare, int flags=0, SInputShaderResources *Res=NULL, uint64 nMaskGen=0)=0;
	// Summary:
	//	Reloads file
	virtual bool          EF_ReloadFile (const char *szFileName)=0;
	// Summary:
	//	Reinitializes all shader files (build hash tables).
	virtual void          EF_ReloadShaderFiles (int nCategory)=0;
	// Summary:
	//	Reloads all texture files.
	virtual void          EF_ReloadTextures ()=0;
	// Summary:
	//	Gets texture object by ID.
	virtual ITexture     *EF_GetTextureByID(int Id)=0;
	// Summary:
	//	Gets texture object by Name.
	virtual ITexture     *EF_GetTextureByName(const char *name, uint32 flags=FT_DONT_GENNAME)=0;
	// Summary:
	//	Loads the texture for name(nameTex).
	virtual ITexture     *EF_LoadTexture(const char* nameTex, const uint32 flags = 0)=0;
	// Summary:
	//	Loads lightmap for name.
	virtual int           EF_LoadLightmap (const char *name)=0;
	virtual bool          EF_ScanEnvironmentCM (const char *name, int size, Vec3& Pos)=0;
	virtual bool          EF_RenderEnvironmentCubeHDR (int size, Vec3& Pos, TArray<unsigned short>& vecData)=0;

	// Summary:
	//	Creates new RE (RenderElement) of type (edt).
	virtual CRendElementBase *EF_CreateRE (EDataType edt)=0;

	// Summary:
	//	Starts using of the shaders (return first index for allow recursions).
	virtual void EF_StartEf ()=0;

	virtual SRenderObjData *EF_GetObjData(CRenderObject *pObj, bool bCreate)=0;
	virtual void EF_ObjRemovePermanent(CRenderObject *pObj)=0;

	// Summary:
	//	Gets CRenderObject for RE transformation.
	//  Following 3 funcs are the result of splitting EF_GetObject for performance reasons:

	//Get permanent RenderObject at index specified
	virtual CRenderObject *EF_GetObject_Index (int index)=0;

	//Get temporary RenderObject
	virtual CRenderObject *EF_GetObject_Temp ()=0;

	//Get permanent RenderObject
	virtual CRenderObject *EF_GetObject_Perm ()=0;
  virtual CRenderObject *EF_DuplicateRO(CRenderObject *pObj, const SRenderObjectModifier * pROII) = 0;

	// Summary:
	//	Adds shader to the list.
	virtual void EF_AddEf (CRendElementBase *pRE, SShaderItem& pSH, CRenderObject *pObj, int nList=EFSLIST_GENERAL, int nAW=1)=0;

	// Summary:
	//	Draws all shaded REs in the list
	virtual void EF_EndEf3D (const int nFlags, const int nPrecacheUpdateId, const int nNearPrecacheUpdateId)=0;

	// Dynamic lights
	void EF_ClearLightsList() {}; // For FC Compatibility.
	virtual bool EF_IsFakeDLight (const CDLight *Source)=0;
	virtual void EF_ADDDlight(CDLight *Source)=0;
	virtual bool EF_UpdateDLight(SRenderLight *pDL)=0;
	virtual bool EF_AddDeferrredDecal(const SDeferrredDecal &rDecal){return true;}

	// Deferred lights/vis areas

	virtual int EF_AddDeferredLight( const CDLight & pLight, float fMult ) = 0;
	virtual uint32 EF_GetDeferredLightsNum(const eDeferredLightType eLightType = eDLT_DeferredLight) = 0;
	virtual void EF_ClearDeferredLightsList() = 0;
	virtual TArray<SRenderLight>* EF_GetDeferredLights(const eDeferredLightType eLightType = eDLT_DeferredLight) = 0;

	virtual void EF_AddDeferredVisArea( const IVisArea *pVisArea ) = 0;
	virtual uint8 EF_GetVisAreaStencilRef( const IVisArea *pVisArea ) = 0;
	virtual void EF_ClearDeferredVisAreasList() = 0;

	// Deferred clip volumes
	virtual void Ef_AddDeferredGIClipVolume( const IRenderMesh *pClipVolume, const Matrix34& mxTransform ) = 0;

	// called in between levels to free up memory
	virtual void EF_ReleaseDeferredData() = 0;

	// called in between levels to free up memory
	virtual void EF_ReleaseInputShaderResource(SInputShaderResources *pRes) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Post processing effects interfaces    

	virtual void EF_SetPostEffectParam(const char *pParam, float fValue, bool bForceValue=false) = 0;
	virtual void EF_SetPostEffectParamVec4(const char *pParam, const Vec4 &pValue, bool bForceValue=false) = 0;
	virtual void EF_SetPostEffectParamString(const char *pParam, const char *pszArg) = 0;

	virtual void EF_GetPostEffectParam(const char *pParam, float &fValue) = 0;  
	virtual void EF_GetPostEffectParamVec4(const char *pParam, Vec4 &pValue) = 0;  
	virtual void EF_GetPostEffectParamString(const char *pParam, const char*& pszArg) = 0;

	virtual int32 EF_GetPostEffectID(const char* pPostEffectName) = 0;

	virtual void EF_ResetPostEffects() = 0;

	virtual void EF_AddPostEffectLight(CDLight &pLight) = 0;

	//////////////////////////////////////////////////////////////////////////
	
	virtual void EF_AddWaterSimHit(const Vec3 &vPos) = 0;

	/////////////////////////////////////////////////////////////////////////////////
	// 2d interface for the shaders
	/////////////////////////////////////////////////////////////////////////////////
	virtual void EF_EndEf2D(const bool bSort)=0;

	// Summary:
	//	 Returns different common shader parameters (used in ShaderBrowser) CryIndEd.exe .
	// Arguments:
	//   Query - e.g. EFQ_GetShaderCombinations.
	//	 Param -
	virtual void *EF_Query(int Query, INT_PTR Param=0)=0;

	// Summary:
	//	 Toggles render mesh garbage collection
	// Arguments:
	//	 Param -
	virtual void ForceGC() = 0;

	// Remarks:
	//	For stats.
	virtual int  GetPolyCount()=0;
	virtual void GetPolyCount(int &nPolygons,int &nShadowVolPolys)=0;

	// Note:
	//	3d engine set this color to fog color.
	virtual void SetClearColor(const Vec3 & vColor)=0;

	// Summary:
	//	Creates/deletes RenderMesh object.
	virtual IRenderMesh * CreateRenderMesh(const char *szType, const char *szSourceName, IRenderMesh::SInitParamerers *pInitParams=NULL )=0;
	
	virtual struct IRenderMesh * CreateRenderMeshInitialized(
		void * pVertBuffer, int nVertCount, EVertexFormat eVF,
		uint16* pIndices, int nIndices,
		const eRenderPrimitiveType nPrimetiveType, const char *szType,const char *szSourceName, ERenderMeshType eBufType = eRMT_Static,
		int nMatInfoCount=1, int nClientTextureBindID=0,
		bool (*PrepareBufferCallback)(IRenderMesh *, bool)=NULL,
		void *CustomData=NULL,
		bool bOnlyVideoBuffer=false, 
		bool bPrecache=true, 
		SPipTangents* pTangents=NULL, bool bLockForThreadAcc=false) = 0;

	virtual void DeleteRenderMesh(struct IRenderMesh * pLBuffer)=0;

	virtual int GetFrameID(bool bIncludeRecursiveCalls=true)=0;

	virtual void MakeMatrix(const Vec3 & pos, const Vec3 & angles,const Vec3 & scale, Matrix34* mat)=0;

	//////////////////////////////////////////////////////////////////////
	// Summary:
	//	Draws an image on the screen as a label text.
	// Arguments:
	//	vPos		-   3d position.
	//    fSize		-   Size of the image.
	//    nTextureId	-   Texture Id dell'immagine.
	virtual void DrawLabelImage(const Vec3 &vPos,float fSize,int nTextureId)=0;

	// Description:
	//	 Draws text queued.
	// Note:
	//	 Position can be in 3d or in 2d depending on the flags.
	virtual void DrawTextQueued( Vec3 pos,SDrawTextInfo &ti,const char *format,va_list args ) = 0;

	// Description:
	//	 Draws text queued.
	// Note:
	//	 Position can be in 3d or in 2d depending on the flags.
	virtual void DrawTextQueued( Vec3 pos,SDrawTextInfo &ti,const char *text ) = 0;

	//////////////////////////////////////////////////////////////////////

	virtual float ScaleCoordX(float value) const = 0;
	virtual float ScaleCoordY(float value) const = 0;
	virtual void ScaleCoord(float& x, float& y) const = 0;

	virtual void SetState(int State, int AlphaRef=-1)=0;
	virtual void SetCullMode  (int mode=R_CULL_BACK)=0;

	virtual void PushProfileMarker(char* label) = 0;
	virtual void PopProfileMarker(char* label) = 0;

	virtual bool EnableFog  (bool enable)=0;
	virtual void SetFog   (float density,float fogstart,float fogend,const float *color,int fogmode)=0;
	virtual void SetFogColor(float * color)=0;

	virtual void SetColorOp(byte eCo, byte eAo, byte eCa, byte eAa)=0;

	virtual void SetTerrainAONodes(PodArray<SSectorTextureSet> * terrainAONodes)=0;


	// Summary:
	//	Builds terrain sector texture.
	virtual void BuildTerrainTexture() {}
	virtual IDynTexture * RequestTerrainTexture(SBuildTerrainTextureParams & params) { return 0; }
	virtual void ProcessVoxTerrainDebugTarget(int * pSrcTexIds, int nMipsNum, bool bOnlySetSrcTexture = true) {}

	// for one frame allows to disable limit of texture streaming requests
	virtual void RequestFlushAllPendingTextureStreamingJobs(int nFrames) { }
	
	// allows to dynamically adjust texture streaming load depending on game conditions
	virtual void SetTexturesStreamingGlobalMipFactor(float fFactor) { }

	// allows to enable/disable TerrainAO on a per-level base
	virtual void EnableTerrainAO(const bool bEnabled) = 0;

	//////////////////////////////////////////////////////////////////////
	// Summary: 
	//	Interface for auxiliary geometry (for debugging, editor purposes, etc.)
	virtual IRenderAuxGeom* GetIRenderAuxGeom() = 0;
	//////////////////////////////////////////////////////////////////////

	virtual IColorGradingController* GetIColorGradingController() = 0;
	virtual IStereoRenderer* GetIStereoRenderer() = 0;

	// Note:
	//	The following functions will be removed.
	virtual void EnableVSync(bool enable)=0;
	virtual void PushMatrix()=0;
	virtual void PopMatrix()=0;

	virtual void CreateResourceAsync(SResourceAsync* Resource)=0;
	virtual void ReleaseResourceAsync(SResourceAsync* Resource)=0;
	virtual unsigned int DownLoadToVideoMemory(unsigned char *data,int w, int h, ETEX_Format eTFSrc, ETEX_Format eTFDst, int nummipmap, bool repeat=true, int filter=FILTER_BILINEAR, int Id=0, const char *szCacheName=NULL, int flags=0, EEndian eEndian = eLittleEndian, RectI * pRegion = NULL, bool bAsynDevTexCreation = false)=0;
	virtual void UpdateTextureInVideoMemory(uint32 tnum, unsigned char *newdata,int posx,int posy,int w,int h,ETEX_Format eTFSrc=eTF_R8G8B8)=0; 
    // Remarks:
	//	 Without header.
	// Arguments:
	//   vLumWeight - 0,0,0 if default should be used.
	virtual bool DXTCompress( byte *raw_data,int nWidth,int nHeight,ETEX_Format eTF, bool bUseHW, bool bGenMips, int nSrcBytesPerPix, const Vec3 vLumWeight, MIPDXTcallback callback )=0;
	virtual bool DXTDecompress(byte *srcData, const size_t srcFileSize, byte *dstData, int nWidth,int nHeight,int nMips,ETEX_Format eSrcTF, bool bUseHW, int nDstBytesPerPix)=0;
	virtual void RemoveTexture(unsigned int TextureId)=0;
	virtual void DXTCompress_FastOnCPU( byte * pInData, byte * pOutData, const int width, const int height, const bool isDXT5 )=0;






	virtual void TextToScreen(float x, float y, const char * format, ...) PRINTF_PARAMS(4, 5)=0;
	virtual void TextToScreenColor(int x, int y, float r, float g, float b, float a, const char * format, ...) PRINTF_PARAMS(8, 9)=0;
	virtual void ResetToDefault()=0;
	virtual void SetMaterialColor(float r, float g, float b, float a)=0;

	virtual void DrawLine(const Vec3 & vPos1, const Vec3 & vPos2)=0;
	virtual void Graph(byte *g, int x, int y, int wdt, int hgt, int nC, int type, char *text, ColorF& color, float fScale)=0;
	virtual void FlushTextMessages()=0;
	virtual void DrawQuad(const Vec3 &right, const Vec3 &up, const Vec3 &origin,int nFlipMode=0)=0;
	virtual void DrawQuad(float dy,float dx, float dz, float x, float y, float z)=0;
	virtual void ClearBuffer(uint32 nFlags, ColorF *vColor, float depth = 1.0f)=0;
	virtual void ReadFrameBuffer(unsigned char * pRGB, int nImageX, int nSizeX, int nSizeY, ERB_Type eRBType, bool bRGBA, int nScaledX=-1, int nScaledY=-1)=0;
	virtual void ReadFrameBufferFast(uint32* pDstARGBA8, int dstWidth, int dstHeight)=0;



	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// This routines uses 2 destination surfaces.  It triggers a backbuffer copy to one of its surfaces,
	// and then copies the other surface to system memory.  This hopefully will remove any
	// CPU stalls due to the rect lock call since the buffer will already be in system
	// memory when it is called
	// Inputs : 
	//			pDstARGBA8			:	Pointer to a buffer that will hold the captured frame (should be at least 4*dstWidth*dstHieght for RGBA surface)
	//			destinationWidth	:	Width of the frame to copy
	//			destinationHeight	:	Height of the frame to copy
	//
	//			Note :	If dstWidth or dstHeight is larger than the current surface dimensions, the dimensions
	//					of the surface are used for the copy
	//
	virtual void CaptureFrameBufferFast(unsigned char *pDstRGBA8, int destinationWidth, int destinationHeight)=0;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// Copy a captured surface to a buffer
	//
	// Inputs : 
	//			pDstARGBA8			:	Pointer to a buffer that will hold the captured frame (should be at least 4*dstWidth*dstHieght for RGBA surface)
	//			destinationWidth	:	Width of the frame to copy
	//			destinationHeight	:	Height of the frame to copy
	//
	//			Note :	If dstWidth or dstHeight is larger than the current surface dimensions, the dimensions
	//					of the surface are used for the copy
	//
	virtual void CopyFrameBufferFast(unsigned char *pDstRGBA8, int destinationWidth, int destinationHeight)=0;



	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// This routine registers a callback address that is called when a new frame is available
	// Inputs : 
	//			pCapture			:	Address of the ICaptureFrameListener object
	//
	// Outputs : returns true if successful, otherwise false
	//
	virtual bool RegisterCaptureFrame(ICaptureFrameListener *pCapture)=0;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// This routine unregisters a callback address that was previously registered
	// Inputs : 
	//			pCapture			:	Address of the ICaptureFrameListener object to unregister
	//
	// Outputs : returns true if successful, otherwise false
	//
	virtual bool UnRegisterCaptureFrame(ICaptureFrameListener *pCapture)=0;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// This routine initializes 2 destination surfaces for use by the CaptureFrameBufferFast routine
	// It also, captures the current backbuffer into one of the created surfaces
	//
	// Inputs :
	//			bufferWidth	: Width of capture buffer, on consoles the scaling is done on the GPU. Pass in 0 (the default) to use backbuffer dimensions
	//			bufferHeight	: Height of capture buffer.
	//
	// Outputs : returns true if surfaces were created otherwise returns false
	//
	virtual bool InitCaptureFrameBufferFast(uint32 bufferWidth=0, uint32 bufferHeight=0)=0;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// This routine releases the 2 surfaces used for frame capture by the CaptureFrameBufferFast routine
	//
	// Inputs : None
	//
	// Returns : None
	//
	virtual void CloseCaptureFrameBufferFast(void)=0;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// This routine checks for any frame buffer callbacks that are needed and calls them
	//
	// Inputs : None
	//
	//	Outputs : None
	//
	virtual void CaptureFrameBufferCallBack(void)=0;


	virtual void Set2DMode(bool enable, int ortox, int ortoy,float znear=-1e10f,float zfar=1e10f)=0;
	virtual int ScreenToTexture(int nTexID)=0;
	virtual void EnableSwapBuffers(bool bEnable) = 0;
	virtual WIN_HWND GetHWND() = 0;

	virtual void OnEntityDeleted(struct IRenderNode * pRenderNode)=0;

	virtual int CreateRenderTarget (int nWidth, int nHeight, ETEX_Format eTF)=0;
	virtual bool DestroyRenderTarget (int nHandle)=0;
	virtual bool SetRenderTarget (int nHandle, int nFlags=0)=0;
	virtual float EF_GetWaterZElevation(float fX, float fY)=0;

	// Note:
	//	Used for pausing timer related stuff.
	// Example:
	//	For texture animations, and shader 'time' parameter.
	virtual void PauseTimer(bool bPause)=0;

	// Description:
	//    Creates an Interface to the public params container.
	// Return:
	//    Created IShaderPublicParams interface.
	virtual IShaderPublicParams* CreateShaderPublicParams() = 0;

#ifndef EXCLUDE_SCALEFORM_SDK
	enum ESFMaskOp
	{
		BeginSubmitMask_Clear,
		BeginSubmitMask_Inc,
		BeginSubmitMask_Dec,
		EndSubmitMask,
		DisableMask
	};
	virtual void SF_ConfigMask(ESFMaskOp maskOp, unsigned int stencilRef) = 0;
	virtual void SF_DrawIndexedTriList(int baseVertexIndex, int minVertexIndex, int numVertices, int startIndex, int triangleCount, const SSF_GlobalDrawParams& params) = 0;
	virtual void SF_DrawLineStrip(int baseVertexIndex, int lineCount, const SSF_GlobalDrawParams& params) = 0;
	virtual void SF_DrawGlyphClear(const SSF_GlobalDrawParams& params) = 0;
	virtual void SF_Flush() = 0;
	virtual int SF_CreateTexture(int width, int height, int numMips, unsigned char* pData, ETEX_Format eTF, int flags) = 0;
	struct SUpdateRect
	{
		int dstX, dstY;
		int srcX, srcY;
		int width, height;
		
		void Set(int dx, int dy, int sx, int sy, int w, int h)
		{
			dstX = dx; dstY = dy;
			srcX = sx; srcY = sy;
			width = w; height = h;
		}
	};
	virtual bool SF_UpdateTexture(int texId, int mipLevel, int numRects, const SUpdateRect* pRects, unsigned char* pData, size_t pitch, ETEX_Format eTF) = 0;
	virtual bool SF_MapTexture(int texID, int level, void*& pBits, uint32& pitch) = 0;
	virtual bool SF_UnmapTexture(int texID, int level) = 0;
	virtual void SF_GetMeshMaxSize(int& numVertices, int& numIndices) const = 0;
	virtual void SF_GetThreadIDs(uint32& mainThreadID, uint32& renderThreadID) const = 0;
#endif // #ifndef EXCLUDE_SCALEFORM_SDK

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//	 Creates an instance of the IVideoPlayer interface.
	virtual IVideoPlayer* CreateVideoPlayerInstance() const = 0;

	virtual void StartLoadtimeFlashPlayback(ILoadtimeCallback* pCallback) = 0;
	virtual void StopLoadtimeFlashPlayback() = 0;

	virtual void SetCloudShadowTextureId( int id, const Vec3 & vSpeed  ) = 0;
	virtual void SetSkyLightRenderParams( const SSkyLightRenderParams* pSkyLightRenderParams ) = 0;
	virtual uint16 PushFogVolumeContribution( const ColorF& fogVolumeContrib ) = 0;

	virtual int GetMaxTextureSize()=0;

	virtual const char * GetTextureFormatName(ETEX_Format eTF) = 0;
	virtual int GetTextureFormatDataSize(int nWidth, int nHeight, int nDepth, int nMips, ETEX_Format eTF) = 0;

	virtual void SetDefaultMaterials(IMaterial * pDefMat, IMaterial * pTerrainDefMat) = 0;

	virtual bool IsMultiGPUModeActive() const = 0;

	virtual bool GetImageCaps(const char* filename,int& width,int& height) const = 0;
	virtual bool MergeImages(const char* output_filename,int out_width,int out_height,const char** files,int* offsetsX,int* offsetsY,int* widths,int* heights,int* src_offsetsX,int* src_offsetsY,int* src_width,int* src_height, int count) const = 0;

	virtual void SetTexturePrecaching( bool stat ) = 0;

	//GPU Timers
	virtual void	RT_BeginGPUTimer(const char* name, gpu_profile_flags_t flags=0) = 0;
	virtual void	RT_EndGPUTimer(const char* name) = 0;
	virtual void	SwapGpuTimers() = 0;
	virtual void	RenderGpuStats() = 0;
	virtual void  RenderGpuStatsDebugNode() = 0;

	virtual void EnableGPUTimers2( bool bEnabled ) = 0;
	virtual float GetGPUTimer2( const char *name, bool bCalledFromMainThread = true ) = 0;

	virtual int GetPolygonCountByType(uint32 EFSList, EVertexCostTypes vct, uint32 z, bool bCalledFromMainThread = true) = 0;

	//platform specific
	virtual void	RT_InsertGpuCallback(uint32 context, GpuCallbackFunc callback) = 0;

	struct SRenderTimes
	{
		float fWaitForMain;
		float fWaitForRender;
		float fWaitForGPU;
		float fTimeProcessedRT;
		float fTimeProcessedRTScene;	//The part of the render thread between the "SCENE" profiler labels
		float fTimeGPUIdlePercent;
	};
	virtual void GetRenderTimes(SRenderTimes &outTimes) = 0;
	virtual float GetGPUFrameTime() = 0;

	// Enable the batch mode if the meshpools are used to enable quick and dirty flushes. 
	virtual void EnableBatchMode(bool enable) = 0;
	// Flag level unloading in progress to disable f.i. rendermesh creation requests
	virtual void EnableLevelUnloading(bool enable) = 0;
	
	struct SDrawCallCountInfo
	{
		SDrawCallCountInfo():pPos(0,0,0), nZpass(0), nShadows(0),nGeneral(0), nTransparent(0), nMisc(0), pRenderMesh(NULL)
		{
		}

		void Update(CRenderObject *pObj, IRenderMesh *pRM);

		Vec3 pPos;
		uint8 nZpass, nShadows, nGeneral, nTransparent, nMisc;
		IRenderMesh *pRenderMesh;
	};

	//Debug draw call info (per node)
	typedef std::map< IRenderNode*, IRenderer::SDrawCallCountInfo > RNDrawcallsMapNode;
	typedef RNDrawcallsMapNode::iterator RNDrawcallsMapNodeItor;
	
	//Debug draw call info (per mesh)
	typedef std::map< IRenderMesh*, IRenderer::SDrawCallCountInfo > RNDrawcallsMapMesh;
	typedef RNDrawcallsMapMesh::iterator RNDrawcallsMapMeshItor;

#if !defined(_RELEASE)
	//Get draw call info for frame
	virtual RNDrawcallsMapMesh& GetDrawCallsInfoPerMesh(bool mainThread=true) = 0;

	virtual void ForceRemoveNodeFromDrawCallsMap( IRenderNode *pNode ) = 0;
#endif

	virtual void CollectDrawCallsInfo(bool status)=0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//	 Helper functions to draw text.
	//////////////////////////////////////////////////////////////////////////
	void DrawLabel(Vec3 pos, float font_size, const char * label_text, ...) PRINTF_PARAMS(4, 5)
	{
		va_list args;
		va_start(args,label_text);
		SDrawTextInfo ti;
		ti.xscale = ti.yscale = font_size;
		ti.flags = eDrawText_FixedSize | eDrawText_800x600;
		DrawTextQueued( pos,ti,label_text,args );
		va_end(args);
	}

	void DrawLabelEx(Vec3 pos, float font_size, const float * pfColor, bool bFixedSize, bool bCenter, const char * label_text, ...) PRINTF_PARAMS(7, 8)
	{
		va_list args;
		va_start(args,label_text);
		SDrawTextInfo ti;
		ti.xscale = ti.yscale = font_size;
		ti.flags = ((bFixedSize)?eDrawText_FixedSize:0) | ((bCenter)?eDrawText_Center:0) | eDrawText_800x600;
		if (pfColor) { ti.color[0] = pfColor[0]; ti.color[1] = pfColor[1]; ti.color[2] = pfColor[2]; ti.color[3] = pfColor[3]; }
		DrawTextQueued( pos,ti,label_text,args );
		va_end(args);
	}

	void Draw2dLabel( float x,float y, float font_size, const float * pfColor, bool bCenter, const char * label_text, ...) PRINTF_PARAMS(7, 8)
	{
		va_list args;
		va_start(args,label_text);
		SDrawTextInfo ti;
		ti.xscale = ti.yscale = font_size;
		ti.flags = eDrawText_2D|eDrawText_800x600 | eDrawText_FixedSize | ((bCenter)?eDrawText_Center:0);
		if (pfColor) { ti.color[0] = pfColor[0]; ti.color[1] = pfColor[1]; ti.color[2] = pfColor[2]; ti.color[3] = pfColor[3]; }
		DrawTextQueued( Vec3(x,y,0.5f),ti,label_text,args );
		va_end(args);
	}


	// Summary:
	//	Determine if a switch to stereo mode will occur at the start of the next frame
	virtual bool IsStereoModeChangePending() = 0;

#if !defined (_RELEASE)
	// Summary:
	//  Adds any texture allocation to the statoscope texture allocations list in order to bucket the texture allocation sizes
	virtual void AddTexturePoolAllocation(int size, string texturepath) = 0;
	virtual std::vector<STexturePoolAllocation*> * GetTexturePoolAllocationList() = 0;
	virtual void ClearTexturePoolAllocationList(int threadid = -1) = 0;
#endif
};

struct SShaderCacheStatistics
{
	size_t m_nPerLevelShaderCacheMisses;
	size_t m_nGlobalShaderCacheMisses;
	bool m_bShaderCompileActive;

	SShaderCacheStatistics() : m_nPerLevelShaderCacheMisses(0),
		m_nGlobalShaderCacheMisses(0), m_bShaderCompileActive(false)
	{}
};

// The statistics about the pool for render mesh data 
// Note:
struct SMeshPoolStatistics
{
	// The size of the mesh data size in bytes 
	size_t nPoolSize;

	// The amount of memory currently in use in the pool 
	size_t nPoolInUse;

	// The highest amount of memory allocated within the mesh data pool 
	size_t nPoolInUsePeak;

	// The size of the mesh data size in bytes 
	size_t nVolatilePoolSize;

	// The amount of memory currently in use in the pool 
	size_t nVolatilePoolInUse;

	// The highest amount of memory allocated within the mesh data pool 
	size_t nVolatilePoolInUsePeak;

	size_t nFallbacks;  
	size_t nVolatileFallbacks;
	size_t nFlushes; 

	SMeshPoolStatistics() 
		: nPoolSize(),
		nPoolInUse(),
		nVolatilePoolSize(),
		nVolatilePoolInUse(),
		nVolatilePoolInUsePeak(),
		nFallbacks(),
		nVolatileFallbacks(),
		nFlushes()
	{}
};

// Query types for CryInd editor (used in EF_Query() function).
enum ERenderQueryTypes
{
	EFQ_DeleteMemoryArrayPtr = 1,
	EFQ_DeleteMemoryPtr,
	EFQ_GetShaderCombinations,
	EFQ_SetShaderCombinations,
	EFQ_CloseShaderCombinations,

	EFQ_ShaderGraphBlocks,
	EFQ_MainThreadList,
	EFQ_RenderThreadList,
	EFQ_RenderMultithreaded,

	EFQ_RecurseLevel,
	EFQ_Pointer2FrameID,
	EFQ_DeviceLost,
	EFQ_D3DDevice,
	EFQ_glReadPixels,
	EFQ_LightSource,

	EFQ_Alloc_APITextures,
	EFQ_Alloc_APIMesh,
	
	// Memory allocated by meshes in system memory.
	EFQ_Alloc_Mesh_SysMem,
	EFQ_Mesh_Count,

	EFQ_HDRModeEnabled,
	EFQ_DeferredShading,

	// Description:
	//		Query will return all textures in the renderer,
	//		first pass 0 as parameter to get number of textures, 
	//		then pass pointer to the ITexture array big enough to hold that many texture pointers.
	EFQ_GetAllTextures,

	// Description:
	//		Query will return all IRenderMesh objects in the renderer,
	//		Pass an array pointer to be allocated and filled with the IRendermesh pointers. The calling function is responsible for freeing this memory.
	//		This was originally a two pass process, but proved to be non-thread-safe, leading to buffer overruns and underruns.
	EFQ_GetAllMeshes,

	// Summary:
	//		Multigpu (crossfire/sli) is enabled.
	EFQ_MultiGPUEnabled,
	EFQ_DrawNearFov,
	EFQ_TextureStreamingEnabled,
	EFQ_FSAAEnabled,

	// Summary:
	//		Pointer to struct with PS3 lowlevel render-api usage stats.
	EFQ_PS3_Resource_Stats,
	EFQ_Fullscreen,
	EFQ_GetTexStreamingInfo,
	EFQ_GetMeshPoolInfo,

	// Description:
	//		True when shading is done in linear space, de-gamma on texture lookup, gamma on frame buffer writing (sRGB), false otherwise.
	EFQ_sLinearSpaceShadingEnabled,

	// Description:
	//		True when a half pixel shift is needed to map between pixels and texels (ex. DX9).
	EFQ_HalfPixelShiftNeeded,

	// The percentages of overscan borders for left/right and top/bottom to adjust the title safe area.
	EFQ_OverscanBorders,

	// Get num active post effects
	EFQ_NumActivePostEffects,

	// Get size of textures memory pool
	EFQ_TexturesPoolSize,
	EFQ_RenderTargetPoolSize,

	EFQ_GetShaderCacheInfo,

	EFQ_GetFogCullDistance
};

//////////////////////////////////////////////////////////////////////

#define STRIPTYPE_NONE           0
#define STRIPTYPE_ONLYLISTS      1
#define STRIPTYPE_SINGLESTRIP    2
#define STRIPTYPE_MULTIPLESTRIPS 3
#define STRIPTYPE_DEFAULT        4

/////////////////////////////////////////////////////////////////////

//DOC-IGNORE-BEGIN
#include "VertexFormats.h"
#include "IRenderMesh.h"
//DOC-IGNORE-END

struct SRestLightingInfo
{
	SRestLightingInfo()
	{
		averDir.zero();
		averCol = Col_Black;
		refPoint.zero();
	}
	Vec3 averDir;
	ColorF averCol;
	Vec3 refPoint;
};

// Description:
//	 Structure used to pass render parameters to Render() functions of IStatObj and ICharInstance.
_MS_ALIGN(16) struct SRendParams
{
	SRendParams()
	{
		memset(this, 0, sizeof(SRendParams));
		fAlpha = 1.f;
		fRenderQuality = 1.f;
		nRenderList = EFSLIST_GENERAL;
		nAfterWater = 1;
		nThreadId = -1;
	}

#if (defined(PS3) && !defined(__SPU__) && defined(PS3_OPT)) || defined(XENON)
	SRendParams (const SRendParams& cpy)
	{ 
		//Size must be a multiple of 16 to ensure correct padding in arrays
		COMPILE_TIME_ASSERT((sizeof(SRendParams)&0xf)==0);

		cryVecMemcpy<sizeof(SRendParams)>(this, &cpy);
	}
  
	const SRendParams& operator = (const SRendParams& cpy)
	{    
		cryVecMemcpy<sizeof(SRendParams)>(this, &cpy);

		return *this;
	}
#endif

	// Summary:
	//	object transformations.
	Matrix34    *pMatrix;
	struct SInstancingInfo * pInstInfo;
	// Summary:
	//	object previous transformations - motion blur specific.
	Matrix34    *pPrevMatrix;
	// Summary:
	//	List of shadow map casters.
	PodArray<ShadowMapFrustum*> * pShadowMapCasters;
	//	VisArea that contains this object, used for RAM-ambient cube query 
	IVisArea*		m_pVisArea;	
	// Summary:
	//	Override material.
	IMaterial *pMaterial;
	// Summary:
	//	 Skeleton implementation for bendable foliage.
	ISkinnable *pFoliage;
	// Summary:
	//	 Weights stream for deform morphs.
	IRenderMesh *pWeights;
	// Summary:
	//	Object Id for objects identification in renderer.
	struct IRenderNode * pRenderNode;
	// Summary:
	//	Unique object Id for objects identification in renderer.
	void* pInstance;
	// Summary:
	//	 TerrainTexInfo for grass.
	struct SSectorTextureSet * pTerrainTexInfo;
	// Summary:
	//	 storage for LOD transition states.
	struct CRNTmpData ** ppRNTmpData;
	// Summary:
	//	Ambient color for the object.
	ColorF AmbientColor;
	// Summary:
	//	Custom sorting offset.
	float       fCustomSortOffset;
	// Summary:
	//	Object alpha.
	float     fAlpha;
	// Summary:
	//	Distance from camera.
	float     fDistance;
	// Summary:
	//	 Quality of shaders rendering.
	float fRenderQuality;
	// Summary:
	//	Light mask to specify which light to use on the object.
	uint32 nDLightMask;
	// Summary:
	//	Approximate information about the lights not included into nDLightMask.
//  SRestLightingInfo restLightInfo;
	// Summary:
	//	CRenderObject flags.
	int32       dwFObjFlags;
	// Summary:
	//	 Material layers blending amount
	uint32 nMaterialLayersBlend;
	// Summary:
	//	Vision modes params
	uint32 nVisionParams;
	// Summary:
	//	Vision modes params
	uint32 nHUDSilhouettesParams;
	// Layer effects
	uint32 pLayerEffectParams;
	// Summary:
	//	Defines what peaces of pre-broken geometry has to be rendered
	uint64 nSubObjHideMask;

	//	 Custom TextureID 
	int16 nTextureID;
	//	 Defines per object custom data
	uint8 nCustomData;

	//	 Defines per object custom flags
	uint8 nCustomFlags;
	// Summary:
	//	 Defines per object DissolveRef value if used by shader.
	uint8 nDissolveRef;
	// Summary:
	//	 per-instance vis area stencil ref id
	uint8   nVisAreaStencilRef;
	// Summary:
	//	 Custom offset for sorting by distance.
	uint8  nAfterWater;

	// Summary:
	//	 Material layers bitmask -> which material layers are active.
	uint8 nMaterialLayers;
	// Summary:
	//	 LOD transition states slot id.
//	uint8 nLodTransSlotId;
	// Summary:
	//	 LOD transition states slot id.
	uint8 nLod;
	// Summary:
	//	ThreadId.
	int8 nThreadId;
	// Summary:
	//	Force a sort value for render elements.
	uint8 nRenderList;

} _ALIGN(16);

#endif //_IRENDERER_H


