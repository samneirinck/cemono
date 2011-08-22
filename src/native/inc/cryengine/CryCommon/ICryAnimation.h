////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ICryAnimation.h
//  Version:     v1.00
//  Created:     1/8/2004 by Ivo Herzeg
//  Compilers:   Visual Studio.NET
//  Description: CryAnimation interface
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(ICryAnimation.h)

#ifndef ICRY_ANIMATION
#define ICRY_ANIMATION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef CRYANIMATION_API
	#ifdef CRYANIMATION_EXPORTS
		#define CRYANIMATION_API DLL_EXPORT
	#else
		#define CRYANIMATION_API DLL_IMPORT
	#endif
#endif



//DOC-IGNORE-BEGIN
#include <Cry_Math.h>
#include <Cry_Geo.h>

#include <IRenderer.h>
#include <IPhysics.h>
#include <I3DEngine.h>
#include <IEntityRenderState.h>
#include <IRenderAuxGeom.h>
#include <IEntitySystem.h>
#include <IParticles.h>

#include "CryCharAnimationParams.h"

//////////////////////////////////////////////////////////////////////
// Handle which determines the object binding to a bone in the character.
//typedef ULONG_PTR ObjectBindingHandle;

// NOTE: This will eventually become deprecated.
enum EAnimationJointType
{
	eIM_PelvisIdx,
	eIM_Spine0Idx,
	eIM_Spine1Idx,
	eIM_Spine2Idx,
	eIM_Spine3Idx,
	eIM_NeckIdx,
	eIM_HeadIdx,

	eIM_LeftEyeIdx,
	eIM_RightEyeIdx,

	eIM_LThighIdx,
	eIM_LCalfIdx,
	eIM_LFootIdx,
	eIM_LHeelIdx,
	eIM_LToe0Idx,

	eIM_RThighIdx,
	eIM_RCalfIdx,
	eIM_RFootIdx,
	eIM_RHeelIdx,
	eIM_RToe0Idx,

	// insert new value id BEFORE last
	eIM_Last,

};

//Flags used by game
enum ECharRenderFlags {
	CS_FLAG_DRAW_MODEL		  = 1 << 0,
	CS_FLAG_DRAW_NEAR			  = 1 << 1,
	CS_FLAG_UPDATE				  = 1 << 2,
	CS_FLAG_UPDATE_ALWAYS   = 1 << 3,
	CS_FLAG_UPDATE_ONRENDER = 1 << 4,
  CS_FLAG_COMPOUND_BASE   = 1 << 5
};

//Flags used human IK
enum IKFlags 
{
	CA_ARM_LEFT		= 0,
	CA_ARM_RIGHT	= 1,
	CA_LEG_LEFT		= 2,
	CA_LEG_RIGHT	= 3
};

//LMG animation check error codes
enum LmgAnimationStatus
{
	LMGAS_OK = 0,

	LMGAS_ANIMATION_NOT_IN_ANIMATION_LIST,
	LMGAS_ASSET_DOESNT_EXIST,
	LMGAS_ANIMATION_HAS_NO_SPEED,
	LMGAS_INVALID_BLEND_CODE
};


#define CHR (0x11223344)
#define CGA (0x55aa55aa)

#define NULL_ANIM "null"
#define NULL_ANIM_FILE "null"


// Forward declarations
struct	IShader;
struct  SRendParams;
struct CryEngineDecalInfo;
struct ParticleParams;
struct CryCharMorphParams;
struct IMaterial;
struct IStatObj;
struct IRenderMesh;
class CDLight;

class CCharacterModel;

struct ICharacterManager;
struct ICharacterInstance;
struct ICharacterModel;
struct IAnimationSet;

struct ISkeletonAnim;
struct ISkeletonPose;
struct IMorphing;
struct IAttachmentManager;

struct IAttachment;
struct IAttachmentObject; // Entity, static object or character
struct IAnimEvents;
struct ExtSkinVertex;
struct TFace;
struct IFacialInstance;
struct IFacialAnimation;

class ICrySizer;
struct CryCharAnimationParams;

struct IAnimationPoseModifier;
typedef int (*CallBackFuncType)(ICharacterInstance*,void*);

struct IAnimationStreamingListener;

// Description:
//     This class is the main access point for any character anymation 
//     required for a program which uses CryEngine.
// See Also:
//     CreateCharManager
UNIQUE_IFACE struct ICharacterManager
{
	virtual ~ICharacterManager(){}
	// Description:
	//     Model keep in memory hints.
	//     The model of a character can be locked in memory. This means, that even if the 
	//     number of characters using this model drops to zero, it'll be kept in memory.
	//     Such model is called Persistent.
	//     A model that's deleted as soon as there are no characters referencing it is called Transient.
	//     The default (when there's no hint) is defined by ca_KeepModels console variable.
	//     If there are both hints, it's up to the character animation system to decide whether to keep it or not.
	// Summary:
	//     Flags to unload a model from memory when it's no longer being used
	enum EModelPersistence
	{
		// Let the Animation System releases the model data when it's no longer used
		nHintModelTransient  = 1, 
		// Force the model data to stay in memory even if no character instance uses it anymore
		nHintModelPersistent = 2, 
	};

	// Description:
	//     Priority when requested to load a DBA
	// See also:
	//     DBA_PreLoad
	//	   DBA_LockStatus	
	enum EStreamingDBAPriority
	{
		eStreamingDBAPriority_Normal = 0,
		eStreamingDBAPriority_Urgent = 1,
	};

	// Description:
	//     Contains statistics about CryCharManager.
	struct Statistics
	{
		// Number of character instances
		unsigned numCharacters;
		// Number of character models (CGF)
		unsigned numCharModels;
		// Number of animobjects
		unsigned numAnimObjects;
		// Number of animobject models
		unsigned numAnimObjectModels;
	};

	// Description:
	//     Will fill the Statistics parameters with statistic on the instance 
	//     of the Animation System.
	//     It isn't recommanded to call this function often.
	// Arguments:
	//     rStats - Structure which hold the statistics
	// Summary:
	//     Get statistics on the Animation System
	virtual void GetStatistics(Statistics& rStats) = 0;

	// Description:
	//     Gather the memory currently used by the animation. The information
	//     returned is classified according to the flags set in the sizer 
	//     argument.
	// Arguments:
	//     pSizer - Sizer class which will store the memory usage
	// Summary:
	//     Track memory usage
	virtual void GetMemoryUsage( ICrySizer* pSizer) const = 0;

	// Description:
	//     Create a new instance for a model Load the model file along with any animation file that might be
	//     available.
	// See Also:
	//     RemoveCharacter
	// Arguments:
	//     szFilename - Filename of the model to be loaded
	//     nFlags     - Set how the model will be kept in memory after being 
	//                  used. Uses flags defined with EModelPersistence.
	// Return Value:
	//     A pointer to a ICharacterInstance class if the model could be loaded 
	//     properly.
	//     NULL if the model couldn't be loaded.
	// Summary:
	//     Create a new instance of a model
	//	virtual ICharacterInstance * MakeCharacter(const char * szFilename, unsigned nFlags = 0)=0;
	virtual ICharacterInstance* CreateInstance(const char * szFilename, uint32 IsSkinAtt=0, IAttachment* pIMasterAttachment=0, uint32 nLoadingFlags=0 )=0;
  virtual ICharacterInstance* CreateCHRInstance( const char* strPath, uint32 nFlags=0, IAttachment* pMasterAttachment=0, uint32 nLoadingFlags=0 ) = 0;
	// Description:
	//     Cleans up all resources. Currently deletes all bodies and characters even if there are references on them.
	// Arguments:
	//     bForceCleanup - When set to true will force all models to be deleted, even if references to them still left.
	// Summary:
	//     Cleans up all resources 
	virtual void ClearResources( bool bForceCleanup ) = 0;

	// Description:
	//     Update the Animation System. It's important to call this function at every frame. This should perform very fast.
	// Summary:
	//     Update the Animation System
	virtual void Update(bool bPaused) = 0;

	// Description:
	//     Increment the frame counter.
	// Summary:
	//     Useful to prevent log spam: "several updates per frame..."
	virtual void DummyUpdate() = 0;

	// Description:
	//     Releases any resource allocated by the Animation System and shut it down properly.
	// Summary:
	//     Release the Animation System
	virtual void Release()=0;

	//! Locks all models in memory

	// Description:
	//     Lock all the models to stay loaded in memory.
	// See Also:
	//     UnlockResources
	// Summary:
	//     Lock all the models to stay loaded in memory.
	virtual void LockResources() = 0;

	// Description:
	//     Unlock all the models allow them to be unloaded from memory.
	// See Also:
	//     LockResources
	// Summary:
	//     Unlock all the models allow them to be unloaded from memory.
	virtual void UnlockResources() = 0;

	virtual uint32 SaveCharacterDefinition(ICharacterInstance* ptr, const char* pathname) = 0;

	// Description:
	//     Retrieve all loaded models.
	virtual void GetLoadedModels( ICharacterModel** pCharacterModels,int &nCount ) = 0;

  // Description
  //     Reloads loaded model
  virtual void ReloadModel(ICharacterModel* pModel) = 0;
  virtual void ReloadAllModels() = 0;

	virtual void PreloadLevelModels() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Retrieve facial animation interface.
	virtual IFacialAnimation* GetIFacialAnimation() = 0;
	virtual IAnimEvents* GetIAnimEvents() = 0;

	virtual void SetScalingLimits( const Vec2& limits ) = 0;


	// Description:
	//		Use to synchronize all animation computations like forwardkinematics and calls
	//		all function which must occur after these like SkeletonPostProcess
	//		Should be called only once per frame and as late as possible to prevent waiting
	//		for functions which run asynchronly
	virtual void SyncAllAnimations() =0;
	
	// Description:
	//		Adds an animation to the synchronize queue from SyncAllAnimations
	virtual void AddAnimationToSyncQueue(ICharacterInstance *pCharInstance) =0;

	// Description:
	//		Removes an animation to the synchronize queue from SyncAllAnimations
	virtual void RemoveAnimationToSyncQueue( ICharacterInstance *pCharInstance ) =0;

	//functions to load, lock and unload dba files
	virtual bool DBA_PreLoad( const char* filepath, ICharacterManager::EStreamingDBAPriority priority) =0;
	virtual bool DBA_LockStatus( const char* filepath, uint32 status, ICharacterManager::EStreamingDBAPriority priority ) =0;
	virtual bool DBA_Unload( const char* filepath ) =0;
	virtual bool DBA_Unload_All( ) =0;

	// Adds a runtime reference to a CAF animation, if not loaded it starts streaming it
	virtual bool CAF_AddRef(uint32 filePathCRC) = 0; 
	virtual bool CAF_IsLoaded(uint32 filePathCRC) const = 0;
	virtual bool CAF_Release(uint32 filePathCRC) = 0;

	virtual int ReloadCAF(const char* szFilePathCAF) = 0;

	// return the DBA filename of an animation that is stored in a DBA
	virtual const char* GetDBAFilenameByGlobalID(int32 globalID) = 0;

	// set the listener which listenes to events that happen during animation streaming
	virtual void SetStreamingListener(IAnimationStreamingListener *pListener) = 0;

	// Add nTicks to the number of Ticks spend this frame in animation functions
	virtual void AddFrameTicks( uint64 nTicks ) = 0;

	// Add nTicks to the number of Ticks spend this frame in syncing animation jobs
	virtual void AddFrameSyncTicks( uint64 nTicks ) = 0;
	
	// Reset Ticks Counter
	virtual void ResetFrameTicks() = 0;

	// Get number of Ticks accumulated over this frame
	virtual uint64 NumFrameTicks() const =0;

	// Get number of Ticks accumulated over this frame in sync functions
	virtual uint64 NumFrameSyncTicks() const =0;
	
	// Get The number of characters instances send to SPU 
	virtual uint32 NumCharacters() const = 0;

	virtual void SetAnimMemoryTracker( const SAnimMemoryTracker& amt )=0;
	virtual SAnimMemoryTracker GetAnimMemoryTracker()=0;

	virtual void UpdateRendererFrame() = 0;
};

// Description:
//		This struct defines the interface for a class that listens to
//		AnimLoaded and AnimUnloaded events

UNIQUE_IFACE struct IAnimationStreamingListener
{
	virtual ~IAnimationStreamingListener(){}
	virtual void NotifyAnimLoaded(const int32 globalID) = 0;
	virtual void NotifyAnimUnloaded(const int32 globalID) = 0;
};

struct CloseInfo {
	uint32 FaceNr;
	Vec3 tv0;
	Vec3 tv1;
	Vec3 tv2;
	Vec3 middle;
};

struct SJointProperty {
	SJointProperty(const char *pname, float val) { name=pname; type=0; fval=val; }
	SJointProperty(const char *pname, bool val) { name=pname; type=1; bval=val; }
	SJointProperty(const char *pname, const char *val) { name=pname; type=2; strval=val; }
	const char *name;
	int type; 
	union {
		float fval;
		bool bval;
		const char *strval;
	};
};


//////////////////////////////////////////////////////////////////////////

struct SIKTargetCharData
{
	uint64	m_spineHandleHash;					//! hash for the spine limb of the character (if existing) - will be used by the Object Interaction System

	std::map<uint64, float>	m_armLengthValues;	//! value used for distance calculation - does not need to represent actual arm value - can be made shorter for effects
	std::map<uint64, Vec3>	m_endEffPullback;	//! hand/finger thickness, the hand will be moved back by this automatically to avoid clipping
	std::map<uint64, int16>	m_animLayers;		

	float m_armLengthDefault;
	Vec3 m_endEffPullbackDefault;
	float m_spineLength;

	SIKTargetCharData() { Reset(); }

	void Reset()
	{
		m_spineHandleHash = 0;
		m_armLengthDefault = 0.45f;
		m_endEffPullbackDefault = Vec3(-0.08f, -0.02f, 0.0f);
		m_spineLength = 0.40f;
		m_armLengthValues.clear();
		m_endEffPullback.clear();
		m_animLayers.clear();
	}
};

//////////////////////////////////////////////////////////////////////////


UNIQUE_IFACE struct ICharacterModelSkeleton
{
	virtual ~ICharacterModelSkeleton(){}
	virtual uint32 GetJointCount() const = 0;
	virtual int32 GetJointIDByName(const char* name) const = 0;
	virtual int32 GetJointIDByCRC32(uint32 crc32) const = 0; 
	virtual const char* GetJointNameByID(int32 id) const = 0;
	virtual int32 GetJointParentIDByID(int32 id) const = 0;

	virtual int32 GetJointIDByType(EAnimationJointType eType) const = 0;

	//virtual const IKLimbType* GetLimbDefinition(int32 index) const = 0;
	virtual int32 GetLimbDefinitionIdx(uint64 handle) const = 0;
	virtual int32 GetLimbEndeffectorIdx(uint64 handle) const = 0;
	virtual int32 GetLimbRootIdx(uint64 handle) const = 0;

	virtual const SIKTargetCharData* GetCharIKTargetData() const = 0;
};

//////////////////////////////////////////////////////////////////////////
UNIQUE_IFACE struct ICharacterModel
{
	virtual ~ICharacterModel(){}
  // Returns type name given to the model on creation time.
  virtual const char* GetName() const = 0;
	// Return number of instances for that model.
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual uint32 GetNumInstances() = 0;
	virtual uint32 GetNumLods() = 0;
	virtual const char* GetModelFilePath() const =0;

  // joints
  virtual phys_geometry* GetJointPhysGeom(uint32 jointIndex,int nLod) = 0;
	virtual DynArray<SJointProperty> GetJointPhysProperties(uint32 jointIndex, int nLod) = 0;
	virtual bool SetJointPhysProperties(uint32 jointIndex, int nLod, const DynArray<SJointProperty> &props) = 0;

//	virtual const char * GetFile() = 0;
	// Retrieve render mesh for specified lod.
	virtual IRenderMesh* GetRenderMesh( int nLod ) = 0;

  virtual CloseInfo FindClosestPointOnMesh( const Vec3& RMWPosition,bool bUseCenterPointDist=true ) = 0; 

	virtual uint32 GetTextureMemoryUsage( ICrySizer *pSizer=0 ) = 0;
	virtual uint32 GetMeshMemoryUsage(ICrySizer *pSizer=0) = 0;
	virtual ICharacterInstance * GetInstance(uint32 num) = 0;

	virtual ICharacterModelSkeleton* GetICharacterModelSkeleton() = 0;

	virtual const char* GetDefaultLookPose() = 0;

	// NOTE: Will become deprecated.
	// Use GetICharacterModelSkeleton() instead!
	virtual uint32 GetNumJoints() = 0;
	// END: Will become deprecated.
};


//DOC-IGNORE-BEGIN
//! TODO:
//! Split this interface up into a few logical interfaces, starting with the ICryCharModel
//DOC-IGNORE-END

struct SCharUpdateFeedback
{
	SCharUpdateFeedback() { flags=0; pPhysHost=0; mtxDelta.SetIdentity(); }
	int flags;									// |1 if pPhysHost is valid, |2 is mtxDelta is valid
	IPhysicalEntity *pPhysHost;	// tells the caller to restore this host as the main phys entity
	Matrix34 mtxDelta;					// tells the caller to instantly post-multiply its matrix with this one
};

struct SAnimationProcessParams
{
	QuatT locationPhysics;
	QuatTS locationAnimation;

	CCamera* pCamera;

	bool bOnRender;
	float zoomAdjustedDistanceFromCamera;
	float trajectoryWeight;

	SAnimationProcessParams() :
		locationPhysics(IDENTITY),
		locationAnimation(IDENTITY),
		pCamera(NULL),
		bOnRender(false),
		zoomAdjustedDistanceFromCamera(0.0f),
		trajectoryWeight(0.0f)
	{
	}
};

// Description:
//     This interface contains methods for manipulating and querying an animated character
//     Instance. The methods allow modify the animated instance to the certain way,
//     animate it, render, retrieve BBox/etc, control physics, particles and skinning, transform.
// Summary:
//     Interface to character animation
struct ICharacterInstance
{
	virtual ~ICharacterInstance(){}
	// Increase reference count of the interface
	virtual void AddRef() = 0;
	// Decrease reference count of the interface
	virtual void Release() = 0;

	//////////////////////////////////////////////////////////////////////
	// Description:
	//     Return a pointer of the instance of a ISkeletonAnim derived class applicable for the model.
	// Return Value:
	//     A pointer to a ISkeletonAnim derived class
	// Summary:
	//     Get the skeleton for this instance
	virtual ISkeletonAnim* GetISkeletonAnim() = 0;


	//////////////////////////////////////////////////////////////////////
	// Description:
	//     Return a pointer of the instance of a ISkeletonPose derived class applicable for the model.
	// Return Value:
	//     A pointer to a ISkeletonPose derived class
	// Summary:
	//     Get the skeleton for this instance
	virtual ISkeletonPose* GetISkeletonPose() = 0;


	//////////////////////////////////////////////////////////////////////
	// Description:
	//     Return a pointer of the instance of a IMorphing derived class.
	// Return Value:
	//     A pointer to a IMorphing derived class
	// Summary:
	//     Get the low-level morphing interface for this instance
	virtual IMorphing* GetIMorphing() = 0;


	//////////////////////////////////////////////////////////////////////
	// Description:
	//     Return a pointer of the instance of a IAttachmentManager derived class applicable for the model.
	// Return Value:
	//     A pointer to a IAttachmentManager derived class
	// Summary:
	//     Get the attachment manager for this instance
	virtual IAttachmentManager* GetIAttachmentManager() = 0;




	//////////////////////////////////////////////////////////////////////
	// Description:
	//     Return shared character model used by this instance.
	virtual ICharacterModel* GetICharacterModel() = 0;

	// Description:
	//     Return a pointer of the instance of a ICryAnimationSet derived class applicable for the model.
	// Return Value:
	//     A pointer to a ICryAnimationSet derived class
	// Summary:
	//     Get the Animation Set defined for the model
	virtual IAnimationSet* GetIAnimationSet() = 0;

	// Description:
	//		Get the name of the file that stores the animation event definitions for this model. This
	//		is usually stored in the CAL file
	// Return Value:
	//		A pointer to a null terminated char string which contains the filename of the database
	// Summary:
	//		Get the animation event file
	virtual const char* GetModelAnimEventDatabase()=0;

	virtual void Serialize(TSerialize ser)=0;

  virtual void ReplaceModel(ICharacterModel* pNewModel) = 0;

	//enables/disables StartAnimation* calls; puts warning into the log if StartAnimation* is called while disabled
	virtual void EnableStartAnimation (bool bEnable)=0;

	virtual void StartAnimationProcessing(const SAnimationProcessParams& params) = 0;

	virtual void UpdateAttachedObjectsFast(const QuatT& rPhysLocationNext, float fZoomAdjustedDistanceFromCamera, uint32 OnRender )=0;

	//! disable rendering of this render this instance
	virtual void HideMaster(uint32 h)=0;

	virtual void DrawWireframeStatic( const Matrix34& m34, int nLOD, uint32 color)=0;

	virtual uint32 GetResetMode()=0;
	virtual void SetResetMode(uint32 rm)=0;


	//! Return dynamic bbox of object
	// Description:
	// Arguments:
	// Summary:
	//     Get the bounding box
	virtual AABB GetAABB()=0;

	//! Return the radius of the object, squared
	// Description:
	// Arguments:
	// Summary:
	//     Get the radius of the object, squared
	virtual float GetRadiusSqr()=0;

	// Summary:
	//     Return the extent (length, volume, or area) of the object
	// Arguments:
	//     eType, eForm - See RandomPos
	virtual f32 ComputeExtent(GeomQuery& geo, EGeomForm eForm)=0;

	// Summary:
	//     Generate a random point in object.
	// Arguments:
	//     eType - Object geometry to use (physics or render geometry)
	//     eForm - Object aspect to generate on (surface, volume, etc)
	virtual void GetRandomPos(RandomPos& ran, GeomQuery& geo, EGeomForm eForm)=0;


	// Description:
	//     Draw the character using specified rendering parameters.
	// Arguments:
	//     RendParams - Rendering parameters
	// Summary:
	//     Draw the character
	virtual void Render(const SRendParams& RendParams, const QuatTS& Offset,  bool* updated = 0)=0;	

	// Description:
	//     Set rendering flags defined in ECharRenderFlags for this character instance
	// Arguments:
	//     Pass the rendering flags
	// Summary:
	//     Set rendering flags
	virtual void SetFlags(int nFlags)=0;

	// Description:
	//     Get the rendering flags enabled. The valid flags are the ones declared in ECharRenderFlags.
	// Return Value:
	//     Return an integer value which hold the different rendering flags
	// Summary:
	//     Set rendering flags
	virtual int  GetFlags()=0;

	// Description:
	//     Get the object type contained inside the character instance. It will return either the CHR or CGA constant.
	// Return Value:
	//     An object type constant
	// Summary:
	//     Get the object type
	virtual int GetOjectType() = 0;

	// Description:
	//     Set the color to be used by the character instance
	// Arguments:
	//     fR - Red value 
	//     fG - Green value
	//     fB - Blue value
	//     fA - Alpha value
	// Summary:
	//     Set the color
	virtual void SetColor(f32 fR, f32 fG, f32 fB, f32 fA) = 0;


	// Description:
	//     Get the RenderMesh materials in this character as they are used in the renderer
	// Return Value:
	//     List of material info
	// Summary:
	//     Return the RenderMesh materials
	virtual const PodArray<struct CRenderChunk>*getRenderMeshMaterials() {return NULL;}

	// Description:
	//     Get a property attached to the model during exporting process.
	// Return Value:
	//     A pointer to a null terminated char string which contain the filename of the model.
	// Summary:
	//     Get the filename of the model
	virtual const char* GetFilePath()=0;


	//! is called when the character is datached (if it was an attachment)
	virtual void OnDetach() = 0;



	//! enable and disable decals for this instance by default its always disabled
	virtual void EnableDecals(uint32 d)=0;
	//! Spawn a decal on animated characters
	//! The decal hit direction and source point are in the local coordinates of the character.
	virtual void CreateDecal(CryEngineDecalInfo& DecalLCS)=0;


	//! Pushes the underlying tree of objects into the given Sizer object for statistics gathering
	virtual void GetMemoryUsage(class ICrySizer* pSizer) const = 0;

	virtual void ReleaseTemporaryResources() = 0;



  // Reload the animation set at any time
	virtual void ReloadAnimationSet() = 0;



	// Description:
	//     Set custom instance material for this character.
	// Arguments:
	//     pMaterial - A valid pointer to the material.
	virtual f32 GetAverageFrameTime() = 0;

	// Description:
	//     Set custom instance material for this character.
	// Arguments:
	//     pMaterial - A valid pointer to the material.
	virtual void SetMaterial( IMaterial *pMaterial ) = 0;

	// Description:
	//     Returns material used to render this character, can be either custom or model material.
	// Arguments:
	//     nType - Pass 0 to get the physic geometry or pass 1 to get the obstruct geometry
	// Return Value:
	//     A pointer to a phys_geometry class. 
	virtual IMaterial* GetMaterial() = 0;

	// Description:
	//     Returns the instance-specific material - if this is 0, then the default model material is in use.
	// Return Value:
	//     A pointer to the material, or 0.
	virtual IMaterial* GetMaterialOverride() = 0;


	//! Renderer calls this function to get the current skeleton-pose to deform the mesh right before the rendering
	virtual uint32 GetSkeletonPose(int nLod, int nList, const Matrix34& RenderMat34, QuatTS*& pBoneQuatsL, QuatTS*& pBoneQuatsS, QuatTS*& pMBBoneQuatsL, QuatTS*& pMBBoneQuatsS, Vec4 shapeDeformationData[], uint32 &DoWeNeedMorphtargets, uint8*& pRemapTable) = 0;


	virtual IRenderMesh* GetRenderMesh () = 0;

	//////////////////////////////////////////////////////////////////////////
	// Facial interface.
	//////////////////////////////////////////////////////////////////////////
	virtual IFacialInstance* GetFacialInstance() = 0;
	virtual void EnableFacialAnimation( bool bEnable ) = 0;
	virtual void EnableProceduralFacialAnimation( bool bEnable ) = 0;

	virtual int FindAffectedVertices(const Vec3 &vPos,float fRadius,std::vector<int> &lstIDs)=0;

	//////////////////////////////////////////////////////////////////////////
	// Lip sync character with the played sound.
	//////////////////////////////////////////////////////////////////////////
	virtual void LipSyncWithSound( uint32 nSoundId, bool bStop=false ) = 0;


	//! Set animations speed scale
	//! This is the scale factor that affects the animation speed of the character.
	//! All the animations are played with the constant real-time speed multiplied by this factor.
	//! So, 0 means still animations (stuck at some frame), 1 - normal, 2 - twice as fast, 0.5 - twice slower than normal.
	virtual void SetAnimationSpeed(f32 fSpeed) = 0;
	virtual f32 GetAnimationSpeed() = 0;
	virtual uint32 IsCharacterVisible() = 0;
	// Return true if animation graph for this character is still valid.
	virtual bool IsAnimationGraphValid() = 0;
	virtual void SetFPWeapon(f32 fp) = 0;
	virtual f32* GetShapeDeformArray()=0;

	// Skeleton effects interface.
	virtual void SpawnSkeletonEffect(int animID, const char* animName, const char* effectName, const char* boneName, const Vec3& offset, const Vec3& dir, const Matrix34& entityTM) = 0;
	virtual void KillAllSkeletonEffects() = 0;

  virtual void SetUniformScale(const float& fUniformScale) = 0;
  virtual float GetUniformScale() const = 0;

  virtual bool GetIgnoreScaleForJoint(uint32 jointIndex) const = 0;
  virtual bool GetIsCustomScaleForJoint(uint32 jointIndex) const = 0;
  virtual float GetCustomScaleForJoint(uint32 jointIndex) const = 0;
  virtual void SetCustomScaleForJoint(uint32 jointIndex,bool bValue,float fValue) = 0;
	virtual void SetForceLOD(const bool bSet) = 0;
	virtual void SetForceLODNum(const int32 level) = 0;


	virtual void CopyPoseFrom(const ICharacterInstance &instance) = 0;

	// functions for asyncronous execution of ProcessAnimationUpdate and ForwardKinematics

	// Makes sure all functions which must be called after forwardkinematics are executed and also syncronizes the execution
	// Is called during GetISkeletonAnim and GetISkeletonPose if the bNeedSync flag is set to true(the default) to ensure all computations have finished when
	// needed
	virtual void FinishAnimationComputations() = 0;

	virtual void ProcessAttachment( IAttachment* pIAttachment, const QuatT& mtx, float fZoomAdjustedDistanceFromCamera, uint32 OnRender ) = 0;

	// OBSOLETE
	virtual void SkeletonPreProcess (const QuatT &rPhysLocationCurr,const QuatTS &rAnimLocationCurr, const CCamera& rCamera, uint32 OnRender ) =0;
	virtual void SetPostProcessParameter( const QuatT &rPhysLocationNext, const QuatTS &rAnimLocationNext, IAttachment*	pIAttachment, float fZoomAdjustedDistanceFromCamera, uint32 nOnRender ) = 0;
};



//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

#include "IAnimationPoseModifier.h"

UNIQUE_IFACE struct ISkeletonAnim
{
	virtual ~ISkeletonAnim(){}
	// Description:
	//     Enable special debug text for this skeleton
	virtual void SetDebugging( uint32 flags ) = 0;

	//motion initialization
	virtual void SetMirrorAnimation(uint32 ts)	=0;
	virtual void SetAnimationDrivenMotion(uint32 ts)	=0;
	virtual uint32 GetAnimationDrivenMotion() = 0;
	virtual void SetTrackViewExclusive(uint32 i) =0;
	virtual void SetTrackViewMixingWeight(uint32 layer, f32 weight) = 0;
	virtual uint32 GetTrackViewStatus() =0;
	virtual void SetCharEditMode( uint32 m )=0; 


	//motion playback and blending
	virtual bool StartAnimation(const char* szAnimName0, const CryCharAnimationParams& Params)=0;
	virtual bool StartAnimationById(int32 id, const CryCharAnimationParams& Params)=0;
	virtual bool StopAnimationInLayer(int32 nLayer, f32 BlendOutTime)=0;
	virtual bool StopAnimationsAllLayers()=0;
	
	// Description
	//   Find an animation with a given user token.
	// Arguments:
	//   nUserToken - User token that identify an animation to search for.
	//   nLayer - Layer of FIFO where to search for animation, if -1 all layers are searched.
	virtual CAnimation* FindAnimInFIFO( uint32 nUserToken,int nLayer=1, bool bIncludeDeferredAnimation=false ) = 0;


	// Description
	//   Remove an animation with a given index and given layer.
	// Arguments:
	//   nLayer - Animation layer to remove from.
	//   num - Transition queue index to remove at.
	//	 forceRemove - Ignore special conditions and force a removal from the layer.
	virtual bool RemoveAnimFromFIFO(uint32 nLayer, uint32 num, bool forceRemove=false) = 0;
	virtual int  GetNumAnimsInFIFO(uint32 nLayer) const = 0;
  virtual void ClearFIFOLayer(uint32 nLayer)=0;
	virtual CAnimation& GetAnimFromFIFO(uint32 nLayer, uint32 num )=0;
	// If manual update is set for anim, then set anim time and handle anim events.
	virtual void ManualSeekAnimationInFIFO(uint32 nLayer, uint32 num, float time, bool triggerAnimEvents) = 0;

	// makes sure there's no anim in this layer's queue that could cause a delay (useful when you want to play an
	// animation that you want to be 100% sure is going to be transitioned to immediately)
	virtual void RemoveTransitionDelayConditions(uint32 nLayer) = 0;

	// [*DavidR | 22/Mar/2010] BEGIN: 
	// TEMPORARY METHODS UNTIL m_arrDeferredQueueUpdates GETS REMOVED
	virtual int  GetNumAnimsInFIFOAndDeferredQueue(uint32 nLayer)=0;
	virtual CAnimation& GetAnimFromFIFOAndDeferredQueue(uint32 nLayer, uint32 num)=0;
	// [*DavidR | 22/Mar/2010] END 

	virtual void SetLayerBlendMultiplier(int32 nLayer, f32 fMult) = 0;

	virtual void SetDesiredLocalLocation(const QuatT& desiredLocalLocation, float deltaTime, float frameTime, float turnSpeedMultiplier) =0;
	virtual void SetDesiredMotionParam(EMotionParamID id, f32 value, f32 frametime, bool initOnly = false) = 0; // Updates the given parameter (will perform clamping and clearing as needed)
	virtual float GetDesiredMotionParam(EMotionParamID id) const = 0; // What is the current value of a given param.
	virtual void SetBlendSpaceOverride(EMotionParamID id, float value, bool enable)=0; // Enable/Disable direct override of blendspace weights, used from CharacterEditor.


		//! This is the same as SetAnimationSpeed, but it sets the speed for layers
	//! NOTE: It does NOT override the overall animation speed, but it's multiplies it
	virtual void SetLayerUpdateMultiplier(int32 nLayer, f32 fSpeed)=0;
	virtual f32 GetLayerUpdateMultiplier(uint32 nLayer)=0;
	virtual void SetAdditiveWeight(int32 nLayer, f32 fWeight)=0;

	//! Set the current time of the given layer, in seconds
	virtual void SetLayerTime (uint32 nLayer, f32 fTimeSeconds) = 0;
	//! Return the current time of the given layer, in seconds
	virtual f32 GetLayerTime (uint32 nLayer) const = 0;

	//return values
	virtual Vec3 GetCurrentVelocity() =0; 
	virtual f32  GetCurrentSlope() = 0;


	virtual void SetEventCallback(CallBackFuncType func, void *pdata) = 0;
	virtual AnimEventInstance GetLastAnimEvent() = 0;

	virtual const QuatT& GetRelMovement() const =0;
 
	virtual f32  GetUserData( int i ) = 0;

	// functions for asynchronous execution of ProcessAnimationUpdate and ForwardKinematics

	// Makes sure all functions which must be called after forward-kinematics are executed and also synchronizes the execution
	// Is called during GetISkeletonAnim and GetISkeletonPose if the bNeedSync flag is set to true(the default) to ensure all computations have finished when
	// needed
	virtual void FinishAnimationComputations() = 0;

	// Set parameters with which SkeletonPostProcess should be called
	// NOTE: PostProcess is only called in case the parameters are set, if not the client is responsible to call PostProcess when needed after synchronizing the animation with FinishAnimationComputations()
	virtual void SetPostProcessParameter( const QuatT &rPhysLocationNext, const QuatTS &rAnimLocationNext, IAttachment*	pIAttachment, float fZoomAdjustedDistanceFromCamera, uint32 nOnRender ) = 0;

	virtual bool SetJointMask(const char* szBoneName, uint32 nLayerNo, uint8 nVal)=0; 
	virtual bool SetLayerMask(uint32 nLayerNo, uint8 nVal)=0;

	// OBSOLETE
	virtual void PushLayer(IAnimationPoseModifierPtr poseModifier) = 0;
	// USE THIS INSTEAD:
	virtual void SetPoseModifier(uint32 layer, IAnimationPoseModifierPtr poseModifier) = 0;
};

struct IAnimationPoseBlenderDir;
struct IAnimationGroundAlignment;

UNIQUE_IFACE struct ISkeletonPose
{
	virtual ~ISkeletonPose(){}
	virtual void SetDefaultPose()=0;

	// Description:
	//     Return the number of joints included in the model. All joint-ids start from 0. The first joint, which has the id 0, is the root joint.
	// Return Value:
	//     An int value which hold the number of joints.
	// Summary:
	//     Get the number of joints
	virtual uint32 GetJointCount() const = 0;

	//! Returns the index of the joint in the set, -1 if there's no such joint
	// Summary:
	//   Searches for the index of an joint using its name.
	// Arguments:
	//   szJointName - Null terminated string holding the name of the joint.
	// Return Value:
	//   An integer representing the index of the joint. In case the joint-name couldn't be found, -1 will be returned.
	virtual int16 GetJointIDByName (const char* szJointName) const =0;

	//this should be in one single joint-structure
	virtual int16 GetParentIDByID (int32 ChildID) const =0;
	virtual uint32 GetJointCRC32 (int32 nJointID) const =0;
	virtual const char* GetJointNameByID(int32 nJointID) const =0;
	virtual const QuatT& GetAbsJointByID(int32 nJointID) =0;
	virtual void SetAbsJointByID(int32 nJointID, const QuatT& pose) =0;
	virtual const QuatT& GetRelJointByID(int32 nJointID) =0;
	virtual const QuatT& GetDefaultRelJointByID(int32 nJointID) =0;
	virtual const QuatT& GetDefaultAbsJointByID(int32 nJointID) =0;

  virtual void SetDefaultAbsInvJointByID(int32 nJointID, const QuatT& pose) =0;

	//motion post-process
	virtual void SetPostProcessCallback(int (*func)(ICharacterInstance*,void*), void *pdata) = 0;
	virtual void* GetPostProcessCallback(void* &pdata) = 0;
	virtual void SetFootAnchoring(uint32 ts)	=0;

	/*
	virtual void SetWeaponRaisedPose(EWeaponRaisedPose pose) = 0;
	virtual void CCDInitIKBuffer(QuatT* pRelativeQuatIK,QuatT* pAbsoluteQuatIK)=0;;
	virtual int32* CCDInitIKChain(int32 sCCDJoint,int32 eCCDJoint)=0;
	virtual void CCDRotationSolver( const Vec3& pos, f32 fThreshold,f32 StepSize, uint32 iTry,const Vec3& normal,QuatT* pRelativeQuatIK,QuatT* pAbsoluteQuatIK,Quat* pQuats=0)=0;
	virtual void CCDTranslationSolver(const Vec3& EndPos,QuatT* g_RelativeQuatIK,QuatT* g_AbsoluteQuatIK)=0;
	virtual void CCDUpdateSkeleton(QuatT* pRelativeQuatIK,QuatT* pAbsoluteQuatIK)=0;
*/
	virtual void SetPostProcessQuat(int32 idx, const QuatT& qt )=0; 

	virtual void SetSuperimposed(uint32 i) =0;
	virtual void SetForceSkeletonUpdate(int32 i) =0;
	virtual IStatObj* GetStatObjOnJoint(int32 nId) = 0;
	virtual void SetStatObjOnJoint(int32 nId, IStatObj* pStatObj) = 0;
	virtual IPhysicalEntity *GetPhysEntOnJoint(int32 nId) = 0;
	virtual void SetPhysEntOnJoint(int32 nId, IPhysicalEntity *pPhysEnt) = 0;
	virtual int GetPhysIdOnJoint(int32 nId) = 0;
	virtual void SetMaterialOnJoint(int32 nId, IMaterial* pMaterial) = 0;
	virtual IMaterial* GetMaterialOnJoint(int32 nId) = 0;

	//! Used by physics engine)
	virtual void BuildPhysicalEntity(IPhysicalEntity *pent,f32 mass,int surface_idx,f32 stiffness_scale=1.0f,int nLod=0,int partid0=0, const Matrix34& mtxloc=Matrix34(IDENTITY)) = 0;
	virtual IPhysicalEntity* CreateCharacterPhysics(IPhysicalEntity *pHost, f32 mass,int surface_idx,f32 stiffness_scale, int nLod=0,const Matrix34& mtxloc=Matrix34(IDENTITY)) = 0;
	virtual int CreateAuxilaryPhysics(IPhysicalEntity *pHost, const Matrix34 &mtx, int nLod=0) = 0;
	virtual IPhysicalEntity *GetCharacterPhysics() const = 0;
	virtual IPhysicalEntity *GetCharacterPhysics(const char *pRootBoneName) = 0;
	virtual IPhysicalEntity *GetCharacterPhysics(int iAuxPhys) = 0;
	virtual void SetCharacterPhysics(IPhysicalEntity *pent) = 0;
	virtual void SynchronizeWithPhysicalEntity(IPhysicalEntity *pent, const Vec3& posMaster=Vec3(ZERO),const Quat& qMaster=Quat(1,0,0,0)) = 0;
	virtual IPhysicalEntity* RelinquishCharacterPhysics(const Matrix34 &mtx, f32 stiffness=0.0f, bool bCopyJointVelocities=false, const Vec3 &velHost=Vec3(ZERO)) = 0;
	virtual void DestroyCharacterPhysics(int iMode=0) = 0;
	virtual bool AddImpact(int partid, Vec3 point,Vec3 impact) = 0;
	virtual int TranslatePartIdToDeadBody(int partid) = 0;
	virtual int GetAuxPhysicsBoneId(int iAuxPhys, int iBone=0) = 0;

	virtual void Fall() = 0;
	virtual void GoLimp() = 0;
	virtual void StandUp(const Matrix34 &mtx, bool b3DOF, IPhysicalEntity *&pNewPhysicalEntity, Matrix34 &mtxDelta) = 0;
	virtual bool SetFnPAnimGroup(const char *name) = 0;
	virtual bool SetFnPAnimGroup(int idx) = 0;

	virtual f32 Falling() const = 0;
	virtual f32 Lying() const = 0;
	virtual f32 StandingUp() const = 0;
	virtual int GetFallingDir() /*const*/ = 0;

	//! Sets the current pose as the one dead body will move non-physicalized parts to
	virtual void SetRagdollDefaultPose() = 0;

	//need to attachmentmanager
	virtual int getBonePhysParentOrSelfIndex (int nBoneIndex, int nLod=0) = 0;
	
	virtual int GetBoneSurfaceTypeId(int nBoneIndex, int nLod=0) = 0;
	virtual void DrawSkeleton( const Matrix34& rRenderMat34, uint32 shift=0 ) = 0;

	// -------------------------------------------------------------------------
	// Pose Modifiers
	// -------------------------------------------------------------------------
	virtual IAnimationPoseBlenderDir* GetIPoseBlenderAim() = 0;
	virtual IAnimationPoseBlenderDir* GetIPoseBlenderLook() = 0;

	// Pose Blender Aim
	// OBSOLETE
	virtual void SetAimIK(bool bUseAimIK, const Vec3& AimAtTarget, float blendTime = -1.0f ) = 0; 
	virtual void SetAimIKFadeAngle(f32 angle) = 0;
	virtual void SetAimIKFadeOutTime(f32 time) = 0;
	virtual void SetAimIKFadeInTime(f32 time) = 0;
	virtual void SetAimIKTargetSmoothTime(f32 fSmoothTime)=0;
	virtual void SetAimIKPolarCoordinates(const Vec2& pc) = 0;
	virtual uint32 GetAimIKStatus() = 0;
	virtual f32 GetAimIKBlend() = 0;
	virtual void SetPoseBlenderAimState(bool bEnable) = 0;
	// USE THIS INSTEAD:

	// New LookIK
	virtual void SetLookIK(uint32 ik, f32 FOR, const Vec3& LookAtTarget, uint32 layer = 15, const char* lookPose = NULL, f32 smoothTime = 0.2f ) = 0;
	virtual uint8 IsLookIKEnabled() = 0;

	// Ground Alignment
	// OBSOLETE
	virtual void EnableFootGroundAlignment(bool enable) = 0;
	virtual void SetGroundAlignmentData(const bool bAlignSkeletonVertical, const f32 rootHeight, const Plane& planeLeft, const Plane& planeRight) = 0;
	// USE THIS INSTEAD:
	virtual IAnimationGroundAlignment* GetGroundAlignment() = 0;

	// Recoil
	// OBSOLETE
	virtual void ApplyRecoilAnimation(f32 fDuration, f32 fKinematicImpact, f32 fKickIn, uint32 arms=3 ) = 0;

	// Limb Ik
	virtual uint32 SetCustomArmIK(const Vec3& wgoal,int32 idx0,int32 idx1,int32 idx2) = 0;
	virtual uint32 SetHumanLimbIK(const Vec3& wgoal, const char* limb) = 0;
	virtual uint32 SetHumanLimbIK(const QuatT& wgoal, const char* limb, const bool adjustEndEffector = false) = 0;
	virtual uint32 SetHumanLimbIK(const QuatT& wgoal, uint64 limb, const bool adjustEndEffector = false) = 0;
	
	// use only if you cannot wait and need the results right away
	virtual uint32 SetHumanLimbIK_ExecuteImmediately(const QuatT& wgoal, const char* limb, const bool adjustEndEffector = false) = 0;
	virtual uint32 SetHumanLimbIK_ExecuteImmediately(const QuatT& wgoal, uint64 limb, const bool adjustEndEffector = false) = 0;
};


//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

UNIQUE_IFACE struct IMorphing
{
	virtual ~IMorphing(){}
  //! returns morphs count
  virtual size_t GetNumMorphs() const = 0;
  //! returns morph name by index
  virtual const char* GetMorphName(size_t index) const = 0;
	//! Start the specified by parameters morph target
	virtual void StartMorph (const char* szMorphTarget, const CryCharMorphParams& params)=0;
	//! Set morph speed scale
	//! Finds the morph target with the given id, sets its morphing speed and returns true;
	//! if there's no such morph target currently playing, returns false
	virtual bool SetMorphSpeed(const char* szMorphTarget, f32 fSpeed) =0;
	//! Stops morph by target id
	virtual bool StopMorph(const char* szMorphTarget)=0;
	//! Stops all morphs
	virtual void StopAllMorphs()=0;
	//! freezes all currently playing morphs at the point they're at
	virtual void FreezeAllMorphs()=0;
	//! play all morphs in a sequence
	virtual void SetLinearMorphSequence(f32 i) =0;
  //! instant morph applying (by default is false)
  virtual void SetInstantMorphsApply(bool value) =0;
};


//DOC-IGNORE-BEGIN
//! This interface is free from concepts specific for CryEngine
//DOC-IGNORE-END

// Description:
//     This interface hold a set of animation in which each animation is described as properties.
// Summary:
//     Hold description of a set of animation
struct IAnimationSet
{
	virtual ~IAnimationSet(){}
	//! Returns the index of the morph target in the set, -1 if there's no such morph target
	// Summary:
	//   Searches for morph target using a specified name.
	// Arguments:
	//   szMorphTarget - Name of the morph target to find.
	// Return Value:
	//   An integer representing the index of the morph target. The value -1 will 
	//   be returned in case that an appropriate morph target haven't been found.
	virtual int FindMorphTarget (const char* szMorphTarget) {return -1;}

	//! Returns the name of the morph target
	// Summary:
	//   Gets the name of a morph target.
	// Arguments:
	//   nMorphTargetId - Id of the morph target.
	// Return Value:
	//   A null terminated string holding the name of the morph target. In case 
	//   the specified id is out of range, the string 
	//   "!MORPH TARGET ID OUT OF RANGE!" is returned.
	virtual const char* GetNameMorphTarget (int nMorphTargetId) =0;

	//! Returns the number of morph targets in the set
	// Summary:
	//   Retrieves the amount of morph target.
	// Return Value: 
	//     An integer holding the amount of morth target
	virtual uint32 numMorphTargets() const =0;



	// Summary:
	//   Retrieves the amount of animation.
	// Return Value: 
	//     An integer holding the amount of animation
	virtual uint32 GetAnimationCount() const = 0;

	//! Returns the index of the animation in the set, -1 if there's no such animation
	// Summary:
	//   Searches for the index of an animation using its name.
	// Arguments:
	//   szAnimationName - Null terminated string holding the name of the animation.
	// Return Value:
	//   An integer representing the index of the animation. In case the animation 
	//   couldn't be found, -1 will be returned.
	virtual int GetAnimIDByName (const char* szAnimationName) const = 0;

	//! Returns the given animation name
	// Summary:
	//   Gets the name of the specified animation.
	// Arguments:
	//   nAnimationId - Id of an animation.
	// Return Value:
	//   A null terminated string holding the name of the animation. In case the 
	//   animation wasn't found, the string "!NEGATIVE ANIMATION ID!" will be 
	//   returned.
	virtual const char* GetNameByAnimID(int nAnimationId) = 0;

	virtual uint32 GetAnimationSize( const uint32 nAnimationId ) const = 0;
	virtual uint32 GetTotalPosKeys( const uint32 nAnimationId ) const = 0;
	virtual uint32 GetTotalRotKeys( const uint32 nAnimationId ) const = 0;
	virtual bool   ExportCAF2HTR(const char* szAnimationName, const char* savePath, const ISkeletonAnim* m_pSkeletonAnim)= 0;
	virtual const char* GetDBAFileName(const uint32 nAnimationId) const = 0;


	virtual int GetAnimIDByCRC(uint32 animationCRC) const = 0;
	virtual uint32 GetCRCByAnimID(int nAnimationId) const = 0;
	virtual uint32 GetFilePathCRCByAnimID(int nAnimationId) const = 0;


	//get file-path either by ID or by animation-name
	virtual const char* GetFilePathByName (const char* szAnimationName) const = 0;
	virtual const char* GetFilePathByID(int nAnimationId) const = 0;

	virtual int32 GetGlobalIDByName(const char* szAnimationName) = 0;
	virtual int32 GetGlobalIDByAnimID(int nAnimationId) = 0;

	//! Returns the given animation's start, in seconds; 0 if the id is invalid
	virtual f32 GetStart (int nAnimationId) {return 0;} // default implementation
	f32 GetStart (const char* szAnimationName) {	return GetStart (GetAnimIDByName(szAnimationName));}

	virtual f32 GetSpeed(int nAnimationId) = 0;
	virtual f32 GetSlope(int nAnimationId) = 0;
	virtual Vec2 GetMinMaxSpeedAsset_msec(int32 animID )=0;
	virtual LMGCapabilities GetLMGPropertiesByName( const char* animname, Vec2& vStrafeDirection, f32 fDesiredTurn, f32 fSlope ) =0;
	virtual f32  GetIWeightForSpeed(int nAnimationId, f32 Speed) = 0;

	virtual f32 GetDuration_sec(int nAnimationId) = 0;
	virtual uint32 GetAnimationFlags(int nAnimationId) = 0;
	virtual uint32 GetBlendSpaceCode(int nAnimationId) = 0;

	virtual CryAnimationPath GetAnimationPath(const char* szAnimationName) = 0;
	virtual CryAnimationPath GetAnimationPath( int AnimID ) = 0;

	virtual const QuatT& GetAnimationStartLocation(const char* szAnimationName) = 0;
	virtual const QuatT& GetAnimationStartLocation( int32 AnimID ) = 0;

	virtual const SAnimationSelectionProperties* GetAnimationSelectionProperties(const char* szAnimationName) = 0;
	virtual const SAnimationSelectionProperties* GetAnimationSelectionProperties( int32 localAnimID ) = 0;

	virtual const QuatT& GetJointStartLocation(const char* szAnimationName, int32 jointCRC32) = 0;

	virtual f32 GetClosestQuatInChannel(const char* szAnimationName,int32 JointID, const Quat& q) = 0;


	// Facial animation information
	virtual const char* GetFacialAnimationPathByName(const char* szName) = 0; // Returns 0 if name not found.
	virtual int GetNumFacialAnimations() = 0;
	virtual const char* GetFacialAnimationName(int index) = 0; // Returns 0 on invalid index.
	virtual const char* GetFacialAnimationPath(int index) = 0; // Returns 0 on invalid index.

	virtual const char *GetFnPAnimGroupName(int idx) = 0;

	virtual int LoadLMGFromMemory(const char* resourceName, const char* szAnimName, XmlNodeRef xmlData) = 0;
	virtual LmgAnimationStatus IsAnimationValidForLMG(const char* lmgBlendCode, const char* szAnimationName) = 0;
};

#include <IAttachment.h>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

struct SAnimationStatistics {
	const char * name;
	long count;
};

UNIQUE_IFACE struct IAnimEvents
{
	virtual ~IAnimEvents(){}
	//! Returns the number of installed anim-events for this asset
	virtual int32 GetAnimEventsCount_CAF(int nGlobalID ) const =0;
	virtual int32 GetAnimEventsCount_CAF(const char* pFilePath ) =0;
	virtual void AddAnimEvent_CAF(int nGlobalID, const char* pName, const char* pParameter, const char* pBone, f32 fTime, const Vec3& vOffset, const Vec3& vDir, const char* pModel) = 0;
	virtual void AddAnimEvent_LMG(int nGlobalID, const char* pName, const char* pParameter, const char* pBone, f32 fTime, const Vec3& vOffset, const Vec3& vDir, const char* pModel) = 0;

	virtual int GetGlobalAnimID_CAF(const char* pFilePath) = 0;
	virtual int GetGlobalAnimID_LMG(const char* pFilePath) = 0;
	virtual void DeleteAllEventsForCAF(int nGlobalID) = 0;
	virtual void DeleteAllEventsForLMG(int nGlobalID) = 0;

	virtual size_t GetGlobalAnimCount() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

	// Description:
	//		Experimental way to track interface version.
	//		This value will be compared with value passed from system module
	const char gAnimInterfaceVersion[64] = __TIMESTAMP__;

	// Description:
	//		CreateCryAnimation function type definition.
	typedef ICharacterManager * (*PFNCREATECRYANIMATION)(ISystem	* pSystem,const char * szInterfaceVersion);

	// Description:
	//     Create an instance of the Animation System. It should usually be called 
	//     by ISystem::InitAnimationSystem().
	// See Also:
	//     ICharacterManager, ICharacterManager::Release
	// Arguments:
	//     ISystem            - Pointer to the current ISystem instance
	//     szInterfaceVersion - String version of with the build date
	// Summary:
	//     Create an instance of the Animation System
	CRYANIMATION_API ICharacterManager * CreateCharManager(ISystem* pSystem, const char * szInterfaceVersion=gAnimInterfaceVersion);

#ifdef __cplusplus
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if defined(ENABLE_LW_PROFILERS) && !defined(__SPU__)
class CAnimationLightProfileSection
{
public:
	CAnimationLightProfileSection() 
	: m_nTicks( CryGetTicks() )
#   if EMBED_PHYSICS_AS_FIBER
		, m_nYields(NPPU::FiberYieldTime())
#   else 
		, m_nYields()
#   endif 
	{
#ifdef SNTUNER
		snPushMarker("Animation");
#endif
	}

	~CAnimationLightProfileSection() 
	{ 
		ICharacterManager *pCharacterManager = gEnv->pCharacterManager;
#   if EMBED_PHYSICS_AS_FIBER
		uint64 nYields = NPPU::FiberYieldTime(); 
#   else 
		uint64 nYields = 0ULL; 
#   endif 
		IF( pCharacterManager != NULL, 1)
		{
			pCharacterManager->AddFrameTicks((CryGetTicks()-m_nTicks)-(nYields-m_nYields));
		}
#ifdef SNTUNER
		snPopMarker();
#endif
	}
private:
	uint64 m_nTicks;
	uint64 m_nYields;
};


class CAnimationLightSyncProfileSection
{
public:
	CAnimationLightSyncProfileSection() 
		: m_nTicks( CryGetTicks() ) 
#   if EMBED_PHYSICS_AS_FIBER
		, m_nYields(NPPU::FiberYieldTime())
#   else 
		, m_nYields()
#   endif 
	{}
	~CAnimationLightSyncProfileSection() 
	{ 
		ICharacterManager *pCharacterManager = gEnv->pCharacterManager;
#   if EMBED_PHYSICS_AS_FIBER
		uint64 nYields = NPPU::FiberYieldTime(); 
#   else 
		uint64 nYields = 0ULL; 
#   endif 
		IF( pCharacterManager != NULL, 1)
		{
			pCharacterManager->AddFrameSyncTicks((CryGetTicks()-m_nTicks)-(nYields-m_nYields));
		}
	}
private:
	uint64 m_nTicks;
	uint64 m_nYields; 
};

#define ANIMATION_LIGHT_PROFILER() CAnimationLightProfileSection _animationLightProfileSection;
#define ANIMATION_LIGHT_SYNC_PROFILER() CAnimationLightSyncProfileSection _animationLightSyncProfileSection;
#else
#define ANIMATION_LIGHT_PROFILER()
#define ANIMATION_LIGHT_SYNC_PROFILER()
#endif

#endif // ICRY_ANIMATION
