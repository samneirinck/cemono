namespace CryEngine
{
    public class ConsoleLogging : ILogging
    {
        public void Log(string msg)
        {
            API.Logging.LogAlways(msg);
        }

        public void Log(string msg, params object[] args)
        {
            API.Logging.LogAlways(msg, args);
        }

        public void LogAlways(string msg)
        {
            API.Logging.LogAlways(msg);
        }

        public void LogAlways(string msg, params object[] args)
        {
            API.Logging.LogAlways(msg, args);
        }
    }
}
