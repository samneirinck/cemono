using System.Linq;

using CryEngine.Initialization;

namespace CryEngine.Flowgraph.Native
{
    internal struct NodeConfig
    {
        public NodeConfig(FlowNodeFilter cat, string desc, FlowNodeFlags nodeFlags, FlowNodeType nodeType, InputPortConfig[] inputPorts, OutputPortConfig[] outputPorts)
            : this()
        {
            flags = nodeFlags;
            filter = cat;
            description = desc;
            type = nodeType;

            inputs = inputPorts.Cast<object>().ToArray();
            outputs = outputPorts.Cast<object>().ToArray();
        }

        FlowNodeFlags flags;

        FlowNodeFilter filter;

        FlowNodeType type;

        string description;

        object[] inputs;
        object[] outputs;
    }
}
