using System;
using System.Text;
using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
    /// Contains methods useful for tracking down bugs.
    /// </summary>
    public static partial class Debug
    {
        static Debug()
        {
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;
        }

        private static void UnhandledExceptionOccurred(object sender, UnhandledExceptionEventArgs e)
        {
            var exception = e.ExceptionObject as Exception;

            // The CLS doesn't force exceptions to derive from System.Exception
            if (exception == null)
                throw new NotSupportedException("An exception that does not derive from System.Exception was thrown.");

            DisplayException(exception);
        }

        /// <summary>
        /// Logs a message to the console
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void Log(string format, params object[] args)
        {
            NativeLoggingMethods.Log(String.Format(format, args));
        }

        /// <summary>
        /// Logs a message to the console
        /// </summary>
        public static void Log(string msg)
        {
            NativeLoggingMethods.Log(msg);
        }

        /// <summary>
        /// Logs a message to the console, regardless of log_verbosity settings
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void LogAlways(string format, params object[] args)
        {
            NativeLoggingMethods.LogAlways(String.Format(format, args));
        }

        /// <summary>
        /// Logs a message to the console, regardless of log_verbosity settings
        /// </summary>
        public static void LogAlways(string msg)
        {
            NativeLoggingMethods.LogAlways(msg);
        }

        /// <summary>
        /// Logs an exception message to the console
        /// </summary>
        /// <remarks>Useful when exceptions are caught and data is still needed from them</remarks>
        /// <param name="ex"></param>
        public static void LogException(Exception ex)
        {
            LogWarning(ex.ToString());
        }

        /// <summary>
        /// Displays an exception via the CryMono exception form.
        /// </summary>
        /// <param name="ex">The exception that occurred</param>
        /// <param name="fatal">Indicates if this exception is fatal</param>
        public static void DisplayException(Exception ex, bool fatal = false)
        {
            // Log exception as well
            LogException(ex);

            var form = new ExceptionMessage(ex, fatal);
            form.ShowDialog();
        }

        /// <summary>
        /// Outputs a warning message
        /// </summary>
        /// <param name="format"></param>
        /// <param name="args"></param>
        public static void LogWarning(string format, params object[] args)
        {
            NativeLoggingMethods.Warning(String.Format(format, args));
        }

        /// <summary>
        /// Outputs a warning message
        /// </summary>
        public static void LogWarning(string msg)
        {
            NativeLoggingMethods.Warning(msg);
        }

        public static void LogStackTrace()
        {
            var stackTrace = new System.Diagnostics.StackTrace(true);
            Debug.LogAlways("Stack trace:");
            for (int i = 1; i < stackTrace.FrameCount; i++)
            {
                var frame = stackTrace.GetFrame(i);

                var method = frame.GetMethod();
                var fileName = frame.GetFileName() ?? "<unknown>";

                Debug.Log("  at {0}.{1}.{2} () in {3}:{4}", method.DeclaringType.Namespace, method.DeclaringType.Name, method.Name, fileName, frame.GetFileLineNumber());
            }
        }
    }
}