#ifndef ILeanPoseModifier_h
#define ILeanPoseModifier_h

#include <ICryAnimation.h>

UNIQUE_IFACE struct ILeanPoseModifier 
	: public IAnimationPoseModifier
{
	enum
	{
		LeanJointCount = 3
	};

	CRYINTERFACE_DECLARE( ILeanPoseModifier, 0x871d994a919c49d4, 0x960bf9817d5d393c );

	virtual void Clear() = 0;
	virtual void SetJointLeanAngle( const uint32 index, const uint32 jointId, const float angleRadians ) = 0;
};

DECLARE_BOOST_POINTERS( ILeanPoseModifier );

#endif
