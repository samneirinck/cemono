using System.Diagnostics;

namespace CryEngine
{
    partial class Console
    {
        class CryTraceListener : TraceListener
        {
            public override void Write(string message)
            {
                WriteLine(message);
            }

            public override void WriteLine(string message)
            {
                Console.Log(message);
            }

            public override void TraceEvent(TraceEventCache eventCache, string source, TraceEventType eventType, int id, string message)
            {
                switch (eventType)
                {
                    default:
                        Console.Log(message);
                        break;
                }
            }
        }
    }
}