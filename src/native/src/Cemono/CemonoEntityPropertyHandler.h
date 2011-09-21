#pragma once

#include <IEntityClass.h>

class CCemonoEntityPropertyHandler : public IEntityPropertyHandler
{
public:
	CCemonoEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CCemonoEntityPropertyHandler();

	// IEntityPropertyHandler interface
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const override;
	virtual void RefreshProperties() override;
	virtual void LoadEntityXMLProperties(IEntity* entity, const XmlNodeRef& xml) override;
	virtual void LoadArchetypeXMLProperties(const char* archetypeName, const XmlNodeRef& xml) override;
	virtual void InitArchetypeEntity(IEntity* entity, const char* archetypeName, const SEntitySpawnParams& spawnParams) override;
	virtual int GetPropertyCount() const override;
	virtual bool GetPropertyInfo(int index, SPropertyInfo& info ) const override;
	virtual void SetProperty(IEntity* entity, int index, const char* value) override;
	virtual const char* GetProperty(IEntity* entity, int index) const override;
	virtual const char* GetDefaultProperty(int index) const override;
	virtual void PropertiesChanged(IEntity* entity) override;
	// -IEntityPropertyHandler

protected:
	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;
};

