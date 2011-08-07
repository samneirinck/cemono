#include "StdAfx.h"
#include "MonoFlowBaseNode.h"

#include "Mono.h"
#include "FGPSHandler.h"

CMonoFlowNode::CMonoFlowNode(SActivationInfo *pActInfo)
{
	m_pActInfo = pActInfo;
}

CMonoFlowNode::~CMonoFlowNode()
{
}

#define SafeString(s) (s?s:"")

void CMonoFlowNode::GetConfiguration(SFlowNodeConfig &config)
{
		void *args[1];
		args[0] = &m_pActInfo->myID;
		/*
		MonoObject *result = g_pMono->InvokeFunc("GetConfiguration", "FlowManager", "CryEngine.FlowSystem", NULL, g_pMono->GetFGPluginManager()->GetFlowSystemInstance(), args);
		SMonoFlowNodeConfig monoConfig = *(SMonoFlowNodeConfig *)mono_object_unbox(result);
		
		SInputPortConfig *inputs = new SInputPortConfig[monoConfig.inputSize];
		SMonoInputPortConfig inputPortConfig;
		for(int i = 0; i < monoConfig.inputSize; i++)
		{
			inputPortConfig = mono_array_get(monoConfig.pInputPorts, SMonoInputPortConfig, i);
			
			inputs[i] = MonoInputPortConfig(inputPortConfig.inputType, SafeString(mono_string_to_utf8(inputPortConfig.name)), 
							SafeString(_HELP(mono_string_to_utf8(inputPortConfig.description))), 
							SafeString(mono_string_to_utf8(inputPortConfig.humanName)), 
							SafeString(mono_string_to_utf8(inputPortConfig.sUIConfig)));
		}

		SOutputPortConfig *outputs = new SOutputPortConfig[monoConfig.outputSize];
		SMonoOutputPortConfig outputPortConfig;

		for(int i = 0; i < monoConfig.outputSize; i++)
		{
			outputPortConfig = mono_array_get(monoConfig.pOutputPorts, SMonoOutputPortConfig, i);

			outputs[i] = MonoOutputPortConfig(outputPortConfig.outputType, SafeString(mono_string_to_utf8(outputPortConfig.name)), 
							SafeString(_HELP(mono_string_to_utf8(outputPortConfig.description))), 
							SafeString(mono_string_to_utf8(outputPortConfig.humanName)));
		}
		*/
		
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig_Void( "Start", _HELP("Start input")),
			InputPortConfig_Void( "Cancel", _HELP("Cancel input")),
			{0}
		};
		
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig_Void ( "Started", _HELP("Called on start")),
			OutputPortConfig_Void ( "Cancelled", _HELP("Called on cancel")),
			{0}
		};
		
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Relatively useless");//_HELP(mono_string_to_utf8(monoConfig.sDescription));
		config.SetCategory(EFLN_APPROVED/*monoConfig.flags*/);
}

void CMonoFlowNode::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{
		uint16 nodeId = pActInfo->myID;

		if(nodeId==0)
			return;

		void *args[2];
	
		args[0] = &event;
		args[1] = &pActInfo->myID;

		g_pMono->InvokeFunc("NodeProcessEvent", "FlowManager", "CryEngine.FlowSystem", NULL /* Use native library */, g_pMono->GetFGPluginManager()->GetFlowSystemInstance(), args);
}