using System;

using System.Reflection;
using System.Diagnostics;

using System.IO;
using System.Collections.Generic;

namespace CryEngine
{
    /// <summary>
    /// The CryConsole provides access to CryENGINE's console interface, including redirecting .NET logging to
    /// the CryENGINE console.
    /// Additionally, it handles CVar interpretation.
    /// TODO: Add CCommand support.
    /// </summary>
    public partial class CryConsole
    {
        /// <summary>
        /// Constructs a new CryConsole object and sets up output redirection and error tracing.
        /// </summary>
        public CryConsole()
        {
            CryConsole.LogAlways("Constructing CryConsole");

            // Catch unhandled exceptions
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;
            
            // Set console outputs (this will redirect Console.Write* output to the cryengine console)
            CryRedirector cryRedirector = new CryRedirector();
            Console.SetOut(cryRedirector);
            Console.SetError(cryRedirector);

            // Add our trace listener (this will redirect Trace.* output to the cryengine console)
            Trace.Listeners.Add(new CryTraceListener());
        }

        private void UnhandledExceptionOccurred(object sender, UnhandledExceptionEventArgs e)
        {
            CryConsole.LogAlways("An unhandled managed exception occured: {0}{1}", Environment.NewLine, e.ExceptionObject.ToString());
        }
    }
}
