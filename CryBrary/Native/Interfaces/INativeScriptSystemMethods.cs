using System;

namespace CryEngine.Native
{
    interface INativeScriptSystemMethods
    {
        void RevertAppDomain();
        void ReloadAppDomain();
    }
}
