using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Native
{
    public struct EntityInitializationParams : IScriptInitializationParams
    {
        public EntityInitializationParams(IntPtr ptr, EntityId id)
        {
            IEntityPtr = ptr;
            Id = id;
            IAnimatedCharacterPtr = IntPtr.Zero;
        }

        public IntPtr IEntityPtr;
        public IntPtr IAnimatedCharacterPtr;
        public EntityId Id;
    }
}
