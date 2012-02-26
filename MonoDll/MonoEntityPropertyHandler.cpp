#include "StdAfx.h"
#include "MonoEntityPropertyHandler.h"

#include "MonoCommon.h"

#include <IMonoScriptSystem.h>
#include <IMonoEntityManager.h>

#include <IMonoObject.h>

#include <IEntitySystem.h>

CEntityPropertyHandler::CEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties)
{
	m_properties = properties;
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

void CEntityPropertyHandler::SetProperty(IEntity *entity, int index, const char *value)
{
	IEntityPropertyHandler::SPropertyInfo propertyInfo = m_properties.at(index);
	CallMonoScript<void>(gEnv->pMonoScriptSystem->GetEntityManager()->GetScriptId(entity->GetId(), true), "SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
}

const char *CEntityPropertyHandler::GetProperty(IEntity *entity, int index) const
{
	return CallMonoScript<const char *>(gEnv->pMonoScriptSystem->GetEntityManager()->GetScriptId(entity->GetId()), "GetPropertyValue", m_properties.at(index).name);
}