#include "StdAfx.h"
#include "EntitySystemBinding.h"
#include <IEntitySystem.h>
//#include "MonoEntity.h"


CEntitySystemBinding::CEntitySystemBinding()
{
	//REGISTER_METHOD(_GetEntities);
}


CEntitySystemBinding::~CEntitySystemBinding()
{
}

MonoArray* CEntitySystemBinding::_GetEntities()
{
	/*MonoClass* pClass = CMonoClassUtils::GetClassByName("Cemono", "Entity");

	std::vector<IEntity*> entities;
	IEntityItPtr pIt = gEnv->pEntitySystem->GetEntityIterator();
	while (!pIt->IsEnd())
	{
		if (IEntity * pEnt = pIt->Next())
		{
			entities.push_back(pEnt);
		}
	}

	MonoArray* entityArray = mono_array_new(mono_domain_get(), pClass, entities.size());

	for(int i=0; i < entities.size(); i++)
	{
		IEntity* pEnt = entities.at(i);
		const char* className = pEnt->GetClass()->GetName();
		CMonoEntity* monoEntity = CMonoEntity::GetById(pEnt->GetId());

		if (monoEntity != NULL)
		{
			mono_array_set(entityArray, MonoObject*, i, monoEntity->GetMonoObject());
		} else {
			mono_array_set(entityArray, MonoObject*, i, CreateMonoEntity(pEnt));
		}

	}
	return entityArray;*/
	return NULL;
}


// UTIL
MonoObject* CEntitySystemBinding::CreateMonoEntity(IEntity *pEnt)
{
	/*if (pEnt)
	{
		uint id = pEnt->GetId();
		void* args[1];
		
		args[0] = &id;

		MonoAssembly *pAss = g_pMono->GetBclAssembly();

		MonoClass *pClass = mono_class_from_name(g_pMono->GetBclImage(), "Cemono", "Entity");
		MonoObject *pEntity = mono_object_new(mono_domain_get(), pClass);

		CMonoClassUtils::CallMethod(pEntity, ":.ctor(long)", args);
		
		return pEntity;
	} else {
		return NULL;
	}*/
	return NULL;
}
