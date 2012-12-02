using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine//.FlowSystem
{
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Property | AttributeTargets.Field)]
    public sealed class PortAttribute : Attribute
    {
        public string Name { get; set; }

        public string Description { get; set; }

        public PortType Type { get; set; }
    }
}
