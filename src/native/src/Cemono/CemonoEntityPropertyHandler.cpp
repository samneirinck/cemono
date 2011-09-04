#include "StdAfx.h"
#include "CemonoEntityPropertyHandler.h"


CCemonoEntityPropertyHandler::CCemonoEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties)
{
	m_properties = properties;
}


CCemonoEntityPropertyHandler::~CCemonoEntityPropertyHandler()
{
}

void CCemonoEntityPropertyHandler::GetMemoryUsage( ICrySizer *pSizer ) const
{

}
void CCemonoEntityPropertyHandler::RefreshProperties()
{

}
void CCemonoEntityPropertyHandler::LoadEntityXMLProperties(IEntity* entity, const XmlNodeRef& xml)
{

}
void CCemonoEntityPropertyHandler::LoadArchetypeXMLProperties(const char* archetypeName, const XmlNodeRef& xml)
{

}
void CCemonoEntityPropertyHandler::InitArchetypeEntity(IEntity* entity, const char* archetypeName, const SEntitySpawnParams& spawnParams)
{

}
int CCemonoEntityPropertyHandler::GetPropertyCount() const
{
	return m_properties.size();
}
bool CCemonoEntityPropertyHandler::GetPropertyInfo(int index, SPropertyInfo& info ) const
{
	info = m_properties.at(index);
	return true;
}
void CCemonoEntityPropertyHandler::SetProperty(IEntity* entity, int index, const char* value)
{

}
const char* CCemonoEntityPropertyHandler::GetProperty(IEntity* entity, int index) const
{
	return "";
}
const char* CCemonoEntityPropertyHandler::GetDefaultProperty(int index) const
{
	return "";
}
void CCemonoEntityPropertyHandler::PropertiesChanged(IEntity* entity)
{

}
