/*=============================================================================
  IShader.h : Shaders common interface.
  Copyright (c) 2001-2002 Crytek Studios. All Rights Reserved.

  Revision history:
    * Created by Honich Andrey

=============================================================================*/
#include DEVIRTUALIZE_HEADER_FIX(IShader.h)

#ifndef _ISHADER_H_
#define _ISHADER_H_


#if defined(LINUX) || defined(PS3)
  #include <platform.h>
#endif

#include "smartptr.h"
#include "IMaterial.h"
#include "VertexFormats.h"

#include "Cry_XOptimise.h"
#include <CrySizer.h>

#include "crc32.h"

struct IRenderMesh;
struct IShader;
struct IVisArea;
class CShader;
struct STexAnim;
struct SShaderPass;
struct SShaderItem;
class ITexture;
struct SParam;
class CMaterial;
struct SShaderSerializeContext;
enum EVertexFormat;
struct IRenderMesh;

//================================================================

// Summary:
//	 Geometry Culling type.
enum ECull
{
  eCULL_Back = 0,	// Back culling flag.
  eCULL_Front,		// Front culling flag.
  eCULL_None		// No culling flag.
};

enum ERenderResource
{
  eRR_Unknown,
  eRR_Mesh,
  eRR_Texture,
  eRR_Shader,
  eRR_ShaderResource,
};

//=========================================================================

// Summary:
//	 Array Pointers for Shaders.

enum ESrcPointer
{
  eSrcPointer_Unknown,
  eSrcPointer_Vert,
  eSrcPointer_Color,
  eSrcPointer_Tex,
  eSrcPointer_TexLM,
  eSrcPointer_Normal,
  eSrcPointer_Binormal,
  eSrcPointer_Tangent,
  eSrcPointer_Max,
};

struct SWaveForm;
struct SWaveForm2;

typedef TAllocator16<Matrix44> MatrixAllocator16;
typedef std::vector<Matrix44, MatrixAllocator16> MatrixArray16;


#define FRF_REFRACTIVE 1
#define FRF_GLOW       2
#define FRF_HEAT       4
#define MAX_HEATSCALE  4

//=========================================================================

enum EParamType
{
  eType_UNKNOWN,
  eType_BYTE,
  eType_BOOL,
  eType_SHORT,
  eType_INT,
  eType_HALF,
  eType_FLOAT,
  eType_STRING,
  eType_FCOLOR,
  eType_VECTOR,
  eType_TEXTURE_HANDLE,
  eType_CAMERA
};

union UParamVal
{
  byte m_Byte;
  bool m_Bool;
  short m_Short;
  int m_Int;
  float m_Float;
  char *m_String;
  float m_Color[4];
  float m_Vector[3];
  CCamera *m_pCamera;
};

// Note:
//	 In order to facilitate the memory allocation tracking, we're using here this class;
//	 if you don't like it, please write a substitute for all string within the project and use them everywhere.
struct SShaderParam
{
  char m_Name[32];
  EParamType m_Type;
  UParamVal m_Value;
  string m_Script;

  inline void Construct()
  {
    memset(&m_Value, 0, sizeof(m_Value));
    m_Type = eType_UNKNOWN;
    m_Name[0] = 0;
  }
  inline SShaderParam()
  {
    Construct();
  }
  size_t Size()
  {
    size_t nSize = sizeof(*this);
    nSize += sizeof(m_Name);
    if (m_Type == eType_STRING)
      nSize += strlen (m_Value.m_String) + 1;

    return nSize;
  }
	void GetMemoryUsage(ICrySizer* pSizer) const
	{
		pSizer->AddObject(m_Script  );
		if (m_Type == eType_STRING)
			pSizer->AddObject( m_Value.m_String, strlen (m_Value.m_String) + 1 );
	}

  inline void Destroy()
  {
    if (m_Type == eType_STRING)
      delete [] m_Value.m_String;
  }
  inline ~SShaderParam()
  {
    Destroy();
  }
  inline SShaderParam (const SShaderParam& src)
  {
    memcpy(m_Name, src.m_Name, sizeof(m_Name));
    m_Script = src.m_Script;
    m_Type = src.m_Type;
    if (m_Type == eType_STRING)
		{
      m_Value.m_String = new char[ strlen(src.m_Value.m_String)+1 ];
      strcpy(m_Value.m_String, src.m_Value.m_String);
		}
		else
			m_Value = src.m_Value;
  }
  inline SShaderParam& operator = (const SShaderParam& src)
  {
    this->~SShaderParam();
    new(this) SShaderParam(src);
    return *this;
  }

  static bool SetParam(const char* name, DynArrayRef<SShaderParam> *Params, UParamVal& pr)
  {
    uint32 i;
    for (i=0; i<(uint32)Params->size(); i++)
    { 
      SShaderParam *sp = &(*Params)[i]; 
      if (!sp)
        continue;
      if (!stricmp(sp->m_Name, name))
      {
        switch (sp->m_Type)
        {          
          case eType_FLOAT:
            sp->m_Value.m_Float = pr.m_Float;
            break;
          case eType_SHORT:
            sp->m_Value.m_Short = pr.m_Short;
            break;
          case eType_INT:
          case eType_TEXTURE_HANDLE:
            sp->m_Value.m_Int = pr.m_Int;
            break;

          case eType_VECTOR:
            sp->m_Value.m_Vector[0] = pr.m_Vector[0];
            sp->m_Value.m_Vector[1] = pr.m_Vector[1];
            sp->m_Value.m_Vector[2] = pr.m_Vector[2];
            break;

          case eType_FCOLOR:
            sp->m_Value.m_Color[0] = pr.m_Color[0];
            sp->m_Value.m_Color[1] = pr.m_Color[1];
            sp->m_Value.m_Color[2] = pr.m_Color[2];
            sp->m_Value.m_Color[3] = pr.m_Color[3];
            break;

          case eType_STRING:
            {
              char *str = pr.m_String;
              size_t len = strlen(str)+1;
              sp->m_Value.m_String = new char [len];
              strcpy(sp->m_Value.m_String, str);
            }
            break;
        }
        break;
      }
    }
    if (i == Params->size())
      return false;
    return true;
  }
  static bool GetValue(const char* szName, DynArrayRef<SShaderParam> *Params, float *v, int nID);
};


// Description:
//    IShaderPublicParams can be used to hold a collection of the shader public params.
//    Manipulate this collection, and use them during rendering by submit to the SRendParams.
UNIQUE_IFACE struct IShaderPublicParams
{
	virtual ~IShaderPublicParams(){}
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  // Description:
  //    Changes number of parameters in collection.
  virtual void SetParamCount( int nParam ) = 0;
  
  // Description:
  //    Retrieves number of parameters in collection.
  virtual int  GetParamCount() const = 0;

  // Description:
  //    Retrieves shader public parameter at specified index of the collection.
  virtual SShaderParam& GetParam( int nIndex ) = 0;
  virtual const SShaderParam& GetParam( int nIndex ) const = 0;

  // Description:
  //    Sets a shader parameter (and if doesn't exists, add it to the parameters list).
  virtual void SetParam(const char *pszName, UParamVal &pParam, EParamType nType = eType_FLOAT) = 0;

  // Description:
  //    Assigns shader public parameter at specified index of the collection.
  virtual void SetParam( int nIndex,const SShaderParam &param ) = 0;

  // Description:
  //    Assigns existing shader parameters list.
  virtual void SetShaderParams( const DynArray<SShaderParam> &pParams) = 0;

  // Description:
  //    Adds a new shader public parameter at the end of the collection.
  virtual void AddParam( const SShaderParam &param ) = 0;

  // Description:
  //    Assigns collection of shader public parameters to the specified render params structure.
  //virtual void AssignToRenderParams( struct SRendParams &rParams ) = 0;

  // Description:
  //    Gets shader parameters.
  virtual DynArray<SShaderParam> *GetShaderParams() = 0;
  virtual const DynArray<SShaderParam> *GetShaderParams() const = 0;
};

//=================================================================================

class CInputLightMaterial
{
public:
  CInputLightMaterial() :
    m_Diffuse(0,0,0,0),
    m_Specular(0,0,0,0),
    m_Emission(0,0,0,0),
    m_SpecShininess(0)
  {
  }

  ColorF m_Diffuse;
  ColorF m_Specular;
  ColorF m_Emission;
  float m_SpecShininess;

  inline friend bool operator == (const CInputLightMaterial &m1, const CInputLightMaterial &m2)
  {
    if (m1.m_Diffuse == m2.m_Diffuse && m1.m_Specular == m2.m_Specular && m1.m_Emission == m2.m_Emission && m1.m_SpecShininess == m2.m_SpecShininess)
      return true;
    return false;
  }

  int Size()
  {
    int nSize = sizeof(CInputLightMaterial);
    return nSize;
  }
};

class CTexture;
#include <ITexture.h>

// Summary:
//	 Vertex modificators definitions (must be 16 bit flag).

#define MDV_BENDING            0x100
#define MDV_DET_BENDING        0x200
#define MDV_DET_BENDING_GRASS  0x400
#define MDV_WIND               0x800
#define MDV_TERRAIN_ADAPT      0x1000
#define MDV_DEPTH_OFFSET       0x2000

// Summary:
//	 Deformations/Morphing types.
enum EDeformType
{
	eDT_Unknown = 0,
	eDT_SinWave = 1,
	eDT_SinWaveUsingVtxColor = 2,
	eDT_Bulge = 3,
	eDT_Squeeze = 4,
	eDT_Perlin2D = 5,
	eDT_Perlin3D = 6,
	eDT_FromCenter = 7,
	eDT_Bending = 8,  
	eDT_ProcFlare = 9,
	eDT_AutoSprite = 10,
	eDT_Beam = 11,
	eDT_FixedOffset = 12,
};

// Summary:
//	 Wave form evaluator flags.
enum EWaveForm
{
  eWF_None,
  eWF_Sin,
  eWF_HalfSin,
  eWF_InvHalfSin,
  eWF_Square,
  eWF_Triangle,
  eWF_SawTooth,
  eWF_InvSawTooth,
  eWF_Hill,
  eWF_InvHill,
};

#define WFF_CLAMP 1
#define WFF_LERP  2

// Summary:
//	 Wave form definition.
struct SWaveForm
{
  EWaveForm m_eWFType;
  byte m_Flags;

  float m_Level;
  float m_Level1;
  float m_Amp;
  float m_Amp1;
  float m_Phase;
  float m_Phase1;
  float m_Freq;
  float m_Freq1;

  SWaveForm(EWaveForm eWFType, float fLevel, float fAmp, float fPhase, float fFreq)
  {
    m_eWFType = eWFType;
    m_Level = m_Level1 = fLevel;
    m_Amp = m_Amp1 = fAmp;
    m_Phase = m_Phase1 = fPhase;
    m_Freq = m_Freq1 = fFreq;
  }

  int Size()
  {
    int nSize = sizeof(SWaveForm);
    return nSize;
  }
  SWaveForm()
  {
    memset(this, 0, sizeof(SWaveForm));
  }
  bool operator == (const SWaveForm& wf)
  {
    if (m_eWFType == wf.m_eWFType && m_Level == wf.m_Level && m_Amp == wf.m_Amp && m_Phase == wf.m_Phase && m_Freq == wf.m_Freq && m_Level1 == wf.m_Level1 && m_Amp1 == wf.m_Amp1 && m_Phase1 == wf.m_Phase1 && m_Freq1 == wf.m_Freq1 && m_Flags == wf.m_Flags)
      return true;
    return false;
  }

  SWaveForm& operator += (const SWaveForm& wf )
  {
    m_Level  += wf.m_Level;
    m_Level1 += wf.m_Level1;
    m_Amp  += wf.m_Amp;
    m_Amp1 += wf.m_Amp1;
    m_Phase  += wf.m_Phase;
    m_Phase1 += wf.m_Phase1;
    m_Freq  += wf.m_Freq;
    m_Freq1 += wf.m_Freq1;
    return *this;
  }
};

struct SWaveForm2
{
  EWaveForm m_eWFType;

  float m_Level;
  float m_Amp;
  float m_Phase;
  float m_Freq;

  SWaveForm2()
  {
    memset(this, 0, sizeof(SWaveForm2));
  }
  bool operator == (const SWaveForm2& wf)
  {
    if (m_eWFType == wf.m_eWFType && m_Level == wf.m_Level && m_Amp == wf.m_Amp && m_Phase == wf.m_Phase && m_Freq == wf.m_Freq)
      return true;
    return false;
  }

  SWaveForm2& operator += (const SWaveForm2& wf )
  {
    m_Level  += wf.m_Level;
    m_Amp  += wf.m_Amp;
    m_Phase  += wf.m_Phase;
    m_Freq  += wf.m_Freq;
    return *this;
  }
};

struct SDeformInfo
{
	EDeformType m_eType;
	SWaveForm2 m_WaveX;
	SWaveForm2 m_WaveY;
	SWaveForm2 m_WaveZ;
	SWaveForm2 m_WaveW;
	float m_fDividerX;
	float m_fDividerY;
	float m_fDividerZ;
	float m_fDividerW;
	Vec3 m_vNoiseScale;

	SDeformInfo()
	{
		m_eType = eDT_Unknown;
		m_fDividerX = 0.01f;
		m_fDividerY = 0.01f;
		m_fDividerZ = 0.01f;
		m_fDividerW = 0.01f;
		m_vNoiseScale = Vec3(1,1,1);
	}

	inline bool operator == (const SDeformInfo &m)
	{
		if (m_eType == m.m_eType && 
				m_WaveX == m.m_WaveX && m_WaveY == m.m_WaveY && 
				m_WaveZ == m.m_WaveZ && m_WaveW == m.m_WaveW &&
				m_vNoiseScale == m.m_vNoiseScale &&
				m_fDividerX != m.m_fDividerX && m_fDividerY != m.m_fDividerY &&
				m_fDividerZ != m.m_fDividerZ && m_fDividerW != m.m_fDividerW )
			return true;

		return false;
	}

  int Size()
  {
    return sizeof(SDeformInfo);
  }

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->Add( *this );
	}
};

struct SDetailDecalInfo
{  
  Vec4   vTileOffs[2];  // xy = tilling, zw = offsets
  uint16 nRotation[2];
  uint8  nThreshold[2];
  uint8  nDeformation[2];
  uint8  nBlending;
  uint8  nSSAOAmount;

  SDetailDecalInfo()
  {
    vTileOffs[0] = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
    vTileOffs[1] = Vec4(0.5f, 0.5f, 0.5f, 0.5f);
    nThreshold[0] = nThreshold[1] = 0;
    nDeformation[0] = nDeformation[1] = 0;
    nRotation[0] = nRotation[1] = 0;
    nBlending = 128;
    nSSAOAmount = 0;
  }
  inline bool operator == (const SDetailDecalInfo &m)
  {
    if (vTileOffs[0].x == m.vTileOffs[0].x && vTileOffs[0].y == m.vTileOffs[0].y &&
        vTileOffs[0].z == m.vTileOffs[0].z && vTileOffs[0].w == m.vTileOffs[0].w &&
        vTileOffs[1].x == m.vTileOffs[1].x && vTileOffs[1].y == m.vTileOffs[1].y &&
        vTileOffs[1].z == m.vTileOffs[1].z && vTileOffs[1].w == m.vTileOffs[1].w &&
        nDeformation[0] == m.nDeformation[0] && nDeformation[1] != m.nDeformation[1] &&
        nRotation[0] == m.nRotation[0] && nRotation[1] != m.nRotation[1] &&
        nThreshold[0] != m.nThreshold[0] && nThreshold[1] != m.nThreshold[1] &&
        nBlending != m.nBlending && nSSAOAmount != m.nSSAOAmount)
      return true;
    return false;
  }

  void Reset()
  {
    vTileOffs[0] = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
    vTileOffs[1] = Vec4(0.5f, 0.5f, 0.5f, 0.5f);
    nThreshold[0] = nThreshold[1] = 0;
    nDeformation[0] = nDeformation[1] = 0;
    nRotation[0] = nRotation[1] = 0;
    nBlending = 128;
    nSSAOAmount = 0;
  }

  int Size()
  {
    return sizeof(SDetailDecalInfo);
  }

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->Add( *this );
	}
};

//==============================================================================
// CRenderObject

//////////////////////////////////////////////////////////////////////
// CRenderObject::m_ObjFlags: Flags used by shader pipeline

#define FOB_TRANS_ROTATE    (1<<0)			// 1
#define FOB_TRANS_SCALE     (1<<1)			// 2
#define FOB_TRANS_TRANSLATE (1<<2)			// 4
#define FOB_TRANS_MASK (FOB_TRANS_ROTATE | FOB_TRANS_SCALE | FOB_TRANS_TRANSLATE)

#define FOB_RENDER_AFTER_POSTPROCESSING (1<<3)	// 8
#define FOB_OWNER_GEOMETRY  (1<<4)			// 0x10
#define FOB_NO_Z_PASS       (1<<5)			// 0x20
#define FOB_SELECTED        (1<<6)			// 0x40
#define FOB_PERMANENT       (1<<7)			// 0x80

#define FOB_GLOBAL_ILLUMINATION  (1<<8)			// 0x100
#define FOB_NO_FOG					(1<<9)			// 0x200
#define FOB_CAMERA_SPACE    (1<<10)			// 0x400
#define FOB_ONLY_Z_PASS			(1<<11)     // 0x800

#define FOB_BLEND_WITH_TERRAIN_COLOR (1<<12)// 0x1000
#define FOB_REMOVED         (1<<13)			// 0x2000
#define FOB_SOFT_PARTICLE   (1<<14)			// 0x4000
#define FOB_REQUIRES_RESOLVE       (1<<15)			// 0x8000

#define FOB_UPDATED_RTMASK	(1<<16)			// 0x10000

//#define FOB_CUSTOM_CAMERA   (1<<16)			// 0x10000
//#define FOB_AMBIENT_OCCLUSION (1<<17)		// 0x20000
#define FOB_AFTER_WATER (1<<17)		// 0x20000
#define FOB_BENDED          (1<<18)			// 0x40000
#define FOB_SHADER_LOD  (1<<19)     // 0x80000

#define FOB_INSHADOW        (1<<20)			// 0x100000
#define FOB_DISSOLVE        (1<<21)				// 0x200000

// available
#define FOB_HAS_PREVSKINXFORM	(1<<22)      // 0x400000

#define FOB_NEAREST         (1<<23)			// 0x800000

#define FOB_CHARACTER       (1<<24)			// 0x1000000
#define FOB_SHADOW_DISSOLVE (1<<25)			// 0x2000000
#define FOB_PARTICLE_MASK		(FOB_SOFT_PARTICLE | FOB_NO_FOG)

#define FOB_NO_STATIC_DECALS	(1<<26)         // 0x4000000

#define FOB_DECAL           (1<<27)			// 0x8000000
#define FOB_DECAL_TEXGEN_2D (1<<28)			// 0x10000000
//#define FOB_STATIC (1<<28)			// 0x10000000
#define FOB_MESH_SUBSET_INDICES (1<<29)			// 0x20000000
#define FOB_VEGETATION      (1<<30)			// 0x40000000
#define FOB_HAS_PREVMATRIX        (1<<31)			// 0x80000000
#define FOB_DECAL_MASK		  (FOB_DECAL | FOB_DECAL_TEXGEN_2D)

#define FOB_MASK_AFFECTS_MERGING_GEOM  (FOB_VEGETATION | FOB_CHARACTER | FOB_BENDED | FOB_NO_STATIC_DECALS | FOB_BLEND_WITH_TERRAIN_COLOR)
// Note:
//	 WARNING: FOB_MASK_AFFECTS_MERGING must start from 0x10000 (important for instancing).
#define FOB_MASK_AFFECTS_MERGING  (FOB_SHADER_LOD| FOB_HAS_PREVSKINXFORM | FOB_HAS_PREVMATRIX | FOB_VEGETATION | FOB_CHARACTER | FOB_BENDED | FOB_INSHADOW/* | FOB_AMBIENT_OCCLUSION*/| FOB_AFTER_WATER | FOB_DISSOLVE | FOB_NEAREST | FOB_NO_STATIC_DECALS)
#define FOB_PERSISTENT  (FOB_PERMANENT | FOB_REMOVED)

//////////////////////////////////////////////////////////////////////
// CRenderObject::m_customFlags: Custom Flags used by shader pipeline
// (Union with CRenderObject::m_nTextureID, which is only used with the terrain)
#define COB_FADE_CLOAK_BY_DISTANCE    (1<<0)			// 1
#define COB_CUSTOM_POST_EFFECT				(1<<1)			// 2
#define COB_IGNORE_HUD_INTERFERENCE_FILTER    (1<<2)			// 4
#define COB_IGNORE_HEAT_AMOUNT				(1<<3)			// 8
#define COB_POST_3D_RENDER					(1<<4)			// 0x10
#define COB_IGNORE_CLOAK_REFRACTION_COLOR		(1<<5)			// 0x20

struct SInstanceInfo
{
  Matrix34A m_Matrix;
  ColorF m_AmbColor;
};

struct SSkyInfo
{
	ITexture *m_SkyBox[3];
	float m_fSkyLayerHeight;

	int Size()
	{
		int nSize = sizeof(SSkyInfo);
		return nSize;
	}
	SSkyInfo()
	{
		memset(this, 0, sizeof(SSkyInfo));
	}
};

// Custom data used for objects without custom textures
struct SCustomObjData
{
	uint8	m_nData;
	uint8	m_nFlags;
};

struct SRenderObjData
{
	void							*m_pInstance;			

	CRendElementBase	*m_pRE;
	struct ISkinnable	*m_pCharInstance;
	
	TArray<Vec4>	m_Constants;

	float	m_fTempVars[10];									// Different useful vars (ObjVal component in shaders)

  union
  {
    CDLight *m_pLight;
		CCamera *m_pCustomCamera;	 //
  };
	uint8	m_nObjID;
	uint8	m_nVisionScale;

	uint32	m_nVisionParams;	
	uint32	m_nHUDSilhouetteParams;	

	union
	{
		uint32	m_HMAData;									// only used for vegetation
		float		m_fHMAData;									// float variant for reinterpret_cast access
		uint32	m_pLayerEffectParams;				// only used for layer effects
	};

	uint16	m_FogVolumeContribIdx[2];

	uint16	m_scissorX;
	uint16	m_scissorY;

	uint16	m_scissorWidth;
	uint16	m_scissorHeight;

	uint8	m_nCoarseShadowMask[4];

	uint8 m_screenBounds[4];
	
	SCustomObjData						m_nCustomData;					// Custom data used for objects without custom textures

	uint64 m_nSubObjHideMask;	

  SRenderObjData()
  {
    Init();
  }

	void Init()
	{
		m_nSubObjHideMask = 0;		
		m_Constants.Free();
		//m_pInstancingInfo = NULL;
		m_pInstance = NULL;
		m_nVisionScale = 1;
    m_nVisionParams = 0;
		m_pRE = NULL;	
		m_HMAData = 0;
		*((uint32*)m_nCoarseShadowMask) = 0;
		m_pCharInstance = NULL;
		m_scissorX = m_scissorY = m_scissorWidth = m_scissorHeight = 0;
		m_screenBounds[0] = m_screenBounds[1] = m_screenBounds[2] = m_screenBounds[3] = 0;
		m_nObjID = 0;
		m_nCustomData.m_nData = m_nCustomData.m_nFlags = 0;
		m_pLayerEffectParams = m_nHUDSilhouetteParams = m_nVisionParams = 0;
	}

	void GetMemoryUsage(ICrySizer *pSizer) const
	{
		pSizer->AddObject(m_Constants);
	}
};

struct SBending
{
  Vec2 m_vBending;
  float m_fMainBendingScale;
  SWaveForm2 m_Waves[2];

  SBending()
  {
    m_vBending.zero();
    m_fMainBendingScale = 1.f;
  }
};

//////////////////////////////////////////////////////////////////////
// Objects using in shader pipeline

// Description:
//	 Interface for the skinnable objects (renderer calls its functions to get the skinning data).
struct ISkinnable 
{
	virtual ~ISkinnable(){}
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  // Description:
  //	 Renderer calls this function to allow update the video vertex buffers right before the rendering.
  virtual uint32 GetSkeletonPose(int nLod, int nList, const Matrix34& RenderMat34, QuatTS*& pBoneQuatsL, QuatTS*& pBoneQuatsS, QuatTS*& pMBBoneQuatsL, QuatTS*& pMBBoneQuatsS, Vec4 shapeDeformationData[], uint32 &DoWeNeedMorphtargets, uint8*& pRemapTable ) = 0;
};

// Summary:
//	 Same as in the 3dEngine.
#define MAX_LIGHTS_NUM 32

struct ShadowMapFrustum;

// Description:
//	 Size of CRenderObject currently is 128 bytes. 
// Note:
//	 Don't change it's size please!
class CRenderObject
{
public:
  CRenderObject()
  {
    //m_ShaderParams = NULL;







    m_nRenderQuality = 65535;
    m_nRTMask = 0;
    m_nObjDataId = -1;
    m_fSort = 0;

    m_nVisAreaStencilRef = 0;
    m_nMaterialLayers = 0;
    m_pRenderNode = NULL;

    m_pBending = NULL;
    m_nMDV = 0;
    m_ObjFlags = 0;
    m_pCurrMaterial = NULL;
    m_DissolveRef = 0;
    m_nCBID = -1;

  }
  CRenderObject(const CRenderObject& other) 
  { CloneObject(const_cast<CRenderObject*>(&other)); }

  ~CRenderObject();

	// trying to avoid L2 cache misses, keep most accessed data in first cache line
	uint32                      m_ObjFlags;
	int16                       m_Id;							// Permanent object.
private:
  int16                       m_nObjDataId;
public:
	float                       m_fAlpha;						// Object alpha.
	float											  m_fDistance;					// Distance to the object.			

	//Moving matrix down by 16 bytes to allow m_fSort and m_fDistance to have spatial locality to avoid cache misses
	//	in mfSortByDist
	union
	{
		float					  m_fSort;						// Custom sort value.
		uint16          m_nSort;
	};

	uint16                      m_nMDV; 	            		// Vertex modificator flags.  	
	uint16										  m_nRenderQuality;				// 65535 - full quality, 0 - lowest quality, used by CStatObj
	int16                     m_nTextureID;						// Custom texture id.
	int16                       m_nCBID;      				// Constant buffer id.

	uint8												m_nLod;	           
  uint8                       m_nVisAreaStencilRef;     // Per instance vis area stencil reference ID
  uint8												m_DissolveRef;								//
	uint8		  									m_RState;									//	

  SInstanceInfo               m_II;  
  
	uint32											m_DynLMMask[RT_COMMAND_BUF_COUNT];
  uint32                      m_nMaterialLayers;          // Which mtl layers active and how much to blend them  
  uint32                      m_nRTMask;

	PodArray<ShadowMapFrustum*> *	m_pShadowCasters;			// List of shadow casters .
	void        *               m_pRenderNode;				// Will define instance id.
	IMaterial   *               m_pCurrMaterial;				// Current material (is this available somewhere ??).
  SBending					  *m_pBending;

  //=========================================================================================================

  inline Vec3 GetTranslation() const
  {
    return m_II.m_Matrix.GetTranslation();
  }
  inline float GetScaleX() const
  {
    return cry_sqrtf(m_II.m_Matrix(0,0)*m_II.m_Matrix(0,0) + m_II.m_Matrix(0,1)*m_II.m_Matrix(0,1) + m_II.m_Matrix(0,2)*m_II.m_Matrix(0,2));
  }
  inline float GetScaleZ() const
  {
    return cry_sqrtf(m_II.m_Matrix(2,0)*m_II.m_Matrix(2,0) + m_II.m_Matrix(2,1)*m_II.m_Matrix(2,1) + m_II.m_Matrix(2,2)*m_II.m_Matrix(2,2));
  }
  inline bool IsMergable()
  {
    if (m_fAlpha != 1.0f)
      return false;
    if (m_pShadowCasters)
      return false;
    if (m_ObjFlags & FOB_CHARACTER)
      return false;
    return true;
  }

  static TArray<SRenderObjData> m_sObjData[RT_COMMAND_BUF_COUNT];
  static SRenderObjData *m_pPermObjData;
  static int m_nPermObjDataID;
  static TArray<int> m_sFreePermObjData;

  static void Tick();
	static void FlushFreeObjects(int list);

	void Init(int nThreadID)
	{
		m_ObjFlags &= FOB_PERSISTENT;
		if (!(m_ObjFlags & FOB_PERMANENT))
			m_nObjDataId = -1;
		m_nRenderQuality = 65535;
		m_DynLMMask[nThreadID] = 0;
		m_RState = 0;
		m_fDistance = 0.0f;

		m_nVisAreaStencilRef = 0;
		m_nMaterialLayers = 0;
		m_DissolveRef = 0;

		m_nMDV = 0;
		m_fSort = 0;
		m_pBending = NULL;
		m_pShadowCasters = NULL;

		m_II.m_AmbColor = Col_White;
		m_fAlpha = 1.0f;
		m_nTextureID = -1;
		m_nCBID = -1;
		m_pCurrMaterial = NULL;

		m_nRTMask = 0;

		m_pRenderNode = NULL;
	}

  void CloneObject(CRenderObject *srcObj)
  {
    int Id = m_Id;
    memcpy(this, srcObj, sizeof(*srcObj));
    m_Id = Id;
  }

  ILINE Matrix34A &GetMatrix()
  {
    return m_II.m_Matrix;
  }

  CRenderObject& operator= (const CRenderObject& other) 
  { 
    if (this != &other) 
      this->CloneObject(const_cast<CRenderObject*>(&other)); 
    return *this; 
  } 

  ILINE SRenderObjData *GetObjData(int nProcessID)
  {
    if ((m_nObjDataId < 0) || 
        ((m_ObjFlags & FOB_PERMANENT) && m_nObjDataId >= m_nPermObjDataID) || 
        (((m_ObjFlags & FOB_PERMANENT) == 0 ) && (size_t)m_nObjDataId >= m_sObjData[nProcessID].Num()))
    {
#ifndef _RELEASE
      if (m_nObjDataId>=0) __debugbreak();
#endif
      return NULL;
    }
    if (m_ObjFlags & FOB_PERMANENT)
      return &m_pPermObjData[m_nObjDataId];
    return &m_sObjData[nProcessID][m_nObjDataId];
  }

  ILINE int16 GetObjDataId() 
  {
    return m_nObjDataId; 
  }

  ILINE void SetObjDataId(int16 newObjDataId) 
  {
    m_nObjDataId = newObjDataId; 
  }

  void CopyTo(CRenderObject *pObjNew);

  ILINE CRendElementBase *GetRE(int nProcessID)
  {
    return m_nObjDataId>=0 ? GetObjData(nProcessID)->m_pRE : NULL;
  }

	void GetMemoryUsage(ICrySizer *pSizer) const{}

  /*ILINE DynArray16<SInstanceInfo> *GetInstanceInfo(int nProcessID)
  {
    return m_nObjDataId>=0 ? GetObjData(nProcessID)->m_pInstancingInfo : NULL;
  }*/
#ifndef PS3
  void* operator new( size_t Size )
  {
    void *pPtrRes = CryModuleMemalign(Size, 16);
    return pPtrRes;
  }
  void* operator new(size_t Size, const std::nothrow_t &nothrow)
  {
    return ::operator new(Size);
  }
  void* operator new[](size_t Size)
  {








    byte *ptr = (byte *)malloc(Size+16+2*sizeof(INT_PTR));
    memset(ptr, 0, Size+16+2*sizeof(INT_PTR));
    byte *bPtrRes = (byte *)((INT_PTR)(ptr+16+2*sizeof(INT_PTR)) & ~0xf);
    ((byte**)bPtrRes)[-2] = ptr;

    return bPtrRes-sizeof(INT_PTR);

  }
  void* operator new[](size_t Size, const std::nothrow_t &nothrow)
  {
    return ::operator new[](Size);
  }
  void operator delete( void *Ptr )
  {
    CryModuleMemalignFree(Ptr);
  }

  void operator delete[]( void *Ptr )
  {




    byte *bActualPtr = ((byte **)Ptr)[-2];
    free ((void *)bActualPtr);

  }

  // TEMPORARY workaround!
  // moved semicolon behind ifdefs to prevent devirt from barfing.

}


#endif//PS3
;


struct SRenderObjectModifier
{
  SRenderObjectModifier() { memset(this, 0, sizeof(*this)); }

	SRenderObjectModifier(const SRenderObjectModifier* pROM, const Matrix34& newMat, 
												const Matrix34& newPrevMat, IRenderMesh* const pNewWeights)
	: mat(newMat)
	, prev_mat(newPrevMat)
	, pWeights(pNewWeights)
	, nMatricesInUse(1)
	, nStatesInUse(pROM?pROM->nStatesInUse:0)
	, fDistance(pROM?pROM->fDistance:0.0f)
	{}

  const bool InUse() const { return nStatesInUse || nMatricesInUse; }

  Matrix34 mat;
  Matrix34 prev_mat;
  IRenderMesh * pWeights;
  byte nMatricesInUse;
  byte nStatesInUse;
  float fDistance;
};

enum EResClassName
{
  eRCN_Texture,
  eRCN_Shader,
};

// className: CTexture, CHWShader_VS, CHWShader_PS, CShader
struct SResourceAsync
{
  int nReady;            // 0: Not ready; 1: Ready; -1: Error
  byte *pData;
  EResClassName eClassName;       // Resource class name
  char *Name;            // Resource name
  union
  {
    // CTexture parameters
		struct { int nWidth, nHeight, nMips, nTexFlags, nFormat, nTexId; };
    // CShader parameters
    struct { int nShaderFlags; };
  };
  void *pResource; // Pointer to created resource

  SResourceAsync()
  {
    memset(this, 0, sizeof(SResourceAsync));
  }

  ~SResourceAsync()
  {
    delete Name;
  }  
};

//==============================================================================

// Summary:
//	 Color operations flags.
enum EColorOp
{
  eCO_NOSET = 0,
  eCO_DISABLE = 1,
  eCO_REPLACE = 2,
  eCO_DECAL = 3,
  eCO_ARG2 = 4,
  eCO_MODULATE = 5,
  eCO_MODULATE2X = 6,
  eCO_MODULATE4X = 7,
  eCO_BLENDDIFFUSEALPHA = 8,
  eCO_BLENDTEXTUREALPHA = 9,
  eCO_DETAIL = 10,
  eCO_ADD = 11,
  eCO_ADDSIGNED = 12,
  eCO_ADDSIGNED2X = 13,
  eCO_MULTIPLYADD = 14,
  eCO_BUMPENVMAP = 15,
  eCO_BLEND = 16,
  eCO_MODULATEALPHA_ADDCOLOR = 17,
  eCO_MODULATECOLOR_ADDALPHA = 18,
  eCO_MODULATEINVALPHA_ADDCOLOR = 19,
  eCO_MODULATEINVCOLOR_ADDALPHA = 20,
  eCO_DOTPRODUCT3 = 21,
  eCO_LERP = 22,
  eCO_SUBTRACT = 23,
};

enum EColorArg
{
  eCA_Specular,
  eCA_Texture,
  eCA_Diffuse,
  eCA_Previous,
  eCA_Constant,
};

#define DEF_TEXARG0 (eCA_Texture|(eCA_Diffuse<<3))
#define DEF_TEXARG1 (eCA_Texture|(eCA_Previous<<3))

enum ETexModRotateType
{
  ETMR_NoChange,
  ETMR_Fixed,
  ETMR_Constant,
  ETMR_Oscillated,
};

enum ETexModMoveType
{
  ETMM_NoChange,
  ETMM_Fixed,
  ETMM_Constant,
  ETMM_Jitter,
  ETMM_Pan,
  ETMM_Stretch,
  ETMM_StretchRepeat,
};

enum ETexGenType
{
  ETG_Stream,
  ETG_World,
  ETG_Camera,
  ETG_WorldEnvMap,
  ETG_CameraEnvMap,
  ETG_NormalMap,
  ETG_SphereMap,
};

#define CASE_TEXMOD(var_name)\
  if(!stricmp(#var_name,szParamName))\
  {\
    var_name = fValue;\
    return true;\
  }\

#define CASE_TEXMODANGLE(var_name)\
  if(!stricmp(#var_name,szParamName))\
{\
  var_name = Degr2Word(fValue);\
  return true;\
}\

#define CASE_TEXMODBYTE(var_name)\
  if(!stricmp(#var_name,szParamName))\
{\
  var_name = (byte)fValue;\
  return true;\
}\

#define CASE_TEXMODBOOL(var_name)\
  if(!stricmp(#var_name,szParamName))\
{\
  var_name = (fValue==1.f);\
  return true;\
}\

struct SEfTexModificator
{
  bool SetMember(const char * szParamName, float fValue)
  {
    CASE_TEXMODBYTE(m_eTGType);
    CASE_TEXMODBYTE(m_eRotType);
    CASE_TEXMODBYTE(m_eUMoveType);
    CASE_TEXMODBYTE(m_eVMoveType);
    CASE_TEXMODBOOL(m_bTexGenProjected);

    CASE_TEXMOD(m_Tiling[0]);
    CASE_TEXMOD(m_Tiling[1]);
    CASE_TEXMOD(m_Tiling[2]);
    CASE_TEXMOD(m_Offs[0]);
    CASE_TEXMOD(m_Offs[1]);
    CASE_TEXMOD(m_Offs[2]);

    CASE_TEXMODANGLE(m_Rot[0]);
    CASE_TEXMODANGLE(m_Rot[1]);
    CASE_TEXMODANGLE(m_Rot[2]);
    CASE_TEXMODANGLE(m_RotOscRate[0]);
    CASE_TEXMODANGLE(m_RotOscRate[1]);
    CASE_TEXMODANGLE(m_RotOscRate[2]);
    CASE_TEXMODANGLE(m_RotOscAmplitude[0]);
    CASE_TEXMODANGLE(m_RotOscAmplitude[1]);
    CASE_TEXMODANGLE(m_RotOscAmplitude[2]);
    CASE_TEXMODANGLE(m_RotOscPhase[0]);
    CASE_TEXMODANGLE(m_RotOscPhase[1]);
    CASE_TEXMODANGLE(m_RotOscPhase[2]);
    CASE_TEXMOD(m_RotOscCenter[0]);
    CASE_TEXMOD(m_RotOscCenter[1]);
    CASE_TEXMOD(m_RotOscCenter[2]);

    CASE_TEXMOD(m_UOscRate);
    CASE_TEXMOD(m_VOscRate);
    CASE_TEXMOD(m_UOscAmplitude);
    CASE_TEXMOD(m_VOscAmplitude);
    CASE_TEXMOD(m_UOscPhase);
    CASE_TEXMOD(m_VOscPhase);

    return false;
  }

  bool m_bTexGenProjected;
  uint8 m_eTGType;
  uint8 m_eRotType;
  uint8 m_eUMoveType;
  uint8 m_eVMoveType;
  uint16 m_Rot[3];
  Matrix44 m_TexGenMatrix;
  Matrix44 m_TexMatrix;

  float m_Tiling[3];
  float m_Offs[3];

  uint16 m_RotOscRate[3];
  uint16 m_RotOscAmplitude[3];
  float m_RotOscCenter[3];

  float m_UOscRate;
  float m_VOscRate;
  float m_UOscAmplitude;
  float m_VOscAmplitude;
  float m_UOscPhase;
  float m_VOscPhase;

  // This members are used only during updating of the matrices
  float m_LastUTime;
  float m_LastVTime;
  float m_CurrentUJitter;
  float m_CurrentVJitter;

  uint16 m_RotOscPhase[3];
	
	inline SEfTexModificator( void )
  {
    memset(this, 0, sizeof(SEfTexModificator));
    m_Tiling[0] = m_Tiling[1] = 1.0f;
  }
	inline SEfTexModificator(const SEfTexModificator &m)
	{
		if (&m != this)
		{
			memcpy(this, &m, sizeof(*this));
		}
	}
  SEfTexModificator& operator = (const SEfTexModificator& src)
  {
    if (&src != this)
    {
      this->~SEfTexModificator();
      new(this) SEfTexModificator(src);
    }
    return *this;
  }
  int Size()
  {
    int nSize = sizeof(SEfTexModificator);

    return nSize;
  }

  inline bool operator != (const SEfTexModificator &m)
  {
    if (m_eTGType != m.m_eTGType ||
        m_eRotType != m.m_eRotType ||
        m_eUMoveType != m.m_eUMoveType ||
        m_eVMoveType != m.m_eVMoveType ||
        m_bTexGenProjected != m.m_bTexGenProjected ||
        m_UOscRate != m.m_UOscRate ||
        m_VOscRate != m.m_VOscRate ||
        m_UOscAmplitude != m.m_UOscAmplitude ||
        m_VOscAmplitude != m.m_VOscAmplitude ||
        m_UOscPhase != m.m_UOscPhase ||
        m_VOscPhase != m.m_VOscPhase)
      return true;
    for (int i=0; i<3; i++)
    {
      if (m_Tiling[i] != m.m_Tiling[i] ||
          m_Offs[i] != m.m_Offs[i] ||
          m_Rot[i] != m.m_Rot[i] ||
          m_RotOscRate[i] != m.m_RotOscRate[i] ||
          m_RotOscAmplitude[i] != m.m_RotOscAmplitude[i] ||
          m_RotOscPhase[i] != m.m_RotOscPhase[i] ||
          m_RotOscCenter[i] != m.m_RotOscCenter[i])
        return true;
			if (m_TexGenMatrix.GetRow( i ) != m.m_TexGenMatrix.GetRow( i ) || m_TexMatrix.GetRow( i ) != m.m_TexMatrix.GetRow( i ) )
				return true;
    }
    return false;
  }



};



struct STexState
{
	struct
	{
		signed char m_nMinFilter:8;
		signed char m_nMagFilter:8;
		signed char m_nMipFilter:8;
		signed char m_nAddressU:8;
		signed char m_nAddressV:8;
		signed char m_nAddressW:8;
		signed char m_nAnisotropy:8;
		signed char padding:8;
	};
	DWORD  m_dwBorderColor;
	void *m_pDeviceState;
  bool m_bActive;
  bool m_bComparison;
  bool m_bSRGBLookup;

  STexState ()
  {
    m_nMinFilter = 0;
    m_nMagFilter = 0;
    m_nMipFilter = 0;
    m_nAnisotropy = 0;
    m_nAddressU = 0;
    m_nAddressV = 0;
    m_nAddressW = 0;
    m_dwBorderColor = 0;
		padding = 0;
    m_bSRGBLookup = false;
    m_bActive = false;
    m_bComparison = false;
    m_pDeviceState = NULL;
  }
  STexState(int nFilter, bool bClamp)
  {
    m_pDeviceState = NULL;
    int nAddress = bClamp ? TADDR_CLAMP : TADDR_WRAP;
    SetFilterMode(nFilter);
    SetClampMode(nAddress, nAddress, nAddress);
    SetBorderColor(0);
		m_bSRGBLookup = false;
    m_bActive = false;
    m_bComparison = false;
		padding = 0;
  }
  STexState(int nFilter, int nAddressU, int nAddressV, int nAddressW, unsigned int borderColor)
  {
    m_pDeviceState = NULL;
    SetFilterMode(nFilter);
    SetClampMode(nAddressU, nAddressV, nAddressW);
    SetBorderColor(borderColor);
		m_bSRGBLookup = false;
    m_bActive = false;
    m_bComparison = false;
		padding = 0;
  }
#ifdef _RENDERER
  ~STexState();
  STexState (const STexState& src);
#else
  ~STexState(){}
  STexState (const STexState& src)
  {
    memcpy(this, &src, sizeof(STexState));
  }
#endif
  STexState& operator = (const STexState& src)
  {
    this->~STexState();
    new(this) STexState(src);
    return *this;
  }
  _inline friend bool operator == (const STexState &m1, const STexState &m2)
  {
    if (*(uint64 *)&m1 == *(uint64 *)&m2 && m1.m_dwBorderColor == m2.m_dwBorderColor &&
        m1.m_bComparison == m2.m_bComparison && m1.m_bSRGBLookup == m2.m_bSRGBLookup)
      return true;
    return false;
  }
  void Release()
  {
    delete this;
  }

  bool SetFilterMode(int nFilter);
  bool SetClampMode(int nAddressU, int nAddressV, int nAddressW);
  void SetBorderColor(DWORD dwColor);
  void SetComparisonFilter(bool bEnable);
  void PostCreate();
};


#define FSAMP_NOMIPS 1
#define FSAMP_BUMPPLANTS 2
#define FSAMP_GLOBAL FT_FROMIMAGE


UNIQUE_IFACE struct IRenderTarget
{
	virtual ~IRenderTarget(){}
  virtual void Release()=0;
  virtual void AddRef()=0;
};

//==================================================================================================================

// FX shader texture sampler (description)
struct STexSamplerFX
{
  string m_szName;
  string m_szTexture;

  union
  {
    struct SHRenderTarget *m_pTarget;
    IRenderTarget *m_pITarget;
  };

  int16 m_nTexState;
  byte m_eTexType;						// ETEX_Type e.g. eTT_2D or eTT_Cube
  uint32 m_nTexFlags;
  STexSamplerFX()
  {
    m_nTexState = -1;
    m_eTexType = eTT_2D;
    m_nTexFlags = 0;
    m_pTarget = NULL;
  }
  ~STexSamplerFX()
  {
    SAFE_RELEASE(m_pITarget);
  }

  int Size()
  {
    int nSize = sizeof(*this);
    nSize += m_szName.capacity();
    nSize += m_szTexture.capacity();
    return nSize;
  }

  void GetMemoryUsage( ICrySizer *pSizer ) const
  {
  }

  uint32 GetTexFlags() { return m_nTexFlags; }
  void Update();
  void PostLoad();
  NO_INLINE STexSamplerFX (const STexSamplerFX& src)
  {
    m_pITarget = src.m_pITarget;
    if (m_pITarget)
      m_pITarget->AddRef();
    m_szName = src.m_szName;
    m_szTexture = src.m_szTexture;
    m_eTexType = src.m_eTexType;
    m_nTexFlags = src.m_nTexFlags;
    m_nTexState = src.m_nTexState;
  }
  NO_INLINE STexSamplerFX& operator = (const STexSamplerFX& src)
  {
    this->~STexSamplerFX();
    new(this) STexSamplerFX(src);
    return *this;
  }
  _inline friend bool operator != (const STexSamplerFX &m1, const STexSamplerFX &m2)
  {
    if (m1.m_szTexture != m2.m_szTexture || m1.m_eTexType != m2.m_eTexType || m1.m_nTexFlags != m2.m_nTexFlags)
      return true;
    return false;
  }
  _inline bool operator == (const STexSamplerFX &m1)
  {
    return !(*this != m1);
  }
};


// Resource texture sampler (runtime)
struct STexSamplerRT
{
  union
  {
    CTexture *m_pTex;
    ITexture *m_pITex;
  };

  union
  {
    struct SHRenderTarget *m_pTarget;
    IRenderTarget *m_pITarget;
  };

	STexAnim *m_pAnimInfo;
	IDynTextureSource* m_pDynTexSource;
	
  uint32 m_nTexFlags;
  int16 m_nTexState;

	uint8 m_eTexType;						// ETEX_Type e.g. eTT_2D or eTT_Cube
  int8 m_nSamplerSlot;
  int8 m_nTextureSlot;
  bool m_bGlobal;

  STexSamplerRT()
  {
    m_nTexState = -1;
    m_pTex = NULL;
    m_eTexType = eTT_2D;
    m_nTexFlags = 0;
    m_pTarget = NULL;
		m_pAnimInfo = NULL;
    m_pDynTexSource = NULL;
    m_nSamplerSlot = -1;
    m_nTextureSlot = -1;
    m_bGlobal = false;
  }
  ~STexSamplerRT()
  {
    Cleanup();
  }

  void Cleanup()
  {
    SAFE_RELEASE(m_pITex);
    // TODO: ref counted deleting of m_pAnimInfo & m_pTarget! - CW
    SAFE_RELEASE(m_pDynTexSource);
    SAFE_RELEASE(m_pITarget);
    SAFE_RELEASE(m_pAnimInfo);
  }
  int Size() const
  {
    int nSize = sizeof(*this);
    return nSize;
  }

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
	}

  uint32 GetTexFlags() const { return m_nTexFlags; }
  void Update();
  void PostLoad();
  NO_INLINE STexSamplerRT (const STexSamplerRT& src)
  {
    m_pITex = src.m_pITex;
    if (m_pITex)
      m_pITex->AddRef();
    m_pDynTexSource = src.m_pDynTexSource;
    if (m_pDynTexSource)
      m_pDynTexSource->AddRef();
    m_pITarget = src.m_pITarget;
    if (m_pITarget)
      m_pITarget->AddRef();
    m_pAnimInfo = src.m_pAnimInfo;
    if (m_pAnimInfo)
      m_pAnimInfo->AddRef();
    m_eTexType = src.m_eTexType;
    m_nTexFlags = src.m_nTexFlags;
    m_nTexState = src.m_nTexState;
    m_nSamplerSlot = src.m_nSamplerSlot;
    m_nTextureSlot = src.m_nTextureSlot;
    m_bGlobal = src.m_bGlobal;
  }
  NO_INLINE STexSamplerRT& operator = (const STexSamplerRT& src)
  {
    this->~STexSamplerRT();
    new(this) STexSamplerRT(src);
    return *this;
  }
  STexSamplerRT (const STexSamplerFX& src)
  {
    m_pITex = NULL;
    m_pDynTexSource = NULL;
    m_pAnimInfo = NULL;
    m_pITarget = src.m_pITarget;
    if (m_pITarget)
      m_pITarget->AddRef();
    m_eTexType = src.m_eTexType;
    m_nTexFlags = src.m_nTexFlags;
    m_nTexState = src.m_nTexState;
    m_nSamplerSlot = -1;
    m_nTextureSlot = -1;
    m_bGlobal = (src.m_nTexFlags & FSAMP_GLOBAL) != 0;
  }
  inline bool operator != (const STexSamplerRT &m) const
  {
    if (m_pTex != m.m_pTex || m_eTexType != m.m_eTexType || m_nTexFlags != m.m_nTexFlags || m_nTexState != m.m_nTexState)
      return true;
    return false;
  }
};


//===============================================================================================================================

struct SEfResTextureExt
{
  int32 m_nFrameUpdated;
  int32 m_nUpdateFlags;
  int32 m_nLastRecursionLevel;
  SEfTexModificator* m_pTexModifier;
  SEfResTextureExt ()
  {
    m_nFrameUpdated = -1;
    m_nUpdateFlags = 0;
    m_nLastRecursionLevel = 0;
    m_pTexModifier = NULL;
  }
  ~SEfResTextureExt ()
  {
    Cleanup();
  }
  void Cleanup()
  {
    SAFE_DELETE(m_pTexModifier);
  }
  inline bool operator != (const SEfResTextureExt &m) const
  {
    if (m_pTexModifier && m.m_pTexModifier)
      return *m_pTexModifier != *m.m_pTexModifier;
    if (!m_pTexModifier && !m.m_pTexModifier)
      return false;
    return true;
  }
  SEfResTextureExt(const SEfResTextureExt& src)
  {
    if (&src != this)
    {
      Cleanup();
      if (src.m_pTexModifier)
      {
        m_pTexModifier = new SEfTexModificator;
        *m_pTexModifier = *src.m_pTexModifier;
      }
      m_nFrameUpdated = -1;
      m_nUpdateFlags = src.m_nUpdateFlags;
      m_nLastRecursionLevel = -1;
    }
  }
  SEfResTextureExt& operator = (const SEfResTextureExt& src)
  {
    if (&src != this)
    {
      Cleanup();
      new(this) SEfResTextureExt(src);
    }
    return *this;
  }

  void CopyTo(SEfResTextureExt *pTo) const
  {
    if (pTo != this)
    {
      pTo->Cleanup();
      pTo->m_nFrameUpdated = -1;
      pTo->m_nUpdateFlags = m_nUpdateFlags;
      pTo->m_nLastRecursionLevel = -1;
      pTo->m_pTexModifier = NULL;
    }
  }
  inline int Size() const
  {
    int nSize = sizeof(SEfResTextureExt);
    if (m_pTexModifier)
      nSize += m_pTexModifier->Size();
    return nSize;
  }
};
// Description:
//	 In order to facilitate the memory allocation tracking, we're using here this class;
//	 if you don't like it, please write a substitute for all string within the project and use them everywhere.
struct SEfResTexture
{
  string m_Name;
  byte m_TexFlags;
  bool m_bUTile;
  bool m_bVTile;
  signed char m_Filter;

  STexSamplerRT m_Sampler;
  SEfResTextureExt m_Ext;

	void UpdateForCreate();
  void Update(int nTSlot);
	void UpdateWithModifier(int nTSlot);

  inline bool operator != (const SEfResTexture &m) const
  {
    if (stricmp(m_Name.c_str(), m_Name.c_str()) != 0 ||
        m_TexFlags != m.m_TexFlags || 
        m_bUTile != m.m_bUTile ||
        m_bVTile != m.m_bVTile ||
        m_Filter != m.m_Filter ||
        m_Ext != m.m_Ext ||
        m_Sampler != m.m_Sampler)
      return true;
    return false;
  }
  inline bool IsHasModificators() const
  {
    return (m_Ext.m_pTexModifier != NULL);
  }

  bool IsNeedTexTransform() const
  {
    if (!m_Ext.m_pTexModifier)
      return false;
    if (m_Ext.m_pTexModifier->m_eRotType != ETMR_NoChange || m_Ext.m_pTexModifier->m_eUMoveType != ETMM_NoChange || m_Ext.m_pTexModifier->m_eVMoveType != ETMM_NoChange)
      return true;
    return false;
  }
  bool IsNeedTexGen() const
  {
    if (!m_Ext.m_pTexModifier)
      return false;
    if (m_Ext.m_pTexModifier->m_eTGType != ETG_Stream)
      return true;
    return false;
  }
  inline float GetTiling(int n) const
  {
    if (!m_Ext.m_pTexModifier)
      return 1.0f;
    return m_Ext.m_pTexModifier->m_Tiling[n];
  }
  inline float GetOffset(int n) const
  {
    if (!m_Ext.m_pTexModifier)
      return 0;
    return m_Ext.m_pTexModifier->m_Offs[n];
  }
  inline SEfTexModificator *AddModificator()
  {
    if (!m_Ext.m_pTexModifier)
      m_Ext.m_pTexModifier = new SEfTexModificator;
    return m_Ext.m_pTexModifier;
  }
  int Size() const
  {
    int nSize = sizeof(SEfResTexture) - sizeof(STexSamplerRT) - sizeof(SEfResTextureExt);
    nSize += m_Name.size();
    nSize += m_Sampler.Size();
    nSize += m_Ext.Size();

    return nSize;
  }

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->Add( *this );
		pSizer->AddObject( m_Name );
		pSizer->AddObject( m_Sampler );
	}
  void Cleanup()
  {
    m_Sampler.Cleanup();
    m_Ext.Cleanup();
  }

  ~SEfResTexture()
  {
    Cleanup();
  }

  void Reset()
  {
    m_bUTile = true;
    m_bVTile = true;
    m_TexFlags = 0;
    m_Filter = FILTER_NONE;
    SAFE_DELETE(m_Ext.m_pTexModifier);
    m_Ext.m_nFrameUpdated = -1;
  }
  SEfResTexture (const SEfResTexture& src)
  {
		if (&src != this)
		{
      Cleanup();
	    m_Sampler = src.m_Sampler;
      m_Ext = src.m_Ext;
	    m_TexFlags = src.m_TexFlags;
	    m_Name = src.m_Name;
	    m_bUTile = src.m_bUTile;
	    m_bVTile = src.m_bVTile;
	    m_Filter = src.m_Filter;
		}
  }
  SEfResTexture& operator = (const SEfResTexture& src)
  {
    if (&src != this)
    {
      Cleanup();
      new(this) SEfResTexture(src);
    }
    return *this;
  }
  void CopyTo(SEfResTexture *pTo) const
  {
    if (pTo != this)
    {
      pTo->Cleanup();
      pTo->m_Sampler = m_Sampler;
      m_Ext.CopyTo(&pTo->m_Ext);
      pTo->m_TexFlags = m_TexFlags;
      pTo->m_Name = m_Name;
      pTo->m_bUTile = m_bUTile;
      pTo->m_bVTile = m_bVTile;
      pTo->m_Filter = m_Filter;
    }
  }

  SEfResTexture()
  {
    Reset();
  }
};

enum EEfResTextures
{
	EFTT_DIFFUSE = 0,
	EFTT_BUMP, 
	EFTT_GLOSS,
	EFTT_ENV,
	EFTT_DETAIL_OVERLAY,
	EFTT_BUMP_DIFFUSE,
	EFTT_BUMP_HEIGHT,
	EFTT_DECAL_OVERLAY,
	EFTT_SUBSURFACE,
	EFTT_CUSTOM,
	EFTT_CUSTOM_SECONDARY,
	EFTT_OPACITY,

	EFTT_MAX
};

struct SBaseShaderResources
{
	DynArray<SShaderParam> m_ShaderParams;
	string m_TexturePath;
	const char *m_szMaterialName;

	float m_AlphaRef;
  uint32 m_ResFlags;

	uint16 m_SortPrio;

	uint8 m_FurAmount;
  uint8 m_HeatAmount;
	uint8 m_CloakAmount;

  int Size() const
  {
    int nSize = sizeof(SBaseShaderResources) + m_ShaderParams.size()*sizeof(SShaderParam);
    return nSize;
  }

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->AddObject(m_ShaderParams);
	}
  SBaseShaderResources& operator=(const SBaseShaderResources& src)
  {
    if (&src != this)
    {
			ReleaseParams();
      m_szMaterialName = src.m_szMaterialName;
      m_ResFlags = src.m_ResFlags;
      m_AlphaRef = src.m_AlphaRef;
      m_HeatAmount = src.m_HeatAmount;
      m_FurAmount = src.m_FurAmount;
			m_CloakAmount = src.m_CloakAmount;
      m_SortPrio = src.m_SortPrio;
      m_ShaderParams = src.m_ShaderParams;
    }
    return *this;
  }

  SBaseShaderResources()
  {
    m_ResFlags = 0;
    m_AlphaRef = 0;
    m_HeatAmount = 0;
    m_FurAmount = 0;
		m_CloakAmount = 255;
    m_SortPrio =0;
    m_szMaterialName = NULL;
  }

  void ReleaseParams()
  {
    m_ShaderParams.clear();
  }
  virtual ~SBaseShaderResources()
  {
    ReleaseParams();
  }
};

UNIQUE_IFACE struct IRenderShaderResources
{
  virtual void AddRef() = 0;
  virtual void UpdateConstants(IShader *pSH) = 0;
  virtual void CloneConstants(const IRenderShaderResources* pSrc) = 0;
  virtual void ExportModificators(IRenderShaderResources* pTrg, CRenderObject *pObj) = 0;
  virtual void SetInputLM(const CInputLightMaterial& lm) = 0;
  virtual void ToInputLM(CInputLightMaterial& lm) = 0;
  virtual ColorF& GetDiffuseColor() = 0;
	virtual float GetDiffuseColorAlpha() = 0;
  virtual ColorF& GetSpecularColor() = 0;
  virtual ColorF& GetEmissiveColor() = 0;
  virtual float& GetSpecularShininess() = 0;
  virtual int GetResFlags() = 0;
  virtual void SetMtlLayerNoDrawFlags( uint8 nFlags ) = 0;
  virtual uint8 GetMtlLayerNoDrawFlags() const = 0;
	virtual SSkyInfo *GetSkyInfo() = 0;
	virtual CCamera *GetCamera() = 0;
  virtual void SetCamera(CCamera *pCam) = 0;
  virtual void SetMaterialName(const char *szName) = 0;
  virtual float& GetGlow() = 0;
  virtual float& GetAlphaRef() = 0;
  virtual float& GetOpacity() = 0;
  virtual SEfResTexture *GetTexture(int nSlot) const = 0;
  virtual DynArrayRef<SShaderParam>& GetParameters() = 0;

	virtual ~IRenderShaderResources() {}
  virtual void Release() = 0;
  virtual void ConvertToInputResource(struct SInputShaderResources *pDst) = 0;
  virtual IRenderShaderResources *Clone() = 0;
  virtual void SetShaderParams(struct SInputShaderResources *pDst, IShader *pSH) = 0;

	virtual size_t GetResourceMemoryUsage(ICrySizer*	pSizer)=0;
  virtual SDetailDecalInfo * GetDetailDecalInfo() = 0;

	virtual void GetMemoryUsage(ICrySizer *pSizer) const = 0;
};


struct SInputShaderResources : public SBaseShaderResources
{
  CInputLightMaterial m_LMaterial;
  SEfResTexture m_Textures[EFTT_MAX];
  SDeformInfo m_DeformInfo;
  SDetailDecalInfo m_DetailDecalInfo;
  float m_Opacity;
  float m_GlowAmount;

  int Size() const
  {
    int nSize = SBaseShaderResources::Size() - sizeof(SEfResTexture) * EFTT_MAX;
    nSize += m_TexturePath.size();
    for (int i=0; i<EFTT_MAX; i++)
    {
      nSize += m_Textures[i].Size();
    }
    return nSize;
  }
  SInputShaderResources& operator=(const SInputShaderResources& src)
  {
    if (&src != this)
    {
      Cleanup();
      SBaseShaderResources::operator = (src);
      m_TexturePath = src.m_TexturePath;
      m_DeformInfo = src.m_DeformInfo;
      m_DetailDecalInfo = src.m_DetailDecalInfo;
      m_Opacity = src.m_Opacity;
      m_GlowAmount = src.m_GlowAmount;
      int i;
      for (i=0; i<EFTT_MAX; i++)
      {
        m_Textures[i] = src.m_Textures[i];
      }
      m_LMaterial = src.m_LMaterial;
    }
    return *this;
  }

  SInputShaderResources()
  {
    for (int i=0; i<EFTT_MAX; i++)
    {
      m_Textures[i].Reset();
    }
    m_Opacity = 0;
    m_GlowAmount = 0.0f;
  }

  SInputShaderResources(struct IRenderShaderResources *pSrc)
  {
    pSrc->ConvertToInputResource(this);
    m_ShaderParams = pSrc->GetParameters();
  }

  void Cleanup()
  {
    for (int i=0; i<EFTT_MAX; i++)
    {
      m_Textures[i].Cleanup();
    }
  }
  virtual ~SInputShaderResources()
  {
    Cleanup();
  }
  bool IsEmpty(int nTSlot) const
  {
    return m_Textures[nTSlot].m_Name.empty();
  }
};

//===================================================================================
// Shader gen structure (used for automatic shader script generating).

//
#define SHGF_HIDDEN   1
#define SHGF_PRECACHE 2
#define SHGF_AUTO_PRECACHE 4
#define SHGF_LOWSPEC_AUTO_PRECACHE 8
#define SHGF_RUNTIME 0x10

#define SHGD_LM_DIFFUSE     1
#define SHGD_TEX_DETAIL     2
#define SHGD_TEX_BUMP       4
#define SHGD_TEX_ENVCM      8
#define SHGD_TEX_GLOSS   0x10
#define SHGD_TEX_BUMPDIF 0x20
#define SHGD_TEX_SUBSURFACE 0x80
#define SHGD_HW_BILINEARFP16   0x100
#define SHGD_HW_SEPARATEFP16   0x200
#define SHGD_TEX_CUSTOM 0x1000
#define SHGD_TEX_CUSTOM_SECONDARY 0x2000
#define SHGD_TEX_DECAL 0x4000
#define SHGD_HW_ALLOW_POM 0x10000
#define SHGD_USER_ENABLED 0x40000
#define SHGD_HW_PS3  0x80000
#define SHGD_HW_X360 0x100000
#define SHGD_HW_DX10 0x200000
#define SHGD_HW_DX9  0x400000

struct SShaderGenBit
{
  SShaderGenBit()
  {
    m_Mask = 0;
    m_Flags = 0;
    m_nDependencySet = 0;
    m_nDependencyReset = 0;
		m_NameLength = 0;
		m_dwToken = 0;
  }
  string m_ParamName;
  string m_ParamProp;
  string m_ParamDesc;
  int m_NameLength;
  uint64 m_Mask;
  uint32 m_Flags;
  uint32 m_dwToken;
  std::vector<uint32> m_PrecacheNames;
  std::vector<string> m_DependSets;
  std::vector<string> m_DependResets;
  uint32 m_nDependencySet;
  uint32 m_nDependencyReset;

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->AddObject(m_ParamName);
		pSizer->AddObject(m_ParamProp);
		pSizer->AddObject(m_ParamDesc);
		pSizer->AddObject(m_PrecacheNames);
		pSizer->AddObject(m_DependSets);
		pSizer->AddObject(m_DependResets);
	}
};

struct SShaderGen
{
  uint32 m_nRefCount;
  TArray<SShaderGenBit *> m_BitMask;
  SShaderGen()
  {
    m_nRefCount = 1;
  }
  ~SShaderGen()
  {
    uint32 i;
    for (i=0; i<m_BitMask.Num(); i++)
    {
      SShaderGenBit *pBit = m_BitMask[i];
      SAFE_DELETE(pBit);
    }
    m_BitMask.Free();
  }
  void Release()
  {
    m_nRefCount--;
    if (!m_nRefCount)
      delete this;
  }

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->AddObject(m_BitMask);		
	}
};

//===================================================================================

enum EShaderType
{
  eST_All=-1,				// To set all with one call.

  eST_General=0,
  eST_Metal,
  eST_Glass,
  eST_Vegetation,
  eST_Ice,
  eST_Terrain,
  eST_Shadow,
  eST_Water,
  eST_FX,
  eST_PostProcess,
  eST_HDR,
  eST_Sky,
  eST_Particle,
  eST_Max						// To define array size.
};

enum EShaderQuality
{
  eSQ_Low=0,
  eSQ_Medium=1,
  eSQ_High=2,
  eSQ_VeryHigh=3,
  eSQ_Max=4
};

enum ERenderQuality
{
  eRQ_Low=0,
  eRQ_Medium=1,
  eRQ_High=2,
  eRQ_VeryHigh=3,
  eRQ_Max=4
};

// Summary:
//	 Shader profile flags .
#define SPF_LOADNORMALALPHA		0x1

struct SShaderProfile
{
  SShaderProfile() :m_iShaderProfileQuality(eSQ_High), m_nShaderProfileFlags(SPF_LOADNORMALALPHA)
  {
  }

  EShaderQuality GetShaderQuality() const 
  { 
    return (EShaderQuality)CLAMP(m_iShaderProfileQuality,0,eSQ_VeryHigh);
  }

  void SetShaderQuality( const EShaderQuality &rValue ) 
  { 
    m_iShaderProfileQuality = (int)rValue;
  }

  // ----------------------------------------------------------------

  int									m_iShaderProfileQuality;		// EShaderQuality e.g. eSQ_Medium, use Get/Set functions if possible
  uint32							m_nShaderProfileFlags;			// SPF_...
};

//====================================================================================
// Phys. material flags

#define MATF_NOCLIP 1

//====================================================================================
// Registered shader techniques ID's

enum EShaderTechniqueID
{
	TTYPE_Z = 0,
	TTYPE_CAUSTICS,
	TTYPE_DETAIL,
	TTYPE_SHADOWPASS,
	TTYPE_SHADOWGEN,
#if !defined(XENON) && !defined(PS3)
	TTYPE_SHADOWGENGS,
#endif
	TTYPE_GLOWPASS,
	TTYPE_MOTIONBLURPASS,
	TTYPE_SCATTERPASS,
	TTYPE_CUSTOMRENDERPASS,
	TTYPE_RAINPASS,
	TTYPE_FURPASS,
	TTYPE_EFFECTLAYER,
#if !defined(XENON) && !defined(PS3)
	TTYPE_DEBUG,
#endif
	TTYPE_SOFTALPHATESTPASS,

	TTYPE_MAX
};

//====================================================================================

// EFSLIST_ lists
// The order of the numbers has no meaning.

#define EFSLIST_INVALID              0   // Don't use, internally used.
#define EFSLIST_PREPROCESS           1   // Pre-process items.
#define EFSLIST_GENERAL              2   // Opaque ambient_light+shadow passes.
#define EFSLIST_TERRAINLAYER         3   // Unsorted terrain layers.
#define EFSLIST_SHADOW_GEN           4   // Shadow map generation.
#define EFSLIST_DECAL                5   // Opaque or transparent decals.
#define EFSLIST_WATER_VOLUMES        6   // After decals.
#define EFSLIST_TRANSP               7   // Sorted by distance under-water render items.
#define EFSLIST_WATER                8   // Water-ocean render items.
#define EFSLIST_HDRPOSTPROCESS       9   // Hdr post-processing screen effects.
#define EFSLIST_AFTER_HDRPOSTPROCESS 10   // After hdr post-processing screen effects.
#define EFSLIST_POSTPROCESS          11  // Post-processing screen effects.
#define EFSLIST_AFTER_POSTPROCESS    12  // After post-processing screen effects.
#define EFSLIST_SHADOW_PASS          13  // Shadow mask generation (usually from from shadow maps).
#define EFSLIST_REFRACTPASS          14  // Refraction.
#define EFSLIST_DEFERRED_PREPROCESS  15  // Pre-process before deferred passes.
#define EFSLIST_SKIN		             16  // Skin rendering pre-process 
#define EFSLIST_HALFRES_PARTICLES    17  // Half resolution particles

#define EFSLIST_NUM                  18  // One higher than the last EFSLIST_...

//================================================================
// Different preprocess flags for shaders that require preprocessing (like recursive render to texture, screen effects, visibility check, ...)
// SShader->m_nPreprocess flags in priority order

#define  SPRID_FIRST          16 
#define  SPRID_CORONA         16 
#define  FSPR_CORONA          (1<<SPRID_CORONA)
#define  SPRID_PANORAMA       17
#define  FSPR_PANORAMA        (1<<SPRID_PANORAMA)
#define  SPRID_PORTAL         18
#define  FSPR_PORTAL          (1<<SPRID_PORTAL)
#define  SPRID_SCANCM         19
#define  FSPR_SCANCM          (1<<SPRID_SCANCM)
#define  SPRID_SCANTEXWATER   20
#define  FSPR_SCANTEXWATER    (1<<SPRID_SCANTEXWATER)
#define  SPRID_SCANTEX        21
#define  FSPR_SCANTEX         (1<<SPRID_SCANTEX)
#define  SPRID_SCANLCM        22
#define  FSPR_SCANLCM         (1<<SPRID_SCANLCM)
#define  SPRID_GENSPRITES     23
#define  FSPR_GENSPRITES      (1<<SPRID_GENSPRITES)
#define  SPRID_CUSTOMTEXTURE  24
#define  FSPR_CUSTOMTEXTURE   (1<<SPRID_CUSTOMTEXTURE)
#define  SPRID_SHADOWMAPGEN   25
#define  FSPR_SHADOWMAPGEN    (1<<SPRID_SHADOWMAPGEN)
#define  SPRID_RAINOVERLAY    26
#define  FSPR_RAINOVERLAY     (1<<SPRID_RAINOVERLAY)
#define  SPRID_REFRACTED      27
#define  FSPR_REFRACTED       (1<<SPRID_REFRACTED)
#define  SPRID_SCREENTEXMAP   28
#define  FSPR_SCREENTEXMAP    (1<<SPRID_SCREENTEXMAP)
#define  SPRID_GENCLOUDS      29
#define  FSPR_GENCLOUDS       (1<<SPRID_GENCLOUDS)

#define  FSPR_MASK            0xffff0000
#define  FSPR_MAX             (1 << 30)

#define FEF_DONTSETTEXTURES   1					// Set: explicit setting of samplers (e.g. tex->Apply(1,nTexStatePoint)), not set: set sampler by sematics (e.g. $ZTarget).
#define FEF_DONTSETSTATES     2

// SShader::m_Flags
// Different useful flags
#define EF_RELOAD        1						// Shader needs tangent vectors array.
#define EF_FORCE_RELOAD  2
#define EF_RELOADED      4
#define EF_NODRAW        8
#define EF_HASCULL       0x10
#define EF_SUPPORTSDEFERREDSHADING_MIXED 0x20
#define EF_SUPPORTSDEFERREDSHADING_FULL 0x40
#define EF_SUPPORTSDEFERREDSHADING ( EF_SUPPORTSDEFERREDSHADING_MIXED | EF_SUPPORTSDEFERREDSHADING_FULL )
#define EF_DECAL         0x80
#define EF_LOADED        0x100
#define EF_LOCALCONSTANTS 0x200
#define EF_BUILD_TREE     0x400
#define EF_LIGHTSTYLE    0x800
#define EF_NOCHUNKMERGING 0x1000
#define EF_SUNFLARES     0x2000
#define EF_NEEDNORMALS   0x4000					// Need normals operations.
#define EF_OFFSETBUMP    0x8000
#define EF_NOTFOUND      0x10000
#define EF_DEFAULT       0x20000
#define EF_SKY           0x40000
#define EF_USELIGHTS     0x80000
#define EF_ALLOW3DC      0x100000
#define EF_FOGSHADER     0x200000
#define EF_USEPROJLIGHTS 0x400000
#define EF_PRECACHESHADER 0x800000
#define EF_SUPPORTSINSTANCING_CONST 0x2000000
#define EF_SUPPORTSINSTANCING_ATTR  0x4000000
#define EF_SUPPORTSINSTANCING (EF_SUPPORTSINSTANCING_CONST | EF_SUPPORTSINSTANCING_ATTR)
#define EF_WATERPARTICLE  0x8000000
#define EF_CLIENTEFFECT  0x10000000
#define EF_SYSTEM        0x20000000
#define EF_REFRACTIVE    0x40000000
#define EF_FORCEREFRACTIONUPDATE 0x1000000
#define EF_NOPREVIEW     0x80000000

#define EF_PARSE_MASK    (EF_SUPPORTSINSTANCING | EF_SKY | EF_HASCULL | EF_USELIGHTS | EF_REFRACTIVE)


// SShader::Flags2
// Additional Different useful flags

#define EF2_PREPR_GENSPRITES 0x1
#define EF2_PREPR_GENCLOUDS 0x2
#define EF2_PREPR_SCANWATER 0x4
#define EF2_NOCASTSHADOWS  0x8
#define EF2_NODRAW         0x10
#define EF2_HASOPAQUE      0x40
#define EF2_AFTERHDRPOSTPROCESS  0x80
#define EF2_DONTSORTBYDIST 0x100
#define EF2_FORCE_WATERPASS    0x200
#define EF2_FORCE_GENERALPASS   0x400
#define EF2_AFTERPOSTPROCESS  0x800
#define EF2_IGNORERESOURCESTATES  0x1000
#define EF2_USELIGHTMATERIAL  0x2000
#define EF2_FORCE_TRANSPASS       0x4000
#define EF2_DEFAULTVERTEXFORMAT 0x8000
#define EF2_FORCE_ZPASS 0x10000
#define EF2_FORCE_DRAWLAST 0x20000
#define EF2_FORCE_DRAWAFTERWATER 0x40000
#define EF2_DEPTHMAP_SUBSURFSCATTER 0x80000
#define EF2_SUPPORTS_REPLACEBASEPASS 0x100000
#define EF2_SINGLELIGHTPASS 0x200000
#define EF2_FORCE_DRAWFIRST 0x400000
#define EF2_HAIR            0x800000
#define EF2_DETAILBUMPMAPPING 0x1000000
#define EF2_HASALPHATEST      0x2000000
#define EF2_HASALPHABLEND     0x4000000
#define EF2_DEFERBACKLIGHTING 0x8000000
#define EF2_VERTEXCOLORS 0x10000000
#define EF2_SKINPASS 0x20000000

UNIQUE_IFACE struct IShader
{
public:
	virtual ~IShader(){}
  virtual int GetID() = 0;
  virtual int AddRef()=0;
  virtual int Release()=0;
  virtual int ReleaseForce()=0;

  virtual const char *GetName()=0;
  virtual const char *GetName() const =0;
  virtual int GetFlags() = 0;
  virtual int GetFlags2() = 0;
  virtual void SetFlags2(int Flags) = 0;
  virtual void ClearFlags2(int Flags) = 0;
  virtual bool Reload(int nFlags, const char *szShaderName) = 0;
  virtual TArray<CRendElementBase *> *GetREs (int nTech) = 0;
  virtual DynArrayRef<SShaderParam>& GetPublicParams() = 0;
  virtual int GetTexId () = 0;
  virtual ITexture *GetBaseTexture(int *nPass, int *nTU) = 0;
  virtual unsigned int GetUsedTextureTypes(void) = 0;
  virtual ECull GetCull(void) = 0;
  virtual int Size(int Flags) = 0;
  virtual uint64 GetGenerationMask() = 0;
  virtual SShaderGen* GetGenerationParams() = 0;
  virtual int GetTechniqueID(int nTechnique, int nRegisteredTechnique) = 0;
  virtual EVertexFormat GetVertexFormat(void) = 0;

  virtual EShaderType GetShaderType() = 0;
  virtual uint32      GetVertexModificator() = 0;

	virtual void GetMemoryUsage(ICrySizer *pSizer) const=0;
};

struct SShaderItem
{
  IShader *m_pShader;
  IRenderShaderResources *m_pShaderResources;
  int32 m_nTechnique;
  uint32 m_nPreprocessFlags;

  SShaderItem()
  {
    m_pShader = NULL;
    m_pShaderResources = NULL;
    m_nTechnique = -1;
    m_nPreprocessFlags = 1;
  }
  SShaderItem(IShader *pSH)
  {
    m_pShader = pSH;
    m_pShaderResources = NULL;
    m_nTechnique = -1;
    m_nPreprocessFlags = 1;
    if (pSH && (pSH->GetFlags2() & EF2_PREPR_GENSPRITES))
      m_nPreprocessFlags |= FSPR_GENSPRITES;
  }
  SShaderItem(IShader *pSH, IRenderShaderResources *pRS)
  {
    m_pShader = pSH;
    m_pShaderResources = pRS;
    m_nTechnique = -1;
    m_nPreprocessFlags = 1;
    if (pSH)
    {
      if (pSH->GetFlags2() & EF2_PREPR_GENSPRITES)
        m_nPreprocessFlags |= FSPR_GENSPRITES;
    }
  }
  SShaderItem(IShader *pSH, IRenderShaderResources *pRS, int nTechnique)
  {
    m_pShader = pSH;
    m_pShaderResources = pRS;
    m_nTechnique = nTechnique;
    m_nPreprocessFlags = 1;
    if (pSH)
    {
      if (pSH->GetFlags2() & EF2_PREPR_GENSPRITES)
        m_nPreprocessFlags |= FSPR_GENSPRITES;
    }
  }
  void PostLoad();
  bool Update();
  // Note: 
  //	 If you change this function please check bTransparent variable in CRenderMesh::Render().
  // See also:
  //	 CRenderMesh::Render()
  inline bool IsZWrite() const
  { 
    IShader *pSH = m_pShader;
    if (pSH->GetFlags() & (EF_NODRAW | EF_DECAL))
      return false;
    if (pSH->GetFlags2() & EF2_FORCE_ZPASS)
      return true;
    if (m_pShaderResources && m_pShaderResources->GetDiffuseColorAlpha() != 1.0f)
       return false;
    return true;
  }
  inline struct SShaderTechnique *GetTechnique() const;
  bool IsMergable(SShaderItem& PrevSI);

	void GetMemoryUsage(ICrySizer *pSizer) const
	{
		pSizer->AddObject(m_pShader);
		pSizer->AddObject(m_pShaderResources);
	}
};


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// DLights

//#define DLF_DETAIL          1
#define DLF_DIRECTIONAL     2
//#define DLF_DYNAMIC         4						// Dynamic lights.
//#define DLF_ACTIVE          8						// Light is active/inactive.
#define DLF_POST_3D_RENDERER	8							// Light only used in post 3D render pass
#define DLF_CASTSHADOW_MAPS 0x10					// Light casting shadows.
#define DLF_POINT           0x20
#define DLF_PROJECT         0x40
#define DLF_HAS_CBUFFER			0x80
#define DLF_REFLECTIVE_SHADOWMAP  0x100
//#define DLF_POSITIONCHANGED 0x100
#define DLF_IGNORES_VISAREAS 0x200
#define DLF_DEFERRED_CUBEMAPS 0x400
#define DLF_DEFERRED_INDIRECT_LIGHT    0x800
#define DLF_DISABLED            0x1000
//#define DLF_STATIC_ADDED    0x2000				// This static light has been already added to the list.
#define DLF_HASCLIPBOUND    0x4000
#define DLF_HASCLIPGEOM			0x8000					// Use stat geom for clip geom
#define DLF_LIGHTSOURCE     0x10000
#define DLF_FAKE            0x20000					// Actually it's not LS, just render elements (Flares, beams, ...).
#define DLF_SUN             0x40000					// Only sun may use this flag.
#define DLF_DISTRATIO_CUSTOM 0x80000
#define DLF_LOCAL           0x100000
#define DLF_LM              0x200000
#define DLF_THIS_AREA_ONLY  0x400000				// Affects only current area/sector.
#define DLF_AMBIENT_LIGHT   0x800000				// Affected by ambient occlusion factor.
#define DLF_NEGATIVE        0x1000000				// Make ambient darker.
#define DLF_INDOOR_ONLY     0x2000000				// Do not affect heightmap.
#define DLF_ONLY_FOR_HIGHSPEC 0x4000000				// This light is active as dynamic light only for high spec machines.
#define DLF_SPECULAR_ONLY_FOR_HIGHSPEC  0x8000000	// This light have specular component enabled only for high spec machines.
#define DLF_DEFERRED_LIGHT    0x10000000
#define DLF_IRRAD_VOLUMES		0x20000000			// Add only to irradiance volume even if it's possible.
#define DLF_SPECULAROCCLUSION 0x40000000			// Use occlusion map for specular part of the light.
#define DLF_DIFFUSEOCCLUSION 0x80000000				// Use occlusion map for diffuse part of the light.

#define DLF_LIGHTTYPE_MASK (DLF_DIRECTIONAL | DLF_POINT | DLF_PROJECT)

//Area light types
#define DLAT_SPHERE				0x1
#define DLAT_RECTANGLE		0x2
#define DLAT_POINT				0x4

struct IEntity;

#include "IRenderMesh.h"

struct SRenderLight
{
  SRenderLight()
  {
    memset(this, 0, sizeof(SRenderLight));
    m_fLightFrustumAngle = 45.0f;
    m_fRadius = 4.0f;
    m_SpecMult = m_BaseSpecMult = 1.0f;
    m_Flags = DLF_LIGHTSOURCE;
    m_n3DEngineLightId = -1;
    m_ProjMatrix.SetIdentity();
    m_ObjMatrix.SetIdentity();
    m_sName = "";
    m_fCoronaDistSizeFactor = 1.0f;
    m_fCoronaDistIntensityFactor = 1.0f;
    m_pDeferredRenderMesh = NULL;
  }

  const Vec3 &GetPosition() const
  {
    return m_Origin;
  }
  void SetPosition(const Vec3& vPos)
  {
    m_BaseOrigin = vPos;
    m_Origin = vPos;
  }

  // Summary:
  //	 Use this instead of m_Color.
  void SetLightColor(const ColorF &cColor)
  {
    m_Color = cColor;
    m_BaseColor = cColor;
  }

  ITexture* GetDiffuseCubemap() const
  {
    assert(m_pDiffuseCubemap != (void*) 0xfeeefeee);			// Fran: check against double delete
    return m_pDiffuseCubemap;
  }

  ITexture* GetSpecularCubemap() const
  {
    assert(m_pSpecularCubemap != (void*) 0xfeeefeee);			// Fran: check against double delete
    return m_pSpecularCubemap;
  }

  IRenderMesh * GetDeferredRenderMesh() const
  {
    return m_pDeferredRenderMesh;
  }


  void GetMemoryUsage(ICrySizer *pSizer ) const {/*LATER*/}

//=========================================================================================================================

  struct ILightSource *						m_pOwner;

  CRenderObject *                 m_pObject[MAX_RECURSION_LEVELS];          // Object for light coronas and light flares.
  IRenderMesh*										m_pDeferredRenderMesh;							// Arbitrary render mesh

  SShaderItem                     m_Shader;

  ShadowMapFrustum						**  m_pShadowMapFrustums;			//

  ITexture*                       m_pDiffuseCubemap;									// Very small cubemap texture to make a lookup for diffuse.
  ITexture*                       m_pSpecularCubemap;									// Cubemap texture to make a lookup for local specular.
  ITexture*                       m_pLightImage;
  ITexture*												m_pLightAttenMap;										// User can specify custom light attenuation gradient

  const char*                     m_sName;									// Optional name of the light source.
  const char*											m_sDeferredGeom;						// Optional deferred geom file

  Matrix44                        m_ProjMatrix;
  Matrix34                        m_ObjMatrix;
  Matrix34                        m_ClipBox;

  ColorF                          m_Color;	// w component unused..
  ColorF                          m_BaseColor;	// w component unused..

  Vec3                            m_Origin;	// World space position.
  Vec3                            m_BaseOrigin;	// World space position.

  float														m_fRadius;
  float														m_SpecMult;
  float														m_BaseSpecMult;
  float														m_fHDRDynamic;	// 0 to get the same results in HDR, <0 to get darker, >0 to get brighter.

  float														m_fAnimSpeed;  
  float														m_fCoronaScale;
  float														m_fCoronaDistSizeFactor;
  float														m_fCoronaDistIntensityFactor;

  float														m_fLightFrustumAngle;
  float														m_fProjectorNearPlane;

  uint32                          m_Flags;		// light flags (DLF_etc).
  int32														m_Id; 
  int32														m_n3DEngineLightId;
  uint32													m_n3DEngineUpdateFrameID;

  // Scissor parameters (2d extent).
  int16														m_sX;
  int16														m_sY;
  int16														m_sWidth;
  int16														m_sHeight;

  // Animation rotation ratio
  int16                           m_AnimRotation[3];

  uint8														m_nLightStyle;
  uint8														m_nLightPhase;
  uint8														m_nPostEffect : 4;    // limited to 16 effects - should be enough
  uint8														m_ShadowChanMask : 4; // only 4 bits required
};

//////////////////////////////////////////////////////////////////////
class CDLight : public SRenderLight
{
public:
  CDLight() : SRenderLight()
  {
  }

  ~CDLight()
  {
    //SAFE_RELEASE(m_Shader.m_pShader);
    SAFE_RELEASE(m_pLightImage);
		SAFE_RELEASE(m_pLightAttenMap);

		ReleaseCubemaps();
		ReleaseDeferredRenderMesh();
  }

  // Summary:
  //	 Good for debugging.
  bool IsOk() const
  {
    for(int i=0;i<3;++i)
    {			
      if(m_Color[i]<0 || m_Color[i]>100.0f || _isnan(m_Color[i]))
        return false;
      if(m_BaseColor[i]<0 || m_BaseColor[i]>100.0f || _isnan(m_BaseColor[i]))
        return false;
    }
    return true;
  }

  CDLight(const CDLight& other)
		: SRenderLight(other)
  {
    if (m_Shader.m_pShader)
      m_Shader.m_pShader->AddRef();
    if (m_pLightImage)
      m_pLightImage->AddRef();
    if (m_pDiffuseCubemap)
      m_pDiffuseCubemap->AddRef();
    if (m_pSpecularCubemap)
      m_pSpecularCubemap->AddRef();
    if(m_pLightAttenMap)
      m_pLightAttenMap->AddRef();
    if (m_pDeferredRenderMesh)
      m_pDeferredRenderMesh->AddRef();
  }

  CDLight& operator=(const CDLight& other)
  {
    if (this == &other) return *this;

    SAFE_RELEASE(m_Shader.m_pShader);
    SAFE_RELEASE(m_pLightImage);
    SAFE_RELEASE(m_pDiffuseCubemap);
    SAFE_RELEASE(m_pSpecularCubemap);
    SAFE_RELEASE(m_pLightAttenMap);
    SAFE_RELEASE(m_pDeferredRenderMesh);
	
		new(this) CDLight(other);

    return *this;
  }

  // Summary:
  //	 Use this instead of m_Color.
  const ColorF &GetFinalColor( const ColorF &cColor ) const
  {
    return m_Color;
  }

	// Summary:
	//	 Use this instead of m_Color.
	void SetSpecularMult( float fSpecMult )
	{
		m_SpecMult = fSpecMult;
		m_BaseSpecMult = fSpecMult;
	}

	// Summary:
	//	 Use this instead of m_Color.
	const float &GetSpecularMult() const
	{
		return m_SpecMult;
	}
  void SetMatrix(const Matrix34& Matrix)
  {
    // Scale the cubemap to adjust the default 45 degree 1/2 angle fustrum to 
    // the desired angle (0 to 90 degrees).
    float scaleFactor = cry_tanf((90.0f-m_fLightFrustumAngle)*gf_PI/180.0f);
    m_ProjMatrix = Matrix33(Matrix) * Matrix33::CreateScale(Vec3(1,scaleFactor,scaleFactor));
    Matrix44 transMat;
    transMat.SetIdentity();
    transMat(3,0) = -Matrix(0,3); transMat(3,1) = -Matrix(1,3); transMat(3,2) = -Matrix(2,3);
    m_ProjMatrix = transMat * m_ProjMatrix;
    m_ObjMatrix = Matrix;
  }

	void SetSpecularCubemap(ITexture* texture)
	{
		m_pSpecularCubemap = texture;
	}

	void SetDiffuseCubemap(ITexture* texture)
	{
		m_pDiffuseCubemap = texture;
	}

	void SetLightAttenMap(ITexture* texture)
	{
		m_pLightAttenMap = texture;
	}

	ITexture* GetLightAttenMap() const
	{
		return m_pLightAttenMap;
	}

	void ReleaseCubemaps()
	{
		SAFE_RELEASE(m_pSpecularCubemap);
		SAFE_RELEASE(m_pDiffuseCubemap);
	}

	void ReleaseDeferredRenderMesh()
	{
		SAFE_RELEASE(m_pDeferredRenderMesh);
	}

	void SetDeferredRenderMesh(IRenderMesh * pRenderMesh)
	{
    SAFE_RELEASE(m_pDeferredRenderMesh); 
    m_pDeferredRenderMesh = pRenderMesh;
		if(m_pDeferredRenderMesh)
			m_pDeferredRenderMesh->AddRef();
	}
};

#define DECAL_HAS_NORMAL_MAP	(1<<0)
#define DECAL_STATIC					(1<<1)

struct SDeferrredDecal
{
	SDeferrredDecal()
	{ 
		ZeroStruct(*this); 
	}

  Matrix34 projMatrix; // defines where projection should be applied in the world
  IMaterial * pMaterial; // decal material
  float fAlpha; // transparency of decal, used mostly for distance fading
  float fGrowAlphaRef;
  uint32 nFlags;
  uint8 nSortOrder; // user defined sort order
};

// Summary:
//	 Runtime shader flags for HW skinning.
enum EHWSkinningRuntimeFlags
{
  eHWS_ShapeDeform = 0x01,
  eHWS_MorphTarget = 0x02,
  eHWS_MotionBlured = 0x04
};


// Summary:
//	 Shader graph support.
enum EGrBlockType
{
  eGrBlock_Unknown,
  eGrBlock_VertexInput,
  eGrBlock_VertexOutput,
  eGrBlock_PixelInput,
  eGrBlock_PixelOutput,
  eGrBlock_Texture,
  eGrBlock_Sampler,
  eGrBlock_Function,
  eGrBlock_Constant,
};

enum EGrBlockSamplerType
{
  eGrBlockSampler_Unknown,
  eGrBlockSampler_2D,
  eGrBlockSampler_3D,
  eGrBlockSampler_Cube,
  eGrBlockSampler_Bias2D,
  eGrBlockSampler_BiasCube,
};

enum EGrNodeType
{
  eGrNode_Unknown,
  eGrNode_Input,
  eGrNode_Output,
};

enum EGrNodeFormat
{
  eGrNodeFormat_Unknown,
  eGrNodeFormat_Float,
  eGrNodeFormat_Vector,
  eGrNodeFormat_Matrix,
  eGrNodeFormat_Int,
  eGrNodeFormat_Bool,
  eGrNodeFormat_Texture2D,
  eGrNodeFormat_Texture3D,
  eGrNodeFormat_TextureCUBE,
};

enum EGrNodeIOSemantic
{
  eGrNodeIOSemantic_Unknown,
  eGrNodeIOSemantic_Custom,
  eGrNodeIOSemantic_VPos,
  eGrNodeIOSemantic_Color0,
  eGrNodeIOSemantic_Color1,
  eGrNodeIOSemantic_Color2,
  eGrNodeIOSemantic_Color3,
  eGrNodeIOSemantic_Normal,
  eGrNodeIOSemantic_TexCoord0,
  eGrNodeIOSemantic_TexCoord1,
  eGrNodeIOSemantic_TexCoord2,
  eGrNodeIOSemantic_TexCoord3,
  eGrNodeIOSemantic_TexCoord4,
  eGrNodeIOSemantic_TexCoord5,
  eGrNodeIOSemantic_TexCoord6,
  eGrNodeIOSemantic_TexCoord7,
  eGrNodeIOSemantic_Tangent,
  eGrNodeIOSemantic_Binormal,
};

struct SShaderGraphFunction
{
  string m_Data;
  string m_Name;
  std::vector<string> inParams;
  std::vector<string> outParams;
  std::vector<string> szInTypes;
  std::vector<string> szOutTypes;
};

struct SShaderGraphNode
{
  EGrNodeType m_eType;
  EGrNodeFormat m_eFormat;
  EGrNodeIOSemantic m_eSemantic;
  string m_CustomSemantics;
  string m_Name;
  bool m_bEditable;
  bool m_bWasAdded;
  SShaderGraphFunction *m_pFunction;
  DynArray<SShaderParam> m_Properties;

  SShaderGraphNode()
  {
    m_eType = eGrNode_Unknown;
    m_eFormat = eGrNodeFormat_Unknown;
    m_eSemantic = eGrNodeIOSemantic_Unknown;
    m_bEditable = false;
    m_bWasAdded = false;
    m_pFunction = NULL;
  }
  ~SShaderGraphNode();
};


typedef std::vector<SShaderGraphNode *> FXShaderGraphNodes;
typedef FXShaderGraphNodes::iterator FXShaderGraphNodeItor;

struct SShaderGraphBlock
{
  EGrBlockType m_eType;
  EGrBlockSamplerType m_eSamplerType;
  string m_ClassName;
  FXShaderGraphNodes  m_Nodes;

  ~SShaderGraphBlock();
};

typedef std::vector<SShaderGraphBlock *> FXShaderGraphBlocks;
typedef FXShaderGraphBlocks::iterator FXShaderGraphBlocksItor;

struct CRenderChunk
{
	PodArray<uint16> m_arrChunkBoneIDs;
	CREMesh *pRE;				// Pointer to the mesh.

	float m_texelAreaDensity;
	
	uint32 nNumIndices;
  uint32 nFirstIndexId;
  
  uint16 nFirstVertId;
  uint16 nNumVerts;
  
  uint16 m_nMatFlags;		// Material flags from originally assigned material @see EMaterialFlags.
  uint16 m_nMatID;				// Material Sub-object id.

	// Index of sub-object that this chunk originates from, used by sub-object hide mask.
	// @see IStatObj::GetSubObject
  uint32 nSubObjectIndex;
    
  //////////////////////////////////////////////////////////////////////////
	CRenderChunk():
	m_texelAreaDensity(1.0f),
	nFirstIndexId(0), 
	nNumIndices(0), 
	nFirstVertId(0),
	nNumVerts(0),
	pRE(0),
	m_nMatFlags(0),
	m_nMatID(0),
	nSubObjectIndex(0)
  {
  }

  int Size();

	void GetMemoryUsage( ICrySizer *pSizer ) const 
	{
		pSizer->AddObject(m_arrChunkBoneIDs);
	}
};

typedef PodArray<CRenderChunk> CRenderChunkArray;


#include "RendElement.h"

#endif // _ISHADER
