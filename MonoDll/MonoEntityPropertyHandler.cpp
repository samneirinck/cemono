#include "StdAfx.h"
#include "MonoEntityPropertyHandler.h"

#include "EntityManager.h"
#include "MonoEntity.h"

#include <MonoCommon.h>
#include <IMonoScriptSystem.h>
#include <IMonoObject.h>

#include <IEntitySystem.h>

CEntityPropertyHandler::CEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties)
	: m_properties(properties)
{
}

int CEntityPropertyHandler::GetPropertyCount() const
{
	return m_properties.size();
}

bool CEntityPropertyHandler::GetPropertyInfo(int index, SPropertyInfo& info ) const
{
	info = m_properties.at(index);
	return true;
}

void CEntityPropertyHandler::SetProperty(IEntity *pEntity, int index, const char *value)
{
	if(auto entity = static_cast<CEntityManager *>(gEnv->pMonoScriptSystem->GetEntityManager())->GetMonoEntity(pEntity->GetGuid()))
		entity->SetPropertyValue(m_properties.at(index), value);
}

const char *CEntityPropertyHandler::GetProperty(IEntity *pEntity, int index) const
{
	//if(IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->GetEntityManager()->GetScript(pEntity->GetId()))
		//return CallMonoScript<const char *>(pScriptClass, "GetPropertyValue", m_properties.at(index).name);

	return "";
}