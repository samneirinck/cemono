using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeUIMethods : INativeUIMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private static IntPtr _CreateEventSystem(string name, UI.EventSystemType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern private static uint _RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint _RegisterEvent(IntPtr eventSystemPtr, string name, string desc, object[] outputs);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SendEvent(IntPtr eventSystemPtr, uint eventId, object[] args);


        public IntPtr CreateEventSystem(string name, UI.EventSystemType type)
        {
            return _CreateEventSystem(name, type);
        }

        public uint RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs)
        {
            return _RegisterFunction(eventSystemPtr, name, desc, inputs);
        }

        public uint RegisterEvent(IntPtr eventSystemPtr, string name, string desc, object[] outputs)
        {
            return _RegisterEvent(eventSystemPtr, name, desc, outputs);
        }

        public void SendEvent(IntPtr eventSystemPtr, uint eventId, object[] args)
        {
            _SendEvent(eventSystemPtr, eventId, args);
        }
    }
}
