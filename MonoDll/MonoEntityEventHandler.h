#ifndef __MONO_ENTITY_EVENT_HANDLER_H__
#define __MONO_ENTITY_EVENT_HANDLER_H__

#include <IEntityClass.h>

class CMonoEntityEventHandler : public IEntityEventHandler
{
public:
	CMonoEntityEventHandler() {}
	virtual ~CMonoEntityEventHandler() {}

	// IEntityEventHandler interface
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const {}
	virtual void RefreshEvents() {}
	virtual void LoadEntityXMLEvents(IEntity* entity, const XmlNodeRef& xml) {}
	virtual int GetEventCount() const { return 0; }
	virtual bool GetEventInfo(int index, SEventInfo& info ) const { return true; }
	virtual void SendEvent(IEntity* entity, const char* eventName) {}
	// -IEntityEventHandler
};

#endif //__MONO_ENTITY_EVENT_HANDLER_H__