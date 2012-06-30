namespace CryEngine.Native
{
    internal interface INativeLoggingMethods
    {
        void LogAlways(string msg);
        void Log(string msg);
        void Warning(string msg);
    }
}