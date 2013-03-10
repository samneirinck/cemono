using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem.Native
{
    struct NodeInitializationParams : IScriptInitializationParams
    {
        public IntPtr nodePtr;

        public UInt16 nodeId;

        public UInt32 graphId;
    }
}
