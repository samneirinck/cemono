////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   bugsflock.h
//  Version:     v1.00
//  Created:     11/4/2003 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __Frogboids_h__
#define __Frogboids_h__
#pragma once

#include "Flock.h"

//////////////////////////////////////////////////////////////////////////
class CFrogBoid : public CBoidBird
{
public:
	CFrogBoid( SBoidContext &bc );
	virtual void Update( float dt,SBoidContext &bc );
	virtual void Think( float dt,SBoidContext &bc );
	virtual void Kill( const Vec3 &hitPoint,const Vec3 &force );
	virtual void Physicalize( SBoidContext &bc );
	virtual void OnPickup( bool bPickup,float fSpeed );
	virtual void OnCollision( SEntityEvent &event );

protected:
	float m_maxIdleTime;
	Vec3 m_avoidanceAccel;
	bool m_bThrown;
	float m_fTimeToNextJump;
};

//////////////////////////////////////////////////////////////////////////
// Frog Flock, is a specialized flock type for Frogs.
//////////////////////////////////////////////////////////////////////////
class CFrogFlock : public CFlock
{
public:
	CFrogFlock( IEntity *pEntity );
	virtual void CreateBoids( SBoidsCreateContext &ctx );
	virtual bool CreateEntities();
};

#endif // __Frogboids_h__
