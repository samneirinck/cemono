using System;

namespace CryEngine.Native
{
    interface INativeScriptSystemMethods
    {
        void UpdateScriptInstance(IntPtr nativeObject, object newInstance);
    }
}
