#include "StdAfx.h"
#include "UI.h"

#include "MonoFlowNode.h"

CUI *CUI::m_pUI = NULL;

CUI::TCallbacks CUI::m_functionCallbacks = CUI::TCallbacks();
CUI::TEvents CUI::m_events = CUI::TEvents();

CUI::CUI()
{
	m_pUI = this;

	REGISTER_METHOD(CreateEventSystem);

	REGISTER_METHOD(RegisterFunction);
	REGISTER_METHOD(RegisterEvent);
}

CUI::~CUI()
{
	// Function = IUIEventSystem::eEST_UI_TO_SYSTEM
}

void CUI::OnEvent(const SUIEvent& event)
{
	m_functionCallbacks[event.event]();
}

IUIEventSystem *CUI::CreateEventSystem(mono::string name, IUIEventSystem::EEventSystemType eventType)
{
	auto pEventSystem = gEnv->pFlashUI->CreateEventSystem(ToCryString(name), eventType);

	pEventSystem->RegisterListener(m_pUI, "CryMonoUI");
	return pEventSystem;
}

void CUI::RegisterFunction(IUIEventSystem *pEventSystem, mono::string name, mono::string desc, mono::array inputs, UICallback callback)
{
	SUIEventDesc eventDesc(ToCryString(name), ToCryString(desc));

	IMonoArray *pInputPorts = *inputs;

	for(int i = 0; i < pInputPorts->GetSize(); i++)
	{
		auto monoInputPortConfig = pInputPorts->GetItem(i)->Unbox<SMonoInputPortConfig>();
		auto inputPortConfig = monoInputPortConfig.Convert();

		switch(monoInputPortConfig.type)
		{
		case eFDT_Bool:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>(inputPortConfig.name, inputPortConfig.description);
			break;
		case eFDT_Int:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>(inputPortConfig.name, inputPortConfig.description);
			break;
		case eFDT_Float:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>(inputPortConfig.name, inputPortConfig.description);
			break;
		case eFDT_Vec3:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Vec3>(inputPortConfig.name, inputPortConfig.description);
			break;
		case eFDT_String:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_String>(inputPortConfig.name, inputPortConfig.description);
			break;
		}
	}

	m_functionCallbacks.insert(TCallbacks::value_type(pEventSystem->RegisterEvent(eventDesc), callback));
}

void CUI::RegisterEvent(IUIEventSystem *pEventSystem, mono::string name, mono::string desc, mono::array outputs)
{
	SUIEventDesc eventDesc(ToCryString(name), ToCryString(desc));

	IMonoArray *pOutputPorts = *outputs;

	for(int i = 0; i < pOutputPorts->GetSize(); i++)
	{
		auto monoOutputPortConfig = pOutputPorts->GetItem(i)->Unbox<SMonoOutputPortConfig>();
		auto outputPortConfig = monoOutputPortConfig.Convert();

		switch(monoOutputPortConfig.type)
		{
		case eFDT_Bool:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>(outputPortConfig.name, outputPortConfig.description);
			break;
		case eFDT_Int:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>(outputPortConfig.name, outputPortConfig.description);
			break;
		case eFDT_Float:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>(outputPortConfig.name, outputPortConfig.description);
			break;
		case eFDT_Vec3:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_Vec3>(outputPortConfig.name, outputPortConfig.description);
			break;
		case eFDT_String:
			eventDesc.AddParam<SUIParameterDesc::eUIPT_String>(outputPortConfig.name, outputPortConfig.description);
			break;
		}
	}

	m_events.push_back(pEventSystem->RegisterEvent(eventDesc));
}