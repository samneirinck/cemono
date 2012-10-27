using System;

namespace CryEngine
{
    internal struct PointerWrapper
    {
        public PointerWrapper(IntPtr pointer)
        {
            ptr = pointer;
        }

        public IntPtr ptr;
    }
}