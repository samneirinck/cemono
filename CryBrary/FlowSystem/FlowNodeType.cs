using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem
{
    public enum FlowNodeType
    {
        /// <summary>
        /// Node has only one instance, never cloned.
        /// </summary>
        Singleton,
        /// <summary>
        /// New instance of node will be created each time it is requested.
        /// </summary>
        Instanced
    }
}
