using System;
using System.Text;

namespace CryEngine
{
	internal interface INativeLoggingMethods
	{
		void _LogAlways(string msg);
		void _Log(string msg);
		void _Warning(string msg);
	}

	public static partial class Debug
	{
		static Debug()
		{
			AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;
		}

		private static INativeLoggingMethods _nativeLoggingMethods;
		internal static INativeLoggingMethods NativeLoggingMethods
		{
			get
			{
				return _nativeLoggingMethods ?? (_nativeLoggingMethods = new LoggingMethods());
			}
			set
			{
				_nativeLoggingMethods = value;
			}
		}

		class LoggingMethods : INativeLoggingMethods
		{
			// Logging is using DllImport, which requires a static extern method
			public void _LogAlways(string msg)
			{
				NativeMethods._LogAlways(msg);
			}

			public void _Log(string msg)
			{
				NativeMethods._Log(msg);
			}
			public void _Warning(string msg)
			{
				NativeMethods._Warning(msg);
			}
		}

		private static void UnhandledExceptionOccurred(object sender, UnhandledExceptionEventArgs e)
		{
			LogException((Exception)e.ExceptionObject);
		}

		/// <summary>
		/// Logs a message to the console
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void Log(string format, params object[] args)
		{
			NativeLoggingMethods._Log(String.Format(format, args));
		}

		/// <summary>
		/// Logs a message to the console
		/// </summary>
		public static void Log(string msg)
		{
			NativeLoggingMethods._Log(msg);
		}

		/// <summary>
		/// Logs a message to the console, regardless of log_verbosity settings
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void LogAlways(string format, params object[] args)
		{
			NativeLoggingMethods._LogAlways(String.Format(format, args));
		}

		/// <summary>
		/// Logs a message to the console, regardless of log_verbosity settings
		/// </summary>
		public static void LogAlways(string msg)
		{
			NativeLoggingMethods._LogAlways(msg);
		}

		/// <summary>
		/// Logs an exception message to the console
		/// </summary>
		/// <remarks>Useful when exceptions are caught and data is still needed from them</remarks>
		/// <param name="ex"></param>
		public static void LogException(Exception ex)
		{
			CVar.Methods.HandleException(ex);
			//Warning(ex.ToString());
		}

		/// <summary>
		/// Outputs a warning message
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void LogWarning(string format, params object[] args)
		{
			NativeLoggingMethods._Warning(String.Format(format, args));
		}

		/// <summary>
		/// Outputs a warning message
		/// </summary>
		public static void LogWarning(string msg)
		{
			NativeLoggingMethods._Warning(msg);
		}

        public static void LogStackTrace()
        {
            var stackTrace = new System.Diagnostics.StackTrace(true);
            Debug.LogAlways("Stack trace:");
            for(int i = 1; i < stackTrace.FrameCount; i++)
            {
                var frame = stackTrace.GetFrame(i);

                var method = frame.GetMethod();
                var fileName = frame.GetFileName() ?? "<unknown>";

                Debug.Log("  at {0}.{1}.{2} () in {3}:{4}", method.DeclaringType.Namespace, method.DeclaringType.Name, method.Name, fileName, frame.GetFileLineNumber());
            }
        }
	}
}