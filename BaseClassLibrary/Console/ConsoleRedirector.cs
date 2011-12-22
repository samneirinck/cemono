using System.IO;
using System;

namespace CryEngine
{
    partial class CryConsole
    {
        class CryRedirector : StringWriter
        {
            // Redirector
            public override void Write(bool value)
            {
                Write(value.ToString());
            }

            public override void Write(char value)
            {
                Write(value.ToString());
            }

            public override void Write(char[] buffer)
            {
                Write(new string(buffer));
            }

            public override void Write(char[] buffer, int index, int count)
            {
                string s = new string(buffer);
                Write(s.Substring(index, count));
            }

            public override void Write(decimal value)
            {
                Write(value.ToString());
            }

            public override void Write(double value)
            {
                Write(value.ToString());
            }

            public override void Write(float value)
            {
                Write(value.ToString());
            }

            public override void Write(int value)
            {
                Write(value.ToString());
            }

            public override void Write(long value)
            {
                Write(value.ToString());
            }

            public override void Write(object value)
            {
                Write(value.ToString());
            }

            public override void Write(string format, object arg0)
            {
                Write(String.Format(format, arg0));
            }

            public override void Write(string format, object arg0, object arg1)
            {
                Write(String.Format(format, arg0, arg1));
            }

            public override void Write(string format, object arg0, object arg1, object arg2)
            {
                Write(String.Format(format, arg0, arg1, arg2));
            }

            public override void Write(string format, params object[] arg)
            {
                Write(format, arg);
            }

            public override void Write(string value)
            {
                CryConsole.Log(value);
            }

            public override void Write(uint value)
            {
                Write(value.ToString());
            }

            public override void Write(ulong value)
            {
                Write(value.ToString());
            }

            public override void WriteLine()
            {
                Write("");
            }

            public override void WriteLine(bool value)
            {
                Write(value);
            }

            public override void WriteLine(char value)
            {
                Write(value);
            }

            public override void WriteLine(char[] buffer)
            {
                Write(buffer);
            }

            public override void WriteLine(char[] buffer, int index, int count)
            {
                Write(buffer, index, count);
            }

            public override void WriteLine(decimal value)
            {
                Write(value);
            }

            public override void WriteLine(double value)
            {
                Write(value);
            }

            public override void WriteLine(float value)
            {
                Write(value);
            }

            public override void WriteLine(int value)
            {
                Write(value);
            }

            public override void WriteLine(long value)
            {
                Write(value);
            }

            public override void WriteLine(object value)
            {
                Write(value);
            }

            public override void WriteLine(string format, object arg0)
            {
                Write(format, arg0);
            }

            public override void WriteLine(string format, object arg0, object arg1)
            {
                Write(format, arg0, arg1);
            }

            public override void WriteLine(string format, object arg0, object arg1, object arg2)
            {
                Write(format, arg0, arg1, arg2);
            }

            public override void WriteLine(string format, params object[] arg)
            {
                Write(format, arg);
            }

            public override void WriteLine(uint value)
            {
                Write(value);
            }

            public override void WriteLine(ulong value)
            {
                Write(value);
            }

            public override void WriteLine(string value)
            {
                Write(value);
            }
        }
    }
}
