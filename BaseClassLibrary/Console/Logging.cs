using System;
using System.Runtime.InteropServices;

namespace CryEngine
{
    partial class Console
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
        public static void LogAlways(string msg)
        {
            _LogAlways(msg);
        }

		/// <summary>
		/// Logs an exception message to the console
		/// </summary>
		/// <remarks>Useful when exceptions are caught and data is still needed from them</remarks>
		/// <param name="ex"></param>
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
        public static void Warning(string msg)
        {
            _Warning(msg);
        }
    }
}
