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

	if(IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->GetEntityManager()->GetScript(entity->GetId(), true))
		CallMonoScript<void>(pScriptClass, "SetPropertyValue", propertyInfo.name, propertyInfo.type, value);
}

const char *CEntityPropertyHandler::GetProperty(IEntity *entity, int index) const
{
	if(IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->GetEntityManager()->GetScript(entity->GetId()))
		return CallMonoScript<const char *>(pScriptClass, "GetPropertyValue", m_properties.at(index).name);

	return "";
}