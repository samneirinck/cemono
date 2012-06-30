using System;

namespace CryEngine.Native
{
    internal interface INativeUIMethods
    {
        IntPtr CreateEventSystem(string name, UI.EventSystemType type);

        uint RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs);
        uint RegisterEvent(IntPtr eventSystemPtr, string name, string desc, object[] outputs);

        void SendEvent(IntPtr eventSystemPtr, uint eventId, object[] args);
    }
}