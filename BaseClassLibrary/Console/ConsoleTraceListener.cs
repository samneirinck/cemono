using System.Diagnostics;

namespace CryEngine
{
    partial class CryConsole
    {
        class CryTraceListener : TraceListener
        {
            public override void Write(string message)
            {
                WriteLine(message);
            }

            public override void WriteLine(string message)
            {
                CryConsole.Log(message);
            }

            public override void TraceEvent(TraceEventCache eventCache, string source, TraceEventType eventType, int id, string message)
            {
                switch (eventType)
                {
                    default:
                        CryConsole.Log(message);
                        break;
                }
            }
        }
    }
}