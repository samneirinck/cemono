using System;

namespace CryEngine.FlowSystem
{
    public struct NodeData
    {
        public NodeData(Type nodeType, object _obj)
        {
            this.Type = nodeType;
            this.Object = _obj;
        }

        public Type Type { get; set; }
        public object Object { get; set; }
    }
}
