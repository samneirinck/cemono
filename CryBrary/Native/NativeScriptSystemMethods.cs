using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace CryEngine.Native
{
    internal class NativeScriptSystemMethods : INativeScriptSystemMethods
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _UpdateScriptInstance(IntPtr nativeObject, object newInstance);

        public void UpdateScriptInstance(IntPtr nativeObject, object newInstance)
        {
            _UpdateScriptInstance(nativeObject, newInstance);
        }
    }
}
