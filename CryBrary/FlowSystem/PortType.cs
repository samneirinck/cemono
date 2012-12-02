using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine//.FlowSystem
{
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
}
