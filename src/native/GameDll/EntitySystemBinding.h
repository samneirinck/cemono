#pragma once
#include "IMonoAPIBinding.h"
#include "Mono.h"

class CEntitySystemBinding : public IMonoAPIBinding
{
public:
	CEntitySystemBinding();
	virtual ~CEntitySystemBinding();

protected:
	static MonoArray* _GetEntities();
	static MonoObject* CreateMonoEntity(IEntity *pEnt);


};

