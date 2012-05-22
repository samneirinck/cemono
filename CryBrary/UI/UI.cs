using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using CryEngine.Extensions;

namespace CryEngine
{
	public delegate void UIFunctionDelegate();

	public class UI
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _CreateEventSystem(string name, EventSystemType type);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static uint _RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static uint _RegisterEvent(IntPtr eventSystemPtr, string name, string desc, object[] outputs);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SendEvent(IntPtr eventSystemPtr, uint eventId);

		internal static IntPtr CreateEventSystem(string name, EventSystemType type)
		{
			var ptr = _CreateEventSystem(name, type);

			Delegates.Add(ptr, new Dictionary<uint, UIFunctionDelegate>());
			return ptr;
		}

		internal static void RegisterFunction(IntPtr eventSystemPtr, string name, string desc, object[] inputs, UIFunctionDelegate funcDelegate)
		{
			Delegates[eventSystemPtr].Add(_RegisterFunction(eventSystemPtr, name, desc, inputs), funcDelegate);
		}

		internal static void OnEvent(PointerWrapper ptrWrapper, uint eventId, object[] args)
		{
			if(!Delegates.ContainsKey(ptrWrapper.ptr))
				throw new ArgumentException("eventSystemPtr has not been registered with the UI system!");

			var delegateList = Delegates[ptrWrapper.ptr];
			if(!delegateList.ContainsKey(eventId))
				throw new ArgumentException(string.Format("eventId {0} has not been registered with the UI system!", eventId));

			delegateList[eventId]();
		}

		static Dictionary<IntPtr, Dictionary<uint, UIFunctionDelegate>> Delegates = new Dictionary<IntPtr, Dictionary<uint, UIFunctionDelegate>>();

		public enum EventSystemType
		{
			ToSystem,
			ToUI
		}
	}
}