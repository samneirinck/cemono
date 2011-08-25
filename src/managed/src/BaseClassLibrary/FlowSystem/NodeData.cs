using System;

namespace CryEngine.FlowSystem
{
    public struct NodeData
    {
        public NodeData(Type nodeType, object _obj)
        {
            type = nodeType;
            obj = _obj;
        }

        public Type type;
        public object obj;
    }
}
