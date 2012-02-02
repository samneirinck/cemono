using System.Diagnostics;


namespace CryEngine
{
    /// <summary>
    /// The Console provides access to CryENGINE's console interface, including redirecting .NET logging to
    /// the CryENGINE console.
    /// Additionally, it handles CVar / CCommand interpretation.
    /// </summary>
    public partial class Console
    {
        /// <summary>
        /// Constructs a new CryConsole object and sets up output redirection and error tracing.
        /// </summary>
        public Console()
        {
            // Catch unhandled exceptions
			System.AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;
            
            // Set console outputs (this will redirect Console.Write* output to the cryengine console)
            CryRedirector cryRedirector = new CryRedirector();
            System.Console.SetOut(cryRedirector);
			System.Console.SetError(cryRedirector);

            // Add our trace listener (this will redirect Trace.* output to the cryengine console)
            Trace.Listeners.Add(new CryTraceListener());
        }

		private void UnhandledExceptionOccurred(object sender, System.UnhandledExceptionEventArgs e)
        {
			Console.LogAlways("An unhandled managed exception occured: {0}{1}", System.Environment.NewLine, e.ExceptionObject.ToString());
        }
    }
}
