/// List of definitions that can be used: (Apply via project settings)
/// RELEASE_ENABLE_CHECKS - enable non-vital checks in release mode, if not set, CryMono will ignore most safety checks and disable the large majority of exceptions.

namespace CryEngine
{
	internal static class ProjectSettings
	{
		/// <summary>
		/// Designates what link will be opened when the user presses 'Report Bug' in the exception form.
		/// </summary>
		public static readonly string BugReportLink = "http://www.crydev.net/posting.php?mode=post&f=375"; /* CryMono sub-forum @ CryDev.net */

		/// <summary>
		/// Message displayed in the exception form when an exception has been thrown.
		/// </summary>
		public static readonly string ExceptionMessage = @"An unhandled exception occurred!

		If you believe this is a CryMono bug, please report it on CryDev, including the stacktrace below.";
	}
}