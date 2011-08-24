using System;
using System.Reflection;
using System.IO;
using System.Diagnostics;
using CryEngine;
namespace Cemono
{
    public class Manager
    {
        #region Fields
        private ConsoleRedirector _consoleRedirector;
        private ConsoleTraceListener _consoleTraceListener;
        private AppDomain _gameDomain;
        private Folders _folders = new Folders();
        #endregion

        #region Properties
        public BaseGame Game { get; private set; }
        #endregion

        #region Constructor(s)
        Manager()
        {
            // Initialize all folders
            InitializeFolders();

            // Set up console redirect (Console.* methods)
            InitializeConsoleRedirect();

            // Catch unhandled exceptions
            AppDomain.CurrentDomain.UnhandledException += UnhandledExceptionOccurred;

            // Initialize game appdomain
            InitializeGameDomain();
        }
        #endregion

        #region Methods
        private void InitializeFolders()
        {
            _folders.RootFolder = Directory.GetCurrentDirectory();
            _folders.GameFolder = Path.Combine(_folders.RootFolder, CryEngine.API.Console.GetCVarValue<string>("sys_game_folder"));
            _folders.NodeFolder = Path.Combine(_folders.GameFolder, "Nodes");
            _folders.LogicFolder = Path.Combine(_folders.GameFolder, "Logic");
            _folders.EntitiesFolder = Path.Combine(_folders.GameFolder, "Entities");
        }

        private void InitializeConsoleRedirect()
        {
            var consoleLogging = new ConsoleLogging();
            _consoleRedirector = new ConsoleRedirector();
            _consoleRedirector.Logging = consoleLogging;
            _consoleTraceListener = new ConsoleTraceListener();
            _consoleTraceListener.Logging = consoleLogging;

            // Set console outputs (this will redirect Console.Write* output to the cryengine console)
            Console.SetOut(_consoleRedirector);
            Console.SetError(_consoleRedirector);

            // Add our trace listener (this will redirect Trace.* output to the cryengine console)
            Trace.Listeners.Add(_consoleTraceListener);
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
            gameLoader.CompileAndLoadScripts(_folders);

            _gameDomain = domain;
        }
        #endregion
    }
}
