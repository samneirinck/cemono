////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   i3dengine.h
//  Version:     v1.00
//  Created:     28/5/2001 by Vladimir Kajalin
//  Compilers:   Visual Studio.NET
//  Description: 3dengine interface
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(I3DEngine.h)

#ifndef CRY3DENGINEINTERFACE_H
#define CRY3DENGINEINTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// !!! Do not add any headers here !!!
#include "CryEngineDecalInfo.h" 
#include "IStatObj.h"
#include "IRenderer.h"
#include "IProcess.h"
#include "IMaterial.h"
#include "ISurfaceType.h"
#include "IStreamEngine.h"
#include "IEntityRenderState.h"
#include "CryArray.h"
#include "IMemory.h"
// !!! Do not add any headers here !!!

struct ISystem;
struct ICharacterInstance;
struct CVars;
struct pe_params_particle;
struct IMaterial;
struct RenderLMData;
struct AnimTexInfo;
struct ISplineInterpolator;
class CContentCGF;
struct SpawnParams;
struct ForceObject;
class I3DSampler;
struct IAutoCubeMapRenderNode;
class ICrySizer;
struct IRenderNode;
struct CRNTmpData;
struct IParticleManager;
struct IDeferredPhysicsEventManager;

enum E3DEngineParameter
{
	E3DPARAM_HDR_DYNAMIC_POWER_FACTOR,
	E3DPARAM_HDR_CONTRAST,
	E3DPARAM_HDR_BLUE_SHIFT,
	E3DPARAM_HDR_BLUE_SHIFT_THRESHOLD,

	E3DPARAM_SUN_COLOR,
	E3DPARAM_SUN_COLOR_MULTIPLIER,

	E3DPARAM_SKY_COLOR,
	E3DPARAM_SKY_COLOR_MULTIPLIER,

	E3DPARAM_AMBIENT_GROUND_COLOR,
	E3DPARAM_AMBIENT_MIN_HEIGHT,
	E3DPARAM_AMBIENT_MAX_HEIGHT,
	E3DPARAM_AMBIENT_AFFECT_GLOBALCUBEMAP,

	E3DPARAM_FOG_COLOR,
	E3DPARAM_FOG_COLOR2,
	E3DPARAM_FOG_USECOLORGRADIENT,

	E3DPARAM_VOLFOG_GLOBAL_DENSITY,
	E3DPARAM_VOLFOG_ATMOSPHERE_HEIGHT,
	E3DPARAM_VOLFOG_RAMP,

	E3DPARAM_SKYLIGHT_SUN_INTENSITY,
	E3DPARAM_SKYLIGHT_SUN_INTENSITY_MULTIPLIER,

	E3DPARAM_SKYLIGHT_KM,
	E3DPARAM_SKYLIGHT_KR,
	E3DPARAM_SKYLIGHT_G,

	E3DPARAM_SKYLIGHT_WAVELENGTH_R,
	E3DPARAM_SKYLIGHT_WAVELENGTH_G,
	E3DPARAM_SKYLIGHT_WAVELENGTH_B,

	E3DPARAM_NIGHSKY_HORIZON_COLOR,
	E3DPARAM_NIGHSKY_ZENITH_COLOR,
	E3DPARAM_NIGHSKY_ZENITH_SHIFT,

	E3DPARAM_NIGHSKY_STAR_INTENSITY,

	E3DPARAM_NIGHSKY_MOON_DIRECTION,
	E3DPARAM_NIGHSKY_MOON_COLOR,
	E3DPARAM_NIGHSKY_MOON_SIZE,
	E3DPARAM_NIGHSKY_MOON_INNERCORONA_COLOR,
	E3DPARAM_NIGHSKY_MOON_INNERCORONA_SCALE,
	E3DPARAM_NIGHSKY_MOON_OUTERCORONA_COLOR,
	E3DPARAM_NIGHSKY_MOON_OUTERCORONA_SCALE,

	E3DPARAM_CLOUDSHADING_SUNLIGHT_MULTIPLIER,
	E3DPARAM_CLOUDSHADING_SKYLIGHT_MULTIPLIER,

	E3DPARAM_CORONA_SIZE,

	E3DPARAM_OCEANFOG_COLOR,
	E3DPARAM_OCEANFOG_COLOR_MULTIPLIER,	
	E3DPARAM_OCEANFOG_DENSITY,
	
	// Sky highlight (ex. From Lightning)
	E3DPARAM_SKY_HIGHLIGHT_COLOR,
	E3DPARAM_SKY_HIGHLIGHT_SIZE,
	E3DPARAM_SKY_HIGHLIGHT_POS,

	E3DPARAM_SKY_SUNROTATION,
	E3DPARAM_SKY_MOONROTATION,

	E3DPARAM_SKY_SKYBOX_ANGLE,
	E3DPARAM_SKY_SKYBOX_STRETCHING,

  EPARAM_SUN_SHAFTS_VISIBILITY,

	E3DPARAM_SKYBOX_MULTIPLIER,

	E3DPARAM_DAY_NIGHT_INDICATOR,

  // --------------------------------------

  E3DPARAM_COLORGRADING_COLOR_SATURATION,
  E3DPARAM_COLORGRADING_FILTERS_PHOTOFILTER_COLOR,
  E3DPARAM_COLORGRADING_FILTERS_PHOTOFILTER_DENSITY,
  E3DPARAM_COLORGRADING_FILTERS_GRAIN

};

//////////////////////////////////////////////////////////////////////////
// Description:
//     This structure is filled and passed by the caller to the DebugDraw functions of the stat object or entity.
struct SGeometryDebugDrawInfo
{
	Matrix34 tm;        // Transformation Matrix
	ColorB   color;     // Optional color of the lines.
	ColorB   lineColor; // Optional color of the lines.
	
	// Optional flags controlling how to render debug draw information.
	uint32   bNoCull  : 1;
	uint32   bNoLines : 1;
	uint32   bExtrude : 1; // Extrude debug draw geometry alittle bit so it is over real geometry.

	SGeometryDebugDrawInfo() : color(255,0,255,255),lineColor(255,255,0,255),bNoLines(0),bNoCull(0) { tm.SetIdentity(); }
};

// Summary:
//     Physics material enumerator, allows for 3dengine to get material id from game code.
struct IPhysMaterialEnumerator
{
	virtual ~IPhysMaterialEnumerator(){}
  virtual int EnumPhysMaterial(const char * szPhysMatName) = 0;
	virtual bool IsCollidable(int nMatId) = 0;
	virtual int	GetMaterialCount() = 0;
	virtual const char* GetMaterialNameByIndex( int index ) = 0;
};

// Summary:
//     Flags used by I3DEngine::DrawLowDetail.
enum EDrawLowDetailFlags
{
	DLD_DETAIL_OBJECTS            =   1<<0,
	DLD_DETAIL_TEXTURES           =   1<<1,
	DLD_TERRAIN_WATER             =   1<<2,
	DLD_FAR_SPRITES               =   1<<3,
	DLD_STATIC_OBJECTS            =   1<<4,
	DLD_PARTICLES                 =   1<<5,
	DLD_DECALS										=   1<<6,
	DLD_TERRAIN_LIGHT             =		1<<7,
	DLD_SHADOW_MAPS               =		1<<9,
	DLD_ENTITIES                  =		1<<10,
	DLD_TERRAIN                   =		1<<11,
};

// Summary:
//	   Physics foreign data flags.
enum EPhysForeignFlags
{
	PFF_HIDABLE										= 1,
	PFF_HIDABLE_SECONDARY					=	2,
	PFF_EXCLUDE_FROM_STATIC				= 4,
	PFF_BRUSH											= 8,
	PFF_VEGETATION								= 16,
	PFF_UNIMPORTANT								= 32,
	PFF_OUTDOOR_AREA							= 64,
};

// Summary:
//	   Ocean data flags.
enum EOceanRenderFlags
{
  OCR_NO_DRAW     =   1<<0,
  OCR_OCEANVOLUME_VISIBLE  =   1<<1,
};

// Summary:
//		Structure to pass vegetation group properties.
UNIQUE_IFACE struct IStatInstGroup
{
	enum EPlayerHideable
	{
		ePlayerHideable_None = 0,
		ePlayerHideable_High,
		ePlayerHideable_Mid,
		ePlayerHideable_Low,

		ePlayerHideable_COUNT,
	};

	IStatInstGroup() 
	{ 
		pStatObj = 0;
		szFileName[0]=0;
		bHideability = 0;
		bHideabilitySecondary = 0;

    bPickable = 0;
		fBending = 0;
    bComplexBending = 0;
		bCastShadow = 0;
		bRecvShadow = 0;
		bPrecShadow = true;
		bUseAlphaBlending = 0;
		fSpriteDistRatio = 1.f;
		fShadowDistRatio = 1.f;
		fMaxViewDistRatio = 1.f;
    fLodDistRatio = 1.f;
		fBrightness = 1.f;
		pMaterial = 0;
    bUseSprites = true;
	
		fDensity=1;
		fElevationMax=4096;
		fElevationMin=8;
		fSize=1 ;
		fSizeVar=0 ;
		fSlopeMax=90 ;
		fSlopeMin=0 ;
		bRandomRotation = false;
    nMaterialLayers = 0;
		bAllowIndoor = bAffectedByVoxels = bUseTerrainColor = bAlignToTerrain = false;
		minConfigSpec = (ESystemConfigSpec)0;
		nTexturesAreStreamedIn = 0;

		nPlayerHideable = ePlayerHideable_None;
	}

	_smart_ptr<IStatObj> pStatObj;
	char  szFileName[256];
	bool	bHideability;
	bool	bHideabilitySecondary;
  bool  bPickable;
	float fBending;
  bool  bComplexBending;
	bool	bCastShadow;
	bool	bRecvShadow;
	bool	bPrecShadow;
	bool	bUseAlphaBlending;
	float fSpriteDistRatio;
  float fLodDistRatio;
	float fShadowDistRatio;
	float fMaxViewDistRatio;
	float	fBrightness;
  bool  bUseSprites;
	bool  bRandomRotation;
	bool  bAlignToTerrain;
  bool  bUseTerrainColor;
	bool  bAllowIndoor;
	bool  bAffectedByVoxels;

	float fDensity;
	float fElevationMax;
	float fElevationMin;
	float fSize;
	float fSizeVar;
	float fSlopeMax;
	float fSlopeMin;

  float fVegRadius;
  float fVegRadiusVert;
  float fVegRadiusHor;

  int nPlayerHideable;

	// Minimal configuration spec for this vegetation group.
	ESystemConfigSpec minConfigSpec;

	// Override material for this instance group.
	_smart_ptr<IMaterial> pMaterial;

  // Material layers bitmask -> which layers are active.
  uint8 nMaterialLayers;

	// Textures Are Streamed In.
	uint8 nTexturesAreStreamedIn;

	// Flags similar to entity render flags.
	int m_dwRndFlags;
};

// Description:
//     Water volumes should usually be created by I3DEngine::CreateWaterVolume.
// Summary:
//     Interface to water volumes.
struct IWaterVolume
{
//DOC-IGNORE-BEGIN
	virtual ~IWaterVolume(){}
	virtual void UpdatePoints(const Vec3 * pPoints, int nCount, float fHeight) = 0;
	virtual void SetFlowSpeed(float fSpeed) = 0;
	virtual void SetAffectToVolFog(bool bAffectToVolFog) = 0;
	virtual void SetTriSizeLimits(float fTriMinSize, float fTriMaxSize) = 0;
//	virtual void SetMaterial(const char * szShaderName) = 0;
	virtual void SetMaterial( IMaterial *pMaterial ) = 0;
	virtual IMaterial * GetMaterial() = 0;
	virtual const char* GetName() const = 0;
	virtual void SetName(const char * szName) = 0;
//DOC-IGNORE-END

	// Description:
	//     Used to change the water level. Will assign a new Z value to all 
	//     vertices of the water geometry.
	// Arguments:
	//     vNewOffset - Position of the new water level
	// Summary:
	//     Sets a new water level.
	virtual void SetPositionOffset(const Vec3 & vNewOffset) = 0;
};

// Summary:
//     Provides information about the different VisArea volumes.
UNIQUE_IFACE struct IVisArea
{
	virtual ~IVisArea(){}
	// Summary:
	//     Gets the last rendered frame id.
	// Return Value:
	//     An int which contains the frame id.
	virtual int GetVisFrameId() = 0;
	
	// Description:
	//     Gets a list of all the VisAreas which are connected to the current one. 
	// Arguments:
	//     pAreas               - Pointer to an array of IVisArea*
	//     nMaxConnNum          - The maximum of IVisArea to write in pAreas
	//     bSkipDisabledPortals - Ignore portals which are disabled
	// Return Value:
	//     An integer which hold the amount of VisArea found to be connected. If 
	//     the return is equal to nMaxConnNum, it's possible that not all 
	//     connected VisAreas were returned due to the restriction imposed by the 
	//     argument.
	// Summary:
	//     Gets all the areas which are connected to the current one.
	virtual	int GetVisAreaConnections(IVisArea ** pAreas, int nMaxConnNum, bool bSkipDisabledPortals = false) = 0;

	// Summary:
	//     Determines if it's connected to an outdoor area.
	// Return Value:
	//     True if the VisArea is connected to an outdoor area.
	virtual bool IsConnectedToOutdoor() = 0;

	// Summary:
	//     Determines if the visarea ignores Global Illumination inside.
	// Return Value:
	//     True if the VisArea ignores Global Illumination inside.
	virtual bool IsIgnoringGI() const = 0;

	// Summary:
	//     Gets the name.
	// Notes:
	//     The name is always returned in lower case.
	// Return Value:
	//     A null terminated char array containing the name of the VisArea.
	virtual const char * GetName() = 0;

	// Summary:
	//     Determines if this VisArea is a portal.
	// Return Value:
	//     True if the VisArea is a portal, or false in the opposite case.
	virtual bool IsPortal() = 0;

	// Description: 
	//     Searches for a specified VisArea to see if it's connected to the current 
	//     VisArea.
	// Arguments:
	//     pAnotherArea         - A specified VisArea to find
	//     nMaxRecursion        - The maximum number of recursion to do while searching
	//     bSkipDisabledPortals - Will avoid searching disabled VisAreas
	//		 pVisitedAreas				- if not NULL - will get list of all visited areas
	// Return Value:
	//     True if the VisArea was found.
	// Summary:
	//     Searches for a specified VisArea.
	virtual bool FindVisArea(IVisArea * pAnotherArea, int nMaxRecursion, bool bSkipDisabledPortals) = 0;

	// Description: 
	//     Searches for the surrounding VisAreas which connected to the current 
	//     VisArea.
	// Arguments:
	//     nMaxRecursion        - The maximum number of recursion to do while searching
	//     bSkipDisabledPortals - Will avoid searching disabled VisAreas
	//	   pVisitedAreas		- if not NULL - will get list of all visited areas
	// Return Value:
	//     None.
	// Summary:
	//     Searches for the surrounding VisAreas.
	virtual void FindSurroundingVisArea( int nMaxRecursion, bool bSkipDisabledPortals, PodArray<IVisArea*> * pVisitedAreas = NULL, int nMaxVisitedAreas = 0, int nDeepness = 0) = 0;

	// Summary:
	//     Determines if it's affected by outdoor lighting.
	// Return Value:
	//     Returns true if the VisArea if it's affected by outdoor lighting, else
	//     false will be returned.
	virtual bool IsAffectedByOutLights() = 0;

	// Summary:
	//     Determines if the spere can be affect the VisArea.
	// Return Value:
	//     Returns true if the VisArea can be affected by the sphere, else
	//     false will be returned.
	virtual bool IsSphereInsideVisArea(const Vec3 & vPos, const f32 fRadius) = 0;

	// Summary:
	//     Clips geometry inside or outside a vis area.
	// Return Value:
	//     Whether geom was clipped.
	virtual bool ClipToVisArea(bool bInside, Sphere& sphere, Vec3 const& vNormal) = 0;
	
	// Summary:
	//     Gives back the axis aligned bounding box of VisArea.
	// Return Value:
	//     Returns the pointer of a AABB.
	virtual const AABB* GetAABBox() const = 0;

	// Summary:
	//     Gives back the axis aligned bounding box of all static objects in the VisArea.
	//     This AABB can be huger than the ViaArea AABB as some objects might not be completely inside the VisArea.
	// Return Value:
	//     Returns the pointer to the AABB.
	virtual const AABB* GetStaticObjectAABBox() const = 0;

	// Summary:
	//     Determines if the point can be affect the VisArea.
	// Return Value:
	//     Returns true if the VisArea can be affected by the point, else
	//     false will be returned.
	virtual bool IsPointInsideVisArea(const Vec3 & vPos) = 0;

	// Description: 
	//     Calculates the 6colors of the Ambient cube for this AABBox.
	// Arguments:
	//     pAmbientCube    - float4[6]	feeded with the color data
	//     AABBMin/AABBMax - BoundingBox
	// Return Value:
	//     None.
	virtual	void	CalcAmbientCube(f32* pAmbientCube,const Vec3& rAABBMin,const Vec3& rAABBMax	)	=0;

	// Description: 
	//     Calculates the 4colors per face of the Ambient cube for this AABBox.
	// Arguments:
	//     pAmbientCube    - float4[24]	feeded with the color data
	//     AABBMin/AABBMax - BoundingBox
	// Return Value:
	//     None.
	virtual	void	CalcHQAmbientCube(f32* pAmbientCube,const Vec3& rAABBMin,const Vec3& rAABBMax	)	=0;

  // Description: 
  //     Return vis area final ambient color (ambient color depends on factors, like if connected to outdoor, is affected by skycolor - etc)
  // Arguments:
  //     none
  // Return Value:
  //     none
  virtual const Vec3 GetFinalAmbientColor() = 0;
};


// Water level unknown.
#define WATER_LEVEL_UNKNOWN -1000000.f
#define BOTTOM_LEVEL_UNKNOWN -1000000.f


// float m_SortId		: offseted by +WATER_LEVEL_SORTID_OFFSET if the camera object line is crossing the water surface
// : otherwise offseted by -WATER_LEVEL_SORTID_OFFSET
#define WATER_LEVEL_SORTID_OFFSET			10000000


// Summary:
//     indirect lighting quadtree definition.
namespace NQT
{
	// Forward declaration
	template <class TLeafContent, uint32 TMaxCellElems, class TPosType, class TIndexType, bool TUseRadius>
	class CQuadTree;
}

#define FILEVERSION_TERRAIN_SHLIGHTING_FILE 5

enum EVoxelBrushShape
{
	evbsSphere = 1,
	evbsBox,
};

enum EVoxelEditTarget
{
	evetVoxelObjects = 1,
};

enum EVoxelEditOperation
{
  eveoNone=0,
  eveoPaintHeightPos,
  eveoPaintHeightNeg,
	eveoCreate,
	eveoSubstract,
	eveoMaterial,
  eveoBaseColor,
	eveoBlurPos,
  eveoBlurNeg,
	eveoCopyTerrainPos,
  eveoCopyTerrainNeg,
  eveoPickHeight,
  eveoIntegrateMeshPos,
  eveoIntegrateMeshNeg,
  eveoForceDepth,
  eveoLimitLod,
  eveoLast,
};

#define COMPILED_HEIGHT_MAP_FILE_NAME       "terrain\\terrain.dat"
#define COMPILED_VISAREA_MAP_FILE_NAME      "terrain\\indoor.dat"
#define COMPILED_TERRAIN_TEXTURE_FILE_NAME  "terrain\\cover.ctc"
#define COMPILED_VOX_MAP_FILE_NAME          "terrain\\voxmap.dat"

//////////////////////////////////////////////////////////////////////////

#pragma pack(push,4)

struct STerrainInfo
{
	int nHeightMapSize_InUnits;
	int nUnitSize_InMeters;
	int nSectorSize_InMeters;
	int nSectorsTableSize_InSectors;
	float fHeightmapZRatio;
	float fOceanWaterLevel;

	AUTO_STRUCT_INFO
};

#define TERRAIN_CHUNK_VERSION 24
#define VISAREAMANAGER_CHUNK_VERSION 6

#define ISOTREE_CHUNK_VERSION_BC 2
#define ISOTREE_CHUNK_VERSION 3

#define SERIALIZATION_FLAG_BIG_ENDIAN 1
#define SERIALIZATION_FLAG_SECTOR_PALETTES 2

#define TCH_FLAG2_AREA_ACTIVATION_IN_USE 1

struct STerrainChunkHeader
{
  int8 nVersion;
  int8 nDummy;
  int8 nFlags;
  int8 nFlags2;
	int32 nChunkSize;
	STerrainInfo TerrainInfo;

	AUTO_STRUCT_INFO
};

struct SVoxTerrainInfo
{
  AABB aabbTerrain;
  float fOceanWaterLevel;

  AUTO_STRUCT_INFO
};

struct SVoxTerrainChunkHeader
{
  int nVersion;
  int nChunkSize;
  SVoxTerrainInfo TerrainInfo;

  AUTO_STRUCT_INFO
};

struct SVisAreaManChunkHeader
{
  int8 nVersion;
  int8 nDummy;
  int8 nFlags;
  int8 nFlags2;
  int nChunkSize;
	int nVisAreasNum;
	int nPortalsNum;
	int nOcclAreasNum;

	AUTO_STRUCT_INFO
};

struct SOcTreeNodeChunk
{
  int16	nChunkVersion;
  int16 ucChildsMask;
  AABB	nodeBox;
  int32 nObjectsBlockSize;

  AUTO_STRUCT_INFO
};

struct SVoxNodeDataChunk
{
  uint8 ucChildsMask;
  uint8 ucMatId;
  uint8 ucDummy1;
  uint8 ucDummy2;
  float fChildsSumm;

  AUTO_STRUCT_INFO
};

struct SVoxNodeMeshChunk
{
  uint8 ucVersion;
  uint8 ucChildsMask;
  uint8 ucDummy0;
  uint8 ucDummy1;
  uint32 nMeshBlockSize;
  uint32 nTextureBlockSize;

  AUTO_STRUCT_INFO
};

struct IGetLayerIdAtCallback
{
	virtual ~IGetLayerIdAtCallback(){}
  virtual uint32 GetLayerIdAtPosition( const int x, const int y ) const = 0;
  virtual uint32 GetSurfaceTypeIdAtPosition( const int x, const int y ) const = 0;
  virtual bool GetHoleAtPosition( const int x, const int y ) const = 0;
  virtual ColorB GetColorAtPosition( const float x, const float y, bool bBilinear ) = 0;
  virtual float GetElevationAtPosition( const float x, const float y ) = 0;
  virtual float GetRGBMultiplier( ) = 0;
};

//	Summary:
//     Interface to terrain engine
struct IVoxTerrain
{
	virtual ~IVoxTerrain(){}
  virtual bool SetCompiledData(byte * pData, int nDataSize, bool bUpdateMesh, EEndian eEndian, AABB * pAreaBox, int nSID=0) = 0;
  virtual IMemoryBlock * GetCompiledData(bool bSaveMesh, EEndian eEndian, bool bSaveForEditing, AABB * pAreaBox, int nSID=0) = 0;
  virtual void DrawEditingHelper(const Sphere & sp, EVoxelEditOperation eOperation, IMaterial * pHelperMat) = 0;
  virtual void OnMouse(bool bUp) = 0;
	virtual PodArray<IRenderNode*> * GetNodesForUpdate() = 0;
  virtual void PaintLayerId( const float fpx, const float fpy, const float radius, const float hardness, const uint32 dwLayerId ) = 0;
  virtual void SetLayerData(void * pData, int nDataSize) = 0;
  virtual void GetLayerData(void ** pData, int & nDataSize) = 0;
  virtual void SetTextureArea(Vec3 * pPoints, int nPointsCount, int nShapePartId) = 0;
};

#define TERRAIN_DEFORMATION_MAX_DEPTH 3.f

struct SHotUpdateInfo
{
  SHotUpdateInfo()
  {
    nHeigtmap = 1;
    nObjTypeMask = ~0;
    areaBox.Reset();
  }

  uint32 nHeigtmap;
  uint32 nObjTypeMask;
  AABB areaBox;

  AUTO_STRUCT_INFO
};

UNIQUE_IFACE struct ITerrain
{
	virtual ~ITerrain(){}
  struct SExportInfo
  {
    SExportInfo()
    {
      bHeigtmap = bObjects = true;
      areaBox.Reset();
    }
    bool bHeigtmap;
    bool bObjects;
    AABB areaBox;
  };

   // Summary:
	//	 Loads data into terrain engine from memory block.
	virtual bool SetCompiledData(byte * pData, int nDataSize, std::vector<struct IStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable, bool bHotUpdate = false, SHotUpdateInfo * pExportInfo = NULL, int nSID = 0, Vec3 vSegmentOrigin = Vec3(0,0,0)) = 0;

	// Summary: 
	//	 Saves data from terrain engine into memory block.
	virtual bool GetCompiledData(byte * pData, int nDataSize, std::vector<struct IStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable, EEndian eEndian, SHotUpdateInfo * pExportInfo = NULL, int nSID = 0) = 0;

	// Summary:
	//	 Returns terrain data memory block size.
	virtual int GetCompiledDataSize(SHotUpdateInfo * pExportInfo = NULL, int nSID = 0) = 0;
	
	// Summary:
	//	 Creates and place a new vegetation object on the terrain.
	virtual IRenderNode* AddVegetationInstance( int nStaticGroupID,const Vec3 &vPos,const float fScale,uint8 ucBright,uint8 angle, int nSID=0 ) = 0;

	// Summary:
	//	 Sets ocean level.
	virtual void SetOceanWaterLevel( float fOceanWaterLevel ) = 0;

	// Summary:
	//	 Returns whole terrain lightmap texture id.
	virtual int GetTerrainLightmapTexId( Vec4 & vTexGenInfo, int nSID = 0 ) = 0;

	// Summary:
	//	 Updates part of height map.
	// Notes:
	//	 In terrain units, by default update only elevation.
	virtual void SetTerrainElevation(int x1, int y1, int nSizeX, int nSizeY, float * pTerrainBlock, uint8 * pSurfaceData, uint32 * pResolMap, int nResolMapSizeX, int nResolMapSizeY, int nSID=0) = 0;

	// Summary:
	//	 Checks if it is possible to paint on the terrain with a given surface type ID.
	// Notes:
	//	 Should be called by the editor to avoid overflowing the sector surface type palettes.
	virtual bool CanPaintSurfaceType(int x, int y, int r, uint16 usGlobalSurfaceType) = 0;

	// Summary:
	//	 Returns current amount of terrain textures requests for streaming, if more than 0 = there is streaming in progress.
	virtual int GetNotReadyTextureNodesCount() = 0;

	// Summary:
	//	 Generates triangle mesh for specified area of heightmap.
	// Arguments:
	//	 nMinX, nMinY, nMaxX, nMaxY - defines area in meters. 
	//   arrVerts					- Vertices of the resulting mesh 
	//   arrIndices					- Indices of the resulting mesh
	// Return Value:
	//   number of triangles generated
  virtual int GenerateMeshFromHeightmap(  int nMinX, int nMinY, int nMaxX, int nMaxY, PodArray<Vec3> & arrVerts, PodArray<uint16> & arrIndices, int nSID=0 ) = 0;

	// Summary:
	//	 Retrieves the resource (mostly texture system memory) memory usage
	//   for a given region of the terrain.
	// Arguments:
	//	pSizer -    Is a pointer to an instance of the CrySizer object. The 
	//				      purpose of this object is making sure each element is
	//              accounted only once.
	//  crstAABB -  Is a reference to the bounding box in which region we
	//							want to analyze the resources.
	virtual void GetResourceMemoryUsage(ICrySizer*	pSizer,const AABB&	crstAABB, int nSID=0)=0;

	// Summary:
  	//	 Return number of used detail texture materials
	//   Fills materials array if materials!=NULL
	virtual int GetDetailTextureMaterials(IMaterial* materials[], int nSID=0) = 0;

  // Description:
  //   Allocate new world segment
  //   Returns handle of newly created segment (usually it is just id of segment in the list of currently loaded segments)
  virtual int CreateSegment() = 0;

  // Description:
  //   Set new origin for existing world segment
  //   Returns true if specified segment exist and origin was successfully updated
  virtual bool SetSegmentOrigin(int nSID, Vec3 vSegmentOrigin) = 0;

  // Description:
  //   Set new origin for existing world segment
  //   Returns true if specified segment was found and successfully deleted
  virtual bool DeleteSegment(int nSID) = 0;
};

UNIQUE_IFACE struct IVisAreaCallback
{
	virtual ~IVisAreaCallback(){}
	virtual void OnVisAreaDeleted( IVisArea* pVisArea ) = 0;
};


UNIQUE_IFACE struct IVisAreaManager
{
	virtual ~IVisAreaManager(){}
	// Summary:
	//	 Loads data into VisAreaManager engine from memory block.
  virtual bool SetCompiledData(uint8 * pData, int nDataSize, std::vector<struct IStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable, bool bHotUpdate, SHotUpdateInfo * pExportInfo) = 0;
	
	// Summary:
	//	 Saves data from VisAreaManager engine into memory block.
	virtual bool GetCompiledData(uint8 * pData, int nDataSize, std::vector<struct IStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable, EEndian eEndian, SHotUpdateInfo * pExportInfo = NULL) = 0;

	// Summary: 
	//	 Returns VisAreaManager data memory block size.
	virtual int GetCompiledDataSize(SHotUpdateInfo * pExportInfo = NULL) = 0;

	// Summary:
	//	 Returns the accumulated number of visareas and portals.
	virtual int GetNumberOfVisArea() const = 0;

	// Summary:
	//	 Returns the visarea interface based on the id (0..GetNumberOfVisArea()) it can be a visarea or a portal.
	virtual IVisArea* GetVisAreaById( int nID ) const = 0;

	virtual void AddListener( IVisAreaCallback *pListener ) = 0;
	virtual void RemoveListener( IVisAreaCallback *pListener ) = 0;
};

struct ITimeOfDayUpdateCallback;

// Summary:
//	 Interface to the Time Of Day functionality.
UNIQUE_IFACE struct ITimeOfDay
{
	virtual ~ITimeOfDay(){}
	enum ETimeOfDayParamID
	{
		PARAM_HDR_DYNAMIC_POWER_FACTOR,
		PARAM_HDR_CONTRAST,
		PARAM_HDR_BLUE_SHIFT,
		PARAM_HDR_BLUE_SHIFT_THRESHOLD,

		PARAM_TERRAIN_OCCL_MULTIPLIER,
		PARAM_SSAO_MULTIPLIER,
		PARAM_SSAO_CONTRAST_MULTIPLIER,
		PARAM_GI_MULTIPLIER,

		PARAM_SUN_COLOR,
		PARAM_SUN_COLOR_MULTIPLIER,
		PARAM_SUN_SPECULAR_MULTIPLIER,

		PARAM_SKY_COLOR,
		PARAM_SKY_COLOR_MULTIPLIER,

		PARAM_AMBIENT_GROUND_COLOR,
		PARAM_AMBIENT_GROUND_COLOR_MULTIPLIER,

		PARAM_AMBIENT_MIN_HEIGHT,
		PARAM_AMBIENT_MAX_HEIGHT,

		PARAM_FOG_COLOR,
		PARAM_FOG_COLOR_MULTIPLIER,
		PARAM_FOG_COLOR2,
		PARAM_FOG_COLOR2_MULTIPLIER,

		PARAM_VOLFOG_GLOBAL_DENSITY,
		PARAM_VOLFOG_ATMOSPHERE_HEIGHT,
		PARAM_VOLFOG_ARTIST_TWEAK_DENSITY_OFFSET,
		PARAM_VOLFOG_RAMP_START,
		PARAM_VOLFOG_RAMP_END,
		PARAM_VOLFOG_RAMP_INFLUENCE,

		PARAM_SKYLIGHT_SUN_INTENSITY,
		PARAM_SKYLIGHT_SUN_INTENSITY_MULTIPLIER,

		PARAM_SKYLIGHT_KM,
		PARAM_SKYLIGHT_KR,
		PARAM_SKYLIGHT_G,

		PARAM_SKYLIGHT_WAVELENGTH_R,
		PARAM_SKYLIGHT_WAVELENGTH_G,
		PARAM_SKYLIGHT_WAVELENGTH_B,

		PARAM_NIGHSKY_HORIZON_COLOR,
		PARAM_NIGHSKY_HORIZON_COLOR_MULTIPLIER,
		PARAM_NIGHSKY_ZENITH_COLOR,
		PARAM_NIGHSKY_ZENITH_COLOR_MULTIPLIER,
		PARAM_NIGHSKY_ZENITH_SHIFT,

		PARAM_NIGHSKY_START_INTENSITY,

		PARAM_NIGHSKY_MOON_COLOR,
		PARAM_NIGHSKY_MOON_COLOR_MULTIPLIER,
		PARAM_NIGHSKY_MOON_INNERCORONA_COLOR,
		PARAM_NIGHSKY_MOON_INNERCORONA_COLOR_MULTIPLIER,
		PARAM_NIGHSKY_MOON_INNERCORONA_SCALE,
		PARAM_NIGHSKY_MOON_OUTERCORONA_COLOR,
		PARAM_NIGHSKY_MOON_OUTERCORONA_COLOR_MULTIPLIER,
		PARAM_NIGHSKY_MOON_OUTERCORONA_SCALE,

		PARAM_CLOUDSHADING_SUNLIGHT_MULTIPLIER,
		PARAM_CLOUDSHADING_SKYLIGHT_MULTIPLIER,

		PARAM_SUN_SHAFTS_VISIBILITY,
		PARAM_SUN_RAYS_VISIBILITY,
		PARAM_SUN_RAYS_ATTENUATION,
		PARAM_SUN_RAYS_SUNCOLORINFLUENCE,
		PARAM_SUN_RAYS_CUSTOMCOLOR,

		PARAM_OCEANFOG_COLOR,
		PARAM_OCEANFOG_COLOR_MULTIPLIER,
		PARAM_OCEANFOG_DENSITY,

		PARAM_SKYBOX_MULTIPLIER,

		PARAM_COLORGRADING_COLOR_SATURATION,
		PARAM_COLORGRADING_COLOR_CONTRAST,
		PARAM_COLORGRADING_COLOR_BRIGHTNESS,

		PARAM_COLORGRADING_LEVELS_MININPUT,
		PARAM_COLORGRADING_LEVELS_GAMMA,
		PARAM_COLORGRADING_LEVELS_MAXINPUT,
		PARAM_COLORGRADING_LEVELS_MINOUTPUT,
		PARAM_COLORGRADING_LEVELS_MAXOUTPUT,

		PARAM_COLORGRADING_SELCOLOR_COLOR,
		PARAM_COLORGRADING_SELCOLOR_CYANS,
		PARAM_COLORGRADING_SELCOLOR_MAGENTAS,
		PARAM_COLORGRADING_SELCOLOR_YELLOWS,
		PARAM_COLORGRADING_SELCOLOR_BLACKS,

		PARAM_COLORGRADING_FILTERS_GRAIN,
		PARAM_COLORGRADING_FILTERS_SHARPENING,
		PARAM_COLORGRADING_FILTERS_PHOTOFILTER_COLOR,
		PARAM_COLORGRADING_FILTERS_PHOTOFILTER_DENSITY,

		PARAM_COLORGRADING_DOF_FOCUSRANGE,
		PARAM_COLORGRADING_DOF_BLURAMOUNT,

		PARAM_TOTAL
	};

	enum EVariableType
	{
		TYPE_FLOAT,
		TYPE_COLOR
	};
	struct SVariableInfo
	{
		const char *name;  // Variable name.
		const char *displayName;  // Variable user readable name.
		const char *group; // Group name.
		int nParamId;
		EVariableType type;
		float fValue[3];    // Value of the variable (3 needed for color type)
		ISplineInterpolator* pInterpolator; // Splines that control variable value
		unsigned int bSelected : 1;
	};
	struct SAdvancedInfo
	{
		float fStartTime;
		float fEndTime;
		float fAnimSpeed;
	};
	//////////////////////////////////////////////////////////////////////////
	// Access to variables that control time of the day appearance.
	//////////////////////////////////////////////////////////////////////////
	virtual int GetVariableCount() = 0;
	virtual bool GetVariableInfo( int nIndex,SVariableInfo &varInfo ) = 0;
	virtual void SetVariableValue( int nIndex,float fValue[3] ) = 0;

	virtual void ResetVariables()=0;

	// Summary:
	//	 Sets the time of the day specified in hours.
	virtual void SetTime( float fHour,bool bForceUpdate=false ) = 0;
	virtual float GetTime() = 0;

	// Summary:
	//	 Updates the current tod.
	virtual void Tick() = 0;

	virtual void SetPaused(bool paused) = 0;

	virtual void SetAdvancedInfo( const SAdvancedInfo &advInfo ) = 0;
	virtual void GetAdvancedInfo( SAdvancedInfo &advInfo ) = 0;

	// Summary:
	//	 Updates engine parameters after variable values have been changed.
	virtual void Update( bool bInterpolate=true,bool bForceUpdate=false ) = 0;
	virtual void SetUpdateCallback(ITimeOfDayUpdateCallback* pCallback) = 0;

	virtual void BeginEditMode() = 0;
	virtual void EndEditMode() = 0;

	virtual void Serialize( XmlNodeRef &node,bool bLoading ) = 0;
	virtual void Serialize( TSerialize ser ) = 0;

	virtual void SetTimer( ITimer * pTimer ) = 0;

	// MP serialization
	static const int NETSER_FORCESET = BIT(0);
	static const int NETSER_COMPENSATELAG = BIT(1);
	static const int NETSER_STATICPROPS = BIT(2);
	virtual void NetSerialize( TSerialize ser, float lag, uint32 flags ) = 0;
};

struct ITimeOfDayUpdateCallback
{
	virtual ~ITimeOfDayUpdateCallback(){}
	virtual void BeginUpdate() = 0;
	virtual bool GetCustomValue(ITimeOfDay::ETimeOfDayParamID paramID, int dim, float* pValues, float& blendWeight) = 0;
	virtual void EndUpdate() = 0;
};

UNIQUE_IFACE struct IFoliage : ISkinnable
{
	virtual ~IFoliage(){}
	enum EFoliageFlags { FLAG_FROZEN=1 };
	virtual int Serialize(TSerialize ser) = 0;
	virtual void SetFlags(int flags) = 0;
	virtual int GetFlags() = 0;
	virtual IRenderNode* GetIRenderNode() = 0;
	virtual int GetBranchCount() = 0;
	virtual IPhysicalEntity *GetBranchPhysics(int iBranch) = 0;
};

struct SSkyLightRenderParams
{
	static const int skyDomeTextureWidth = 64;
	static const int skyDomeTextureHeight =32;
	static const int skyDomeTextureSize = 64 * 32;

	static const int skyDomeTextureWidthBy8 = 8;
	static const int skyDomeTextureWidthBy4Log = 4; // = log2(64/4)
	static const int skyDomeTextureHeightBy2Log = 4; // = log2(32/2)

	SSkyLightRenderParams()
	: m_pSkyDomeMesh( 0 )
	, m_pSkyDomeTextureDataMie( 0 )
	, m_pSkyDomeTextureDataRayleigh( 0 )
	, m_skyDomeTexturePitch( 0 )
	, m_skyDomeTextureTimeStamp( -1 )
	, m_partialMieInScatteringConst( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_partialRayleighInScatteringConst( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_sunDirection( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_phaseFunctionConsts( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_hazeColor( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_hazeColorMieNoPremul( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_hazeColorRayleighNoPremul( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_skyColorTop( 0.0f, 0.0f, 0.0f )
	, m_skyColorNorth( 0.0f, 0.0f, 0.0f )
	, m_skyColorEast( 0.0f, 0.0f, 0.0f )
	, m_skyColorSouth( 0.0f, 0.0f, 0.0f )
	, m_skyColorWest( 0.0f, 0.0f, 0.0f )
	{
	}

	// Sky dome mesh
	IRenderMesh* m_pSkyDomeMesh;

	// temporarily add padding bytes to prevent fetching Vec4 constants below from wrong offset
	uint32 dummy0;
	uint32 dummy1;

	// Sky dome texture data
	const void* m_pSkyDomeTextureDataMie;
	const void* m_pSkyDomeTextureDataRayleigh;
	size_t m_skyDomeTexturePitch;
	int m_skyDomeTextureTimeStamp;

	int pad;//Enable 16 byte alignment for Vec4s

	// Sky dome shader constants
	Vec4 m_partialMieInScatteringConst;
	Vec4 m_partialRayleighInScatteringConst;
	Vec4 m_sunDirection;
	Vec4 m_phaseFunctionConsts;
	Vec4 m_hazeColor;
	Vec4 m_hazeColorMieNoPremul;
	Vec4 m_hazeColorRayleighNoPremul;

	// Sky hemisphere colors
	Vec3 m_skyColorTop;
	Vec3 m_skyColorNorth;
	Vec3 m_skyColorEast;
	Vec3 m_skyColorSouth;
	Vec3 m_skyColorWest;
};

struct sRAEColdData
{
	Vec4												m_RAEPortalInfos[96];											// It stores all data needed to solve the problem between the portals & indirect lighting.
//	byte												m_OcclLights[MAX_LIGHTS_NUM];
};

struct SVisAreaInfo
{
  float fHeight;
  Vec3 vAmbientColor;
  bool bAffectedByOutLights;
  bool bIgnoreSkyColor;
  bool bSkyOnly;
  float fViewDistRatio;
  bool bDoubleSide;
  bool bUseDeepness;
  bool bUseInIndoors;
	bool bOceanIsVisible;
	bool bIgnoreGI;
};

struct SDebugFPSInfo
{
	SDebugFPSInfo() : fAverageFPS(0.0f), fMaxFPS(0.0f), fMinFPS(0.0f)
	{
	}
	float fAverageFPS;
	float fMinFPS;
	float fMaxFPS;
};

UNIQUE_IFACE struct IMaterialMergePipe
{
	virtual ~IMaterialMergePipe(){}
  enum EMergeMaterialsFlags
  {
    mfDefaut              = 0,
    mfDoNotMergeTextures  = BIT(0),
    mfOnlyUsedSpace       = BIT(1),// Use only used space from source texture
    mfUseDiffuseAtlas     = BIT(2),// Uses diffuse channel atlas to generate output textures (ie resizes all other channels to diffuse sizes)
    mfVerbose             = BIT(3),// Verbose
  };
  virtual int GetMaterialsCount() const = 0;
  virtual IMaterial* GetMaterial(int index) const = 0;
  virtual const char* GetTexmapName(int mtlIndex,int mtlSubsetIndex,int texmapIndex) = 0;
  // Used space
  virtual size_t GetTextureCoordinatesSubsets(IMaterial* mtl) = 0;
  virtual const Vec2* GetTextureCoordinates(int subset,IMaterial* mtl,int& stride,int& firstVertex,int& count) = 0;
  virtual const uint16* GetIndicies(int subset,IMaterial* mtl,int& stride,int& firstIndex,int& count) = 0;
};

UNIQUE_IFACE struct IMergePipeBackend
{
	virtual ~IMergePipeBackend(){}
  virtual void GetFaceVerticies(int chrIndex,int face,bool bIncludeMorphs,Vec3* vert123) const = 0;
};

UNIQUE_IFACE struct IMergePipe
{
public:
	virtual ~IMergePipe(){}
  enum EMergeFlags
  {
    mfDefault         = 0,
    mfPhysics         = BIT(0),
    mfSkin            = BIT(1),
    mfMergeMaterials  = BIT(2),// or use incoming
    mfMergeMorphs     = BIT(3),
    mfVerbose_LOD     = BIT(4),
  };
  enum ELodFlags
  {
    elfNothing =        0,
    elfHandleSplits =   BIT(0),
    elfHandleSkinning = BIT(1),// handle skinning using less accurate method
  };
public:
  virtual int GetModelsCount() const = 0;
  virtual const char* GetModelFilename(int index) const = 0;
  virtual IMaterial* GetModelMaterial(int index) const = 0;
  virtual float GetModelLodMaxAngle(int chrindex) const = 0;
	virtual IMergePipe::ELodFlags GetModelLodFlags(int chrindex) const = 0;
  virtual const char* GetModelLogPrefix(int chrindex) const = 0;
  // Summary:
  //	Sets merged material.
  virtual void SetMergedMaterial(IMaterial* pMergedMaterial) = 0;
  virtual const char* GetMaterialMergeTexmapName(int mtlIndex,int mtlSubsetIndex,int texmapIndex) = 0;
  virtual IMaterialMergePipe::EMergeMaterialsFlags GetMaterialMergeFlags() const = 0;
  // Summary:
  //	Returns the name of newly created node.
  virtual const char* GetModelName() const = 0;
  //
  struct SMergePhysicsBoneResult
  {
    SMergePhysicsBoneResult()
    {
      okToMerge = false;
      new_name = NULL;
      bOverrideB2W = false;
    }
    bool okToMerge;
    const char* new_name;
    bool bOverrideB2W;
    Matrix34 m_B2W;
  };
	virtual IMergePipe::SMergePhysicsBoneResult GetPhysicsBoneMergeInfo(int chrindex,const char* modelName,const char* boneName,IMergePipeBackend* info) const = 0;
  //
  virtual float GetMorphTargetWeight(int chrIndex,const char* targetName) const = 0;
};

struct IMergeMaterialsResult
{
	virtual ~IMergeMaterialsResult(){}
  virtual void Release() = 0;
};


struct IMergeCHRMorphsPipe
{
	virtual ~IMergeCHRMorphsPipe(){}
  virtual size_t getNumMorphs() const = 0;
  virtual const char* getMorphName(size_t index) const = 0;
  virtual float getMorphWeight(size_t index) const = 0;
};

struct IScreenshotCallback
{
	virtual ~IScreenshotCallback(){}
	virtual void SendParameters(void* data, uint32 width, uint32 height, f32 minx, f32 miny, f32 maxx, f32 maxy) = 0;
};

// Summary:
//     Interface to the 3d Engine.
UNIQUE_IFACE struct I3DEngine : public IProcess
{	
	
	DEVIRTUALIZATION_VTABLE_FIX

	// Summary:
	//     Initializes the 3D Engine.
	// See Also:
	//     ShutDown
	// Notes:
	//     Only call once, after creating the instance.
	virtual bool Init() = 0;

	// Summary:
	//     Sets the path used to load levels.
	// See Also:
	//     LoadLevel
	// Arguments:
	//     szFolderName - Should contains the folder to be used
	virtual void SetLevelPath( const char * szFolderName ) = 0;

	// Description:
	//     Will load a level from the folder specified with SetLevelPath. If a 
	//     level is already loaded, the resources will be deleted before.
	// See Also:
	//     SetLevelPath
	// Arguments:
	//     szFolderName - Name of the subfolder to load
	//     szMissionName - Name of the mission
	// Return Value:
	//     A boolean which indicate the result of the function; true is 
	//     succeed, or false if failed.
	// Summary:
	//     Load a level.
	virtual bool LoadLevel(const char * szFolderName, const char * szMissionName) = 0;
	virtual bool InitLevelForEditor(const char * szFolderName, const char * szMissionName) = 0;

	// Summary:
	//   Clears all rendering resources, all objects, characters and materials, voxels and terrain.
	// Notes:
	//   Should always be called before LoadLevel, and also before loading textures from a script.
	virtual void UnloadLevel()=0;

	// Description:
	//    Must be called after the game completely finishes loading the level.
	//    3D engine uses it to pre-cache some resources needed for rendering.
	// See Also:
	//    LoadLevel
	// Summary:
	//	  Pre-caches some resources need for rendering.
	virtual void PostLoadLevel() = 0;

	// Summary:
	//     Handles any work needed at start of new frame.
	// Notes:
	//     Should be called for every frame.
	virtual void OnFrameStart() = 0;

	// Summary:
	//     Updates the 3D Engine.
	// Notes:
	//     Should be called for every frame.
	virtual void Update() = 0;

	// Summary:
	//     Gets the current 3d engine camera.
	// Notes:
	//     Only valid during 3dengine::Render call, makes sense only for internal debugging
	virtual const CCamera & GetCurrentCamera() = 0;

	// Summary:
	//     Draws the world.
	// See Also:
	//     SetCamera
	// Arguments:
	//   szDebugName - name that can be visualized for debugging purpose, must not be 0
	virtual void RenderWorld(const int nRenderFlags, 
    const CCamera * pCameras, int nCamerasNum, 
    const char *szDebugName, 
    const int dwDrawFlags = -1, const int nFilterFlags=-1) = 0;

	// Summary:
	//     Draws the world for rendering into a texture.
	// Arguments:
	//     DrawFlags - Define what to draw, use any flags defined in EDrawLowDetailFlags
//	virtual void DrawLowDetail(const int & DrawFlags) = 0;

	// Summary:
	//     Shuts down the 3D Engine.
	virtual void ShutDown() = 0;

	// Summary:
	//     Deletes the 3D Engine instance.
	virtual void Release() = 0;

	// Summary:
	//     Loads a static object from a CGF file.
	// See Also:
	//     IStatObj
	// Arguments:
	//     szFileName - CGF Filename - should not be 0 or ""
	//     szGeomName - Optional name of geometry inside CGF.
	//     ppSubObject - [Out]Optional Out parameter,Pointer to the
	//		nLoadingFlags - Zero or a bitwise combination of the flags from ELoadingFlags,
	//										defined in IMaterial.h, under the interface IMaterialManager.
	// Return Value:
	//     A pointer to an object derived from IStatObj.
	virtual IStatObj* LoadStatObj( const char *szFileName,const char *szGeomName=NULL,/*[Out]*/IStatObj::SSubObject **ppSubObject=NULL, bool bUseStreaming=true,unsigned long nLoadingFlags=0) = 0;

	// Summary:
	//     Finds a static object created from the given filename
	// See Also:
	//     IStatObj
	// Arguments:
	//     szFileName - CGF Filename - should not be 0 or ""
	// Return Value:
	//     A pointer to an object derived from IStatObj.
	virtual IStatObj * FindStatObjectByFilename(const char * filename) = 0;

	virtual void SetCoverageBufferDepthReady() = 0;
	virtual void ResetCoverageBufferSignalVariables() = 0;
	virtual void DrawDeferredCoverageBufferDebug() = 0;

	// Summary:
	//     Gets the amount of loaded objects.
	// Return Value:
	//     An integer representing the amount of loaded objects.
	virtual int GetLoadedObjectCount() { return 0; }

	// Summary:
	//     Fills pObjectsArray with pointers to loaded static objects
	//     if pObjectsArray is NULL only fills nCount parameter with amount of loaded objects.
	virtual void GetLoadedStatObjArray( IStatObj** pObjectsArray,int &nCount ) = 0;

	struct SObjectsStreamingStatus
	{
		int nReady;
		int nInProgress;
		int nTotal;
		int nActive;
		int nAllocatedBytes;
		int nMemRequired;
		int nMeshPoolSize; // in MB
	};
	// Summary:
	//     Gets stats on streamed objects
  virtual void GetObjectsStreamingStatus(SObjectsStreamingStatus &outStatus) = 0;

	struct SStremaingBandwidthData
	{
		SStremaingBandwidthData()
		{
			memset(this, 0, sizeof(SStremaingBandwidthData));
		}
		float fBandwidthActual;
		float fBandwidthRequested;
	};
	enum eStreamingSubsystem
	{
		eStreamingSubsystem_Textures,
		eStreamingSubsystem_Objects,
		eStreamingSubsystem_Audio,
	};

	// Summary:
	//	Statistic Data requests
	//  returns the statistic for each type
	struct SStatisticData
	{
		SStatisticData()
		{
			memset(this,0,sizeof(SStatisticData));
		}

		float textureUpdates;
		float textureRequests;
		float textureRenders;
		float texturePoolMemUsage;
		float texturePoolMemRequired;
	};

	// Summary:
	//	Statistic Data requests
	//  returns the statistic for each type
	//Arguments: 
	//	statistic to gather
	//	outData value containing the data
	virtual void GetStatisticsData(SStatisticData &outData) = 0;

	// Summary:
	//		Gets stats on the streaming bandwidth requests from subsystems
	// Arguments:
	//		subsystem - the streaming subsystem we want bandwidth data for
	//		outData - structure containing the bandwidth data for the subsystem requested
	virtual void GetStreamingSubsystemData(EStreamTaskType subsystem, SStremaingBandwidthData &outData) = 0;

	// Summary:
	//     Registers an entity to be rendered.
	// Arguments:
	//     pEntity - The entity to render
	virtual void RegisterEntity( IRenderNode * pEntity, int nSID=-1 )=0;

	// Summary:
	//     Selects an entity for debugging.
	// Arguments:
	//     pEntity - The entity to render
	virtual void SelectEntity( IRenderNode * pEntity )=0;

	// Summary:
	//     Notices the 3D Engine to stop rendering a specified entity.
	// Arguments:
	//     pEntity - The entity to stop render
	virtual bool UnRegisterEntity( IRenderNode * pEntity )=0;

	// Summary:
	//	   Returns whether a world pos is under water.
	virtual bool IsUnderWater( const Vec3& vPos) const = 0;
  
	// Summary:
	//	   Returns whether ocean volume is visible or not.
	virtual void SetOceanRenderFlags( uint8 nFlags ) = 0;
	virtual uint8 GetOceanRenderFlags() const = 0;
	virtual uint32 GetOceanVisiblePixelsCount() const = 0;

	// Summary:
	//     Gets the closest walkable bottom z straight beneath the given reference position.
	// Notes:
	//     This function will take into account both the global terrain elevation and local voxel (or other solid walkable object).
	// Arguments:
	//     referencePos - Position from where to start searching downwards.
	//     maxRelevantDepth - Max depth caller is interested in relative to referencePos (for ray casting performance reasons).
	//     objtypes - expects physics entity flags.  Use this to specify what object types make a valid bottom for you.
	// Return Value:
	//     A float value which indicate the global world z of the bottom level beneath the referencePos.
	//     If the referencePos is below terrain but not inside any voxel area BOTTOM_LEVEL_UNKNOWN is returned.
	virtual float GetBottomLevel(const Vec3 & referencePos, float maxRelevantDepth, int objtypes) = 0;
	// A set of overloads for enabling users to use different sets of input params.  Basically, only
	// referencePos is mandatory.  The overloads as such don't need to be virtual but this seems to be
	// a purely virtual interface.
	virtual float GetBottomLevel(const Vec3 & referencePos, float maxRelevantDepth = 10.0f) = 0;
	virtual float GetBottomLevel(const Vec3 & referencePos, int objflags) = 0;

	// Summary:
	//     Gets the water level for a specified position.
	// Notes:
	//     This function will take into account both the global water level and any water volume present.
	//     Function is provided twice for performance with diff. arguments.
	// Arguments:
	//     pvPos - Desired position to inspect the water level
	//     pvFlowDir - Pointer to return the flow direction (optional)
	// Return Value:
	//     A float value which indicate the water level. In case no water was 
	//     found at the specified location, the value WATER_LEVEL_UNKNOWN will 
	//     be returned.
	virtual float GetWaterLevel(const Vec3 * pvPos, Vec3 * pvFlowDir = NULL) = 0;
	virtual float GetWaterLevel() = 0;

	// Summary:
	//     Gets the ocean water level for a specified position.
	// Notes:
	//     This function only takes into account ocean water.
	// Arguments:
	//     pCurrPos - Position to check water level
	// Return Value:
	//     A float value which indicate the water level.
	virtual float GetOceanWaterLevel( const Vec3 &pCurrPos ) const = 0;

	// Summary:
	//     Gets caustics parameters.
	// Return Value:
	//     A Vec4 value which constains:
	//     x = unused, y = distance attenuation, z = caustics multiplier, w = caustics darkening multiplier  
	virtual Vec4 GetCausticsParams() const = 0;  

  // Summary:
  //     Gets ocean animation caustics parameters.
  // Return Value:
  //     A Vec4 value which constains:
  //     x = unused, y = height, z = depth, w = intensity
  virtual Vec4 GetOceanAnimationCausticsParams() const = 0;  


	// Summary:
	//     Gets ocean animation parameters.
	// Return Value:
	//     2 Vec4 which constain:
	//     0: x = ocean wind direction, y = wind speed, z = waves speed, w = waves amount  
	//     1: x = waves size, y = free, z = free, w = free  

	virtual void GetOceanAnimationParams(Vec4 &pParams0, Vec4 &pParams1 ) const = 0;  

	// Summary:
	//     Gets HDR setup parameters.
	// Return Value:
	virtual void GetHDRSetupParams	(Vec4 pParams[5]) const = 0; 

	// Summary:
	//     Removes all particles and decals from the world.
	virtual void ResetParticlesAndDecals( ) = 0;

	// Summary:
	//     Creates new decals on the walls, static objects, terrain and entities.
	// Arguments:
	//     Decal - Structure describing the decal effect to be applied
	virtual void CreateDecal( const CryEngineDecalInfo & Decal )=0;

	// Summary:
	//     Removes decals in a specified range.
	// Arguments:
	//     vAreaBox - Specify the area in which the decals will be removed
	//     pEntity  - if not NULL will only delete decals attached to this entity
	virtual void DeleteDecalsInRange( AABB * pAreaBox, IRenderNode * pEntity ) = 0;

	//DOC-IGNORE-BEGIN
	  // Summary:
	  //	Renders far trees/objects as sprites.
	  // Description:
	  //	It's a call back for renderer. It renders far trees/objects as sprites. 
	  // Notes:
	  //	Will be removed from here.
	  virtual void DrawFarTrees() = 0; // used by renderer

	  virtual void GenerateFarTrees() = 0; // used by renderer

	//DOC-IGNORE-END

	// Summary:
	//     Sets the current outdoor ambient color. 
	virtual void SetSkyColor(Vec3 vColor)=0;

	// Summary:
	//     Sets the current sun color.
	virtual void SetSunColor(Vec3 vColor)=0;

	// Summary:
	//     Sets the current sky brightening multiplier.
	virtual void SetSkyBrightness(float fMul) = 0;

	// Summary:
	//     Gets the current sun/sky color relation.
	virtual float GetSunRel() const = 0;

	// Summary:
	//     Sets current rain parameters.
	virtual void SetRainParams(const Vec3 & vCenter, float fRadius, float fAmount, const Vec3 & vColor) = 0;

	// Summary:
	//     Sets additional rain parameters.
	virtual void SetRainParams(float fReflAmount, float fFakeGlossiness, float fPuddlesAmount, bool bRainDrops, float fRainDropsSpeed, float fUmbrellaRadius) = 0;

	// Summary:
	//     Gets the validity and fills current rain parameters.
	virtual bool GetRainParams(Vec3 & vCenter, float & fRadius, float & fAmount, Vec3 & vColor) = 0;

	// Summary:
	//     Gets additional rain parameters.
	virtual void GetRainParams(float & fReflAmount, float & fFakeGlossiness, float & fPuddlesAmount, bool & bRainDrops, float & fRainDropsSpeed, float & fUmbrellaRadius) const = 0;

	// Summary:
	//     Sets the view distance scale.
	// Arguments:
	//     fScale - may be between 0 and 1, 1.f = Unmodified view distance set by level designer, value of 0.5 will reduce it twice
  // Notes:
  //     This value will be reset automatically to 1 on next level loading.
  virtual void SetMaxViewDistanceScale(float fScale) = 0;

	// Summary:
	//     Gets the view distance.
	// Return Value:
	//     A float value representing the maximum view distance.
	virtual float GetMaxViewDistance( bool bScaled = true ) = 0;

	// Summary:
	//     Sets the fog color.
	virtual void SetFogColor(const Vec3& vFogColor)=0;

	// Summary:
	//     Gets the fog color.
	virtual Vec3 GetFogColor( )=0;
	
	// Summary:
	//   Gets volumetric fog settings.
	virtual void GetVolumetricFogSettings( float& globalDensity, float& atmosphereHeight, float& artistTweakDensityOffset, float& globalDensityMultiplierLDR ) = 0;
	
	// Summary:
	//   Sets volumetric fog settings.
	virtual void SetVolumetricFogSettings( float globalDensity, float atmosphereHeight, float artistTweakDensityOffset ) = 0;

	// Summary:
	//   Gets volumetric fog modifiers.
	virtual void GetVolumetricFogModifiers(float& globalDensityModifier, float& atmosphereHeightModifier) = 0;

	// Summary:
	//   Sets volumetric fog modifiers.
	virtual void SetVolumetricFogModifiers(float globalDensityModifier, float atmosphereHeightModifier, bool reset = false) = 0;

	// Summary:
	//   Gets various sky light parameters.
	virtual void GetSkyLightParameters( Vec3& sunIntensity, float& Km, float& Kr, float& g, Vec3& rgbWaveLengths ) = 0;

	// Summary:
	//   Sets various sky light parameters.
	virtual void SetSkyLightParameters( const Vec3& sunIntensity, float Km, float Kr, float g, 
		const Vec3& rgbWaveLengths, bool forceImmediateUpdate = false ) = 0;
	
	virtual void GetCloudShadingMultiplier( float& sunLightMultiplier, float& skyLightMultiplier ) = 0;
	virtual void SetCloudShadingMultiplier( float sunLightMultiplier, float skyLightMultiplier ) = 0;

	virtual IAutoCubeMapRenderNode* GetClosestAutoCubeMap(const Vec3& p) = 0;

	// Return Value:
	//   In the range 0.01 .. 100
	virtual float GetHDRDynamicMultiplier() const = 0;
	// Arguments:
	//   value - becomes clamped in range 0.01 .. 100
	virtual void SetHDRDynamicMultiplier( const float value ) = 0;

  virtual void SetRenderIntoShadowmap( bool bSet ) = 0;
  virtual bool GetRenderIntoShadowmap() const = 0;

	// allows to modify material on render nodes at run-time (make sure it is properly restored back)
	virtual void SetRenderNodeMaterialAtPosition( EERType eNodeType, const Vec3 & vPos, IMaterial * pMat ) = 0;

	// begin streaming of meshes and textures for specified position, pre-cache stops after fTimeOut seconds
	virtual void SetPrecachePoint( const Vec3 vPos, float fTimeOut = 3.f) = 0;

	virtual void TraceFogVolumes( const Vec3& worldPos, ColorF& fogVolumeContrib ) = 0;
	
	// Summary:
	//     Gets the interpolated terrain elevation for a specified location.
	// Notes:
	//     All x,y values are valid.
	// Arguments:
	//     x						- X coordinate of the location
	//     y						- Y coordinate of the location
	//	   bIncludeOutdoorVoxles	-
	// Return Value:
	//     A float which indicate the elevation level.
	virtual float GetTerrainElevation(float x, float y, bool bIncludeOutdoorVoxles = false) = 0;

	// Summary:
	//     Gets the terrain elevation for a specified location.
	// Notes:
	//     Only values between 0 and WORLD_SIZE.
	// Arguments:
	//     x - X coordinate of the location
	//     y - Y coordinate of the location
	// Return Value:
	//     A float which indicate the elevation level.
	virtual float GetTerrainZ(int x, int y) = 0;

  // Summary:
  //     Gets the terrain hole flag for a specified location.
  // Notes:
  //     Only values between 0 and WORLD_SIZE.
  // Arguments:
  //     x - X coordinate of the location
  //     y - Y coordinate of the location
  // Return Value:
  //     A bool which indicate is there hole or not.
  virtual bool GetTerrainHole(int x, int y) = 0;

  // Summary:
  //     Gets the terrain surface normal for a specified location.
  // Arguments:
  //     vPos.x - X coordinate of the location
  //     vPos.y - Y coordinate of the location
  //     vPos.z - ignored
  // Return Value:
  //     A terrain surface normal.
  virtual Vec3 GetTerrainSurfaceNormal(Vec3 vPos) = 0;

	// Summary:
	//     Gets the unit size of the terrain
	// Notes:
	//     The value should currently be 2.
	// Return Value:
	//     A int value representing the terrain unit size in meters.
	virtual int   GetHeightMapUnitSize() = 0;

	// Summary:
	//     Gets the size of the terrain
	// Notes:
	//     The value should be 2048 by default.
	// Return Value:
	//     An int representing the terrain size in meters.
	virtual int   GetTerrainSize()=0;

	// Summary:
	//     Gets the size of the terrain sectors
	// Notes:
	//     The value should be 64 by default.
	// Return Value:
	//     An int representing the size of a sector in meters.
	virtual int   GetTerrainSectorSize()=0;

//DOC-IGNORE-BEGIN

	// Internal functions, mostly used by the editor, which won't be documented for now

	// Summary:
	//		Places object at specified position (for editor)
//	virtual bool AddStaticObject(int nObjectID, const Vec3 & vPos, const float fScale, unsigned char ucBright=255) = 0;
	// Summary:
	//		Removes static object from specified position (for editor)
//	virtual bool RemoveStaticObject(int nObjectID, const Vec3 & vPos) = 0;
	// Summary:
	//		On-demand physicalization of a static object
//	virtual bool PhysicalizeStaticObject(void *pForeignData,int iForeignData,int iForeignFlags) = 0;
	// Summary:
	//		Removes all static objects on the map (for editor)
	virtual void RemoveAllStaticObjects(int nSID=0) = 0;
	// Summary:
	//		Allows to set terrain surface type id for specified point in the map (for editor)
	virtual void SetTerrainSurfaceType(int x, int y, int nType)=0; // from 0 to 6 - sur type ( 7 = hole )
  
	// Summary:
	//		Returns true if game modified terrain hight map since last update by editor
	virtual bool IsTerrainHightMapModifiedByGame() = 0;
	// Summary:
	//		Updates hight map max height (in meters)
	virtual void SetHeightMapMaxHeight(float fMaxHeight) = 0;
    
	// Summary:
	//		Sets terrain sector texture id, and disable streaming on this sector
	virtual void SetTerrainSectorTexture( const int nTexSectorX, const int nTexSectorY, unsigned int textureId ) = 0;

	// Summary:
	//		Returns size of smallest terrain texture node (last leaf) in meters
	virtual int GetTerrainTextureNodeSizeMeters() = 0;

	// Arguments:
	//   nLayer - 0=diffuse texture, 1=occlusionmap
	// Return value:
	//   an integer value representing the size of terrain texture node in pixels
	virtual int GetTerrainTextureNodeSizePixels(int nLayer) = 0;

	// Summary:
	//		Sets group parameters
	virtual bool SetStatInstGroup(int nGroupId, const IStatInstGroup & siGroup, int nSID=0) = 0;

	// Summary:
	//		Gets group parameters
	virtual bool GetStatInstGroup(int nGroupId, IStatInstGroup & siGroup, int nSID=0) = 0;

	// Summary:
	//		Sets burbed out flag
	virtual void SetTerrainBurnedOut(int x, int y, bool bBurnedOut) = 0;
	
	// Summary:
	//		Gets burbed out flag
	virtual bool IsTerrainBurnedOut(int x, int y) = 0;

//DOC-IGNORE-END

	// Summary:
	//   Notifies of an explosion, and maybe creates an hole in the terrain
	// Description:
	//   This function should usually make sure that no static objects are near before making the hole.
	// Arguments:
	//   vPos - Position of the explosion
	//   fRadius - Radius of the explosion
	//   bDeformTerrain - Allow to deform the terrain
	virtual void OnExplosion(Vec3 vPos, float fRadius, bool bDeformTerrain = true) = 0;

	// Summary:
	//   Sets the physics material enumerator
	// Arguments:
	//   pPhysMaterialEnumerator - The physics material enumarator to set
	virtual void SetPhysMaterialEnumerator(IPhysMaterialEnumerator * pPhysMaterialEnumerator) = 0;

	// Summary:
	//   Gets the physics material enumerator
	// Return Value:
	//   A pointer to an IPhysMaterialEnumerator derived object.
	virtual IPhysMaterialEnumerator * GetPhysMaterialEnumerator() = 0;

//DOC-IGNORE-BEGIN
//Internal functions
	
	// Summary:
	//	 Allows to enable fog in editor
	virtual void SetupDistanceFog() = 0;

	// Summary:
	//	 Loads environment settings for specified mission
	virtual void LoadMissionDataFromXMLNode(const char * szMissionName) = 0;

	virtual void LoadEnvironmentSettingsFromXML(XmlNodeRef pInputNode, int nSID=0) = 0;

	// Summary:
	//	 Loads detail texture and detail object settings from XML doc (load from current LevelData.xml if pDoc is 0)
	virtual void	LoadTerrainSurfacesFromXML(XmlNodeRef pDoc, bool bUpdateTerrain, int nSID=0) = 0;

//DOC-IGNORE-END

	// Description:
	//   Physics applied to the area will apply to vegetations and allow it to move/blend.
	// Arguments:
	//   vPos - Center position to apply physics
	//   fRadius - Radius which specify the size of the area to apply physics
	//   fAmountOfForce - The amount of force, should be at least of 1.0f
	// Summary:
	//   Applies physics in a specified area
	virtual void ApplyForceToEnvironment(Vec3 vPos, float fRadius, float fAmountOfForce) = 0;

//DOC-IGNORE-BEGIN
	// Set direction to the sun
//	virtual void SetSunDir( const Vec3& vNewSunDir ) = 0;

	// Summary:
	//		Return non-normalized direction to the sun
	virtual Vec3 GetSunDir()  const= 0;

	// Summary:
	//		Return normalized direction to the sun
	virtual Vec3 GetSunDirNormalized()  const= 0;

	// Summary:
	//		Return realtime (updated every frame with real sun position) normalized direction to the scene
	virtual Vec3 GetRealtimeSunDirNormalized()  const= 0;

// Internal function used by the 3d engine
	// Summary:
	//		Returns lighting level for this point.
	virtual Vec3 GetAmbientColorFromPosition(const Vec3 & vPos, float fRadius=1.f) = 0;

//Internal function used by 3d engine and renderer
	// Summary:
	//		Gets distance to the sector containig ocean water
	virtual float GetDistanceToSectorWithWater() = 0;
//DOC-IGNORE-END

	// Summary:
	//   Gets the environment ambient color.
	// Notes:
	//   Should have been specified in the editor.
	// Return Value:
	//   An rgb value contained in a Vec3 object.
	virtual Vec3 GetSkyColor() const = 0;

	// Summary:
	//   Gets the sun color
	// Notes:
	//   Should have been specified in the editor.
	// Return Value:
	//   An rgb value contained in a Vec3 object.
	virtual Vec3 GetSunColor() const = 0;

	// Summary:
	//   Retrieves the current sky brightening multiplier
	// Notes:
	// Return Value:
	//   Scalar value
	virtual float GetSkyBrightness() const = 0;

  // Summary:
  //   Retrieves the current SSAO multiplier
  // Notes:
  // Return Value:
  //   scalar value
  virtual float GetSSAOAmount() const = 0;

	// Summary:
	//   Retrieves the current SSAO contrast multiplier
	// Notes:
	// Return Value:
	//   scalar value
	virtual float GetSSAOContrast() const = 0;

	// Summary:
	//   Retrieves the current GI multiplier
	// Notes:
	// Return Value:
	//   scalar value
	virtual float GetGIAmount() const = 0;

  // Summary:
  //   Returns terrain texture multiplier.
  // Return Value:
  //   Scalar value
	virtual float GetTerrainTextureMultiplier(int nSID=0) const = 0;

	//  check object visibility taking into account portals and terrain occlusion test
	//  virtual bool IsBoxVisibleOnTheScreen(const Vec3 & vBoxMin, const Vec3 & vBoxMax, OcclusionTestClient * pOcclusionTestClient = NULL)=0;
	//  check object visibility taking into account portals and terrain occlusion test
	//  virtual bool IsSphereVisibleOnTheScreen(const Vec3 & vPos, const float fRadius, OcclusionTestClient * pOcclusionTestClient = NULL)=0;

//mat: todo

	// Summary:
	//	 Frees entity render info.
	virtual void FreeRenderNodeState(IRenderNode * pEntity) = 0;

	// Summary:
	//   Adds the level's path to a specified filename.
	// Arguments:
	//   szFileName - The filename for which we need to add the path
	// Return Value:
	//   Full path for the filename; including the level path and the filename appended after.
	virtual const char * GetLevelFilePath(const char * szFileName) = 0;

	// Summary:
	//   Displays statistic on the 3d Engine.
	// Arguments:
	//   fTextPosX - X position for the text
	//   fTextPosY - Y position for the text
	//   fTextStepY - Amount of pixels to distance each line
	//   bEnhanced - false=normal, true=more interesting information
	virtual void DisplayInfo(float & fTextPosX, float & fTextPosY, float & fTextStepY, const bool bEnhanced ) = 0;

	// Summary:
	//	 Draws text right aligned at the y pixel precision.
	virtual void DrawTextRightAligned( const float x, const float y, const char * format, ...) PRINTF_PARAMS(4, 5) =0;
	virtual void DrawTextRightAligned( const float x, const float y, const float scale,const ColorF &color,const char * format, ...) PRINTF_PARAMS(6, 7) =0;

	// Summary:
	//   Enables or disables portal at a specified position.
	// Arguments:
	//   vPos - Position to place the portal
	//   bActivate - Set to true in order to enable the portal, or to false to disable
	//   szEntityName -
	virtual void ActivatePortal(const Vec3 &vPos, bool bActivate, const char * szEntityName) = 0;

//DOC-IGNORE-BEGIN
	// Summary:
	//	 Counts memory usage
	virtual void GetMemoryUsage(ICrySizer * pSizer) const =0;

	// Summary:
	//	 Counts resource memory usage
	// Arguments:
	//		cstAABB - Use the whole level AABB if you want to grab the resources
	//              from the whole level.  For height level, use something BIG 
	//							(ie: +-FLT_MAX)
	// See also:
	//		ITerrain::GetTerrainSize().
	virtual void GetResourceMemoryUsage(ICrySizer * pSizer,const AABB& cstAABB)=0;
//DOC-IGNORE-END

	// Summary:
	//   Creates a new VisArea.
	// Return Value:
	//   A pointer to a newly created VisArea object
	virtual IVisArea * CreateVisArea() = 0;

	// Summary:
	//   Deletes a VisArea.
	// Arguments:
	//   pVisArea - A pointer to the VisArea to delete
	virtual void DeleteVisArea(IVisArea * pVisArea) = 0;

//mat: todo

	// Summary:
	//   Updates the VisArea
	// Arguments:
	//   pArea -
	//   pPoints - 
	//   nCount -
	//   szName -
	//	 info	-
	//	 bReregisterObjects -
	virtual void UpdateVisArea(IVisArea * pArea, const Vec3 * pPoints, int nCount, const char * szName, 
    const SVisAreaInfo & info, bool bReregisterObjects) = 0;

	// Summary:
	//   Determines if two VisAreas are connected.
	// Description:
	//   Used to determine if a sound is potentially hearable between two VisAreas.
	// Arguments:
	//   pArea1 - A pointer to a VisArea
	//   pArea2 - A pointer to a VisArea
	//   nMaxRecursion - Maximum number of recursions to be done
	//   bSkipDisabledPortals - Indicate if disabled portals should be skipped
	// Return Value:
	//   A boolean value set to true if the two VisAreas are connected, else false will be returned.
	virtual bool IsVisAreasConnected(IVisArea * pArea1, IVisArea * pArea2, int nMaxRecursion = 1, bool bSkipDisabledPortals = true) = 0;

//mat: todo

	// Summary:
	//   Creates instance of IRenderNode object with specified type.
	virtual IRenderNode* CreateRenderNode( EERType type ) = 0;

	// Summary:
	//	 Delete RenderNode object.
	virtual void DeleteRenderNode(IRenderNode * pRenderNode) = 0;

	// Summary:
	//	 Set global wind vector.
	virtual void SetWind( const Vec3 & vWind ) = 0;

	// Summary:
	//	 Gets wind direction and force, averaged within a box.
	virtual Vec3 GetWind( const AABB & box, bool bIndoors ) const =0;

	// Description:
	//   Gets the VisArea which is present at a specified point.
	// Arguments:
	//	 vPos: 
	// Return Value:
	//	 VisArea containing point, if any. 0 otherwise.
	virtual	IVisArea * GetVisAreaFromPos(const Vec3 &vPos) = 0;	

	// Description:
	//   Tests for intersection against Vis Areas.
	// Arguments:
	//   box: Volume to test for intersection.
	//   pNodeCache (out, optional): Set to a cached pointer, for quicker calls to ClipToVisAreas.
	// Return Value:
	//	 Whether box intersects any vis areas.
	virtual	bool IntersectsVisAreas(const AABB& box, void** pNodeCache = 0) = 0;	

	// Description:
	//   Clips geometry against the boundaries of VisAreas.
	// Arguments:
	//   pInside: Vis Area to clip inside of. If 0, clip outside all Vis Areas.
	//	 sphere -
	//	 vNormal -
	//	 pNodeChache -
	// Return Value:
	//	 Whether it was clipped
	virtual	bool ClipToVisAreas(IVisArea* pInside, Sphere& sphere, Vec3 const& vNormal, void* pNodeCache = 0) = 0;	

	// Summary:
	//   Enables or disables ocean rendering.
	// Arguments:
	//   bOcean - Will enable or disable the rendering of ocean
	virtual void EnableOceanRendering(bool bOcean) = 0;

	// Description:
	//    Retrieve pointer to the material manager interface.
	virtual IMaterialManager* GetMaterialManager() = 0;
	
	// Summary:
	//   Creates a new light source.
	// Return Value:
	//   Pointer to newly created light or -1 if it fails.
	virtual struct ILightSource * CreateLightSource() = 0;

	// Summary:
	//   Deletes a light.
	// Arguments:
	//   Pointer to the light
	virtual void DeleteLightSource(ILightSource * pLightSource) = 0;

	// Summary:
	//   Gives access to the list holding all static light sources
	// Return Value:
	//   An array holding all the CDLight pointers.
	virtual const PodArray<CDLight*> * GetStaticLightSources() = 0;
	virtual const PodArray<ILightSource*> * GetLightEntities() = 0;

	// Summary:
	//   Reload the heightmap.
	// Description:
	//   Reloading the heightmap will resets all decals and particles.
	// Notes:
	//	 In future will restore deleted vegetations
	// Returns:
	//   success
  virtual bool RestoreTerrainFromDisk(int nSID=0) = 0;

//DOC-IGNORE-BEGIN
  // tmp
  virtual const char * GetFilePath(const char * szFileName) { return GetLevelFilePath(szFileName); }
//DOC-IGNORE-END

	// Summary:
	//	 Post processing effects interfaces.
	virtual void SetPostEffectParam(const char *pParam, float fValue, bool bForceValue=false) const = 0;
	virtual void SetPostEffectParamVec4(const char *pParam, const Vec4 &pValue, bool bForceValue=false) const = 0;
	virtual void SetPostEffectParamString(const char *pParam, const char *pszArg) const = 0;

	virtual void GetPostEffectParam(const char *pParam, float &fValue) const = 0;  
	virtual void GetPostEffectParamVec4(const char *pParam, Vec4 &pValue) const = 0;  
	virtual void GetPostEffectParamString(const char *pParam, const char *pszArg) const = 0;

	virtual int32 GetPostEffectID(const char* pPostEffectName) = 0;

	virtual void ResetPostEffects() const = 0;

	virtual void SetShadowsGSMCache(bool bCache) = 0;
    
	// Summary:
	//	 Physicalizes area if not physicalized yet.
	virtual void CheckPhysicalized(const Vec3 & vBoxMin, const Vec3 & vBoxMax) = 0;

	// Summary:
	//	 In debug mode check memory heap and makes assert, do nothing in release
	virtual void CheckMemoryHeap() = 0;

	// Summary:
	//	 Sets material parameter.
	// Arguments:
	//	 szMatName - materal name
	//	 nTexSlot  - text slot id, see EFTT_DIFFUSE for example
	//	 nSubMatId - submaterial id, -1 means use root material
	//	 szParamName - can be one of:
		
	//		m_eTGType
	//		m_eRotType
	//		m_eUMoveType
	//		m_eVMoveType
	//		m_bTexGenProjected

	//		m_Tiling[0]
	//		m_Tiling[1]
    //		m_Tiling[2]
	//		m_Offs[0]
	//		m_Offs[1]
	//		m_Offs[2]	

	//		m_Rot[0]
	//		m_Rot[1]
	//		m_Rot[2]
	//		m_RotOscRate[0]
	//		m_RotOscRate[1]
	//		m_RotOscRate[2]
	//		m_RotOscAmplitude[0]
	//		m_RotOscAmplitude[1]
	//		m_RotOscAmplitude[2]
	//		m_RotOscPhase[0]
	//		m_RotOscPhase[1]
	//		m_RotOscPhase[2]

	//		m_UOscRate
	//		m_VOscRate
	//		m_UOscAmplitude
	//		m_VOscAmplitude
	//		m_UOscPhase
	//		m_VOscPhase

	//	 fValue - new value
	virtual bool SetMaterialFloat( char * szMatName, int nSubMatId, int nTexSlot, char * szParamName, float fValue ) = 0;

	// Summary:
	//	 Closes terrain texture file handle and allows to replace/update it.
	virtual void CloseTerrainTextureFile(int nSID=0) = 0;
	
	// Summary:
	//	 Removes all decals attached to specified entity.
	virtual void DeleteEntityDecals(IRenderNode * pEntity) = 0;

	// Summary:
	//	 Finishes objects geometery generation/loading.
	virtual void CompleteObjectsGeometry() = 0;

	// Summary:
	//	 Disables CGFs unloading.
	virtual void LockCGFResources() = 0;

	// Summary:
	//	 Enables CGFs unloading (this is default state), this function will also release all not used CGF's.
	virtual void UnlockCGFResources() = 0;

	// Summary:
	//	 Gives access to materials library.
	virtual class CMatMan * GetMatMan() = 0;

	// Summary:
	//	 Creates static object containing empty IndexedMesh.
	virtual IStatObj * CreateStatObj() = 0;

	// Summary:
	//	 Creates the instance of the indexed mesh.
	virtual IIndexedMesh* CreateIndexedMesh() = 0;

	// Summary:
	//	 Paints voxel shape.
	virtual void Voxel_Paint(Vec3 vPos, float fRadius, int nSurfaceTypeId, Vec3 vBaseColor, EVoxelEditOperation eOperation, EVoxelBrushShape eShape, EVoxelEditTarget eTarget, PodArray<IRenderNode*> * pBrushes, float fMinVoxelSize) = 0;

	// Description:
	//	 Gets list of voxel objects that will be affected by paint operation, IMemoryBlock will contain array of IVoxelObject pointers.
	virtual IMemoryBlock * Voxel_GetObjects(Vec3 vPos, float fRadius, int nSurfaceTypeId, EVoxelEditOperation eOperation, EVoxelBrushShape eShape, EVoxelEditTarget eTarget) = 0;

	// Summary:
	//	Setups voxel flags.
	virtual void Voxel_SetFlags(bool bPhysics, bool bSimplify, bool bShadows, bool bMaterials) = 0;

	// Summary:
	//	 Updates rendering mesh in the stat obj associated with pPhysGeom. 
	// Notes:
	//	Creates or clones the object if necessary.
	virtual IStatObj * UpdateDeformableStatObj(IGeometry *pPhysGeom, bop_meshupdate *pLastUpdate=0, IFoliage *pSrcFoliage=0) = 0;

	// Summary:
	//	 Saves/loads state of engine objects.
	virtual void SerializeState( TSerialize ser ) = 0;
	
	// Summary:
	//	 Cleanups after save/load.	
	virtual void PostSerialize( bool bReading ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// CGF Loader.
	//////////////////////////////////////////////////////////////////////////
  // Description:
	// Creates a chunkfile content instance
  // Returns 'NULL' if the memory for the instance could not be allocated
  virtual CContentCGF* CreateChunkfileContent( const char *filename ) = 0;

  // Description:
	// Deletes the chunkfile content instance
  virtual void ReleaseChunkfileContent( CContentCGF* ) = 0;

  // Description:
	// Loads the contents of a chunkfile into the given CContentCGF. 
  // Returns 'false' on error.
	virtual bool LoadChunkFileContent( CContentCGF* pCGF, const char *filename,bool bNoWarningMode=false ) = 0;

	// Description:
	//    Creates ChunkFile.
	virtual IChunkFile * CreateChunkFile( bool bReadOnly=false ) = 0;

	// Description:
	//	  Returns interface to terrain engine
	virtual ITerrain * GetITerrain() = 0;

	// Description:
	//	  Creates terrain engine
	virtual ITerrain * CreateTerrain(const STerrainInfo & TerrainInfo) = 0;

	// Description:
	//	  Deletes terrain
	virtual void DeleteTerrain() = 0;

	// Description:
	//	  Creates 3d terrain engine
	virtual IVoxTerrain * GetIVoxTerrain() = 0;
	virtual IVoxTerrain * CreateVoxTerrain(const SVoxTerrainInfo & info) = 0;
	virtual void DeleteVoxTerrain() = 0;

	// Description:
	//	  Returns interface to visarea manager.
	virtual IVisAreaManager * GetIVisAreaManager() = 0;

	// Description:
	//	  Callback function used by renderer for imposters creation.
//	virtual void RenderImposterContent(class CREImposter * pImposter, const CCamera & cam) = 0;

	// Description:
	//	  Returns array of voxel render meshes, nCount will get number of elements written.
	virtual void GetVoxelRenderNodes(IRenderNode**pRenderNodes, int & nCount) = 0;
   
	// Description:
	//	  Returns amount of light affecting a point in space inside a specific range (0 means no light affecting,
	//	  1 is completely affected by light). Use accurate parameter for a more expensive but with higher accuracy computation.
	virtual float GetLightAmountInRange(const Vec3 &pPos, float fRange, bool bAccurate = 0) = 0;

	// Description:
	//	  Places camera into every visarea or every manually set pre-cache points and render the scenes.
	virtual void PrecacheLevel(bool bPrecacheAllVisAreas, Vec3 * pPrecachePoints, int nPrecachePointsNum) = 0;

	// Description:
	//	  Proposes 3dengine to load on next frame all shaders and textures synchronously.
	virtual void ProposeContentPrecache() = 0;

	// Description:
	//	  Returns TOD interface.
	virtual ITimeOfDay* GetTimeOfDay() = 0;

	// Description:
	//	  Returns SkyBox material.
	virtual IMaterial* GetSkyMaterial() = 0;

	// Description:
	//    Sets global 3d engine parameter.
	virtual void SetGlobalParameter( E3DEngineParameter param,const Vec3 &v ) = 0;
	void SetGlobalParameter( E3DEngineParameter param,float val ) { SetGlobalParameter( param,Vec3(val,0,0) ); };

	// Description:
	//    Retrieves global 3d engine parameter.
	virtual void GetGlobalParameter( E3DEngineParameter param,Vec3 &v ) = 0;
	float GetGlobalParameter( E3DEngineParameter param ) { Vec3 v(0,0,0); GetGlobalParameter(param,v); return v.x; };

	// Description:
	//		Saves pStatObj to a stream. 
	// Notes:
	//		Full mesh for generated ones, path/geom otherwise
	virtual int SaveStatObj(IStatObj *pStatObj, TSerialize ser) = 0;
	// Description:
	//		Loads statobj from a stream
	virtual IStatObj *LoadStatObj(TSerialize ser) = 0;

	// Description:
	//		Returns true if input line segment intersect clouds sprites.
	virtual bool CheckIntersectClouds(const Vec3 & p1, const Vec3 & p2) = 0;

	// Description:
	//		Removes references to RenderMesh
	virtual void OnRenderMeshDeleted(IRenderMesh * pRenderMesh) = 0;

	// Description:
	//	 Used to visualize frustum updates
	// Arguments:
	//   szName		- must not be 0
	//	 rCam		-
	//	 col		-
	//	 fQuadDist	-
	virtual void DebugDraw_PushFrustrum( const char *szName, const CCamera &rCam, const ColorB col, const float fQuadDist=-1.0f ) = 0;

	// Description:
	//	 Used to visualize frustum updates
	// Arguments:
	//   szName - must not be 0
	virtual void DebugDraw_PushFrustrum( const char *szName, const CRenderCamera &rCam, const ColorB col, const float fQuadDist=-1.0f ) = 0;

	// Description:
	//	 Used to highlight an object under the reticule
	virtual void DebugDraw_UpdateDebugNode() = 0;

	// Description:
	//	 Used by editor during AO computations
	virtual bool RayObjectsIntersection2D( Vec3 vStart, Vec3 vEnd, Vec3 & vHitPoint, EERType eERType ) = 0;


	// Description:
	//    Used by editor during object alignment
	virtual bool RenderMeshRayIntersection( IRenderMesh *pRenderMesh, SRayHitInfo &hitInfo,IMaterial *pCustomMtl=0 ) = 0;


	// Description:
	//	 Frees lod transition state
	virtual void FreeRNTmpData(CRNTmpData ** ppInfo) = 0;

	// Return Value:
	//	 True if e_ambient_occlusion is ON and AO data is valid
	virtual bool IsAmbientOcclusionEnabled() = 0;

	// Description:
	//	 Call function 2 times (first to get the size then to fill in the data)
	// Arguments:
	//   pObjects - 0 if only the count is required
	// Return Value:
	//   Count returned
	virtual uint32 GetObjectsByType( EERType objType, IRenderNode **pObjects=0 )=0;
	virtual uint32 GetObjectsByTypeInBox( EERType objType, const AABB &bbox, IRenderNode **pObjects=0 ) = 0;
	virtual uint32 GetObjectsInBox(const AABB &bbox, IRenderNode **pObjects=0 ) =0 ;

	virtual void FillDebugFPSInfo(SDebugFPSInfo&) = 0;

	virtual void SetTerrainLayerBaseTextureData(int nLayerId, byte*pImage, int nDim, const char * nImgFileName, IMaterial * pMat, float fBr, float fTiling, int nDetailSurfTypeId, float fTilingDetail, float fSpecularAmount, float fSortOrder, ColorF layerFilterColor, float fUseRemeshing, bool bShowSelection) = 0;

	virtual bool IsAreaActivationInUse() = 0;

	virtual void RenderRenderNode(IShadowCaster * pRNode, SRenderObjectModifier * pROModifier) = 0;

	// Arguments:
	//	 srcfilename -
	//	 filename	 -
	//	 pipe		 -
	//	 bSaveMorps  - saves all other morphs that are not in morphs list
	virtual bool SaveCHR(const char* srcfilename,const char* filename,IMergeCHRMorphsPipe* pipe,bool bSaveMorps=false) = 0;







	virtual bool MergeCHRs(const char* basechr,IMergePipe* pipe,const char* result_filename,IMergeMaterialsResult**,IMergePipe::EMergeFlags flags) = 0;

	// Description:
	//	 Returns true if materials can be merged in to one
	virtual bool CanBeMerged(IMaterial* mt1,IMaterial* mt2,bool bVerbose,IMaterialMergePipe::EMergeMaterialsFlags flags) = 0;
	virtual IMergeMaterialsResult* MergeMaterials(IMaterialMergePipe* pipe,IMaterialMergePipe::EMergeMaterialsFlags flags) = 0;

	virtual const char * GetVoxelEditOperationName(EVoxelEditOperation eOperation) = 0;

  // Summary:
  //   Gives 3dengine access to original and most precise heighmap data in the editor
  virtual void SetGetLayerIdAtCallback(IGetLayerIdAtCallback * pCallBack) = 0;

	virtual PodArray<CDLight*>* GetDynamicLightSources() = 0;

	virtual IParticleManager * GetParticleManager() = 0;
	
	// Description:
	//		Syncs and performs outstanding operations for the Asyncrhon ProcessStreaming Update
	virtual void SyncProcessStreamingUpdate() = 0;

	// Set Callback for Editor to store additional information in Minimap tool
	virtual void SetScreenshotCallback(IScreenshotCallback* pCallback) = 0;

  // Show/Hide objects by layer (useful for streaming and performance)
  virtual void ActivateObjectsLayer(uint16 nLayerId, bool bActivate, bool bPhys, const char * pLayerName) = 0;

  // Activate streaming of character and all sub-components
  virtual void PrecacheCharacter(IRenderNode * pObj, const float fImportance,  ICharacterInstance * pCharacter, IMaterial* pSlotMat, const Matrix34& matParent, const float fEntDistance, const float fScale, int nMaxDepth, bool bForceStreamingSystemUpdate ) = 0;

	// Activate streaming of render node and all sub-components
	virtual void PrecacheRenderNode(IRenderNode * pObj, float fEntDistanceReal) = 0;

	virtual IDeferredPhysicsEventManager* GetDeferredPhysicsEventManager() =0;

	// Return true if terrain texture streaming takes place
	virtual bool IsTerrainTextureStreamingInProgress() = 0;
};

//==============================================================================================

// Summary:
//	 Types of binary files used by 3dengine
enum EFileTypes
{
	eTerrainTextureFile=100,
};

#define FILEVERSION_TERRAIN_TEXTURE_FILE 9

// Summary:
//	 Common header for binary files used by 3dengine
struct SCommonFileHeader
{
	void Set(uint16 t, uint16 v) { strcpy(signature,"CRY"); file_type = (uint8)t; version = v; }
	bool Check(uint16 t, uint16 v) { return strcmp(signature,"CRY")==0 && t==file_type && v==version; }

	char				signature[4];						// File signature, should be "CRY "
  uint8			  file_type;							// File type
  uint8			  flags;							    // File common flags
	uint16			version;								// File version

	AUTO_STRUCT_INFO
};

// Summary:
//	 Sub header for terrain texture file
// Notes:
//	 "locally higher texture resolution" following structure can be removed (as well in autotype)
struct STerrainTextureFileHeader_old
{
	uint16			nSectorSizeMeters;			//
	uint16			nLodsNum;								//
	uint16			nLayerCount;						// STerrainTextureLayerFileHeader count following (also defines how may layers are interleaved) 1/2
	uint16			nReserved;

  AUTO_STRUCT_INFO
};

#define TTFHF_AO_DATA_IS_VALID 1
#define TTFHF_BIG_ENDIAN 2

// Summary:
// Sub header for terrain texture file
struct STerrainTextureFileHeader
{
	uint16			nLayerCount;						// STerrainTextureLayerFileHeader count following (also defines how may layers are interleaved) 1/2
	uint16			dwFlags;
  float				fBrMultiplier;

  AUTO_STRUCT_INFO
};

// Summary:
//	 Layer header for terrain texture file (for each layer)
struct STerrainTextureLayerFileHeader
{
	uint16			nSectorSizePixels;	//
	uint16			nReserved;					// ensure padding and for later usage
	ETEX_Format eTexFormat;					// typically eTF_DXT1, eTF_A4R4G4B4 or eTF_R5G6B5
	uint32			nSectorSizeBytes;		// redundant information for more convenient loading code

  AUTO_STRUCT_INFO
};

#pragma pack(pop)


#endif //CRY3DENGINEINTERFACE_H

