using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem.Native
{
    struct NodeInfo
    {
        public NodeInfo(IntPtr Pointer, UInt16 NodeId, UInt32 GraphId)
        {
            nodePtr = Pointer;
            nodeId = NodeId;
            graphId = GraphId;
        }

        public IntPtr nodePtr;

        public UInt16 nodeId;

        public UInt32 graphId;
    }
}
