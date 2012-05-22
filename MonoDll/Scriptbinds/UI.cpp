#include "StdAfx.h"
#include "UI.h"

#include "MonoFlowNode.h"

CUI *CUI::m_pUI = NULL;

CUI::CUI()
{
	m_pUI = this;

	REGISTER_METHOD(CreateEventSystem);

	REGISTER_METHOD(RegisterFunction);
	REGISTER_METHOD(RegisterEvent);

	REGISTER_METHOD(SendEvent);
}

template <typename T>
T GetValue(const TUIData &arg)
{
	T value;
	arg.GetValueWithConversion(value);

	return value;
}

void SEventSystemHandler::OnEvent(const SUIEvent& event)
{
	IMonoArray *pArgs = CreateMonoArray(3);

	auto pConverter = gEnv->pMonoScriptSystem->GetConverter();

	pArgs->Insert(pConverter->ToManagedType(pConverter->GetCommonClass(eCMT_PointerWrapper), &mono::pointer(m_pEventSystem)));
	pArgs->Insert(event.event);

	IMonoArray *pArray = CreateMonoArray(event.args.GetArgCount());
	for(int i = 0; i < pArray->GetSize(); i++)
	{
		auto arg = event.args.GetArg(i);

		switch(arg.GetType())
		{
			case eUIDT_Int:
				pArray->Insert(GetValue<int>(arg));
				break;
			case eUIDT_Float:
				pArray->Insert(GetValue<float>(arg));
				break;
			case eUIDT_EntityId:
				pArray->Insert(GetValue<EntityId>(arg));
				break;
			case eUIDT_Vec3:
				pArray->Insert(GetValue<Vec3>(arg));
				break;
			case eUIDT_String:
				pArray->Insert(GetValue<string>(arg));
				break;
			case eUIDT_Bool:
				pArray->Insert(GetValue<bool>(arg));
				break;
		}
	}

	pArgs->InsertArray(pArray);

	CUI::GetInstance()->GetClass()->CallMethod("OnEvent", pArgs, true);
}

IUIEventSystem *CUI::CreateEventSystem(mono::string name, IUIEventSystem::EEventSystemType eventType)
{
	auto eventSystemHandler = new SEventSystemHandler(ToCryString(name), eventType);

	return eventSystemHandler->GetEventSystem();
}

unsigned int CUI::RegisterFunction(IUIEventSystem *pEventSystem, mono::string name, mono::string desc, mono::array inputs)
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

	return pEventSystem->RegisterEvent(eventDesc);
}

unsigned int CUI::RegisterEvent(IUIEventSystem *pEventSystem, mono::string name, mono::string desc, mono::array outputs)
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

	return pEventSystem->RegisterEvent(eventDesc);
}

void CUI::SendEvent(IUIEventSystem *pEventSystem, unsigned int eventId)
{
	SUIEvent event(eventId);

	pEventSystem->SendEvent(event);
}