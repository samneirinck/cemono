using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem.Native
{
    internal struct NodeConfig
    {
        public NodeConfig(FlowNodeFilter cat, string desc, FlowNodeFlags nodeFlags = 0)
            : this()
        {
            flags = nodeFlags;
            filter = cat;
            description = desc;
        }

        FlowNodeFlags flags;

        FlowNodeFilter filter;

        string description;
    }
}
