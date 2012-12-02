using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem.Native
{
    internal struct NodeConfig
    {
        public NodeConfig(FlowNodeFilter cat, string desc, FlowNodeFlags nodeFlags = 0, FlowNodeType nodeType = FlowNodeType.Instanced)
            : this()
        {
            flags = nodeFlags;
            filter = cat;
            description = desc;
            type = nodeType;
        }

        FlowNodeFlags flags;

        FlowNodeFilter filter;

        FlowNodeType type;

        string description;
    }
}
