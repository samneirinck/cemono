namespace Cemono
{
    public interface ILogging
    {
        void Log(string msg);
        void Log(string msg, params object[] args);
        void LogAlways(string msg);
        void LogAlways(string msg, params object[] args);
    }
}
