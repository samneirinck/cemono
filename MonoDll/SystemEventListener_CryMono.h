#ifndef __SYSTEM_EVENT_LISTENER_CRYMONO__
#define __SYSTEM_EVENT_LISTENER_CRYMONO__

#include <ISystem.h>

class CSystemEventListener_CryMono : public ISystemEventListener
{
	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);
	// ~ISystemEventListener
};

static CSystemEventListener_CryMono g_systemEventListener_CryMono;

#endif // __SYSTEM_EVENT_LISTENER_CRYMONO__