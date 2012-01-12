/*=============================================================================
  IShader.h : Shaders common interface.
  Copyright (c) 2001-2008 Crytek Studios. All Rights Reserved.

  Revision history:
    * Created by Anton Kaplanyan

=============================================================================*/
#include DEVIRTUALIZE_HEADER_FIX(ITexture.h)

#ifndef _ITEXTURE_H_
#define _ITEXTURE_H_

#include "Cry_Math.h"
#include "Cry_Color.h"
#include "Tarray.h"

class CTexture;

#ifndef COMPILER_SUPPORTS_ENUM_SPECIFICATION
# if defined(_MSC_VER)
#  define COMPILER_SUPPORTS_ENUM_SPECIFICATION 1
# else
#  define COMPILER_SUPPORTS_ENUM_SPECIFICATION 0
# endif
#endif

#if COMPILER_SUPPORTS_ENUM_SPECIFICATION
enum ETEX_Type : uint8
#else
typedef uint8 ETEX_Type;
enum eTEX_Type
#endif
{
	eTT_1D = 0,
	eTT_2D,
	eTT_3D,
	eTT_Cube,
	eTT_AutoCube,
	eTT_Auto2D,
	eTT_User,
	eTT_NearestCube,
	eTT_MaxTexType,		// not used
};


// Texture formats
#if COMPILER_SUPPORTS_ENUM_SPECIFICATION
enum ETEX_Format : uint8
#else
typedef uint8 ETEX_Format;
enum eTEX_Format
#endif
{
	eTF_Unknown = 0,
	eTF_R8G8B8 = 1, // may be saved into file,
	eTF_A8R8G8B8 = 2, // may be saved into file
	eTF_X8R8G8B8,
	eTF_A8,
	eTF_A8L8,
	eTF_L8,
	eTF_A4R4G4B4,
	eTF_R5G6B5,
	eTF_R5G5B5,
	eTF_V8U8,
	eTF_CxV8U8,
	eTF_X8L8V8U8,
	eTF_L8V8U8,
	eTF_L6V5U5,
	eTF_V16U16,
	eTF_A16B16G16R16,
	eTF_A16B16G16R16F,
	eTF_A32B32G32R32F,
	eTF_G16R16F,
	eTF_R16F,
	eTF_R32F,
	eTF_DXT1 = 22, // may be saved into file
	eTF_DXT3 = 23, // may be saved into file
	eTF_DXT5 = 24, // may be saved into file
	eTF_3DC,

	eTF_G16R16,

	eTF_NULL,

	//hardware depth buffers
	eTF_DF16,
	eTF_DF24,
	eTF_D16,
	eTF_D24S8,

	eTF_D32F,

	eTF_DEPTH16,
	eTF_DEPTH24,




  eTF_A2R10G10B10,
	eTF_CTX1,						// 4bpp normal map format for Xbox 360





#if !defined(EXCLUDE_SCALEFORM_SDK)
	eTF_YUV,
	eTF_YUVA,
#endif

	eTF_MaxFormat				// unused, must be always the last in the list
};

#define FT_NOMIPS           0x1
#define FT_TEX_NORMAL_MAP   0x2
#define FT_TEX_WAS_NOT_PRE_TILED 0x4
#define FT_USAGE_DEPTHSTENCIL 0x8
#define FT_USAGE_ALLOWREADSRGB  0x10
#define FT_FILESINGLE			  0x20				// suppress loading of additional files like _DDNDIF (faster, RC can tag the file for that)
#define FT_TEX_FONT         0x40
#define FT_HAS_ATTACHED_ALPHA 0x80
#define FT_DONTSYNCMULTIGPU 0x100				// through NVAPI we tell driver not to sync
#define FT_USAGE_READBACK   0x200
#define FT_USAGE_FSAA       0x400
#define FT_FORCE_MIPS       0x800
#define FT_USAGE_RENDERTARGET 0x1000
#define FT_USAGE_DYNAMIC    0x2000
#define FT_DONT_RESIZE      0x4000
#define FT_CUSTOM_FORMAT       0x8000
#define FT_DONT_RELEASE     0x10000
#define FT_DONT_GENNAME     0x20000
#define FT_DONT_STREAM      0x40000
#define FT_USAGE_PREDICATED_TILING 0x80000
#define FT_FAILED           			0x100000
#define FT_FROMIMAGE        			0x200000
#define FT_STATE_CLAMP      			0x400000
#define FT_USAGE_ATLAS      			0x800000
#define FT_ALPHA            			0x1000000
#define FT_REPLICATE_TO_ALL_SIDES 0x2000000
#define FT_USAGE_VERTEX_BUFFER		0x4000000
#define FT_SPLITTED								0x8000000	// for splitted dds files
#define FT_FILTER_POINT     			0x10000000
#define FT_FILTER_LINEAR    			0x20000000
#define FT_FILTER_BILINEAR  			0x30000000
#define FT_FILTER_TRILINEAR 			0x40000000
#define FT_FILTER_ANISO2    			0x50000000
#define FT_FILTER_ANISO4    			0x60000000
#define FT_FILTER_ANISO8    			0x70000000
#define FT_FILTER_ANISO16   			0x80000000
#define FT_FILTER_MASK      			0xf0000000

#define FT_AFFECT_INSTANCE  (FT_USAGE_FSAA | FT_REPLICATE_TO_ALL_SIDES | FT_FILTER_MASK | FT_ALPHA)

struct SD3DSurface;

struct STextureStreamingStats
{
	STextureStreamingStats(bool bComputeTexturesPerFrame) : bComputeReuquiredTexturesPerFrame(bComputeTexturesPerFrame)
	{
		nMaxPoolSize = 0;
		nCurrentPoolSize = 0;
		nStaticTexturesSize = 0;
		nThroughput = 0;
		nNumTexturesPerFrame = 0;
		nRequiredTexturesSize = 0;
		nRequiredTexturesCount = 0;
		bPoolOverflow = false;
		bPoolOverflowTotally = false;
	}
	uint32 nMaxPoolSize;
	uint32 nCurrentPoolSize;
	uint32 nStaticTexturesSize;
	uint32 nNumTexturesPerFrame;
	uint32 nThroughput;
	uint32 nRequiredTexturesSize;
	uint32 nRequiredTexturesCount;
	uint32 bPoolOverflow:1;
	uint32 bPoolOverflowTotally:1;
	const bool bComputeReuquiredTexturesPerFrame;
};

//////////////////////////////////////////////////////////////////////
// Texture object interface
UNIQUE_IFACE class ITexture
{
protected:
	virtual ~ITexture() {}
public:
	virtual int AddRef()=0;
	virtual int Release()=0;
	virtual int ReleaseForce()=0;

	virtual const char *GetName() const =0;
	virtual const int GetWidth() const = 0;
	virtual const int GetHeight() const = 0;
	virtual const int GetDepth() const = 0;
	virtual const int GetTextureID() const = 0;
	virtual const uint32 GetFlags() const = 0;
	virtual const int GetNumMips() const = 0;
	virtual const int GetDeviceDataSize()const  = 0;
	virtual const int GetDataSize() const = 0;
	virtual const ETEX_Type GetTextureType() const = 0;
	virtual const bool IsTextureLoaded() const = 0;
	virtual void PrecacheAsynchronously(float fMipFactor, int nFlags, int nUpdateId, int nCounter=1) = 0;
	virtual byte *GetData32(int nSide=0, int nLevel=0, byte * pDst=NULL, ETEX_Format eDstFormat = eTF_A8R8G8B8)=0;
	virtual byte *LockData(int& nPitch, int nSide=0, int nLevel=0)=0;
	virtual void UnlockData(int nSide=0, int nLevel=0)=0;
	virtual bool SaveTGA(const char *szName, bool bMips=false)=0;
	virtual bool SaveJPG(const char *szName, bool bMips=false)=0;
	virtual bool SetFilter(int nFilter)=0;   // FILTER_ flags
	virtual void SetClamp(bool bEnable) = 0; // Texture addressing set
	virtual const ColorB& GetAvgColor() const = 0;
	
	virtual int StreamCalculateMipsSigned(float fMipFactor) const = 0;
	virtual int GetStreamableMipNumber() const = 0;
	virtual int GetStreamableMemoryUsage(int nStartMip) const = 0;
	virtual int GetMinLoadedMip() const = 0;

	// Used for debugging/profiling.
	virtual const char* GetFormatName() const = 0;
	virtual const char* GetTypeName() const = 0;
	virtual const bool IsStreamedVirtual() const = 0;
	virtual const bool IsShared() const = 0;
	virtual const bool IsStreamable() const = 0;
	virtual const int GetAccessFrameId() const = 0;

  virtual const ETEX_Format GetTextureDstFormat() const = 0;
  virtual const ETEX_Format GetTextureSrcFormat() const = 0;

	virtual const bool IsParticularMipStreamed(float fMipFactor) const = 0;

  // copy texture back into system memory (used for decals baked into terrain texture)
  virtual byte ** GetSystemCopy() = 0;

	void GetMemoryUsage( ICrySizer *pSizer ) const {
		COMPILE_TIME_ASSERT(eTT_MaxTexType <= 255);
		COMPILE_TIME_ASSERT(eTF_MaxFormat <= 255);
		/*LATER*/
	}
};

//=========================================================================================

struct IDynTextureSource
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	enum EDynTextureSource
	{
		DTS_UNKNOWN,

		DTS_I_FLASHPLAYER,
		DTS_I_VIDEOPLAYER,
		DTS_I_UIELEMENT
	};
	virtual void* GetSource(EDynTextureSource type) const = 0;
	virtual EDynTextureSource GetSourceType() const = 0;

	enum EDynTextureRTType
	{
		DTS_RT_SHARED,
		DTS_RT_UNIQUE
	};
	virtual EDynTextureRTType GetRTType() const = 0;

	virtual void EnablePerFrameRendering(bool enable) = 0;

protected:
	virtual ~IDynTextureSource() {}
};

//=========================================================================================

class IDynTexture
{
public:
	virtual ~IDynTexture(){}
	enum
	{
		fNeedRegenerate = 1ul<<0,
	};
	virtual void Release() = 0;
	virtual void GetSubImageRect(uint32 & nX, uint32 & nY, uint32 & nWidth, uint32 & nHeight) = 0;
	virtual void GetImageRect(uint32 & nX, uint32 & nY, uint32 & nWidth, uint32 & nHeight) = 0;
	virtual int GetTextureID() = 0;
	virtual void Lock() = 0;
	virtual void UnLock() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual bool IsValid() = 0;
	virtual byte GetFlags() const = 0;
	virtual void SetFlags(byte flags) {}
	virtual bool Update(int nNewWidth, int nNewHeight)=0;
	virtual void Apply(int nTUnit, int nTS=-1)=0;
	virtual bool SetRT(int nRT, bool bPush, struct SD3DSurface *pDepthSurf, bool bScreenVP=false)=0;
	virtual bool SetRectStates()=0;
	virtual bool RestoreRT(int nRT, bool bPop)=0;
	virtual ITexture *GetTexture()=0;
	virtual void SetUpdateMask()=0;
	virtual void ResetUpdateMask()=0;
	virtual bool IsSecondFrame()=0;
  virtual bool GetImageData32(byte * pData, int nDataSize) { return 0; }
};

// Animating Texture sequence definition
struct STexAnim
{
  int m_nRefCount;
	TArray<CTexture *> m_TexPics;
	int m_Rand;
	int m_NumAnimTexs;
	bool m_bLoop;
	float m_Time;

	int Size()
	{
		int nSize = sizeof(STexAnim);
		nSize += m_TexPics.GetMemoryUsage();
		return nSize;
	}
  void Release()
  {
    long refCnt = CryInterlockedDecrement(&m_nRefCount);
    if (refCnt > 0)
      return;
    delete this;
  }
  void AddRef() {
    CryInterlockedIncrement(&m_nRefCount);
  }

	STexAnim()
	{
    m_nRefCount = 1;
		m_Rand = 0;
		m_NumAnimTexs = 0;
		m_bLoop = false;
		m_Time = 0.0f;
	}

	~STexAnim()
	{     
		for (uint32 i=0; i<m_TexPics.Num(); i++)
		{
			ITexture *pTex = (ITexture *) m_TexPics[i];
			SAFE_RELEASE(pTex);
		}
		m_TexPics.Free();
	}

	STexAnim& operator = (const STexAnim& sl)
	{
		// make sure not same object
		if(this == &sl)   
		{
			return *this;
		}

		for (uint32 i=0; i<m_TexPics.Num(); i++)
		{
			ITexture *pTex = (ITexture *)m_TexPics[i];
			SAFE_RELEASE(pTex);
		}
		m_TexPics.Free();

		for (uint32 i=0; i<sl.m_TexPics.Num(); i++)
		{
			ITexture *pTex = (ITexture *)sl.m_TexPics[i];
			if(pTex)
			{
				pTex->AddRef();
			}

			m_TexPics.AddElem(sl.m_TexPics[i]);
		}

		m_Rand = sl.m_Rand;
		m_NumAnimTexs = sl.m_NumAnimTexs;
		m_bLoop = sl.m_bLoop;
		m_Time = sl.m_Time;

		return *this;
	}
};

#endif// _ITEXTURE_H_
