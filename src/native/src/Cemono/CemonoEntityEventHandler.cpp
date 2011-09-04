#include "StdAfx.h"
#include "CemonoEntityEventHandler.h"


CCemonoEntityEventHandler::CCemonoEntityEventHandler()
{
}


CCemonoEntityEventHandler::~CCemonoEntityEventHandler()
{
}

void CCemonoEntityEventHandler::GetMemoryUsage( ICrySizer *pSizer ) const
{
}
void CCemonoEntityEventHandler::RefreshEvents()
{
}

void CCemonoEntityEventHandler::LoadEntityXMLEvents(IEntity* entity, const XmlNodeRef& xml)
{
}
int CCemonoEntityEventHandler::GetEventCount() const
{
	return 0;
}
bool CCemonoEntityEventHandler::GetEventInfo(int index, SEventInfo& info ) const
{
	return true;
}
void CCemonoEntityEventHandler::SendEvent(IEntity* entity, const char* eventName)
{

}
