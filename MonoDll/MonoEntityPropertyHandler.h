/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Entity property handler used for mono entities.
//////////////////////////////////////////////////////////////////////////
// 08/12/2011 : Created by Filip 'i59' Lundgren (Based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ENTITY_PROPERTY_HANDLER_H__
#define __MONO_ENTITY_PROPERTY_HANDLER_H__

#include <IEntityClass.h>

#include "MonoEntity.h"

class CEntityPropertyHandler : public IEntityPropertyHandler
{
public:
	CEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CEntityPropertyHandler() {} 

	// IEntityPropertyHandler interface
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const { pSizer->Add(m_properties); }
	virtual void RefreshProperties() {}
	virtual void LoadEntityXMLProperties(IEntity* entity, const XmlNodeRef& xml) {}
	virtual void LoadArchetypeXMLProperties(const char* archetypeName, const XmlNodeRef& xml) {}
	virtual void InitArchetypeEntity(IEntity* entity, const char* archetypeName, const SEntitySpawnParams& spawnParams) {}
	virtual int GetPropertyCount() const;
	virtual bool GetPropertyInfo(int index, SPropertyInfo& info ) const;
	virtual void SetProperty(IEntity* entity, int index, const char* value);
	virtual const char* GetProperty(IEntity* entity, int index) const;
	virtual const char* GetDefaultProperty(int index) const { return ""; }
	virtual void PropertiesChanged(IEntity* entity) {}
	// -IEntityPropertyHandler

	SQueuedProperty *GetQueuedProperties(EntityId id, int &numProperties);

protected:

	typedef std::map<EntityId, DynArray<SQueuedProperty>> TQueuedPropertyMap;
	TQueuedPropertyMap m_queuedProperties;
	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;
};

#endif //__MONO_ENTITY_PROPERTY_HANDLER_H__