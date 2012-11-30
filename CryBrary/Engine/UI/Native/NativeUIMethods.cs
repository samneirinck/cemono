using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal static class NativeUIMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr CreateEventSystem(string name, UI.EventSystemType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint RegisterEvent(IntPtr eventSystemPtr, string name, string desc, object[] outputs);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SendEvent(IntPtr eventSystemPtr, uint eventId, object[] args);
    }
}
