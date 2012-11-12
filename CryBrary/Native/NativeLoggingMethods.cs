using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using System.Security;

namespace CryEngine.Native
{
    class NativeLoggingMethods
    {
        [SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void LogAlways(string msg);
        [SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void Log(string msg);
        [SuppressUnmanagedCodeSecurity]
        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", MessageId = "0"), DllImport("CryMono.dll")]
        public extern static void Warning(string msg);
    }
}