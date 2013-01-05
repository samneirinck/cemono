using System;

namespace CryEngine.Utilities
{
    public struct PointerWrapper
    {
        public PointerWrapper(IntPtr pointer)
        {
            ptr = pointer;
        }

        public IntPtr ptr;
    }
}