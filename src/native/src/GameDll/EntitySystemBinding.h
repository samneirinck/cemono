#pragma once
#include "MonoAPIBinding.h"
#include "Mono.h"

class CEntitySystemBinding : public MonoAPIBinding
{
public:
	CEntitySystemBinding();
	virtual ~CEntitySystemBinding();

protected:
	virtual const char* GetClassName() { return "EntitySystem"; }
	virtual const char* GetNamespaceExtension() { return "API"; }

	static MonoArray* _GetEntities();
	static MonoObject* CreateMonoEntity(IEntity *pEnt);


};

