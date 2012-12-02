using System;

namespace CryEngine//.FlowSystem
{
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class FlowNodeAttribute : Attribute
    {
        /// <summary>
        /// Gets or sets the name of the node, if not set will use the node class name.
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Gets or sets the category in which the node will appear when right-clicking in the Flowgraph Editor.
        /// </summary>
        public string Category { get; set; }

        /// <summary>
        /// Gets or sets the Sandbox filtering category
        /// </summary>
        public FlowNodeFilter Filter { get; set; }

        /// <summary>
        /// Gets or sets the node description
        /// </summary>
        public string Description { get; set; }

        public bool HasTargetEntity { get; set; }

        internal FlowNodeFlags Flags { get; set; }
    }
}