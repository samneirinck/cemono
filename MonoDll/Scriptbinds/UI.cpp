#include "StdAfx.h"
#include "UI.h"

#include <IMonoAssembly.h>
#include <MonoAnyValue.h>
#include "MonoArray.h"

MonoAnyValue UIDTToMAV(const TUIData &d)
{
	switch (d.GetType())
	{
	case eUIDT_Int:
		return MonoAnyValue(*d.GetPtr<int>());
	case eUIDT_Float:
		return MonoAnyValue(*d.GetPtr<float>());
	case eUIDT_EntityId:
		return MonoAnyValue((unsigned int)(*d.GetPtr<EntityId>()));
	case eUIDT_Vec3:
		return MonoAnyValue(*d.GetPtr<Vec3>());
	case eUIDT_String:
		return MonoAnyValue(*d.GetPtr<string>());
	case eUIDT_WString:
		return MonoAnyValue(*d.GetPtr<wstring>());
	}

	return MonoAnyValue();
}

TUIData MAVToUIDT(const MonoAnyValue &d)
{
	switch (d.type)
	{
	case MONOTYPE_BOOL:
		return TUIData(d.b);
	case MONOTYPE_FLOAT:
		return TUIData(d.f);
	case MONOTYPE_INT:
	case MONOTYPE_SHORT:
		return TUIData(d.i);
	case MONOTYPE_UINT:
	case MONOTYPE_USHORT:
		return TUIData(d.u);
	case MONOTYPE_STRING:
		return TUIData(string(d.str));
	case MONOTYPE_WSTRING:
		return TUIData(wstring(d.wstr));
	case MONOTYPE_VEC3:
		return TUIData(Vec3(d.vec3.x, d.vec3.y, d.vec3.z));
	}

	return TUIData(false);
}

CScriptbind_UI	*CScriptbind_UI::s_pInstance = NULL;

CUICallback::CUICallback(const char *_name, CScriptbind_UI *pParent, IUIEventSystem *pUIEventSystem, IUIEventSystem::EEventSystemType type)
	: m_name(_name)
	, m_pParent(pParent)
	, m_pSystem(pUIEventSystem)
	, m_type(type)
{
	//Only listen for UI to system events
	if (m_type == IUIEventSystem::eEST_UI_TO_SYSTEM)
		m_pSystem->RegisterListener(this, "CUICallback");
}

CUICallback::~CUICallback()
{
	if (m_type == IUIEventSystem::eEST_UI_TO_SYSTEM)
		m_pSystem->UnregisterListener(this);
}

const char* CUICallback::FindEvent(uint ID)
{
	const SUIEventDesc *pDesc;

	for (int i = 0; i < m_pSystem->GetEventCount(); i++)
	{
		pDesc = m_pSystem->GetEventDesc(i);
		if (pDesc && ID == m_pSystem->GetEventId(pDesc->sName))
			return pDesc->sName;
	}

	return NULL;
}

int CUICallback::FindEvent(const char *pName)
{
	return (int)m_pSystem->GetEventId(pName);
}

void CUICallback::OnEvent(const SUIEvent& event)
{
	const char *eventName = FindEvent(event.event);
	if (!eventName)
		eventName = "<Unknown>";

	m_pParent->OnEvent(m_name.c_str(), eventName, event);
}

CScriptbind_UI::CScriptbind_UI()
{
	REGISTER_METHOD(RegisterEvent);
	REGISTER_METHOD(RegisterToEventSystem);
	REGISTER_METHOD(UnregisterFromEventSystem);
	REGISTER_METHOD(SendEvent);
	REGISTER_METHOD(SendNamedEvent);

	s_pInstance = this;
}

CScriptbind_UI::~CScriptbind_UI()
{
	if(s_pInstance == this)
		s_pInstance = NULL;
	
	m_EventMapS2UI.clear();
	m_EventMapUI2S.clear();
}

CUICallback *CScriptbind_UI::GetOrCreateSystem(const char *s, IUIEventSystem::EEventSystemType type)
{
	if (!gEnv->pFlashUI)
		return NULL;
	
	if (CUICallback *pCB = (type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[s])
		return pCB;

	IUIEventSystem *pSystem = gEnv->pFlashUI->GetEventSystem(s, type);
	if (!pSystem)
	{
		pSystem = gEnv->pFlashUI->CreateEventSystem(s, type);
		if (!pSystem)
			return NULL;
	}

	CUICallback *pCB = new CUICallback(s, this, pSystem, type);
	if (!pCB)
		return NULL;

	(type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[s] = pCB;

	return pCB;
}

bool CScriptbind_UI::SystemExists(const char *pName, IUIEventSystem::EEventSystemType type)
{
	return ((type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[pName]) != NULL;
}


CUICallback *CScriptbind_UI::FindSystem(const char *pName, IUIEventSystem::EEventSystemType type)
{
	return ((type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[pName]);
}

void CScriptbind_UI::RemoveSystem(CUICallback *pCB)
{
	(pCB->GetEventSystemType() == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI).erase(pCB->GetEventSystemName());

	delete pCB;
}

void CScriptbind_UI::OnEvent(const char *SystemName, const char *EventName, const SUIEvent& event)
{
	IMonoArray *pArgs = CreateMonoArray(event.args.GetArgCount());

	if (!pArgs)
		return;

	IMonoArray *pArray = CreateMonoArray(4);

	if (!pArray)
		return pArgs->Release();

	for (int i = 0; i < pArgs->GetSize(); i++)
		pArgs->Insert(UIDTToMAV(event.args.GetArg(i)));

	pArray->Insert(SystemName);
	pArray->Insert(EventName);
	pArray->Insert((int)event.event);
	pArray->Insert(pArgs);
	GetClass()->CallMethod("OnEvent", pArray, true);
	pArray->Release();
	pArgs->Release();
}

int CScriptbind_UI::RegisterEvent(mono::string eventsystem, IUIEventSystem::EEventSystemType direction, SMonoUIEventDesc desc)
{
	if (!gEnv->pFlashUI)
		return -1;

	CUICallback *pCB = s_pInstance->GetOrCreateSystem(ToCryString(eventsystem), direction);
	if (!pCB)
		return -1;

	//Already have an event with this name?
	if (pCB->GetEventSystem()->GetEventDesc(ToCryString(desc.Name)))
		return -1;

	SUIEventDesc uidesc(ToCryString(desc.Name), ToCryString(desc.DisplayName), ToCryString(desc.Description), desc.IsDynamic, ToCryString(desc.DynamicName), ToCryString(desc.DynamicDescription));
	if (desc.Params)
	{
		CScriptArray *pArray = new CScriptArray(desc.Params);
		if (!pArray)
			return -1;

		for (int i = 0; i < pArray->GetSize(); i++)
		{
			IMonoObject *pObject = pArray->GetItem(i);
			if (!pObject)
				return delete pArray, -1;

			SMonoUIParameterDesc param = pObject->Unbox<SMonoUIParameterDesc>();
			uidesc.Params.push_back(SUIParameterDesc(ToCryString(param.Name), ToCryString(param.DisplayName), ToCryString(param.Description), param.Type));
		}
		delete pArray;
	}

	return (int)pCB->GetEventSystem()->RegisterEvent(uidesc);
}

bool CScriptbind_UI::RegisterToEventSystem(mono::string eventsystem, IUIEventSystem::EEventSystemType type)
{
	if (!gEnv->pFlashUI || !s_pInstance)
		return false;

	if (s_pInstance->SystemExists(ToCryString(eventsystem), type))
		return true;

	return s_pInstance->GetOrCreateSystem(ToCryString(eventsystem), type) != NULL;
}

void CScriptbind_UI::UnregisterFromEventSystem(mono::string eventsystem, IUIEventSystem::EEventSystemType type)
{
	if (!gEnv->pFlashUI || !s_pInstance)
		return;

	if (CUICallback *pCB = s_pInstance->FindSystem(ToCryString(eventsystem), type))
		s_pInstance->RemoveSystem(pCB);
}


void CScriptbind_UI::SendEvent(mono::string eventsystem, int event, mono::array args)
{
	if (!s_pInstance || !args)
		return;

	CUICallback *pCB = s_pInstance->FindSystem(ToCryString(eventsystem), IUIEventSystem::eEST_SYSTEM_TO_UI);
	if (!pCB)
		return;

	CScriptArray *pArray = new CScriptArray(args);
	if (!pArray)
		return;

	SUIArguments uiargs;
	for (int i = 0; i < pArray->GetSize(); i++)
	{
		IMonoObject *pObject = pArray->GetItem(i);
		if (!pObject)
		{
			delete pArray;
			return;
		}

		uiargs.AddArgument(MAVToUIDT(pObject->GetAnyValue()));
	}

	delete pArray;

	pCB->GetEventSystem()->SendEvent(SUIEvent((uint)event, uiargs));
}

void CScriptbind_UI::SendNamedEvent(mono::string eventsystem, mono::string event, mono::array args)
{
	if (!s_pInstance)
		return;

	CUICallback *pCB = s_pInstance->FindSystem(ToCryString(eventsystem), IUIEventSystem::eEST_SYSTEM_TO_UI);
	if (!pCB)
		return;

	SendEvent(eventsystem, pCB->FindEvent(ToCryString(event)), args);
}