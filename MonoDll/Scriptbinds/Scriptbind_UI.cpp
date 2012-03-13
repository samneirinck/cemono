#include "StdAfx.h"
#include "Scriptbind_UI.h"

#include <IMonoAssembly.h>
#include <MonoAnyValue.h>
#include "MonoArray.h"

MonoAnyValue UIDTToMAV(const TUIData &d){

	switch (d.GetType()){
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

TUIData MAVToUIDT(const MonoAnyValue &d){
	switch (d.type){
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

class CMonoSUIEvent {
public:
	int	Event;
	mono::array	Args;
	ILINE CMonoSUIEvent(){
		Event = 0;
		Args = NULL;
	}
	ILINE ~CMonoSUIEvent(){
	}
};

CScriptbind_UI*	CScriptbind_UI::s_pInstance = NULL;

CScriptbind_UI::CUICallback::CUICallback(const char* name, CScriptbind_UI* parent, IUIEventSystem*system, IUIEventSystem::EEventSystemType type){
	Name = name;
	Parent = parent;
	System = system;
	Type = type;
	//Only listen for UI to system events
	if (Type == IUIEventSystem::eEST_UI_TO_SYSTEM)
		system->RegisterListener(this, "CUICallback");
}

CScriptbind_UI::CUICallback::~CUICallback(){
	if (Type == IUIEventSystem::eEST_UI_TO_SYSTEM)
		System->UnregisterListener(this);
}

const char* CScriptbind_UI::CUICallback::FindEvent(uint ID){
	int i, c;
	const SUIEventDesc* pDesc;
	c = System->GetEventCount();
	for (i = 0; i < c; i++){
		pDesc = System->GetEventDesc(i);
		if (pDesc && ID == System->GetEventId(pDesc->sName))
			return pDesc->sName;
	}
	return NULL;
}

int CScriptbind_UI::CUICallback::FindEvent(const char* pName){
	uint ID;
	ID = System->GetEventId(pName);
	return (int)ID;
}

void CScriptbind_UI::CUICallback::OnEvent(const SUIEvent& event){
	const char* pName;
	pName = FindEvent(event.event);
	if (!pName)
		pName = "<Unknown>";
	Parent->OnEvent(Name.c_str(), pName, event);
}


CScriptbind_UI::CUICallback* CScriptbind_UI::GetOrCreateSystem(const char* pName, IUIEventSystem::EEventSystemType type){
	IUIEventSystem* pSystem;
	CUICallback* pCB;
	string s;

	if (!gEnv->pFlashUI)
		return NULL;
	
	s = pName;
	
	pCB = (type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[s];
	if (pCB)
		return pCB;


	pSystem = gEnv->pFlashUI->GetEventSystem(pName, type);
	if (!pSystem){
		pSystem = gEnv->pFlashUI->CreateEventSystem(pName, type);
		if (!pSystem)
			return NULL;
	}
	pCB = new CUICallback(pName, this, pSystem, type);
	if (!pCB)
		return NULL;
	(type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[s] = pCB;
	return pCB;
}

bool CScriptbind_UI::SystemExists(const char* pName, IUIEventSystem::EEventSystemType type){
	return ((type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[pName]) != NULL;
}


CScriptbind_UI::CUICallback* CScriptbind_UI::FindSystem(const char* pName, IUIEventSystem::EEventSystemType type){
	return ((type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI)[pName]);
}

void CScriptbind_UI::RemoveSystem(CUICallback* pCB){
	(pCB->Type == IUIEventSystem::eEST_UI_TO_SYSTEM ? m_EventMapUI2S : m_EventMapS2UI).erase(pCB->Name);
	delete pCB;
}

CScriptbind_UI::CScriptbind_UI()
{
	s_pInstance = this;
	REGISTER_METHOD(RegisterEvent);
	REGISTER_METHOD(RegisterToEventSystem);
	REGISTER_METHOD(UnregisterFromEventSystem);
	REGISTER_METHOD(SendEvent);
	REGISTER_METHOD(SendNamedEvent);
}

CScriptbind_UI::~CScriptbind_UI()
{
	s_pInstance = NULL;
	
	for(TEventMap::iterator it = m_EventMapS2UI.begin(); it != m_EventMapS2UI.end(); ++it)	{
		m_EventMapS2UI.erase(it);
		delete (*it).second;
	}
	
	for(TEventMap::iterator it = m_EventMapUI2S.begin(); it != m_EventMapUI2S.end(); ++it)	{
		m_EventMapUI2S.erase(it);
		delete (*it).second;
	}
}

void CScriptbind_UI::OnReset()
{
	m_pUIClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("UI");
#ifdef _DEBUG
	m_pUIClass->CallMethod("TestInit", NULL, true);
#endif //_DEBUG
}

void CScriptbind_UI::OnEvent(const char* SystemName, const char* EventName, const SUIEvent& event)
{
	IMonoArray* pArray, *pArgs;
	CMonoSUIEvent mevent;
	int c, i;

	c = event.args.GetArgCount();

	pArgs = CreateMonoArray(c);

	if (!pArgs)
		return;

	pArray = CreateMonoArray(4);

	if (!pArray)
		return pArgs->Release();

	mevent.Args = *pArgs;

	for (i = 0; i < c; i++){
		pArgs->Insert(UIDTToMAV(event.args.GetArg(i)));
	}
	pArray->Insert(SystemName);
	pArray->Insert(EventName);
	pArray->Insert((int)event.event);
	pArray->Insert(pArgs);
	m_pUIClass->CallMethod("OnEvent", pArray, true);
	pArray->Release();
	pArgs->Release();
}

int CScriptbind_UI::RegisterEvent(mono::string eventsystem, int direction, SMonoUIEventDesc desc){
	const char* pEventSystem;
	CUICallback* pCB;
	CScriptArray* pArray;
	SMonoUIParameterDesc param;
	int i, c;
	IMonoObject* pObject;
	if (!gEnv->pFlashUI)
		return -1;
	pEventSystem = ToCryString(eventsystem);
	if (!pEventSystem || !*pEventSystem)
		return -1;
	pCB = s_pInstance->GetOrCreateSystem(pEventSystem, (IUIEventSystem::EEventSystemType)direction);
	if (!pCB)
		return -1;
	//Already have an event with this name?
	if (pCB->System->GetEventDesc(ToCryString(desc.Name)))
		return -1;

	SUIEventDesc uidesc(ToCryString(desc.Name), ToCryString(desc.DisplayName), ToCryString(desc.Description), desc.IsDynamic, ToCryString(desc.DynamicName), ToCryString(desc.DynamicDescription));
	pArray = new CScriptArray(desc.Params);
	if (!pArray)
		return -1;
	c = pArray->GetSize();
	for (i = 0; i < c; i++){
		pObject = pArray->GetItem(i);
		if (!pObject)
			return delete pArray, -1;
		param = pObject->Unbox<SMonoUIParameterDesc>();
		uidesc.Params.push_back(SUIParameterDesc(ToCryString(param.Name), ToCryString(param.DisplayName), ToCryString(param.Description), (SUIParameterDesc::EUIParameterType)param.Type));
	}
	delete pArray;

	return (int)pCB->System->RegisterEvent(uidesc);
}

bool CScriptbind_UI::RegisterToEventSystem(mono::string eventsystem, int type){
	if (!gEnv->pFlashUI || !s_pInstance)
		return false;
	if (s_pInstance->SystemExists(ToCryString(eventsystem), (IUIEventSystem::EEventSystemType)type))
		return true;

	return s_pInstance->GetOrCreateSystem(ToCryString(eventsystem), (IUIEventSystem::EEventSystemType)type) != NULL;
}

void CScriptbind_UI::UnregisterFromEventSystem(mono::string eventsystem, int type){
	CUICallback* pCB;
	if (!gEnv->pFlashUI || !s_pInstance)
		return;
	pCB = s_pInstance->FindSystem(ToCryString(eventsystem), (IUIEventSystem::EEventSystemType)type);
	if (pCB)
		s_pInstance->RemoveSystem(pCB);
}


void CScriptbind_UI::SendEvent(mono::string eventsystem, int event, mono::array args){
	CUICallback* pCB;
	SUIArguments uiargs;
	CScriptArray* pArray;
	IMonoObject* pObject;
	int i, c;
	if (!s_pInstance)
		return;
	pCB = s_pInstance->FindSystem(ToCryString(eventsystem), IUIEventSystem::eEST_SYSTEM_TO_UI);
	if (!pCB)
		return;
	pArray = new CScriptArray(args);
	if (!pArray)
		return;

	c = pArray->GetSize();
	for (i = 0; i < c; i++){
		pObject = pArray->GetItem(i);
		if (!pObject){
			delete pArray;
			return;
		}
		uiargs.AddArgument(MAVToUIDT(pObject->GetAnyValue()));
	}
	delete pArray;
	pCB->System->SendEvent(SUIEvent((uint)event, uiargs));

}

void CScriptbind_UI::SendNamedEvent(mono::string eventsystem, mono::string event, mono::array args){
	CUICallback* pCB;
	if (!s_pInstance)
		return;
	pCB = s_pInstance->FindSystem(ToCryString(eventsystem), IUIEventSystem::eEST_SYSTEM_TO_UI);
	if (!pCB)
		return;
	SendEvent(eventsystem, pCB->FindEvent(ToCryString(event)), args);
}
