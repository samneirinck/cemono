#include "StdAfx.h"
#include "MonoEntityPropertyHandler.h"

#include "MonoCommon.h"

#include <IMonoScriptSystem.h>
#include <IMonoEntityManager.h>

#include <IMonoObject.h>

#include <IEntitySystem.h>

CMonoEntityPropertyHandler::CMonoEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties)
{
	m_properties = properties;
}

int CMonoEntityPropertyHandler::GetPropertyCount() const
{
	return m_properties.size();
}

bool CMonoEntityPropertyHandler::GetPropertyInfo(int index, SPropertyInfo& info ) const
{
	info = m_properties.at(index);
	return true;
}

void CMonoEntityPropertyHandler::SetProperty(IEntity *entity, int index, const char *value)
{
	IEntityPropertyHandler::SPropertyInfo propertyInfo = m_properties.at(index);
	CallMonoScript(gEnv->pMonoScriptSystem->GetEntityManager()->GetScriptId(entity->GetId(), true), "SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
}

const char *CMonoEntityPropertyHandler::GetProperty(IEntity *entity, int index) const
{
	return CallMonoScript(gEnv->pMonoScriptSystem->GetEntityManager()->GetScriptId(entity->GetId()), "GetPropertyValue", m_properties.at(index).name)->Unbox<const char *>();
}