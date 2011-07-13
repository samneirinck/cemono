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

        }
    }
}
