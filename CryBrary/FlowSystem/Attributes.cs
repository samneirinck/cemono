using System;

namespace CryEngine
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

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Property | AttributeTargets.Field)]
    public sealed class PortAttribute : Attribute
    {
        public string Name { get; set; }

        public string Description { get; set; }

        public PortType Type { get; set; }
    }

    public enum PortType
    {
        None,

        /// <summary>
        /// Only applicable on string inputs.
        /// </summary>
        Sound,

        DialogLine,

        /// <summary>
        /// Only applicable on Vec3 inputs.
        /// </summary>
        Color,

        Texture,

        Object,

        File,

        EquipmentPack,

        ReverbPreset,

        GameToken,

        /// <summary>
        /// Only applicable on string inputs.
        /// </summary>
        Material,

        Sequence,

        Mission,

        Animation,

        AnimationState,

        AnimationStateEx,

        Bone,

        Attachment,

        Dialog,

        MaterialParamSlot,

        MaterialParamName,

        MaterialParamCharacterAttachment,
    }

    [Flags]
    internal enum FlowNodeFlags
    {
        /// <summary>
        /// This node targets an entity, entity id must be provided.
        /// </summary>
        TargetEntity = 0x0001,

        /// <summary>
        /// This node cannot be selected by user for placement in flow graph UI.
        /// </summary>
        HideUI = 0x0002,

        /// <summary>
        /// This node is setup for dynamic output port growth in runtime.
        /// </summary>
        DynamicOutput = 0x0004,

        /// <summary>
        /// This node cannot be deleted by the user.
        /// </summary>
        Unremovable = 0x0008,
    }

    public enum FlowNodeFilter
    {
        /// <summary>
        /// This node is approved for designers
        /// </summary>
        Approved = 0x0010,

        /// <summary>
        /// This node is slightly advanced and approved.
        /// </summary>
        Advanced = 0x0020,

        /// <summary>
        /// This node is for debug purpose only.
        /// </summary>
        Debug = 0x0040,

        /// <summary>
        /// This node is obsolete and is not available in the editor.
        /// </summary>
        Obsolete = 0x0200,
    }
}