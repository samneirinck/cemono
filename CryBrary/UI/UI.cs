using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using CryEngine.Native;
using CryEngine.Utilities;

namespace CryEngine
{
    public delegate void UIFunctionDelegate();

    public class UI
    {
        internal static IntPtr CreateEventSystem(string name, EventSystemType type)
        {
            var ptr = NativeMethods.UI.CreateEventSystem(name, type);

            Delegates.Add(ptr, new Dictionary<uint, MethodInfo>());
            return ptr;
        }

        internal static void RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs, MethodInfo methodInfo)
        {
            Delegates[eventSystemPtr].Add(NativeMethods.UI.RegisterFunction(eventSystemPtr, name, desc, inputs), methodInfo);
        }

        internal static void OnEvent(PointerWrapper ptrWrapper, uint eventId, object[] args)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!Delegates.ContainsKey(ptrWrapper.ptr))
                throw new ArgumentException("eventSystemPtr has not been registered with the UI system!");
#endif

            var delegateList = Delegates[ptrWrapper.ptr];

#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!delegateList.ContainsKey(eventId))
                throw new ArgumentException(string.Format("eventId {0} has not been registered with the UI system!", eventId));
#endif

            delegateList[eventId].Invoke(null, args);
        }

        static Dictionary<IntPtr, Dictionary<uint, MethodInfo>> Delegates = new Dictionary<IntPtr, Dictionary<uint, MethodInfo>>();

        public enum EventSystemType
        {
            ToSystem,
            ToUI
        }
    }
}