using System.Runtime.InteropServices;


namespace CryEngine
{
    public class CallbackHandler
    {
        public delegate void Callback();

        public static void RegisterCallback(string func, string className, Callback callback)
        {
            NativeMethods._RegisterCallback(func, className, callback);
        }
    }
}
