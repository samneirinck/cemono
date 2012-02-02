using System.Runtime.InteropServices;


namespace CryEngine
{
    public class CallbackHandler
    {
        public delegate void Callback();

        [DllImport("CryMono.dll")]
        extern static void _RegisterCallback(string func, string className,  Callback cb);

        public static void RegisterCallback(string func, string className, Callback callback)
        {
            _RegisterCallback(func, className, callback);
        }
    }
}
