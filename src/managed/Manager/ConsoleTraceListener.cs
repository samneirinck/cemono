using System.Diagnostics;
namespace Cemono
{
    public class ConsoleTraceListener : TraceListener
    {
        public ILogging Logging { get; set; }

        public override void Write(string message)
        {
            WriteLine(message);
        }

        public override void WriteLine(string message)
        {
            Logging.Log(message);
        }

        public override void TraceEvent(TraceEventCache eventCache, string source, TraceEventType eventType, int id, string message)
        {
            switch (eventType)
            {
                default:
                    Logging.Log(message);
                    break;
            }
        }
    }
}
