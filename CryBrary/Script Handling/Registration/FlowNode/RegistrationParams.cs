using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Reflection;

namespace CryEngine.Initialization
{
    public struct FlowNodeRegistrationParams : IScriptRegistrationParams
    {
        public string name;
        public string category;

        public string description;

        public FlowNodeFilter filter;

        public FlowNodeType type;

        public bool hasTargetEntity;

        public InputPortConfig[] inputPorts;
        public MethodInfo[] inputMethods;

        public OutputPortConfig[] outputPorts;
        public MemberInfo[] outputMembers;
    }
}
