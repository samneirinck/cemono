
#ifndef __RENDELEMENT_H__
#define __RENDELEMENT_H__

//=============================================================

#include "VertexFormats.h"

class CRendElementBase;
struct CRenderChunk;
struct PrimitiveGroup;
class CShader;
struct SShaderTechnique;
class CParserBin;
struct SParserFrame;

enum EDataType
{
  eDATA_Unknown = 0,
  eDATA_Dummy,
  eDATA_Sky,		
  eDATA_Beam,		
  eDATA_ClientPoly,
  eDATA_ClientPoly2D,
  eDATA_Flare,
  eDATA_Terrain,
  eDATA_SkyZone,
  eDATA_Mesh,
  eDATA_Imposter,
	eDATA_PanoramaCluster,
  eDATA_TerrainSector,
  eDATA_FarTreeSprites,  
  eDATA_ShadowMapGen,
  eDATA_TerrainDetailTextureLayers,
  eDATA_TerrainParticles,
  eDATA_Ocean,  
  eDATA_OcclusionQuery,
  eDATA_TempMesh,
	eDATA_Particle,
  eDATA_PostProcess, 
  eDATA_HDRProcess,  
  eDATA_Cloud,  
	eDATA_HDRSky,  
	eDATA_FogVolume,
	eDATA_WaterVolume,
  eDATA_WaterWave,
  eDATA_WaterOcean,
	eDATA_VolumeObject,
	eDATA_IrradianceVolume,
	eDATA_PrismObject,				// normally this would be #if !defined(EXCLUDE_DOCUMENTATION_PURPOSE) but we keep it to get consistent numbers for serialization
	eDATA_DeferredShading,
	eDATA_GameEffect,
	eDATA_LightShape,
};

#include <Cry_Color.h>

//=======================================================

#define FCEF_TRANSFORM 1
#define FCEF_DIRTY     2
#define FCEF_NODEL     4
#define FCEF_DELETED   8

#define FCEF_MODIF_TC   0x10
#define FCEF_MODIF_VERT 0x20
#define FCEF_MODIF_COL  0x40
#define FCEF_MODIF_MASK 0xf0

#define FCEF_NEEDFILLBUF 0x100
#define FCEF_ALLOC_CUST_FLOAT_DATA 0x200
#define FCEF_MERGABLE    0x400

#define FCEF_SKINNED    0x800

#define FGP_NOCALC 1
#define FGP_SRC    2
#define FGP_REAL   4
#define FGP_WAIT   8

#define FGP_STAGE_SHIFT 0x10

#define MAX_CUSTOM_TEX_BINDS_NUM 2

class CRendElement
{	
public:
  static CRendElement m_RootGlobal;
  static CRendElement m_RootRelease[];
  CRendElement *m_NextGlobal;
  CRendElement *m_PrevGlobal;

  EDataType m_Type;
protected:

  _inline void UnlinkGlobal()
  {
    if (!m_NextGlobal || !m_PrevGlobal)
      return;
    m_NextGlobal->m_PrevGlobal = m_PrevGlobal;
    m_PrevGlobal->m_NextGlobal = m_NextGlobal;
    m_NextGlobal = m_PrevGlobal = NULL;
  }
  _inline void LinkGlobal( CRendElement* Before )
  {
    if (m_NextGlobal || m_PrevGlobal)
      return;
    m_NextGlobal = Before->m_NextGlobal;
    Before->m_NextGlobal->m_PrevGlobal = this;
    Before->m_NextGlobal = this;
    m_PrevGlobal = Before;
  }

public:
  CRendElement();
  virtual ~CRendElement();
  virtual void Release(bool bForce=false);
	virtual void GetMemoryUsage(ICrySizer *pSizer) const {/*nothing*/}

  const char *mfTypeString();
  _inline EDataType mfGetType() { return m_Type; }
  void mfSetType(EDataType t) { m_Type = t; }

  virtual int Size() { return 0; }
	virtual void mfReset() {}

  static void ShutDown();
  static void Tick();

	static void Cleanup();
};

class CRendElementBase : public CRendElement
{
public:
  uint16 m_Flags;
  uint16 m_nFrameUpdated;

public:
  void *m_CustomData;
  int m_CustomTexBind[MAX_CUSTOM_TEX_BINDS_NUM];

public:
  CRendElementBase();
  virtual ~CRendElementBase();

  inline uint32 mfGetFlags(void) { return m_Flags; }
  inline void mfSetFlags(uint32 fl) { m_Flags = fl; }
  inline void mfUpdateFlags(uint32 fl) { m_Flags |= fl; }
  inline void mfClearFlags(uint32 fl) { m_Flags &= ~fl; }
  inline bool mfCheckUpdate(EVertexFormat eVertFormat, int Flags, uint16 nFrame)
  {
    if (nFrame != m_nFrameUpdated || (m_Flags & (FCEF_DIRTY | FCEF_SKINNED)))
    {
      m_nFrameUpdated = nFrame;
      return mfUpdate(eVertFormat, Flags);
    }
    return true;
  }

  virtual void mfPrepare(bool bCheckOverflow); // False - mergable, True - static mesh
  virtual bool mfCullByClipPlane(CRenderObject *pObj);
  virtual CRenderChunk *mfGetMatInfo();
  virtual PodArray<CRenderChunk> *mfGetMatInfoList();
  virtual int mfGetMatId();
  virtual void mfReset();
  virtual bool mfIsHWSkinned() { return false; }
  virtual CRendElementBase *mfCopyConstruct(void);
  virtual void mfCenter(Vec3& centr, CRenderObject*pObj);
  virtual void mfGetBBox(Vec3& vMins, Vec3& vMaxs)
  {
    vMins.Set(0,0,0);
    vMaxs.Set(0,0,0);
  }
  virtual void mfGetPlane(Plane& pl);
  virtual float mfDistanceToCameraSquared(Matrix34& matInst);
  virtual bool mfCompile(CParserBin& Parser, SParserFrame& Frame) { return false; }
  virtual bool mfDraw(CShader *ef, SShaderPass *sfm);
  virtual void *mfGetPointer(ESrcPointer ePT, int *Stride, EParamType Type, ESrcPointer Dst, int Flags);
  virtual bool mfPreDraw(SShaderPass *sl) { return true; }
  virtual bool mfUpdate(EVertexFormat eVertFormat, int Flags) { return true; }
  virtual void mfPrecache(const SShaderItem& SH) {}
  virtual void mfExport(struct SShaderSerializeContext& SC) {}
  virtual int Size() {return 0;}
	virtual void GetMemoryUsage(ICrySizer *pSizer) const {}
};

#include "CREMesh.h"
#include "CRESky.h"
#include "CREDummy.h"
#include "CRETerrainSector.h"
#include "CREOcclusionQuery.h"
#include "CREImposter.h"
#include "CREBaseCloud.h" 
#include "CREPostProcess.h" 
#include "CREFogVolume.h" 
#include "CREWaterVolume.h" 
#include "CREWaterWave.h" 
#include "CREWaterOcean.h" 
#include "CREParticle.h" 
#include "CREVolumeObject.h" 
#include "CREIrradianceVolume.h" 
#include "CREGameEffect.h" 
#include "CRELightShape.h"


#if !defined(EXCLUDE_DOCUMENTATION_PURPOSE)
#include "CREPrismObject.h" 
#endif // EXCLUDE_DOCUMENTATION_PURPOSE

//==========================================================

#endif  // __RENDELEMENT_H__
