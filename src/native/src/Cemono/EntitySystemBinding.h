#pragma once

#include "BaseCemonoClassBinding.h"

class CEntitySystemBinding : public BaseCemonoClassBinding
{
public:
	CEntitySystemBinding();
	virtual ~CEntitySystemBinding();

protected:
	virtual const char* GetClassName() { return "EntitySystem"; }

	static MonoArray* _GetEntities();
	static MonoObject* CreateMonoEntity(IEntity *pEnt);


};

