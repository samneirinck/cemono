using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeInputMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void RegisterAction(string actionName);
    }
}
