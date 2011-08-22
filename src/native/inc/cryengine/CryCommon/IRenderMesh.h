#include DEVIRTUALIZE_HEADER_FIX(IRenderMesh.h)

#ifndef _RenderMesh_H_
#define _RenderMesh_H_

#include "VertexFormats.h"
#include <IMaterial.h>
#include <Cry_Geo.h>

class CMesh;
struct IIndexedMesh;
struct SMRendTexVert;
struct UCol;
struct GeomInfo;

struct ExtSkinVertex;
struct TFace;
struct SMeshSubset;
struct GeomQuery;

enum ERenderMeshType
{
  eRMT_Static = 0,
  eRMT_KeepSystem = 1,
};


//! structure for tangent basises storing
struct TangData
{
  Vec3 tangent;
  Vec3 binormal;      
  Vec3 tnormal;     
};


#define FSM_MORPH_TARGETS 1
#define FSM_NO_TANGENTS   2
#define FSM_VOXELS        4
#define FSM_CREATE_DEVICE_MESH 8
#define FSM_SETMESH_ASYNC 16


// Invalidate video buffer flags
#define FMINV_STREAM      1
#define FMINV_STREAM_MASK ((1<<VSF_NUM)-1)
#define FMINV_INDICES     0x100
#define FMINV_ALL        -1

// Stream lock flags
#define  FSL_READ         1
#define  FSL_WRITE        2
#define  FSL_DYNAMIC      4
#define  FSL_DISCARD      8
#define  FSL_VIDEO            0x10
#define  FSL_SYSTEM           0x20
#define  FSL_INSTANCED        0x40 
#define  FSL_LOCKED           0x80			 // For internal use
#define  FSL_NONSTALL_MAP     0x100  // Map must not stall for VB/IB locking
#define  FSL_SYSTEM_CREATE    (FSL_WRITE|FSL_DISCARD|FSL_SYSTEM)
#define  FSL_SYSTEM_UPDATE    (FSL_WRITE|FSL_SYSTEM)
#define  FSL_VBIBPUSHDOWN     0x200 

UNIQUE_IFACE struct IRenderMesh
{
	virtual ~IRenderMesh(){}
  enum EMemoryUsageArgument
  {
    MEM_USAGE_COMBINED,
    MEM_USAGE_ONLY_SYSTEM,
    MEM_USAGE_ONLY_VIDEO,
		MEM_USAGE_ONLY_STREAMS,
  };

	//Rendermesh stats
	struct SRenderMeshStat
	{
		uint64 nStartTime;
		uint64 nTotalTime;
		uint16 nFrameID;					//Can hold about 36 minutes worth of frame IDs
		uint8 nShadowBatchNumber;
		uint8 nZPassBatchNumber;
		uint8 nOpaqueBatchNumber;
		uint8 nTransparentBatchNumber;
		uint8 nPredictedTotalBatchNumber;	//Total number of draw calls scheduled in a frame
		uint8 nTotalBatchNumber;	//Total number of draw calls, for verification and reference

		ILINE void ResetBatchCounts()
		{
			nShadowBatchNumber = 0;
			nZPassBatchNumber = 0;
			nOpaqueBatchNumber = 0;
			nTransparentBatchNumber = 0;
			nPredictedTotalBatchNumber = 0;
			nTotalBatchNumber = 0;
		}
	};

	// Render mesh initialization parameters, that can be used to create RenderMesh from row pointers.
	struct SInitParamerers
	{
		EVertexFormat eVertexFormat;
		ERenderMeshType eType;

		void *pVertBuffer;
		int nVertexCount;
		SPipTangents *pTangents;
		uint16* pIndices;
		int nIndexCount;
		eRenderPrimitiveType nPrimetiveType;
		int nRenderChunkCount;
		int nClientTextureBindID;
		bool bOnlyVideoBuffer;
		bool bPrecache;
		bool bLockForThreadAccess;

		SInitParamerers() : eVertexFormat(eVF_P3F_C4B_T2F),eType(eRMT_Static),pVertBuffer(0),nVertexCount(0),pTangents(0),pIndices(0),nIndexCount(0),
			nPrimetiveType(eptTriangleList),nRenderChunkCount(0),nClientTextureBindID(0),bOnlyVideoBuffer(false),bPrecache(true),bLockForThreadAccess(false) {}
	};

  //////////////////////////////////////////////////////////////////////////
  // Reference Counting.
  virtual void AddRef() = 0;
  virtual int Release() = 0;
  //////////////////////////////////////////////////////////////////////////

	// Prevent rendering if video memory could not been allocated for it
	virtual bool CanRender() = 0;

  // Returns type name given to the render mesh on creation time.
  virtual const char* GetTypeName() = 0;
  // Returns the name of the source given to the render mesh on creation time.
  virtual const char* GetSourceName() const = 0;

  virtual int  GetIndicesCount() = 0;
  virtual int  GetVerticesCount() = 0;
  virtual EVertexFormat GetVertexFormat() = 0;
  virtual uint32 GetMeshType() = 0;

	virtual const IRenderMesh::SRenderMeshStat* GetRenderStats() const = 0;

  // Create render buffers from render mesh. Returns the final size of the render mesh or ~0U on failure
  virtual size_t SetMesh( CMesh &mesh, int nSecColorsSetOffset=0, uint32 flags=0, const Vec3 *pPosOffset=0, bool requiresLock=true) = 0;	
  virtual void CopyTo(IRenderMesh *pDst, int nAppendVtx=0, bool bDynamic=false) = 0;
  virtual void SetSkinningDataVegetation(struct SMeshBoneMapping *pBoneMapping ) = 0;
  virtual void SetSkinningDataCharacter(CMesh& mesh, struct SMeshBoneMapping *pBoneMapping) = 0;
  // Creates an indexed mesh from this render mesh (accepts an optional pointer to an IIndexedMesh object that should be used)
  virtual IIndexedMesh* GetIndexedMesh(IIndexedMesh *pIdxMesh=0) = 0;
  virtual int GetRenderChunksCount(IMaterial *pMat, int& nRenderTrisCount) = 0;

  virtual IRenderMesh *GenerateMorphWeights() = 0; 
  virtual IRenderMesh *GetMorphBuddy() = 0;
  virtual void SetMorphBuddy(IRenderMesh *pMorph) = 0;

  virtual bool UpdateVertices(const void *pVertBuffer, int nVertCount, int nOffset, int nStream=VSF_GENERAL, bool requiresLock=true) = 0;
  virtual bool UpdateIndices(const uint16 *pNewInds, int nInds, int nOffsInd=0, bool requiresLock=true) = 0;
  virtual void SetCustomTexID( int nCustomTID ) = 0;
  virtual void SetChunk(int nIndex, CRenderChunk &chunk) = 0;
  virtual void SetChunk(IMaterial *pNewMat, int nFirstVertId, int nVertCount, int nFirstIndexId, int nIndexCount, float texelAreaDensity, int nMatID = 0) = 0;
	
	// Assign array of render chunks.
	// Initializes render element for each render chunk.
	virtual void SetRenderChunks( CRenderChunk *pChunksArray,int nCount,bool bSubObjectChunks ) = 0;

  virtual void GenerateQTangents() = 0;
  virtual void CreateChunksSkinned() = 0;
  virtual void NextDrawSkinned() = 0;
  virtual IRenderMesh *GetVertexContainer() = 0;
  virtual void SetVertexContainer(IRenderMesh *pBuf) = 0;
  virtual PodArray<CRenderChunk>& GetChunks() = 0;
  virtual PodArray<CRenderChunk>* GetChunksSkinned() = 0;
  virtual void SetBBox(const Vec3& vBoxMin, const Vec3& vBoxMax) = 0;
  virtual void GetBBox(Vec3& vBoxMin, Vec3& vBoxMax) = 0;
  virtual void UpdateBBoxFromMesh() = 0;
  virtual uint32 * GetPhysVertexMap() = 0;
  virtual void SetPhysVertexMap(uint32 * pVtxMap) = 0;
  virtual bool IsEmpty() = 0;

  virtual byte *GetPosPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;
  virtual byte *GetColorPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;
  virtual byte *GetNormPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;
  virtual byte *GetUVPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;

  virtual byte *GetTangentPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;
  virtual byte *GetBinormalPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;

  virtual byte *GetHWSkinPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;
  virtual byte *GetShapePtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;
  virtual byte *GetMorphTargetPtr(int32& nStride, uint32 nFlags, int32 nOffset=0) = 0;

  virtual void UnlockStream(int nStream) = 0;
  virtual void UnlockIndexStream() = 0;

  virtual uint16 *GetIndexPtr(uint32 nFlags, int32 nOffset=0) = 0;
  virtual const PodArray<std::pair<int,int> > * GetTrisForPosition(const Vec3 & vPos, IMaterial * pMaterial) = 0;
  virtual void GetRandomPos(RandomPos& ran, GeomQuery& geo, EGeomForm eForm)=0;
  virtual float ComputeExtent(GeomQuery& geo, EGeomForm eForm)=0;

  virtual void Render(const struct SRendParams& rParams, CRenderObject* pObj, IMaterial *pMaterial, bool bSkinned=false) = 0;
  virtual void Render(CRenderObject * pObj, const SRenderObjectModifier * pROII=NULL) = 0;
  virtual void AddRenderElements(IMaterial * pIMatInfo, CRenderObject * pObj=0, int nSortId=EFSLIST_GENERAL, int nAW=1) = 0;
  virtual void AddRE(IMaterial * pMaterial, CRenderObject * pObj, IShader * pEf, int nList=EFSLIST_GENERAL, int nAW=1) = 0;
  virtual void SetREUserData(float * pfCustomData, float fFogScale=0, float fAlpha=1) = 0;

  // Debug draw this render mesh.
  virtual void DebugDraw(const struct SGeometryDebugDrawInfo &info, uint32 nVisibleChunksMask=~0, float fExtrdueScale = 0.01f ) = 0;

  // Returns mesh memory usage and add it to the CrySizer (if not NULL).
  // Arguments:
  //     pSizer - Sizer interface, can be NULL if caller only want to calculate size
  //     nType - see EMemoryUsageArgument
  virtual size_t GetMemoryUsage( ICrySizer *pSizer,EMemoryUsageArgument nType ) = 0;
	virtual void GetMemoryUsage( ICrySizer *pSizer) const=0;

  // Get allocated only in video memory or only in system memory.
  virtual int GetAllocatedBytes( bool bVideoMem ) = 0;
  virtual float GetAverageTrisNumPerChunk(IMaterial * pMat) = 0;
  virtual int GetTextureMemoryUsage( IMaterial *pMaterial,ICrySizer *pSizer=NULL, bool bStreamedIn = true ) = 0;
  virtual void KeepSysMesh(bool keep) = 0;	// workaround: temp workaround for GDC-888
	virtual void UnKeepSysMesh() = 0;
  virtual void SetMeshLod( int nLod ) = 0;

	virtual void LockForThreadAccess() = 0;
	virtual void UnLockForThreadAccess()= 0;
};

struct SBufferStream
{
  void *m_pLocalData;      // pointer to buffer data
  int32 m_nDevID;
  SBufferStream()
  {
    m_pLocalData = NULL;
    m_nDevID = -1;
  }
};

//////////////////////////////////////////////////////////////////////
// General VertexBuffer created by CreateVertexBuffer() function
class CVertexBuffer
{
public:
  CVertexBuffer()
  {
    m_eVF = eVF_Unknown;
    m_nVerts = 0;
  }
  CVertexBuffer(void* pData, EVertexFormat eVF, int nVerts=0)
  {
    m_VS.m_pLocalData = pData;
    m_eVF = eVF;
    m_nVerts = nVerts;
  }
#ifdef _RENDERER
  ~CVertexBuffer();
#endif

  SBufferStream m_VS;
  EVertexFormat m_eVF;
  int32 m_nVerts;
};

class CIndexBuffer
{
public:
  CIndexBuffer()
  {
    m_nInds = 0;
  }

  CIndexBuffer(uint16* pData)
  {
    m_VS.m_pLocalData = pData;
    m_nInds = 0;
  }
#ifdef _RENDERER
  ~CIndexBuffer();
#endif

  SBufferStream m_VS;
  int32 m_nInds;
};



#endif // _RenderMesh_H_
