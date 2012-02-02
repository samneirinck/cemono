using System.Collections.Generic;

using CryEngine;

namespace CryGameCode
{
    [FlowNode(UICategory = "Samples", Category = FlowNodeCategory.Approved, Description = "Does awesome CryMono things")]
    public class SampleNode : FlowNode
    {
        public override void OnInit()
        {
        }

        [InputPort(Name = "Activate", Description = "Test of a void input")]
        public void OnActivateTriggered()
        {
            Console.LogAlways("The activate port was triggered.");

            ActivateOutput(activatedPortId);
        }

        [InputPort(Name = "Test Int", Description = "Test of an int input")]
        public void OnIntTriggered(int value)
        {
            Console.LogAlways("The int port was triggered, value is {0}", value.ToString());

            ActivateOutput(testIntPortId, value);
        }

        [OutputPort(Name = "Activated", Description = "", Type = NodePortType.Void)]
        public static int activatedPortId;

        [OutputPort(Name = "Test Int", Description = "", Type = NodePortType.Int)]
        public static int testIntPortId;
    }
}