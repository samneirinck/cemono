using System;
namespace CryEngine
{
    [Flags]
    public enum EntityClassFlags
    {
        /// <summary>
        /// If set this class will not be visible in editor,and entity of this class cannot be placed manually in editor.
        /// </summary>
        ECLF_INVISIBLE = 0x0001,

        /// <summary>
        /// Default entity class
        /// </summary>
        ECLF_DEFAULT = 0x0002

    }
}
