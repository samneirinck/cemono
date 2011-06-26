using System;
namespace Cemono
{
    public class Manager
    {
        #region Fields
        private ConsoleRedirector _consoleRedirector;
        #endregion

        #region Properties

        #endregion

        #region Constructor(s)
        public Manager()
        {
            InitializeConsoleRedirect();

            // Catch unhandled exceptions
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;

            Console.WriteLine("Finished intializing cemono manager");
        }

        #endregion

        #region Methods
        private void InitializeConsoleRedirect()
        {
            _consoleRedirector = new ConsoleRedirector();
            _consoleRedirector.Logging = new ConsoleLogging();
            Console.SetOut(_consoleRedirector);
            Console.SetError(_consoleRedirector);
        }

        private void UnhandledExceptionOccurred(object sender, UnhandledExceptionEventArgs e)
        {
            Console.Error.WriteLine("An unhandled managed exception occured: {0}{1}", Environment.NewLine, e.ToString());
        }

        #endregion
    }
}
