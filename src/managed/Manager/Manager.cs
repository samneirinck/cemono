using System;
using System.Reflection;
using System.IO;
namespace Cemono
{
    public class Manager
    {
        #region Fields
        private ConsoleRedirector _consoleRedirector;
        private AppDomain _gameDomain;
        #endregion

        #region Properties

        #endregion

        #region Constructor(s)
        public Manager()
        {
            InitializeConsoleRedirect();

            // Catch unhandled exceptions
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;

            // Initialize game appdomain
            InitializeGameDomain();
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
            Console.Error.WriteLine("An unhandled managed exception occured: {0}{1}", Environment.NewLine, e.ExceptionObject.ToString());
        }

        private void InitializeGameDomain()
        {
            AppDomain domain = AppDomain.CreateDomain("cemono Game");

            GameLoader gameLoader = (GameLoader)domain.CreateInstanceFromAndUnwrap(Assembly.GetExecutingAssembly().Location, typeof(GameLoader).ToString());
            gameLoader.ConsoleRedirector = _consoleRedirector;
            gameLoader.CompileAndLoad("", @"E:\Games\Crysis Wars\Mods\cemono\Game");

            _gameDomain = domain;
        }
        #endregion
    }
}
