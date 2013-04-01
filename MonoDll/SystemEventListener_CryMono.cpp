#include "StdAfx.h"
#include "SystemEventListener_CryMono.h"

#include "MonoScriptSystem.h"

#include <Windows.h>

void CSystemEventListener_CryMono::OnSystemEvent(ESystemEvent event, UINT_PTR wParam, UINT_PTR lParam)
{
	switch(event)
	{
	case ESYSTEM_EVENT_CHANGE_FOCUS:
		{
			if(g_pScriptSystem == nullptr)
				return;

			if(wParam != 0 && g_pScriptSystem->DetectedChanges() && GetFocus())
				g_pScriptSystem->Reload();
		}
		break;
	case ESYSTEM_EVENT_SHUTDOWN:
		{
			SAFE_RELEASE(g_pScriptSystem);
		}
		break;
	}
}