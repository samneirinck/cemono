using System;


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
		private static INativeLoggingMethods _methods;
		internal static INativeLoggingMethods Methods
		{
			get
			{
				return _methods ?? (_methods = new LoggingMethods());
			}
			set
			{
				_methods = value;
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

		/// <summary>
		/// Logs a message to the console
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void Log(string format, params object[] args)
		{
			Methods._Log(String.Format(format, args));
		}

		/// <summary>
		/// Logs a message to the console
		/// </summary>
		public static void Log(string msg)
		{
			Methods._Log(msg);
		}

		/// <summary>
		/// Logs a message to the console, regardless of log_verbosity settings
		/// </summary>
		/// <param name="format"></param>
		/// <param name="args"></param>
		public static void LogAlways(string format, params object[] args)
		{
			Methods._LogAlways(String.Format(format, args));
		}

		/// <summary>
		/// Logs a message to the console, regardless of log_verbosity settings
		/// </summary>
		public static void LogAlways(string msg)
		{
			Methods._LogAlways(msg);
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
			Methods._Warning(String.Format(format, args));
		}

		/// <summary>
		/// Outputs a warning message
		/// </summary>
		public static void Warning(string msg)
		{
			Methods._Warning(msg);
		}
	}
}