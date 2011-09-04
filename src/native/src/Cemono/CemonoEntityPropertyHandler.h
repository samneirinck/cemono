#pragma once

#include <IEntityClass.h>

class CCemonoEntityPropertyHandler : public IEntityPropertyHandler
{
public:
	CCemonoEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CCemonoEntityPropertyHandler();

	// IEntityPropertyHandler interface
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const;
	virtual void RefreshProperties();
	virtual void LoadEntityXMLProperties(IEntity* entity, const XmlNodeRef& xml);
	virtual void LoadArchetypeXMLProperties(const char* archetypeName, const XmlNodeRef& xml);
	virtual void InitArchetypeEntity(IEntity* entity, const char* archetypeName, const SEntitySpawnParams& spawnParams);
	virtual int GetPropertyCount() const;
	virtual bool GetPropertyInfo(int index, SPropertyInfo& info ) const;
	virtual void SetProperty(IEntity* entity, int index, const char* value);
	virtual const char* GetProperty(IEntity* entity, int index) const;
	virtual const char* GetDefaultProperty(int index) const;
	virtual void PropertiesChanged(IEntity* entity);
	// -IEntityPropertyHandler

protected:
	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;
};

