using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public delegate void UIFunctionDelegate();

	public class UI
	{
        private static INativeUIMethods _nativeUIMethods;
        internal static INativeUIMethods NativeUIMethods
        {
            get { return _nativeUIMethods ?? (_nativeUIMethods = new NativeUIMethods()); }
            set { _nativeUIMethods = value; }
        }

		internal static IntPtr CreateEventSystem(string name, EventSystemType type)
		{
			var ptr = NativeUIMethods.CreateEventSystem(name, type);

			Delegates.Add(ptr, new Dictionary<uint, MethodInfo>());
			return ptr;
		}

		internal static void RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs, MethodInfo methodInfo)
		{
            Delegates[eventSystemPtr].Add(NativeUIMethods.RegisterFunction(eventSystemPtr, name, desc, inputs), methodInfo);
		}

		internal static void OnEvent(PointerWrapper ptrWrapper, uint eventId, object[] args)
		{
			if(!Delegates.ContainsKey(ptrWrapper.ptr))
				throw new ArgumentException("eventSystemPtr has not been registered with the UI system!");

			var delegateList = Delegates[ptrWrapper.ptr];
			if(!delegateList.ContainsKey(eventId))
				throw new ArgumentException(string.Format("eventId {0} has not been registered with the UI system!", eventId));

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