using System;
namespace CryEngine.FlowSystem
{
    [Flags]
    public enum FlowNodeFlags
    {
        /// <summary>
        /// CORE FLAG: This node targets an entity, entity id must be provided.
        /// </summary>
        TargetEntity = 0x0001,

        /// <summary>
        /// CORE FLAG: This node cannot be selected by user for placement in flow graph UI.
        /// </summary>
        HideUI = 0x0002, // 

        /// <summary>
        /// CORE FLAG: This node is setup for dynamic output port growth in runtime.
        /// </summary>
        DynamicOutput = 0x0004, // 

        /// <summary>
        /// CORE_MASK
        /// </summary>
        CoreMask = 0x000F, // 

        /// <summary>
        /// CATEGORY:  This node is approved for designers. 
        /// </summary>
        Approved = 0x0010, //

        /// <summary>
        /// CATEGORY:  This node is slightly advanced and approved.
        /// </summary>
        Advanced = 0x0020, // 

        /// <summary>
        /// CATEGORY:  This node is for debug purpose only.
        /// </summary>
        Debug = 0x0040, // 

        //EFLN_WIP                   = 0x0080, // CATEGORY:  This node is work-in-progress and shouldn't be used by designers.
        //EFLN_LEGACY                = 0x0100, // CATEGORY:  This node is legacy and will VERY soon vanish.

        /// <summary>
        /// CATEGORY:  This node is obsolete and is not available in the editor.
        /// </summary>
        Obsolete = 0x0200, // 
        //EFLN_NOCATEGORY            = 0x0800, // CATEGORY:  This node has no category yet! Default!

        /// <summary>
        /// CATEGORY_MASK
        /// </summary>
        CategoryMask = 0x0FF0, // 

        /// <summary>
        /// USAGE_MASK
        /// </summary>
        UsageMask = 0xF000, // 
    }
}
