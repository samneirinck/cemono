using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace CryEngine
{
    partial class CryConsole
    {
        [DllImport("CryMono.dll")]
        extern static void _LogAlways(string msg);
        [DllImport("CryMono.dll")]
        extern static void _Log(string msg);
        [DllImport("CryMono.dll")]
        extern static void _Warning(string msg);
        
        /// <summary>
        /// Logs a message to the console
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void Log(string format, params object[] args)
        {
            _Log(String.Format(format, args));
        }

        /// <summary>
        /// Logs a message to the console
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void Log(string msg)
        {
            _Log(msg);
        }

        /// <summary>
        /// Logs a message to the console, regardless of log_verbosity settings
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void LogAlways(string format, params object[] args)
        {
            _LogAlways(String.Format(format, args));
        }

        /// <summary>
        /// Logs a message to the console, regardless of log_verbosity settings
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void LogAlways(string msg)
        {
            _LogAlways(msg);
        }

        public static void LogException(Exception ex)
        {
            Warning(ex.ToString());
        }

        /// <summary>
        /// Outputs a warning message
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void Warning(string format, params object[] args)
        {
            _Warning(String.Format(format, args));
        }

        /// <summary>
        /// Outputs a warning message
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void Warning(string msg)
        {
            _Warning(msg);
        }
    }
}
