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
        }

        #endregion

        #region Methods
        private void InitializeConsoleRedirect()
        {
            _consoleRedirector = new ConsoleRedirector();
            Console.SetOut(_consoleRedirector);
            Console.SetError(_consoleRedirector);
        }
        #endregion
    }
}
