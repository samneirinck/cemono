using System;
using System.Collections.Generic;

using CryEngine;

namespace CryGameCode
{
    [FlowNode(UICategory = "Samples", Category = FlowNodeCategory.Approved, Description = "Does awesome Mono things")]
    public class SampleNode : BaseFlowNode
    {
        public override void OnInitialized()
        {
            SetRegularlyUpdated(true);
        }

        public override void OnUpdate()
        {
        }

        [InputPort(Name = "Activate", Description = "Test of a void input")]
        public void OnActivateTriggered()
        {
            CryConsole.LogAlways("The activate port was triggered.");

            ActivateOutput(activatedPortId);
        }

        [InputPort(Name = "Test Int", Description = "Test of an int input")]
        public void OnIntTriggered(int value)
        {
            CryConsole.LogAlways("The int port was triggered, value is {0}", value.ToString());

            ActivateOutput(testIntPortId, value);
        }

        [OutputPort(Name = "Activated", Description = "", Type = NodePortType.Void)]
        public static int activatedPortId;

        [OutputPort(Name = "Test Int", Description = "", Type = NodePortType.Int)]
        public static int testIntPortId;
    }
}