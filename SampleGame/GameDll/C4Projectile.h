/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id:$
$DateTime$
Description:  C4 projectile specific stuff
-------------------------------------------------------------------------
History:
- 08:06:2007   : Created by Benito G.R.

*************************************************************************/

#ifndef __C4PROJECTILE_H__
#define __C4PROJECTILE_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include "Projectile.h"

#define MAX_STICKY_POINTS					3

class CC4Projectile : public CProjectile
{
public:
	CC4Projectile();
	virtual ~CC4Projectile();

	virtual void HandleEvent(const SGameObjectEvent &event);
	virtual void Launch(const Vec3 &pos, const Vec3 &dir, const Vec3 &velocity, float speedScale);
	virtual void Explode(bool destroy, bool impact/* =false */, const Vec3 &pos/* =ZERO */, const Vec3 &normal/* =FORWARD_DIRECTION */, const Vec3 &vel/* =ZERO */, EntityId targetId/* =0  */);
	virtual void OnHit(const HitInfo& hit);

	struct ProjectileStaticParams
	{
		ProjectileStaticParams(){pos.Set(0,0,0); rot.SetIdentity();}
		ProjectileStaticParams(Vec3 &_pos, Quat &_rot): pos(_pos), rot(_rot) {}

		Vec3 pos;
		Quat rot;

		void SerializeWith(TSerialize ser)
		{
			ser.Value("c4Pos",pos);
			ser.Value("c4Rot",rot);
		}
	};

	struct ProjectileStickToEntity
	{
		ProjectileStickToEntity():targetId(0) {localCollisonPos.Set(0,0,0); localRotation.SetIdentity();}
		ProjectileStickToEntity(EntityId id, Vec3 &pos, Quat &r):targetId(id),localCollisonPos(pos),localRotation(r) {}

		EntityId targetId;
		Vec3 localCollisonPos;
		Quat localRotation;

		void SerializeWith(TSerialize ser)
		{
			ser.Value("targetId",targetId,'eid');
			ser.Value("localPos",localCollisonPos);
			ser.Value("localRot",localRotation);
		}
		
	};	

	DECLARE_CLIENT_RMI_NOATTACH(ClSetPosition, ProjectileStaticParams, eNRT_ReliableUnordered);
	DECLARE_CLIENT_RMI_NOATTACH(ClStickToEntity, ProjectileStickToEntity, eNRT_ReliableUnordered);

private:

	void Stick(EventPhysCollision *pCollision);
	void StickToStaticObject(EventPhysCollision *pCollision, IPhysicalEntity* pTarget);
	void StickToEntity(IEntity* pEntity, Matrix34 &localMatrix);
	bool StickToCharacter(bool stick, IEntity* pActor);
	
	virtual void SetParams(EntityId ownerId, EntityId hostId, EntityId weaponId, int damage, int hitTypeId, float damageDrop = 0.0f, float damageDropMinR = 0.0f);

	int				m_teamId;
	bool			m_stuck;
	bool      m_notStick;
	bool      m_frozen;

	int       m_nConstraints;
	int       m_constraintIds[MAX_STICKY_POINTS];

};

#endif