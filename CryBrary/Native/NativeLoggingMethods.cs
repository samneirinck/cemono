using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using System.Security;

namespace CryEngine.Native
{
    class NativeLoggingMethods : INativeLoggingMethods
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
        
        public void LogAlways(string msg)
        {
            _LogAlways(msg);
        }

        public void Log(string msg)
        {
            _Log(msg);
        }
        public void Warning(string msg)
        {
            _Warning(msg);
        }
    }
}