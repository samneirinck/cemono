
/*=============================================================================
CREImposter.h : Imoster RE declarations/implementations.
Copyright (c) 2001-2005 Crytek Studios. All Rights Reserved.

  Revision history:
  
=============================================================================*/


#ifndef __CREIMPOSTER_H__
#define __CREIMPOSTER_H__


//================================================================================

struct SDynTexture2;
struct SDynTexture;

class CREImposter : public CRendElementBase
{
  friend class CRECloud;

  CRenderCamera m_LastCamera;
  bool m_bScreenImposter;
  bool m_bSplit;
  float m_fRadiusX;
  float m_fRadiusY;
	Vec3 m_vQuadCorners[4];								// in world space, relative to m_vPos, in clockwise order, can be rotated
  Vec3 m_vNearPoint;
  Vec3 m_vFarPoint;
  int m_nLogResolutionX;
	int m_nLogResolutionY;
  IDynTexture *m_pTexture;
  static IDynTexture *m_pScreenTexture;
	IDynTexture *m_pFrontTexture;
  IDynTexture *m_pTextureDepth;
  float m_fErrorToleranceCosAngle;			// cosine of m_fErrorToleranceAngle used to check if IsImposterValid
  SMinMaxBox m_WorldSpaceBV;
	struct CTerrainNode * m_pTerrainNode;
  uint32 m_State;
  int m_AlphaRef;
  float m_fCurTransparency;
  ColorF m_ColorHelper;
  Vec3 m_vPos;
	Vec3 m_vLastSunDir;
	uint8 m_nLastBestEdge;								// 0..11 this edge is favored to not jitter between different edges
  float m_fNear;
	float m_fFar;

  bool IsImposterValid(const CRenderCamera& cam, float fRadiusX, float fRadiusY, float fCamRadiusX, float fCamRadiusY, 
		const int iRequiredLogResX, const int iRequiredLogResY, const uint32 dwBestEdge );

  void ReleaseResources();
  const SMinMaxBox& mfGetWorldSpaceBounds()  { return m_WorldSpaceBV; }
  bool Display(bool bDisplayFrontOfSplit);
  bool IsSplit() { return m_bSplit; }
  bool UpdateImposter();
  bool PrepareForUpdate();
  Vec3 GetPosition();

public:
  int m_nFrameReset;
  int m_FrameUpdate;
  float m_fTimeUpdate;
  static int m_MemUpdated;
  static int m_MemPostponed;
  static int m_PrevMemUpdated;
  static int m_PrevMemPostponed;

  CREImposter() : CRendElementBase(),
    m_pTexture(NULL),
    m_pFrontTexture(NULL),
    m_pTextureDepth(NULL),
    m_bSplit(false),
    m_fRadiusX(0),
    m_fRadiusY(0),
    m_fErrorToleranceCosAngle(cos(DEG2RAD(0.25f))),
    m_bScreenImposter(false),
    m_State(GS_DEPTHWRITE),
    m_AlphaRef(-1),
    m_fCurTransparency(1.0f),
    m_FrameUpdate(0),
    m_nFrameReset(0),
    m_fTimeUpdate(0),
		m_pTerrainNode(0),
		m_vLastSunDir(0, 0, 0),
		m_nLogResolutionX(0),
		m_nLogResolutionY(0),
		m_nLastBestEdge(0),
	  m_vNearPoint(ZERO),
		m_vFarPoint(ZERO)
  {
    mfSetType(eDATA_Imposter);
    mfUpdateFlags(FCEF_TRANSFORM);
    m_ColorHelper = Col_White;
  }
  virtual ~CREImposter()
  {
		ReleaseResources();
	}
	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
	}
  virtual float mfDistanceToCameraSquared(Matrix34& matInst);
  virtual void mfPrepare(bool bCheckOverflow);
  virtual bool mfDraw(CShader *ef, SShaderPass *sl);
	
	virtual CTerrainNode * GetTerrainNode() { return m_pTerrainNode; }
	virtual void SetTerrainNode(CTerrainNode * pTerrainNode) { m_pTerrainNode = pTerrainNode; }
	virtual void SetBBox(const Vec3 & min, const Vec3 & max) { m_WorldSpaceBV.SetMin(min); m_WorldSpaceBV.SetMax(max); }
};

#include "Cry_Camera.h"

class CREPanoramaCluster : public CRendElementBase
{
	friend class CRECloud;

	SDynTexture * m_pTexture;
	SDynTexture * m_pTexture1;   // For ATI HDR
	int m_AlphaRef;
	Vec3 m_vPrevCamPos;
	CCamera m_Camera;
	int m_nTexRes;
	int m_nDLDFlags;
	bool m_bTextureIsEmpty;

	void ReleaseResources();
	bool Display(bool bDisplayFrontOfSplit);

public:
	int m_FrameUpdate;
	float m_fTimeUpdate;
	float m_fCVarsCheckSumm;
	Vec3 m_vSunDir;
	Vec3 m_vSunColor;
	Vec3 m_vSkyColor;

	CREPanoramaCluster() : CRendElementBase(),
		m_pTexture(NULL),
		m_pTexture1(NULL),
		m_AlphaRef(-1),
		m_FrameUpdate(0),
		m_fTimeUpdate(0),
		m_vPrevCamPos(0,0,0),
		m_nTexRes(256),
		m_nDLDFlags(0),
		m_fCVarsCheckSumm(0),
		m_vSunDir(0,0,0),
		m_vSunColor(0,0,0),
		m_vSkyColor(0,0,0)
	{
		mfSetType(eDATA_PanoramaCluster);
		mfUpdateFlags(FCEF_TRANSFORM);
	}
	virtual ~CREPanoramaCluster()
	{
		ReleaseResources();
	}
	virtual void GetMemoryUsage(ICrySizer *pSizer) const 
	{
		pSizer->AddObject(this, sizeof(*this));
	}
  bool UpdateImposter();

	virtual void mfPrepare(bool bCheckOverflow);
	virtual bool mfDraw(CShader *ef, SShaderPass *sl);
	virtual float mfDistanceToCameraSquared(Matrix34& matInst);

	virtual void SetCamera(const CCamera & cam) { m_Camera = cam; }
	virtual void SetPrevCamPos(const Vec3 & vPrevCamPos) { m_vPrevCamPos = vPrevCamPos; }
	virtual Vec3 GetPrevCamPos() { return m_vPrevCamPos; }
	virtual void SetTexRes(int nTexRes) { m_nTexRes = nTexRes; }
	virtual void SetDLDFlags(int nDLDFlags) { m_nDLDFlags = nDLDFlags; }
};

#endif	// __CREIMPOSTER_H__
