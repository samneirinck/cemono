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
        public FlowNodeConfig GetConfiguration()
	    {
            Logging./*CryConsole.*/LogAlways("Retrieving flownode configuration");

            FlowNodeConfig config = new FlowNodeConfig();

            InputPortConfig res = new InputPortConfig("Start", InputPortTypes.Void, "Start input");
		    InputPortConfig[] inputs =
		    {
                new InputPortConfig("Start", InputPortTypes.Void, "Start input"),
                new InputPortConfig("Cancel", InputPortTypes.Void, "Cancel input")
		    };

		    OutputPortConfig[] outputs =
		    {
			    new OutputPortConfig("Started", OutputPortTypes.Void, "Called on start"),
                new OutputPortConfig("Cancelled", OutputPortTypes.Void, "Called on cancel")
		    };

		    config.inputs = inputs;
		    config.outputs = outputs;
            config.description = "Does nothing!";
            config.category = FlowNodeFlags.Debug;

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