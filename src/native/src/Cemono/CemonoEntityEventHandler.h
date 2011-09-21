#pragma once
#include <IEntityClass.h>

class CCemonoEntityEventHandler : public IEntityEventHandler
{
public:
	CCemonoEntityEventHandler();
	virtual ~CCemonoEntityEventHandler();

	// IEntityEventHandler interface
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const override;
	virtual void RefreshEvents() override;
	virtual void LoadEntityXMLEvents(IEntity* entity, const XmlNodeRef& xml) override;
	virtual int GetEventCount() const override;
	virtual bool GetEventInfo(int index, SEventInfo& info ) const override;
	virtual void SendEvent(IEntity* entity, const char* eventName) override;
	// -IEntityEventHandler

protected:
};

