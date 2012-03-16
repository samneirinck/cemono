using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.InteropServices;

namespace CryEngine
{
    internal class NativeMethods
    {
        [DllImport("CryMono.dll")]
        public extern static void _LogAlways(string msg);
        [DllImport("CryMono.dll")]
        public extern static void _Log(string msg);
        [DllImport("CryMono.dll")]
        public extern static void _Warning(string msg);

        [DllImport("CryMono.dll")]
        public extern static void _RegisterCallback(string func, string className, CallbackHandler.Callback cb);
    }
}
