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
        extern private static void _RevertAppDomain();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _ReloadAppDomain();

        /// <summary>
        /// Revert the last script reload attempt.
        /// </summary>
        public void RevertAppDomain()
        {
            _RevertAppDomain();
        }

        /// <summary>
        /// Attempt to reload scripts again
        /// </summary>
        public void ReloadAppDomain()
        {
            _ReloadAppDomain();
        }
    }
}
