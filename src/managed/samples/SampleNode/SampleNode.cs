using CryEngine.API;
using CryEngine.FlowSystem;

using System;

namespace WhateverNamespaceTheNodeMakerWants
{
    [NodeCategory("Mono")] // Decides which category the node will appear in, inside the Flowgraph Editor.
    // The node name is actually the class name, no need to define it somewhere else!
    class SampleNode : FlowNode // Basic functionality provided by the base FlowNode, all nodes inheriting from the FlowNode class will be registered automatically.
    {
        public SampleNode()
        {
            Logging./*CryConsole.*/LogAlways("Constructing SampleNode");
        }

        ////////////////////////////////////////////////////
        public SFlowNodeConfig GetConfiguration()
	    {
            Logging./*CryConsole.*/LogAlways("Retrieving flownode configuration");

            SFlowNodeConfig config = new SFlowNodeConfig();

            SInputPortConfig res = new SInputPortConfig("Start", EInputPortType.Void, "Start input");
		    SInputPortConfig[] inputs =
		    {
                new SInputPortConfig("Start", EInputPortType.Void, "Start input"),
                new SInputPortConfig("Cancel", EInputPortType.Void, "Cancel input")
		    };

		    SOutputPortConfig[] outputs =
		    {
			    new SOutputPortConfig("Started", EOutputPortType.Void, "Called on start"),
                new SOutputPortConfig("Cancelled", EOutputPortType.Void, "Called on cancel")
		    };

		    config.inputs = inputs;
		    config.outputs = outputs;
            config.description = "Does nothing!";
            config.category = EFlowNodeFlags.EFLN_DEBUG;

            return config;
	    }   

        public void ProcessEvent(FlowNode.EFlowEvent _event, UInt16 id)
        {
            Logging./*CryConsole.*/LogAlways("Process Event; Mono style.");

            switch (_event)
            {
                case EFlowEvent.Activate:
                    {/*
                        if (IsPortActive(nodeInfo, (int)EInputPorts.EIP_Start))
                        {
                            ActivateOutput(nodeInfo, (int)EOutputPorts.EOP_Started);
                        }*/
                    }
                    break;
            }
        }
    }
}