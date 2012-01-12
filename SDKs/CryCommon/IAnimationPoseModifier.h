////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2009.
// -------------------------------------------------------------------------
//  File name:   AnimationPoseModifier.h
//  Version:     v1.00
//  Created:     30/7/2009 by Ivo Frey
//  Compilers:   Visual Studio.NET
//  Description: CryAnimation interfaces
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(IAnimationPoseModifier.h)
#ifndef AnimationPoseModifier_h
#define AnimationPoseModifier_h

#include <CryExtension/ICryUnknown.h>

//

struct IAnimationPoseModifier;

//

struct SAnimationPoseModiferParams
{
	ICharacterInstance* pCharacterInstance;

	f32 timeDelta;

	QuatT locationNextPhysics;
	QuatTS locationNextAnimation;

	SPU_DOMAIN_LOCAL QuatT* pPoseRelative;
	SPU_DOMAIN_LOCAL QuatT* pPoseAbsolute;
	uint32 jointCount;

	ILINE ISkeletonPose* GetISkeletonPose() const { return pCharacterInstance->GetISkeletonPose(); }
	ILINE ISkeletonAnim* GetISkeletonAnim() const  { return pCharacterInstance->GetISkeletonAnim(); }

	ILINE ICharacterModel* GetICharacterModel() const { return pCharacterInstance->GetICharacterModel(); }
	ILINE ICharacterModelSkeleton* GetICharacterModelSkeleton() const { return pCharacterInstance->GetICharacterModel()->GetICharacterModelSkeleton(); }
};

//

struct IAnimationPoseModifier :
	public ICryUnknown
{
	CRYINTERFACE_DECLARE(IAnimationPoseModifier, 0x22fe47755e42447f, 0xbab6274ed39af449)

	// Called from the main thread before the Pose Modifier is added to the
	// Command Buffer. Pose data will not be available at this stage.
	virtual bool Prepare(const SAnimationPoseModiferParams& params) = 0;

	// Called from an arbitrary worker thread when the Command associated with
	// this Pose Modifier is executed. Pose data is available for read/write.
	// NOTE: On PS3 the worker thread is effectively a SPU job.
	virtual bool Execute(const SAnimationPoseModiferParams& params) = 0;

	// Called from the main thread after the Command Buffer this Pose Modifier
	// was part of finished its execution.
	virtual void Synchronize() = 0;

	virtual void GetMemoryUsage(ICrySizer* pSizer) const = 0;
};

DECLARE_BOOST_POINTERS(IAnimationPoseModifier);

//

struct IAnimationPoseBlenderDir :
	public IAnimationPoseModifier
{
	CRYINTERFACE_DECLARE(IAnimationPoseBlenderDir, 0x1725a49dbd684ff4, 0x852cd0d4b7f86c28)

	enum EArm
	{
		eArm_Left = 1<<0,
		eArm_Right = 1<<1,
	};

	virtual void SetState(bool state)=0;
	virtual void SetTarget(const Vec3& target) = 0;
	virtual void SetLayer(uint32 nLayer)=0;
	virtual void SetBlendTime(f32 time) = 0;
	virtual void SetFadeoutAngle(f32 a) = 0;
	virtual void SetFadeOutTime(f32 time) = 0;
	virtual void SetTargetSmoothTime(f32 time) = 0;
	virtual void SetPolarCoordinates(const Vec2& pc) = 0;
	virtual f32 GetBlend() = 0;
};

DECLARE_BOOST_POINTERS(IAnimationPoseBlenderDir);

//

UNIQUE_IFACE struct IAnimationGroundAlignment :
	public IAnimationPoseModifier
{
	CRYINTERFACE_DECLARE(IAnimationGroundAlignment, 0xb8bf63b98d304d7b,	0xaaa5fbdf665715b2)

	virtual void SetData(const bool bAlignSkeletonVertical, const f32 rootHeight, const Plane& planeLeft, const Plane& planeRight) = 0;
};

DECLARE_BOOST_POINTERS(IAnimationGroundAlignment);

#endif // AnimationPoseModifier_h
