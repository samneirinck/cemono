using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics.CodeAnalysis;

using System.Security;

using System.Runtime.InteropServices;

namespace CryEngine
{
    internal class NativeMethods
    {
		[SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void _LogAlways(string msg);
		[SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void _Log(string msg);
		[SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void _Warning(string msg);

		[SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "1"), System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void _RegisterCallback(string func, string className, CallbackHandler.Callback cb);
    }
}
