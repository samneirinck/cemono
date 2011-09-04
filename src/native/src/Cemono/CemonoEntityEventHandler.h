#pragma once
#include <IEntityClass.h>

class CCemonoEntityEventHandler : public IEntityEventHandler
{
public:
	CCemonoEntityEventHandler();
	virtual ~CCemonoEntityEventHandler();

	// IEntityEventHandler interface
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const;
	virtual void RefreshEvents();
	virtual void LoadEntityXMLEvents(IEntity* entity, const XmlNodeRef& xml);
	virtual int GetEventCount() const;
	virtual bool GetEventInfo(int index, SEventInfo& info ) const;
	virtual void SendEvent(IEntity* entity, const char* eventName);
	// -IEntityEventHandler

protected:
};

