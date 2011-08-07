#include "StdAfx.h"
#include "MonoFlowBaseNode.h"

#include "Mono.h"
#include "FGPSHandler.h"

CMonoFlowNode::CMonoFlowNode(SActivationInfo *pActInfo)
{
	m_nodeId = pActInfo->myID;
}

CMonoFlowNode::~CMonoFlowNode()
{
}

void CMonoFlowNode::GetConfiguration(SFlowNodeConfig &config)
{
	CryLogAlways("Get config start");

	if(g_pMono && g_pMono->IsFullyInitialized())
	{
		void *args[1];
		args[0] = &m_nodeId;

		CryLogAlways("1");

		MonoObject *result = g_pMono->InvokeFunc("GetConfiguration", "FlowManager", "CryEngine.FlowSystem", NULL /* Use native library */, g_pMono->GetFGPluginManager()->GetFlowSystemInstance(), args);
		SMonoFlowNodeConfig monoConfig = *(SMonoFlowNodeConfig *)mono_object_unbox(result);

		CryLogAlways("2");

		SInputPortConfig *inputs = new SInputPortConfig[monoConfig.inputSize];
		SMonoInputPortConfig inputPortConfig;
		for(int i = 0; i < monoConfig.inputSize; i++)
		{
			CryLogAlways("node input %i of %i", i + 1, monoConfig.inputSize);

			inputPortConfig = mono_array_get(monoConfig.pInputPorts, SMonoInputPortConfig, i);

			inputs[i] = MonoInputPortConfig(inputPortConfig.inputType, mono_string_to_utf8(inputPortConfig.name), 
							mono_string_to_utf8(inputPortConfig.description), 
							mono_string_to_utf8(inputPortConfig.humanName), 
							mono_string_to_utf8(inputPortConfig.sUIConfig));
		}

		SOutputPortConfig *outputs = new SOutputPortConfig[monoConfig.outputSize];
		SMonoOutputPortConfig outputPortConfig;
		for(int i = 0; i < monoConfig.outputSize; i++)
		{
			outputPortConfig = mono_array_get(monoConfig.pOutputPorts, SMonoOutputPortConfig, i);

			outputs[i] = MonoOutputPortConfig(outputPortConfig.outputType, mono_string_to_utf8(outputPortConfig.name), 
							_HELP(mono_string_to_utf8(outputPortConfig.description)), 
							mono_string_to_utf8(outputPortConfig.humanName));
		}

		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP(mono_string_to_utf8(monoConfig.sDescription));
		config.SetCategory(monoConfig.flags);

		for(int i = 0; i < monoConfig.inputSize; i++)
		{
			CryLogAlways(config.pInputPorts[i].description);
			CryLogAlways(config.pInputPorts[i].name);
		}
	}

	CryLogAlways("Get config end");
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

/*void Tokenize(const string& str,
                      std::vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

const char *CMonoFlowNode::GetNodeName(SActivationInfo *pActInfo)
{
	std::vector<string> tokens;
	Tokenize(pActInfo->pGraph->GetNodeTypeName(pActInfo->myID), tokens, ":"); // Do this in C# instead?

	return tokens[1]; // Node name without category
}*/